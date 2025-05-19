

#ifndef APP_H
#define APP_H

#include <stdint.h>

// Contains all the init and execute functions for all modules
void app_main(void);



void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);

void button_task_execute(void);

#endif
