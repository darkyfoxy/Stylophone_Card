/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */



/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUT_2_Pin GPIO_PIN_13
#define BUT_2_GPIO_Port GPIOC
#define BUT_4_Pin GPIO_PIN_15
#define BUT_4_GPIO_Port GPIOC
#define C_Pin GPIO_PIN_0
#define C_GPIO_Port GPIOC
#define Csh_Pin GPIO_PIN_1
#define Csh_GPIO_Port GPIOC
#define D_Pin GPIO_PIN_2
#define D_GPIO_Port GPIOC
#define Dsh_Pin GPIO_PIN_3
#define Dsh_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOA
#define E_Pin GPIO_PIN_4
#define E_GPIO_Port GPIOC
#define F_Pin GPIO_PIN_5
#define F_GPIO_Port GPIOC
#define Fsh_Pin GPIO_PIN_0
#define Fsh_GPIO_Port GPIOB
#define G_Pin GPIO_PIN_1
#define G_GPIO_Port GPIOB
#define Gsh_Pin GPIO_PIN_2
#define Gsh_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_10
#define A_GPIO_Port GPIOB
#define Ash_Pin GPIO_PIN_11
#define Ash_GPIO_Port GPIOB
#define Cshp_Pin GPIO_PIN_12
#define Cshp_GPIO_Port GPIOB
#define Dp_Pin GPIO_PIN_13
#define Dp_GPIO_Port GPIOB
#define Dshp_Pin GPIO_PIN_6
#define Dshp_GPIO_Port GPIOC
#define Cp_Pin GPIO_PIN_7
#define Cp_GPIO_Port GPIOC
#define Ep_Pin GPIO_PIN_8
#define Ep_GPIO_Port GPIOC
#define B_Pin GPIO_PIN_9
#define B_GPIO_Port GPIOC
#define Am_Pin GPIO_PIN_10
#define Am_GPIO_Port GPIOC
#define Ashm_Pin GPIO_PIN_11
#define Ashm_GPIO_Port GPIOC
#define BUT_1_Pin GPIO_PIN_12
#define BUT_1_GPIO_Port GPIOC
#define Bm_Pin GPIO_PIN_9
#define Bm_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define BUFF_SIZE 1152


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
