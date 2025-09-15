/* USER CODE BEGIN Header */
// MASTER MASTER MASTER MASTER MASTER MASTER MASTER MASTER MASTER MASTER MASTER
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
#include "can.h"
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyStepperLib.h"
// #include "stm32_ros.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SLAVE_ID 0x121
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile float latest_deg1 = 1.0;
volatile float latest_deg2 = 1.0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

// CAN pack & send helper
static inline void CAN_Send2F(uint16_t std_id, float a, float b) {
    CAN_TxHeaderTypeDef tx = {0};
    uint8_t data[8];

    memcpy(&data[0], &a, 4);   // STM32 = little-endian
    memcpy(&data[4], &b, 4);

    tx.StdId = std_id;         // 0x121 / 0x122 / 0x123

    uint32_t mbx;
    HAL_StatusTypeDef st = HAL_CAN_AddTxMessage(&hcan1, &tx, data, &mbx);
    if (st != HAL_OK) {
        // Only abort if we actually got a valid mailbox bit
        if (mbx == CAN_TX_MAILBOX0 || mbx == CAN_TX_MAILBOX1 || mbx == CAN_TX_MAILBOX2) {
            HAL_CAN_AbortTxRequest(&hcan1, mbx);
        }
    }
}

static void CAN_SetFilter_Std(void)
{
    CAN_FilterTypeDef f = {0};
    f.FilterBank           = 0;                  // must be a CAN1 bank (see step 2)
    f.FilterMode           = CAN_FILTERMODE_IDMASK;
    f.FilterScale          = CAN_FILTERSCALE_32BIT;
    f.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    f.FilterActivation     = CAN_FILTER_ENABLE;
    f.FilterIdHigh = 0x0000; f.FilterIdLow = 0x0000;
    f.FilterMaskIdHigh = 0x0000; f.FilterMaskIdLow = 0x0000; // << accept everything
    if (HAL_CAN_ConfigFilter(&hcan1, &f) != HAL_OK) Error_Handler();
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void TaskFunction(void)
{

}

CAN_HandleTypeDef    CanHandle;
CAN_TxHeaderTypeDef  TxHeader;
CAN_RxHeaderTypeDef  RxHeader;
uint8_t              TxData[8];
uint8_t              RxData[8];
uint32_t             TxMailbox;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx;
    uint8_t d[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx, d) != HAL_OK) return;

    if (rx.IDE == CAN_ID_STD && rx.StdId == SLAVE_ID && rx.DLC == 8) {
        float a, b;
        memcpy(&a, &d[0], 4);
        memcpy(&b, &d[4], 4);
        latest_deg1 = a;
        latest_deg2 = b;
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */

    CAN_SetFilter_Std();
    if (HAL_CAN_Start(&hcan1) != HAL_OK) Error_Handler();
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) Error_Handler();
    

  // TimIT taskTimer(TIM1, &htim1);
  // taskTimer.setUserCallback(TaskFunction);
  // taskTimer.start();

  // TimPWM pwm1(TIM3, &htim3);
  // TimPWM pwm2(TIM8, &htim8);

  // EncoderIT enc1(&htim2, true);
  // EncoderIT enc2(&htim4, true);
  // enc1.start();
  // enc2.start();

  // DigitalOut dir1(DIR1_GPIO_Port, DIR1_Pin);
  // DigitalOut dir2(DIR2_GPIO_Port, DIR2_Pin);

  // OpenLoopStepper   motor1(pwm1, dir1, 200.0f, 10.0f);
  // OpenLoopStepper   motor2(pwm2, dir2, 10000.0f,  50.0f);

  // motor1.setSpeed(1500);
  // motor1.setTargetDegrees(5.0f);  // half turn at OUTPUT shaft
 
  // motor2.setSpeed(1500);
  // motor2.setTargetDegrees(5.0f);   // quarter turn at OUTPUT shaft

  DigitalOut red_LED(RED_LED_GPIO_Port, RED_LED_Pin);
  DigitalOut blue_LED(BLUE_LED_GPIO_Port, BLUE_LED_Pin);

  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
{
    // motor1.update();
    // HAL_Delay(1);
    // motor2.update();
    // HAL_Delay(1);

    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
    CAN_RxHeaderTypeDef rx;
    uint8_t d[8];
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx, d) == HAL_OK) {
        if (rx.IDE == CAN_ID_STD && rx.StdId == SLAVE_ID && rx.DLC == 8) {
            float a, b;
            memcpy(&a, &d[0], 4);
            memcpy(&b, &d[4], 4);
            latest_deg1 = a;
            latest_deg2 = b;
        }
      }
    }

    red_LED.toggle();
    uint32_t dly = (uint32_t)latest_deg2;
    if (dly > 10000U) dly = 10000U;
    HAL_Delay(dly);

    blue_LED.toggle();
    HAL_Delay(1000U);
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
