#include <stdint.h>
#include "metadata.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void UART_start(void);
extern void UART_transmit(uint8_t numBytes, uint8_t *buffer);
extern void UART_receive(uint32_t bytesToRead, uint8_t* buffer);
extern void UART_sendAnimation(struct SpriteSendable sendable);
extern void UART_readCharacterSDCard(uint8_t characterIndex);

#ifdef __cplusplus
}
#endif
