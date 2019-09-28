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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
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


int le_teclas(void)
{
	int tecla = 12;
	
	GPIOB->BSRR = (1<<(16+7));
	GPIOC->BSRR = (1<<12) + (1<<11) + (1<<10);
	
	if((GPIOA->IDR & (1<<15)) == 0)
	{
		tecla = 1;
	}
	else if((GPIOB->IDR & (1<<2)) == 0)
	{
		tecla = 2;
	}
	else if((GPIOB->IDR & (1<<11)) == 0)
	{
		tecla = 3;
	}
	
	GPIOB->BSRR = (1<<(7));
	GPIOC->BSRR = (1<<(12+16)) + (1<<11) + (1<<10);
	
	if((GPIOA->IDR & (1<<15)) == 0)
	{
		tecla = 4;
	}
	else if((GPIOB->IDR & (1<<2)) == 0)
	{
		tecla = 5;
	}
	else if((GPIOB->IDR & (1<<11)) == 0)
	{
		tecla = 6;
	}
	
	GPIOB->BSRR = (1<<(7));
	GPIOC->BSRR = (1<<(12)) + (1<<(11+16)) + (1<<10);
	
	if((GPIOA->IDR & (1<<15)) == 0)
	{
		tecla = 7;
	}
	else if((GPIOB->IDR & (1<<2)) == 0)
	{
		tecla = 8;
	}
	else if((GPIOB->IDR & (1<<11)) == 0)
	{
		tecla = 9;
	}
	
	GPIOB->BSRR = (1<<7);
	GPIOC->BSRR = (1<<(12)) + (1<<11) + (1<<(10+16));
	
	if((GPIOA->IDR & (1<<15)) == 0)
	{
		tecla = 10;
	}
	else if((GPIOB->IDR & (1<<2)) == 0)
	{
		tecla = 0;
	}
	else if((GPIOB->IDR & (1<<11)) == 0)
	{
		tecla = 11;
	}
	
	return tecla;
	
}

int aguarda_tecla(void)
{
	int tecla = 12;
	
	while(le_teclas()==12)
	{
	
	}
	
	tecla = le_teclas();
	
	while(le_teclas() != 12)
	{
	
	}
	
	return tecla;
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
	
	int pegador = 0;
	
	int tecla;
	
	int n_dias = 0;
	
	char str[30];
	
	d = 31;
	mm = 12;
	a = 2016;
	
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
  /* USER CODE BEGIN 2 */
	lcd_init(cursor_blink);
	GPIOB->BSRR = (1 << 6);

	/*tecla = 0;
	
	while(tecla != 10)
	{
		lcd_goto(8,1);
		sprintf(str,"%02d",h);
		lcd_wrstr(str);
		
		tecla = aguarda_tecla();
		
		lcd_goto(0,0);
		sprintf(str,"%02d",tecla);
		lcd_wrstr(str);
		
		if(tecla == 11)
			h = 0;
		else
		{
			if(pegador == 0)
			{
				h = tecla * 10;
				pegador = 1;
			}
			else if(pegador == 1)
			{
				h += tecla;
				pegador = 2;
			}
			else if(pegador == 2)
			{
				
			}
		}
	}*/


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
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
