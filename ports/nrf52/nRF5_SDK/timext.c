#include "libmcu/timext.h"
#include "nrf_delay.h"

void sleep_ms(unsigned int msec)
{
	nrf_delay_ms(msec);
}
