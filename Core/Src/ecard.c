/**
  ******************************************************************************
  * @file       ecard.c
  * @brief      This file provides functions to the main stylophone card driver.
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

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "main.h"
#include "ecard.h"
#include <string.h>
#include "arm_math.h"

/* Exported variables --------------------------------------------------------*/

extern Universal_Buffer_TypeDef SPK_buffer_0;
extern Universal_Buffer_TypeDef SPK_buffer_1;

extern Universal_Buffer_TypeDef MIC_buffer_0;
extern Universal_Buffer_TypeDef MIC_buffer_1;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  sampl_calculation
  *         Calculating note sample
  * @param  ecard: Pointer to ecard handler
  * @param  buffer: Pointer to buffer for writing sample
  * @param  note: Note number
  * @param  buff_size: Sample buffer size
  * @param  exp_delay: Relative note decay delay
  * @retval status
  */
static int sampl_calculation(ecard_t *ecard,
                             Universal_Buffer_TypeDef *buffer,\
                             uint8_t note,\
                             uint16_t buff_size,\
                             uint16_t exp_delay)
{
  int32_t *p_buffer = (int32_t *)buffer->buffer;

  if(note == PAUSE) /* If key not pressed */
  {
    memset(p_buffer, 0, buff_size*4);
  }
  else
  {
    const int16_t *note_form = ecard->table_note_form[note];
    uint8_t sampl_index = ecard->notes_table->index[note];
    uint16_t form_size = ecard->notes_table->size[note];

    if (ecard->vibrato == 1) /* If vibrato is ON */
    {
      const float *temp_vib_form = ecard->vib_form;
      uint32_t sampl_vib_index = ecard->vibrato_index;
      uint32_t temp_vib_index = ecard->vibrato_temp_index;

      for (int i = 0; i < buff_size; i++)
	  {
    	float note_S = (float) (note_form[sampl_index]);
		float vib_S = temp_vib_form[sampl_vib_index];
		float mult = note_S * vib_S;

	    p_buffer[i] = (int16_t)mult;
	    sampl_index++;

	    if(sampl_index == form_size)
	    {
		  sampl_index = 0;
	    }

	    temp_vib_index++;

		if(temp_vib_index == VIBRATO_TIME_SIZE)
		{
		  sampl_vib_index++;
		  temp_vib_index = 0;
		}

		if(sampl_vib_index == 480)
		{
		  sampl_vib_index = 0;
		  temp_vib_index = 0;
		}

	  }

      ecard->vibrato_temp_index = temp_vib_index;
      ecard->vibrato_index = sampl_vib_index;
    }
    else /* If vibrato is OFF */
    {
      for (int i = 0; i < buff_size; i++)
      {
        p_buffer[i] = note_form[sampl_index];
        sampl_index++;

        if(sampl_index == form_size)
        {
          sampl_index = 0;
        }
      }
    }
    ecard->notes_table->index[note] = sampl_index;
  }


  /* Decay calculation */
  for (int ni = 0; ni < 20; ni++)
  {
    if (ecard->notes_table->exp_flag[ni] != 0)
    {
      const int16_t *note_form = ecard->table_note_form[ni];
      uint8_t sampl_index = ecard->notes_table->index[ni];
      uint8_t form_size = ecard->notes_table->size[ni];

      const float *temp_exp_form = ecard->exp_form;
      uint16_t sampl_exp_index = ecard->notes_table->exp_index[ni];
      uint16_t sampl_temp = ecard->notes_table->temp[ni];

      for (int i = 0; i < buff_size; i++)
      {
        float note_S = (float) (note_form[sampl_index]);
        float exp_S = temp_exp_form[sampl_exp_index];
        float mult = note_S * exp_S;
        p_buffer[i] = (int32_t)p_buffer[i] + (int32_t)(mult);

        sampl_index++;

        if(sampl_index == form_size)
        {
          sampl_index = 0;  
        }

        sampl_temp++;

        if(sampl_temp == exp_delay)
        {
          sampl_exp_index++;
          sampl_temp = 0;
        }

        if(sampl_exp_index == 500)
        {
          sampl_exp_index = 0;
          ecard->notes_table->exp_flag[ni] = 0;
          sampl_index = 0;
          break;
        }

      }

      ecard->notes_table->temp[ni] = sampl_temp;
      ecard->notes_table->exp_index[ni] = sampl_exp_index;
      ecard->notes_table->index[ni] = sampl_index;
    }
  }
  return 0;
}

