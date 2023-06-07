#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rng.h"

static void initialize_rng(void)
{
	static bool initialized;

	if (!initialized) {
		MX_RNG_Init();
		initialized = true;
	}
}

int mbedtls_hardware_poll(void *data, unsigned char *output,
		size_t len, size_t *olen)
{
	initialize_rng();

	uint32_t random_value;
	size_t unit_len = sizeof(random_value);

	for (size_t index = 0; index < len/unit_len; index++) {
		if (rng_get_random_32bit(&random_value) == 0) {
			*olen += unit_len;
			memset(&(output[index * unit_len]),
					(int)random_value, unit_len);
		}
	}

	return 0;
}
