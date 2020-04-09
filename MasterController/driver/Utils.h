#include <stdint.h>
#include <ti/sysbios/knl/Task.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE 4000

Task_Struct taskStruct;
char stack[SIZE];

extern void sleep(uint32_t ms);
extern uint32_t millis();
extern double absVal(double d);

#ifdef __cplusplus
}
#endif
