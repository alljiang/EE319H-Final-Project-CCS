#include <stdint.h>
#include "metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void UART_start(void (*fxn)(void));
extern void UART_transmit(uint8_t numBytes, uint8_t *buffer);
extern void UART_receive(uint32_t bytesToRead, uint8_t* buffer);
extern void UART_waitForAcknowledge();
extern void UART_sendAnimation(struct SpriteSendable sendable);
extern void UART_commandUpdate();
extern void UART_readCharacterSDCard(uint8_t characterIndex);
extern void UART_setBackgroundColors(uint8_t stage);
extern void UART_readPersistentSprite(uint8_t spriteIndex, uint16_t x, uint16_t y);
extern void UART_commandReset();

#ifdef __cplusplus
}
#endif