/**
  * @brief  init_note_form
  *         Start note table initialization
  * @param  ecard: Pointer to ecard handler
  * @param  notes_table: Pointer to note table
  * @param  index: Pointer to table with current note index
  * @param  note_exp_flag: Pointer to table with decay flag
  * @param  note_exp_index: Pointer to table with current decay index
  * @param  exp_form: Pointer to decay form
  * @param  vib_form: Pointer to vibrato form
  * @param  temp: Pointer to table with temp values
  * @retval status
  */
static int init_note_form(ecard_t *ecard,\
                          note_table_t *notes_table,\
                          uint16_t *index,\
                          uint8_t *note_exp_flag,\
                          uint16_t *note_exp_index,\
                          const float *exp_form,\
						  const float *vib_form,\
                          uint16_t *temp)
{
  ecard->exp_form = exp_form;
  ecard->vib_form = vib_form;

  ecard->notes_table = notes_table;

  ecard->notes_table->index = index;

  ecard->notes_table->exp_flag = note_exp_flag;
  ecard->notes_table->exp_index = note_exp_index;
  ecard->notes_table->temp = temp;

  return 0;
}

/**
  * @brief  read_keys
  *         Read keyboard status
  * @param  ecard: Pointer to ecard handler
  * @retval status
  */
static int read_keys(ecard_t *ecard)
{
  ecard->keys = 0;
  uint32_t port_c = ~(uint32_t)GPIOC->IDR;
  uint32_t port_b = ~(uint32_t)GPIOB->IDR;

  ecard->keys |= (port_c & (Am | Ashm)) >> 10;
  ecard->keys |= (port_b & Bm) >> 7;
  ecard->keys |= (port_c & (C | Csh | D | Dsh | E | F)) << 3;
  ecard->keys |= (port_b & (Fsh | G | Gsh)) << 9;
  ecard->keys |= (port_b & (A | Ash)) << 2;

  ecard->keys |= (port_c & Cp) << 8;
  ecard->keys |= (port_b & (Cshp | Dp)) << 4;
  ecard->keys |= (port_c & Dshp) << 12;

  ecard->keys |= (port_c & B) << 5;
  ecard->keys |= (port_c & Ep) << 11;

  return 0;
}

/**
  * @brief  read_button
  *         Read buttons status
  * @param  ecard: Pointer to ecard handler
  * @retval status
  */
static int read_button(ecard_t *ecard)
{
  uint32_t port_c = ~(uint32_t)GPIOC->IDR;

  uint32_t temp = (port_c & (GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)) >> 12;

  if(temp != ecard->buttons)
  {
    ecard->but_temp++;
  }
  else
  {
    ecard->but_temp = 0;
  }

  if (ecard->but_temp >= 2)
  {
    ecard->buttons = temp;
    ecard->but_temp = 0;
  }
  return 0;
}

/**
  * @brief  set_leds
  *         Set leds status
  * @param  ecard: Pointer to ecard handler
  * @param  leds: Leds status
  * @retval status
  */
static int set_leds(ecard_t *ecard, uint8_t leds)
{
  GPIOA->ODR = leds;

  return 0;
}

/**
  * @brief  set_note_table
  *         Set style of note samples
  * @param  ecard: Pointer to ecard handler
  * @param  table_note_form: Pointer to a table with samples
  * @param  size: Pointer to a table with sample sizes
  * @retval status
  */
