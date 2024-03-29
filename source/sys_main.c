/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#include "H_LiquidCrystal.h"

#include <stdio.h>
#include <math.h>

#include "sci.h"

/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
//              |  VARIABLES  |

unsigned char command[80];
unsigned int NumberOfChar;

#define PORT_LCD16x2_D4 gioPORTA
#define BIT_LCD16x2_D4 0

#define PORT_LCD16x2_D5 gioPORTA
#define BIT_LCD16x2_D5 1

#define PORT_LCD16x2_D6 gioPORTA
#define BIT_LCD16x2_D6 2

#define PORT_LCD16x2_D7 gioPORTA
#define BIT_LCD16x2_D7 5

#define PORT_LCD16x2_RS gioPORTA
#define BIT_LCD16x2_RS 6

#define PORT_LCD16x2_RW gioPORTA
#define BIT_LCD16x2_RW 255

#define PORT_LCD16x2_E gioPORTA
#define BIT_LCD16x2_E 7

GIO LCD16x2_D4, LCD16x2_D5, LCD16x2_D6, LCD16x2_D7, LCD16x2_RS, LCD16x2_RW, LCD16x2_E, GIO_NULL;

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void inicializaciones_puertos(void);
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    inicializaciones_puertos();
    sciInit();


    while (1) {

        LCD_setCursor(0, 1); // col: 0 - 15 | row: 0 - 1
        LCD_printLn("Hola mundo!");
    }

    /* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
void inicializaciones_puertos(void) {
    LCD16x2_D4.PORT = PORT_LCD16x2_D4;
    LCD16x2_D4.BIT = BIT_LCD16x2_D4;

    LCD16x2_D5.PORT = PORT_LCD16x2_D5;
    LCD16x2_D5.BIT = BIT_LCD16x2_D5;

    LCD16x2_D6.PORT = PORT_LCD16x2_D6;
    LCD16x2_D6.BIT = BIT_LCD16x2_D6;

    LCD16x2_D7.PORT = PORT_LCD16x2_D7;
    LCD16x2_D7.BIT = BIT_LCD16x2_D7;

    LCD16x2_E.PORT = PORT_LCD16x2_E;
    LCD16x2_E.BIT = BIT_LCD16x2_E;

    LCD16x2_RW.PORT = PORT_LCD16x2_RW;
    LCD16x2_RW.BIT = BIT_LCD16x2_RW;

    LCD16x2_RS.PORT = PORT_LCD16x2_RS;
    LCD16x2_RS.BIT = BIT_LCD16x2_RS;

    GIO_NULL.PORT = NULL;
    GIO_NULL.BIT = 0;

    LCD_init(true, LCD16x2_RS, LCD16x2_RW, LCD16x2_E, LCD16x2_D4, LCD16x2_D5, LCD16x2_D6, LCD16x2_D7, GIO_NULL, GIO_NULL, GIO_NULL, GIO_NULL);

    // set up the LCD's number of columns and rows:
    LCD_begin(16, 2, LCD_5x8DOTS);
}
/* USER CODE END */
