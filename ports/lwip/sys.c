#include "lwip/sys.h"

sys_prot_t sys_arch_protect(void)
{
	return 0;
}

void sys_arch_unprotect(sys_prot_t pval)
{
	(void)pval;
}
