#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void delay(uint32_t ms);

extern uint32_t ticksToMicros(uint32_t ticks);
extern uint32_t ticksToMillis(uint32_t ticks);
extern uint32_t msToTicks(uint32_t ms);
extern uint32_t millis(void);

#ifdef __cplusplus
}
#endif
