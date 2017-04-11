#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

//KEYÇý¶¯  

#define WK_UP     HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP°´¼üPA0

#define WKUP_PRES 1

void KEY_Init(void);
u8   KEY_Scan(u8 mode);

#endif
