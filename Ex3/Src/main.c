/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define pino_rs_0   GPIOA->BSRR = 1<<(16+9)  
#define pino_rs_1	GPIOA->BSRR = 1<<9
#define pino_e_0	GPIOC->BSRR = 1<<(16+7)
#define pino_e_1	GPIOC->BSRR = 1<<7

#define cursor_on 	 0x0c
#define cursor_off   0x0e
#define cursor_blink 0x0f

#define liga_led GPIOA->BSRR = (1 << 5);
#define desliga_led GPIOA->BRR = (1 << 5);



void uDelay(void)
{
  int x=10;
	
  while(x) x--;
}

void delayUs(int tempo)
{
  while(tempo--) uDelay();
}
void lcd_send4bits(unsigned char dat)
{
	if((dat & (1 << 0)) == 0)
		GPIOB->BRR = (1 << 5);
	else
		GPIOB->BSRR = (1 << 5);
	
	if((dat & (1 << 1)) == 0)
		GPIOB->BRR = (1 << 4);
	else
		GPIOB->BSRR = (1 << 4);

	if((dat & (1 << 2)) == 0)
		GPIOB->BRR = (1 << 10);
	else
		GPIOB->BSRR = (1 << 10);
	
	if((dat & (1 << 3)) == 0)
		GPIOA->BRR = (1 << 8);
	else
		GPIOA->BSRR = (1 << 8);	
}


void lcd_wrcom4(unsigned char com)
{
	lcd_send4bits(com);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	HAL_Delay(5);
}

void lcd_wrcom(unsigned char com)
{
	lcd_send4bits(com/0x10);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	lcd_send4bits(com%0x10);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	HAL_Delay(5);
}


void lcd_wrchar(unsigned char ch)
{
	lcd_send4bits(ch/0x10);
	pino_rs_1;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	lcd_send4bits(ch % 0x10);
	pino_rs_1;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	HAL_Delay(5);
}
void lcd_init(unsigned char cursor)
{
	lcd_wrcom4(3);
  lcd_wrcom4(3);
  lcd_wrcom4(3);
  lcd_wrcom4(2);

  lcd_wrcom(0x28);
  lcd_wrcom(cursor);
  lcd_wrcom(0x06);
  lcd_wrcom(0x01);

}

void lcd_goto(unsigned char x, unsigned char y)
{
  if(x<16)
  {
    if(y==0) lcd_wrcom(0x80+x);
    if(y==1) lcd_wrcom(0xc0+x);
    if(y==2) lcd_wrcom(0x90+x);
    if(y==3) lcd_wrcom(0xd0+x);
  }
}

void lcd_wrstr(char * str)
{
	while((*str))
	{
		lcd_wrchar(*str++);
	}
}


int le_AD(void)
{
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,50);
	int X = HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);
	return X;
}
void print_AD(int v)
{
	char str[30];
	sprintf(str,"%04d",v);
	lcd_goto(0,0);
	lcd_wrstr(str);
}
int le_teclas(void) // 0 nada 3 down 4 up 1 select 2 left 5 rigth
{
	// rigth == 0
	// 550 <= up <= 620
	// 1470 <= down <= 1500
	// 2400 <= left <= 2550
	// 3600 <= select <= 3700
	int tecla = 0; // nada
		
	int v = le_AD();
	//print_AD(v);
	
	if(v < 400)
		tecla = 5;
	else if(v >= 400 && v < 1200)
		tecla = 4;
	else if(v >= 1200 && v < 2000)
		tecla = 3;
	else if(v >= 2000 && v < 3200)
		tecla = 2;
	else if(v >= 3200 && v < 4000)
		tecla = 1;
	else
		tecla = 0;
	
	return tecla;
}
int aguarda_tecla(void)
{
	int tecla = 0;
	
	while(le_teclas()==0)
	{
	
	}
	
	tecla = le_teclas();
	
	while(le_teclas() != 0)
	{
	
	}
	
	return tecla;
}
void configura_relogio(int * var, unsigned char x, unsigned char y, int lim)
{
	int tecla = 0;
	char str[30];
	
	while(tecla != 5)
	{
		lcd_goto(x,y);
		sprintf(str,"%02d",*var);
		lcd_wrstr(str);
		tecla = aguarda_tecla();
		if(tecla == 4)
		{
			(*var)++;
			if(*var == lim)
				(*var) = 0;
		}
		else if(tecla == 3)
		{
			(*var)--;
			if(*var < 0)
				*var = lim-1;
		}
		else if(tecla == 2)
		{
			(*var) = 0;
		}
	}
}

