#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define

extern void UART_start(void);
extern void UART_transmit(uint8_t numBytes, uint8_t *buffer);
extern void UART_receive(uint32_t bytesToRead, uint8_t* buffer);
extern void UART_sendByte(uint8_t byte);

#ifdef __cplusplus
}
#endif
