/*
 * keyboad.c
 *
 *  Created on: 14 февр. 2023 г.
 *      Author: Vlad
 */
#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "main.h"
#include "ecard.h"

extern  uint16_t usb_buff [BUFF_SIZE];

static int full_buffer(ecard_t *ecard, uint32_t *p_fubber, uint8_t note, uint16_t buff_size){
	if(note == PAUSE){
		for (int i = 0; i < buff_size; i++){
				  p_fubber[i] = 1500 + usb_buff[i];
		}
	}
	else{
		const int32_t *note_form = ecard->table_note_form[note];
		uint8_t sampl_index = ecard->notes_table->index[note];
		uint8_t form_size = ecard->notes_table->size[note];

		for (int i = 0; i < buff_size; i++){
		  p_fubber[i] = 1500 + note_form[sampl_index] + usb_buff[i];
		  sampl_index++;

		  if(sampl_index == form_size){
			  sampl_index = 0;
		  }
		}

		ecard->notes_table->index[note] = sampl_index;
	}




	for (int ni = 0; ni < 20; ni++){
		if (ecard->notes_table->exp_flag[ni] != 0){

			const int32_t *note_form = ecard->table_note_form[ni];
			uint8_t sampl_index = ecard->notes_table->index[ni];
			uint8_t form_size = ecard->notes_table->size[ni];

			const float *temp_exp_form = ecard->exp_form;
			uint16_t sampl_exp_index = ecard->notes_table->exp_index[ni];
			uint16_t sampl_temp = ecard->notes_table->temp[ni];

			for (int i = 0; i < buff_size; i++){
				float note_S = (float) (note_form[sampl_index]);
				float exp_S = temp_exp_form[sampl_exp_index];
				float mult = note_S * exp_S;
				p_fubber[i] = (int32_t)p_fubber[i] + (int32_t)(mult);

			    sampl_index++;

			    if(sampl_index == form_size){
				  sampl_index = 0;
			    }

			    sampl_temp++;

				if(sampl_temp == EXP_TIME_SIZE){
					sampl_exp_index++;
					sampl_temp = 0;
				}

				if(sampl_exp_index == 500){
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


static int init_note_form(ecard_t *ecard,\
		                  const int32_t **table_note_form,\
						  note_table_t *notes_table,\
						  uint8_t *size,\
						  uint8_t *index,\
						  uint8_t *note_exp_flag,\
						  uint16_t *note_exp_index,\
						  const float *exp_form,\
						  uint16_t *temp){

	ecard->table_note_form = table_note_form;

	ecard->exp_form = exp_form;

	ecard->notes_table = notes_table;

	ecard->notes_table->size = size;
	ecard->notes_table->index = index;

	ecard->notes_table->exp_flag = note_exp_flag;
	ecard->notes_table->exp_index = note_exp_index;
	ecard->notes_table->temp = temp;

	return 0;
}


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


static int read_button(ecard_t *ecard){
	uint32_t port_c = ~(uint32_t)GPIOC->IDR;

	uint32_t temp = (port_c & (GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)) >> 12;

	if(temp != ecard->buttons){
		ecard->but_temp++;
	}
	else{
		ecard->but_temp = 0;
	}

	if (ecard->but_temp >= 2){
		ecard->buttons = temp;
		ecard->but_temp = 0;
	}
	return 0;
}

static int set_leds(ecard_t *ecard, uint8_t leds){
	GPIOA->ODR = leds;

	return 0;
}

static int set_note_table(ecard_t *ecard, const int32_t **table_note_form){

	ecard->table_note_form = table_note_form;

	return 0;
}

int ecard_init(ecard_t *ecard){
	ecard->buttons = 0x00;
	ecard->keys = 0x00;

	note_table_t note_table;
	ecard->notes_table = &note_table;

	ecard->read_keys               = read_keys;
	ecard->init_note_form          = init_note_form;
	ecard->set_note_table		   = set_note_table;

	ecard->full_buffer             = full_buffer;

	ecard->read_button             = read_button;

	ecard->set_leds                = set_leds;
	return 0;
}
