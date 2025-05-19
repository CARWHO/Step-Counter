/*
 * display.h
 *
 *  Created on: Mar 10, 2025
 *      Author: OKH20
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "data_types.h"



// Initialising function for the module
void display_init (void);

// Execute function for the module
void display_execute(void);


// Pads the string before displaying on the screen
void pad_string(char* dest, size_t dest_size, const char* src, int width);

// For displaying title on the screen
void display_title( char* title);


/*
 * For displaying string on the screen
 * Note: it is not a variac function,
 * can only take in one (1) input/ format specifier
 */
void format_display(int x, int y, const char* display_char, uint32_t input1);





#endif /* INC_DISPLAY_H_ */
