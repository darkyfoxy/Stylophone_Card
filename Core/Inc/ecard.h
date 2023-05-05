/**
  ******************************************************************************
  * @file       ecard.h
  * @brief      Header for the ecard.c file.
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

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

/* Private defines -----------------------------------------------------------*/

/** @defgroup Ecard_KeyBoard Pin numbers to notes
  * @{
  */
#define Am     GPIO_PIN_10
#define Ashm   GPIO_PIN_11
#define Bm     GPIO_PIN_9
#define C      GPIO_PIN_0
#define Csh    GPIO_PIN_1
#define D      GPIO_PIN_2
#define Dsh    GPIO_PIN_3
#define E      GPIO_PIN_4
#define F      GPIO_PIN_5
#define Fsh    GPIO_PIN_0
#define G      GPIO_PIN_1
#define Gsh    GPIO_PIN_2
#define A      GPIO_PIN_10
#define Ash    GPIO_PIN_11
#define B      GPIO_PIN_9
#define Cp     GPIO_PIN_7
#define Cshp   GPIO_PIN_12
#define Dp     GPIO_PIN_13
#define Dshp   GPIO_PIN_6
#define Ep     GPIO_PIN_8

/**
  * @}
  */

/** @defgroup Ecard_Note_Number Note numbers
  * @{
  */
#define NOTE_Am        0
#define NOTE_Ashm      1
#define NOTE_Bm        2
#define NOTE_C         3
#define NOTE_Csh       4
#define NOTE_D         5
#define NOTE_Dsh       6
#define NOTE_E         7
#define NOTE_F         8
#define NOTE_Fsh       9
#define NOTE_G         10
#define NOTE_Gsh       11
#define NOTE_A         12
#define NOTE_Ash       13
#define NOTE_B         14
#define NOTE_Cp        15
#define NOTE_Cshp      16
#define NOTE_Dp        17
#define NOTE_Dshp      18
#define NOTE_Ep        19
#define PAUSE          20

/**
  * @}
  */

/** @defgroup Ecard_Contral Settings for ecard
  * @{
  */
#define EXP_TIME_SIZE  20                           /*!< Relative note decay delay */

#define VIBRATO_TIME_SIZE  15                       /*!< Relative vibrato frequency  */
/**
  * @}
  */


/* Include HAL library */
#include "stm32l4xx_hal.h"

/* Private types -------------------------------------------------------------*/

/**
  * @brief Table with main parameters for notes structure definition
  */
typedef struct _note_table
{
  uint16_t *size;                                   /*!< Table with note sample sizes */

  uint16_t *index;                                  /*!< Table with current sample index */

  uint8_t *exp_flag;                                /*!< Table with note decay flags */

  uint16_t *exp_index;                              /*!< Table with current decay flags */

  uint16_t *temp;									/*!< Table with temp values */
} note_table_t;


/**
  * @brief  Main buffer structures definition
  */
typedef struct
{
  uint32_t buffer[BUFF_SIZE];
  uint8_t valid;
} Universal_Buffer_TypeDef;

/**
  * @brief  Main driver structures definition
  */
typedef struct _ecard ecard_t;
typedef struct _ecard
{
  uint32_t  vibrato;                              /*!< Vibrato on/off flag */

  uint32_t  vibrato_index;                        /*!< Vibrato current sample index */

  uint32_t  vibrato_temp_index;                   /*!< Vibrato temp sample index */

  const float *vib_form;                          /*!< Pointer to vibrato form sample */

  const float *exp_form;                          /*!< Pointer to decay form sample */

  uint32_t  but_temp;                             /*!< temp buttons status for debounce */

  uint32_t  buttons;                              /*!< current buttons status */

  uint32_t  keys;                                 /*!< current keyboard status */

  const int16_t **table_note_form;                /*!< Pointer to table with note samples */

  note_table_t *notes_table;                      /*!< Pointer to table with note parameters */

  //function pointers
  int  (*read_keys)              (ecard_t *ecard);

  int  (*init_note_form)         (ecard_t *ecard,\
                                  note_table_t *notes_table,\
                                  uint16_t *index,\
                                  uint8_t *note_exp_flag,\
                                  uint16_t *note_exp_index,\
                                  const float *exp_form,\
								  const float *vib_form,\
                                  uint16_t *temp);

  int (*sampl_calculation)       (ecard_t *ecard,\
                                  Universal_Buffer_TypeDef *buffer,\
                                  uint8_t note,\
                                  uint16_t buff_size,\
                                  uint16_t exp_delay);

  int (*prepare_to_DAC)          (ecard_t *ecard,\
                                  Universal_Buffer_TypeDef *in_buffer,\
                                  Universal_Buffer_TypeDef *adc_buffer,\
                                  uint16_t buff_size);

  int (*prepare_to_MIC)          (ecard_t *ecard,\
                                  Universal_Buffer_TypeDef *in_buffer,\
                                  uint16_t buff_size);

  int (*set_note_table)          (ecard_t *ecard,\
                                  const int16_t **table_note_form,\
                                  uint16_t *size);

  int (*read_button)             (ecard_t *ecard);

  int (*set_leds)                (ecard_t *ecard, uint8_t leds);

} ecard_t;


/* Functions prototypes ---------------------------------------------*/
/* Driver initialization function ***********************************************/
int ecard_init(ecard_t *ecard);

#endif /* INC_KEYBOARD_H_ */
