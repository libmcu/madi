# SPDX-License-Identifier: MIT

LWIP_ROOT := $(BASEDIR)/external/lwip

LWIP_SRCS := \
	$(LWIP_ROOT)/src/core/altcp.c \
	$(LWIP_ROOT)/src/core/altcp_alloc.c \
	$(LWIP_ROOT)/src/core/altcp_tcp.c \
	$(LWIP_ROOT)/src/core/def.c \
	$(LWIP_ROOT)/src/core/dns.c \
	$(LWIP_ROOT)/src/core/inet_chksum.c \
	$(LWIP_ROOT)/src/core/init.c \
	$(LWIP_ROOT)/src/core/ip.c \
	$(LWIP_ROOT)/src/core/mem.c \
	$(LWIP_ROOT)/src/core/memp.c \
	$(LWIP_ROOT)/src/core/netif.c \
	$(LWIP_ROOT)/src/core/pbuf.c \
	$(LWIP_ROOT)/src/core/raw.c \
	$(LWIP_ROOT)/src/core/stats.c \
	$(LWIP_ROOT)/src/core/sys.c \
	$(LWIP_ROOT)/src/core/tcp.c \
	$(LWIP_ROOT)/src/core/tcp_in.c \
	$(LWIP_ROOT)/src/core/tcp_out.c \
	$(LWIP_ROOT)/src/core/timeouts.c \
	$(LWIP_ROOT)/src/core/udp.c \
	$(LWIP_ROOT)/src/core/ipv4/autoip.c \
	$(LWIP_ROOT)/src/core/ipv4/dhcp.c \
	$(LWIP_ROOT)/src/core/ipv4/etharp.c \
	$(LWIP_ROOT)/src/core/ipv4/icmp.c \
	$(LWIP_ROOT)/src/core/ipv4/igmp.c \
	$(LWIP_ROOT)/src/core/ipv4/ip4.c \
	$(LWIP_ROOT)/src/core/ipv4/ip4_addr.c \
	$(LWIP_ROOT)/src/core/ipv4/ip4_frag.c \
	$(LWIP_ROOT)/src/core/ipv6/dhcp6.c \
	$(LWIP_ROOT)/src/core/ipv6/ethip6.c \
	$(LWIP_ROOT)/src/core/ipv6/icmp6.c \
	$(LWIP_ROOT)/src/core/ipv6/inet6.c \
	$(LWIP_ROOT)/src/core/ipv6/ip6.c \
	$(LWIP_ROOT)/src/core/ipv6/ip6_addr.c \
	$(LWIP_ROOT)/src/core/ipv6/ip6_frag.c \
	$(LWIP_ROOT)/src/core/ipv6/mld6.c \
	$(LWIP_ROOT)/src/core/ipv6/nd6.c \
	$(LWIP_ROOT)/src/netif/ethernet.c \
	$(LWIP_ROOT)/src/netif/slipif.c \
	$(LWIP_ROOT)/src/apps/http/httpd.c \
	$(LWIP_ROOT)/src/apps/http/fs.c

LWIP_INCS := \
	$(LWIP_ROOT)/src/include \
	$(LWIP_ROOT)/src/include/lwip/apps \
	$(LWIP_PORT_ROOT)

$(addprefix $(OUTDIR)/, $(LWIP_SRCS:%=%.o)): CFLAGS+=-Wno-error
