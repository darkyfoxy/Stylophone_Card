/*
 * keyboard.h
 *
 *  Created on: 14 февр. 2023 г.
 *      Author: Vlad
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#include "stm32l4xx_hal.h"



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


#define EXP_TIME_SIZE  20

typedef struct _note_table {
  uint16_t *size;
  uint16_t *index;
  uint8_t *exp_flag;
  uint16_t *exp_index;
  uint16_t *temp;
} note_table_t;

typedef struct
{
  uint32_t buffer[BUFF_SIZE];
  uint8_t valid;
} Universal_Buffer_TypeDef;


typedef struct _ecard ecard_t;
typedef struct _ecard {
  uint32_t  but_temp;
  uint32_t  buttons;

  uint32_t  keys;

  const int16_t **table_note_form;

  const float *exp_form;

  note_table_t *notes_table;

  // Sensor function pointers
  int  (*read_keys)              (ecard_t *ecard);

  int  (*init_note_form)         (ecard_t *ecard,\
                                  note_table_t *notes_table,\
                                  uint16_t *index,\
                                  uint8_t *note_exp_flag,\
                                  uint16_t *note_exp_index,\
                                  const float *exp_form,\
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


int ecard_init(ecard_t *ecard);

#endif /* INC_KEYBOARD_H_ */
