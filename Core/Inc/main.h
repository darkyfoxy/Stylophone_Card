/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file       main.h
  * @brief      Header for the main.c file.
  *
  * @author     darkyfoxy [*GitHub*](https://github.com/darkyfoxy)
  * @version    0.01
  * @date       05.05.2023
  *
  ******************************************************************************
  * @copyright  <h3>Copyright (c) 2023 Pavlov V.</h3>
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
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
#define BUT_3_Pin GPIO_PIN_14
#define BUT_3_GPIO_Port GPIOC
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


/** @defgroup Ecard_Contral Settings for ecard
  * @{
  */
#define BUFF_SIZE 1152                        /*!< Buffer size in uint32_t */

#define ARM_MATH_CM4                          /*!< Define for DSP library */

/**
  * @}
  */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
