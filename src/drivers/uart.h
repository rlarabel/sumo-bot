#ifndef UART_H
#define UART_H

void uart_init(void);
void _putchar(char c);

// These function should only be called by the assert handler
void uart_init_assert(void);
void uart_trace_assert(const char *string);

#endif