/*
 * serial_msp.h
 *
 *  Created on: Jun 1, 2012
 *      Author: Erioto
 */

#ifndef SERIAL_MSP_H_
#define SERIAL_MSP_H_

#include <msp430g2553.h>
#include <stdio.h>

// Function prototypes
void serial_init(void);
void serial_charTX(char c);
char serial_charRX(void);
void serial_string(char string[]);
void serial_number(int value);

#endif 

/* SERIAL_MSP_H_ */