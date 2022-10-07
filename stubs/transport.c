#include "net/transport.h"
#include "libmcu/compiler.h"

LIBMCU_WEAK
struct transport_interface *tls_transport_create(
		const struct transport_conn_param *param)
{
	unused(param);
	return NULL;
}

LIBMCU_WEAK
void tls_transport_delete(struct transport_interface *instance)
{
	unused(instance);
}
