/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyStepperLib.h"
#include "stm32_ros.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  DMA::init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */
  // TimPWM pwm1(TIM3, &htim3);

  // EncoderIT enc1(&htim2);

  // DigitalOut dir1(DIR1_GPIO_Port, DIR1_Pin);

  // StepperMotor motor1(enc1, pwm1, dir1);
  // motor1.setSpeed(1500);
  // motor1.setTargetPosition(5000);

  DigitalOut led1(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
  DigitalOut led2(ORANGE_LED_GPIO_Port, ORANGE_LED_Pin);
  DigitalOut led3(RED_LED_GPIO_Port, RED_LED_Pin);
  DigitalOut led4(BLUE_LED_GPIO_Port, BLUE_LED_Pin);

  TimIT tim1(TIM1, &htim1);
  uint64_t tick1_ms_previous = 0;
  uint64_t tick2_ms_previous = 0;
  uint64_t tick3_ms_previous = 0;
  uint64_t tick4_ms_previous = 0;
  tim1.start();

  uint16_t led1_delay = 0;
  uint16_t led2_delay = 0;
  uint16_t led3_delay = 0;
  uint16_t led4_delay = 0;
  
  // UART DMA read and write variables
  UartParser uart1(USART6, &huart6);
  uint8_t data[64] = {0};
  uart1.start_read();

  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */

    uint16_t len = uart1.read(data, sizeof(data));
    if (len > 0)
    {
      while(!uart1.is_tx_complete());
      uart1.write(data, len);
    }
    // if (uart1.is_tx_complete()) // optional
    // {
    //   memset(data, 0, sizeof(data));
    // }
    led1_delay = static_cast<uint16_t>(uart1.getJointPosition(1));
    led2_delay = static_cast<uint16_t>(uart1.getJointPosition(2));
    led3_delay = static_cast<uint16_t>(uart1.getJointPosition(3));
    led4_delay = static_cast<uint16_t>(uart1.getJointPosition(4));

    if (tim1.delay_ms(tick1_ms_previous, led1_delay))
    {
      tick1_ms_previous = tim1.read();
      led1.toggle();
    }

    if (tim1.delay_ms(tick2_ms_previous, led2_delay))
    {
      tick2_ms_previous = tim1.read();
      led2.toggle();
    }

    if (tim1.delay_ms(tick3_ms_previous, led3_delay))
    {
      tick3_ms_previous = tim1.read();
      led3.toggle();
    }

    if (tim1.delay_ms(tick4_ms_previous, led4_delay))
    {
      tick4_ms_previous = tim1.read();
      led4.toggle();
    }

    // motor1.update();
    // HAL_Delay(1);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
