#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alphabet.h"
#include "soft_alphabet_overlay.h"

static char *alphabet = { "abcdefghijklmnopqrstuvwxyz" };
static int overlay_digital(unsigned char *yuvOut, const unsigned char *osd, int osdW, int osdH, int yuvW, int yuvH,
			   int x, int y)
{
	int i = 0, j = 0;
	int ret = 0;
	if (x > yuvW || y > yuvH) {
		printf("pram err\n");
		return 0;
	}
	for (i = 0; i < osdH; i++) {
		for (j = 0; j < osdW; j++) {
			unsigned char osddata = osd[osdW * i + j];
			if (osddata != 0x00) {
				yuvOut[yuvW * (i + y) + j + x] = 0x80;
			} else {
				yuvOut[yuvW * (i + y) + j + x] = 0x0;
			}
		}
	}
	yuvOut += yuvW * yuvH;
	for (i = 0; i < osdH / 2; i++) {
		for (j = 0; j < osdW / 2; j++) {
			unsigned char osddata = osd[osdW * i * 2 + j * 2];
			if (osddata != 0x00) {
				yuvOut[yuvW * (i + y / 2) + x + j * 2] = 0xff; //u
				yuvOut[yuvW * (i + y / 2) + x + j * 2 + 1] = 0x80; //v
			}
		}
	}
	return ret;
}
int overlay_id(int id, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y)
{
	int temp_id = id;
	int units_digit = 0;
	int temp_x = (x / 2) * 2; //x need devided by 2
	int i = 0;
	int units_digit_num = 3;
	int postion = temp_x + digital_width * units_digit_num;
	int ret = 0;
	if (id == -1) {
		for (i = 0; i < units_digit_num; i++) {
			if (units_digit >= 0 && units_digit <= 9) {
				overlay_digital(yuvOut, DigitalArray[0], digital_width, digital_height, yuvW, yuvH,
						postion, y);
			}
			postion -= digital_width;
		}
	} else {
		for (i = 0; i < units_digit_num; i++) {
			units_digit = temp_id % 10;
			temp_id = temp_id / 10;
			if (units_digit >= 0 && units_digit <= 9) {
				overlay_digital(yuvOut, DigitalArray[units_digit], digital_width, digital_height, yuvW,
						yuvH, postion, y);
			}
			postion -= digital_width;
		}
	}
	return ret;
}

int overlay_confidence(float confidence, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y)
{
	int temp_id = confidence * 100;
	int units_digit = 0;
	int temp_x = (x / 2) * 2; //x need devided by 2
	int i = 0;
	int units_digit_num = 4;
	int postion = temp_x + digital_width * units_digit_num;
	int ret = 0;
	if (confidence == -1) {
		for (i = 0; i < units_digit_num; i++) {
			if (units_digit >= 0 && units_digit <= 9) {
				overlay_digital(yuvOut, DigitalArray[0], digital_width, digital_height, yuvW, yuvH,
						postion, y);
			}
			postion -= digital_width;
		}
	} else {
		for (i = 0; i < units_digit_num; i++) {
			units_digit = temp_id % 10;
			temp_id = temp_id / 10;
			if (i == 2) {
				overlay_digital(yuvOut, DigitalArray[12], digital_width, digital_height, yuvW, yuvH,
						postion, y);
			} else {
				if (units_digit >= 0 && units_digit <= 9) {
					overlay_digital(yuvOut, DigitalArray[units_digit], digital_width,
							digital_height, yuvW, yuvH, postion, y);
				}
			}
			postion -= digital_width;
		}
	}
	return ret;
}

int overlay_letter(char *name, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y)
{
	//int units_digit = 0;
	int temp_x = (x / 2) * 2 + 8; //x need devided by 2
	int alphabet_index = 0;
	int name_index = 0;
	int units_letter_num = strlen(name);
	//printf("letter len :%d\n", units_letter_num);
	int postion = temp_x;
	int ret = 0;
	for (name_index = 0; name_index < units_letter_num; name_index++) {
		for (alphabet_index = 0; alphabet_index < 26; alphabet_index++) {
			if (name[name_index] == alphabet[alphabet_index]) {
				break;
			}
		}
		overlay_digital(yuvOut, LetterArray[alphabet_index], digital_width, digital_height, yuvW, yuvH, postion,
				y + 8);
		postion += letter_position_width;
	}
	return ret;
}

int overlay_letter_confidence(char *name, float confidence, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y)
{
	//int units_digit = 0;
	int temp_x = (x / 2) * 2 + 8; //x need devided by 2
	int alphabet_index = 0;
	int name_index = 0;
	int units_letter_num = strlen(name);
	//printf("letter len :%d\n", units_letter_num);
	int postion = temp_x;
	int ret = 0;
	for (name_index = 0; name_index < units_letter_num; name_index++) {
		for (alphabet_index = 0; alphabet_index < 26; alphabet_index++) {
			if (name[name_index] == alphabet[alphabet_index]) {
				break;
			}
		}
		overlay_digital(yuvOut, LetterArray[alphabet_index], digital_width, digital_height, yuvW, yuvH, postion,
				y + 8);
		postion += letter_position_width;
	}
	overlay_confidence(confidence, yuvOut, yuvW, yuvH, postion, y + 8);
	return ret;
}
