#ifndef __osx_system_data_logger__converters__
#define __osx_system_data_logger__converters__

#include <MacTypes.h>
#include "smc.h"

float _strtof(char *str, int size, int e);
UInt32 _strtoul(char *str, int size, int base);
void _ultostr(char *str, UInt32 val);

float FP1F_to_f(SMCVal_t val);
float FP4C_to_f(SMCVal_t val);
float FP5B_to_f(SMCVal_t val);
float FP6A_to_f(SMCVal_t val);
float FP79_to_f(SMCVal_t val);
float FPE2_to_f(SMCVal_t val);
float FPA6_to_f(SMCVal_t val);
float FPC4_to_f(SMCVal_t val);
float FP88_to_f(SMCVal_t val);
float SP1E_to_f(SMCVal_t val);
float SP3C_to_f(SMCVal_t val);
float SP4B_to_f(SMCVal_t val);
float SP5A_to_f(SMCVal_t val);
float SP69_to_f(SMCVal_t val);
float SP78_to_f(SMCVal_t val);
float SP87_to_f(SMCVal_t val);
float SP96_to_f(SMCVal_t val);
float SPB4_to_f(SMCVal_t val);
float SPF0_to_f(SMCVal_t val);
int SI8_to_i(SMCVal_t val);
int SI16_to_i(SMCVal_t val);
float PWM_to_f(SMCVal_t val);
unsigned UInt_to_u(SMCVal_t val);


#endif /* defined(__osx_system_data_logger__converters__) */