void configura_data(int * var, unsigned char x, unsigned char y, int lim)
{
	int tecla = 0;
	char str[30];
	
	while(tecla != 5)
	{
		lcd_goto(x,y);
		sprintf(str,"%02d",*var);
		lcd_wrstr(str);
		tecla = aguarda_tecla();
		if(tecla == 4)
		{
			(*var)++;
			if(*var == lim)
				(*var) = 1;
		}
		else if(tecla == 3)
		{
			(*var)--;
			if(*var < 1)
				*var = lim-1;
		}
		else if(tecla == 2)
		{
			(*var) = 1;
		}
	}
}

void configura_ano(int * var)
{
	int tecla = 0;
	char str[30];
	
	unsigned char pegador = 0;

	
	while(tecla != 5)
	{
		lcd_goto(12,0);
		sprintf(str,"%04d",*var);
		lcd_wrstr(str);
		tecla = aguarda_tecla();
		if(tecla == 4)
		{
			if(pegador == 1)
			{
				*var = *var + 100;
			}
			else
			{
				(*var)++;
			}
		}
		else if(tecla == 3)
		{
			if(pegador == 1)
			{
				*var -= 100;
			}
			else
			{
				(*var)--;
			}
		}
		else if(tecla == 2)
		{
			(*var) = 2000;
		}
		else if(tecla == 1)
		{
			if(pegador == 0)
				pegador = 1;
			else
				pegador = 0;
		}
	}	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
	int hora, min, seg, h, m, s;
	
	int dia, mes, ano, d, mm, a;
	
	int n_dias = 0;
	
	char str[30];
	
	d = 1;
	mm = 1;
	a = 2000;
	
	h = 0;
	m = 0;
	s = 0;
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
	lcd_init(cursor_blink);
	GPIOB->BSRR = (1 << 6);

	configura_relogio(&h,8,1,24);
	lcd_wrchar(':');
	configura_relogio(&m,11,1,60);
	lcd_wrchar(':');
	configura_relogio(&s,14,1,60);
	
	configura_data(&d,6,0,32);
	lcd_wrchar('/');
	configura_data(&mm,9,0,13);
	lcd_wrchar('/');
	configura_ano(&a);
	lcd_wrchar('/');

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	
  while (1)
  {
		for(ano = a; ano < 9999; ano++)
		{
			for(mes = mm; mes <= 12; mes++)
			{
				
				if(mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12)
				{
					n_dias = 31;
				}
				else if(mes == 2)
				{
					if((ano%400 == 0) || ((ano%4 == 0) && (ano%100 != 0)))
					{
						n_dias = 29;
					}
					else
						n_dias = 28;
				}
				else
					n_dias = 30;
				
				for(dia = d; dia <= n_dias; dia++)
				{
					for(hora = h; hora < 24; hora++)
					{
						for(min = m; min < 60; min++)
						{
							for(seg = s; seg < 60; seg++)
							{
								lcd_goto(8,1);
								sprintf(str,"%02d:%02d:%02d",hora,min,seg);
								lcd_wrstr(str);
								
								lcd_goto(6,0);
								sprintf(str,"%02d/%02d/%04d",dia,mes,ano);
								lcd_wrstr(str);
								
								HAL_Delay(900);						
							}s = 0;
						}m = 0;
					}h = 0;
				}d = 1;
			}mm = 1;
		}a = 0;
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA8 PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
