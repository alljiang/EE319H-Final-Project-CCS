#include <stdint.h>
#include <ti/sysbios/knl/Task.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE 4200

extern void sleep(uint32_t ms);
extern uint32_t millis();
extern double absVal(double d);
extern int random(int low, int high);

#ifdef __cplusplus
}
#endif
