#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void sleep(uint32_t ms);
extern void sleepMicros(uint32_t us);
extern uint32_t ticksToMicros(uint32_t ticks);
extern uint32_t ticksToMillis(uint32_t ticks);
extern uint32_t msToTicks(uint32_t ms);
extern uint32_t usToTicks(uint32_t us);
extern uint32_t millis();
extern uint32_t micros();

#ifdef __cplusplus
}
#endif
