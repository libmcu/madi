#include "net/wifi.h"
#include <stddef.h>
#include "libmcu/compiler.h"

LIBMCU_WEAK
struct wifi *wifi_create_default(void)
{
	return NULL;
}

LIBMCU_WEAK
void wifi_destroy_default(struct wifi *iface)
{
	unused(iface);
}