static int set_note_table(ecard_t *ecard, const int16_t **table_note_form, uint16_t *size)
{
  ecard->table_note_form = table_note_form;

  ecard->notes_table->size = size;

  return 0;
}

/**
  * @brief  prepare_to_DAC
  *         Prepare sample to write to DAC
  * @param  ecard: Pointer to ecard handler
  * @param  in_buffer: Pointer to buffer for read sample
  * @param  adc_buffer: Pointer to buffer for writing sample
  * @param  buff_size: Sample buffer size
  * @retval status
  */
static int prepare_to_DAC(ecard_t *ecard,\
	                      Universal_Buffer_TypeDef *in_buffer,\
	                      Universal_Buffer_TypeDef *adc_buffer,\
	                      uint16_t buff_size)
{
  int32_t *in_buff = (int32_t *)in_buffer->buffer;
  uint32_t *adc_buff = adc_buffer->buffer;

  if(SPK_buffer_0.valid == 1)  /* IF USB speaker is available */
  {
    int16_t *spk_buff = (int16_t *)SPK_buffer_0.buffer;

    for (int i = 0; i < buff_size; i++)
    {
      adc_buff[i] = in_buff[i] + spk_buff[i] + 1500;
    }
    SPK_buffer_0.valid = 0;
  }
  else if(SPK_buffer_1.valid == 1) /* IF USB speaker is available */
  {
    int16_t *spk_buff = (int16_t *)SPK_buffer_1.buffer;

    for (int i = 0; i < buff_size; i++)
    {
      adc_buff[i] = in_buff[i] + spk_buff[i] + 1500;
    }
    SPK_buffer_1.valid = 0;
  }
  else /* IF USB speaker is not available */
  {
    for (int i = 0; i < buff_size; i++)
    {
      adc_buff[i] = in_buff[i] + 1500;
    }
  }
  return 0;
}

/**
  * @brief  prepare_to_MIC
  *         Prepare sample and write to USB microphone buffers
  * @param  ecard: Pointer to ecard handler
  * @param  in_buffer: Pointer to buffer for read sample
  * @param  buff_size: Sample buffer size
  * @retval status
  */
static int prepare_to_MIC(ecard_t *ecard,\
	                      Universal_Buffer_TypeDef *in_buffer,\
	                      uint16_t buff_size)
{

  int32_t *in_buff = (int32_t *)in_buffer->buffer;

  if(MIC_buffer_0.valid == 1) /* IF USB microphone is available */
  {
    int16_t *spk_buff = (int16_t *)MIC_buffer_0.buffer;

    for (int i = 0; i < buff_size; i++)
    {
      spk_buff[i] = in_buff[i];
    }
    MIC_buffer_0.valid = 0;
  }
  else if(MIC_buffer_1.valid == 1)
  {
    int16_t *spk_buff = (int16_t *)MIC_buffer_1.buffer;

    for (int i = 0; i < buff_size; i++)
    {
      spk_buff[i] = in_buff[i];
    }
    MIC_buffer_1.valid = 0;
  }
  return 0;
}

/* Functions ---------------------------------------------------------------------*/

/**
  * @brief  ecard_init
  *         Initialization main driver structure
  * @param  ecard: Pointer to ecard handler
  * @retval status
  */
int ecard_init(ecard_t *ecard)
{
  ecard->buttons = 0x00;
  ecard->keys = 0x00;

  ecard->vibrato = 0x00;
  ecard->vibrato_index = 0x00;
  ecard->vibrato_temp_index = 0x00;

  /* Function initialization */
  ecard->read_keys               = read_keys;
  ecard->init_note_form          = init_note_form;
  ecard->set_note_table          = set_note_table;

  ecard->sampl_calculation       = sampl_calculation;
  ecard->prepare_to_DAC          = prepare_to_DAC;
  ecard->prepare_to_MIC          = prepare_to_MIC;

  ecard->read_button             = read_button;

  ecard->set_leds                = set_leds;
  return 0;
}
