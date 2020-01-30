
#ifdef __cplusplus
extern "C" {
#endif


extern void UART_start(void);
extern void UART_transmit(char msg[]);
extern void UART_receive(char* buffer, uint32_t bytesToRead);

#ifdef __cplusplus
}
#endif
