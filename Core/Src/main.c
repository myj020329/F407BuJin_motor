/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define STEPMOTOR_MICRO_STEP      32  // 步进电机驱动器细分，必须与驱动器实际设置对应
uint8_t dir=0; // 0 ：顺时针   1：逆时针 
uint8_t ena=0; // 0 ：正常运行 1：停机
extern __IO uint16_t Toggle_Pulse; /* 步进电机速度控制，可调节范围为 300 -- 3500 ，值越小速度越快 */
/*
*    当步进电机驱动器细分设置为1时，每200个脉冲步进电机旋转一周
*                          为32时，每6400个脉冲步进电机旋转一周
*    下面以设置为32时为例讲解：
*    pulse_count用于记录输出脉冲数量，pulse_count为脉冲数的两倍，
*    比如当pulse_count=12800时，实际输出6400个完整脉冲。
*    这样可以非常方便步进电机的实际转动圈数，就任意角度都有办法控制输出。
*    如果步进电机驱动器的细分设置为其它值，pulse_count也要做相应处理
*
*/
__IO uint32_t pulse_count = 0; /*  脉冲计数，一个完整的脉冲会增加2 */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
 /* 确定定时器 */
  HAL_TIM_Base_Start(&htim8);
  /* 使能中断 关闭比较输出*/
  HAL_TIM_OC_Start_IT(&htim8,TIM_CHANNEL_1);
  TIM_CCxChannelCmd(TIM8,TIM_CHANNEL_1,TIM_CCx_DISABLE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	   
	if(Key_Scan(KEY1_GPIO_Port,KEY1_Pin) == KEY_ON)
	{
		pulse_count=0;            
		ena=0;          
		TIM_CCxChannelCmd(TIM8,TIM_CHANNEL_1,TIM_CCx_ENABLE);
	}
	

	if( Key_Scan(KEY2_GPIO_Port,KEY2_Pin) == KEY_ON)
	{
		Toggle_Pulse-=50;
		if(Toggle_Pulse<50)  // 最快速度限制
		  Toggle_Pulse=50;
	}
	

	if( Key_Scan(KEY3_GPIO_Port,KEY3_Pin) == KEY_ON)
	{
		Toggle_Pulse+=100;
		if(Toggle_Pulse>3500)         // 最慢速度限制
        Toggle_Pulse=3500;
	}
	

	if( Key_Scan(KEY4_GPIO_Port,KEY4_Pin) == KEY_ON)
	{
			if(dir==0)
		  {
			STEPMOTOR_DIR_REVERSAL();  // 反转
			dir=1;
		  }
		  else
		  {
			STEPMOTOR_DIR_FORWARD();   // 正转
			dir=0;
		  }
	}
	
	

	if( Key_Scan(KEY5_GPIO_Port,KEY5_Pin) == KEY_ON)
	{
		 if(ena==1)
		  {
			STEPMOTOR_OUTPUT_ENABLE();   // 正常运行
			HAL_TIM_OC_Start_IT(&htim8,TIM_CHANNEL_1);
			ena=0;
		  }
		  else
			  
		  {
			STEPMOTOR_OUTPUT_DISABLE();// 停机
			HAL_TIM_OC_Stop_IT(&htim8,TIM_CHANNEL_1);
			ena=1;
		  }
		}
		if(pulse_count >= STEPMOTOR_MICRO_STEP*200*2*10)  // 转动10圈后停机 
		{
		  TIM_CCxChannelCmd(TIM8,TIM_CHANNEL_1,TIM_CCx_DISABLE);
		}
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
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                 // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
/**
  * 函数功能: 定时器比较输出中断回调函数
  * 输入参数: htim：定时器句柄指针
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint32_t count;
  __IO uint32_t tmp;
  count =__HAL_TIM_GET_COUNTER(&htim8);
  tmp = 65535 & (count+Toggle_Pulse);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,tmp);
  pulse_count++;
}
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
