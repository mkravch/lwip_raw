/**
 * @file
 * Modules initialization
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "opt.h"

#include "init.h"
#include "stats.h"
#include "sys.h"
#include "mem.h"
#include "memp.h"
#include "pbuf.h"
#include "netif.h"
#include "sockets.h"
#include "ip.h"
#include "raw.h"
#include "udp.h"
#include "tcp_impl.h"
#include "snmp_msg.h"
#include "autoip.h"
#include "igmp.h"
#include "dns.h"
#include "timers.h"
#include "../netif/etharp.h"
#include "api.h"

#if !NO_SYS
#include "lwip/tcpip.h"
#endif

/* Compile-time sanity checks for configuration errors.
 * These can be done independently of LWIP_DEBUG, without penalty.
 */
#ifndef BYTE_ORDER
  #error "BYTE_ORDER is not defined, you have to define it in your cc.h"
#endif
#if (!IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV)
  #error "If you want to use broadcast filter per pcb on recv operations, you have to define IP_SOF_BROADCAST=1 in your lwipopts.h"
#endif
#if (!LWIP_ARP && ARP_QUEUEING)
  #error "If you want to use ARP Queueing, you have to define LWIP_ARP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_UDPLITE)
  #error "If you want to use UDP Lite, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_SNMP)
  #error "If you want to use SNMP, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_DHCP)
  #error "If you want to use DHCP, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_IGMP)
  #error "If you want to use IGMP, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_SNMP)
  #error "If you want to use SNMP, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if (!LWIP_UDP && LWIP_DNS)
  #error "If you want to use DNS, you have to define LWIP_UDP=1 in your lwipopts.h"
#endif
#if !MEMP_MEM_MALLOC /* MEMP_NUM_* checks are disabled when not using the pool allocator */
#if (LWIP_ARP && ARP_QUEUEING && (MEMP_NUM_ARP_QUEUE<=0))
  #error "If you want to use ARP Queueing, you have to define MEMP_NUM_ARP_QUEUE>=1 in your lwipopts.h"
#endif
#if (LWIP_RAW && (MEMP_NUM_RAW_PCB<=0))
  #error "If you want to use RAW, you have to define MEMP_NUM_RAW_PCB>=1 in your lwipopts.h"
#endif
#if (LWIP_UDP && (MEMP_NUM_UDP_PCB<=0))
  #error "If you want to use UDP, you have to define MEMP_NUM_UDP_PCB>=1 in your lwipopts.h"
#endif
#if (LWIP_TCP && (MEMP_NUM_TCP_PCB<=0))
  #error "If you want to use TCP, you have to define MEMP_NUM_TCP_PCB>=1 in your lwipopts.h"
#endif
#if (LWIP_IGMP && (MEMP_NUM_IGMP_GROUP<=1))
  #error "If you want to use IGMP, you have to define MEMP_NUM_IGMP_GROUP>1 in your lwipopts.h"
#endif
#if ((LWIP_NETCONN || LWIP_SOCKET) && (MEMP_NUM_TCPIP_MSG_API<=0))
  #error "If you want to use Sequential API, you have to define MEMP_NUM_TCPIP_MSG_API>=1 in your lwipopts.h"
#endif
/* There must be sufficient timeouts, taking into account requirements of the subsystems. */
#if LWIP_TIMERS && (MEMP_NUM_SYS_TIMEOUT < (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT))
  #error "MEMP_NUM_SYS_TIMEOUT is too low to accomodate all required timeouts"
#endif
#if (IP_REASSEMBLY && (MEMP_NUM_REASSDATA > IP_REASS_MAX_PBUFS))
  #error "MEMP_NUM_REASSDATA > IP_REASS_MAX_PBUFS doesn't make sense since each struct ip_reassdata must hold 2 pbufs at least!"
#endif
#endif /* !MEMP_MEM_MALLOC */
#if (LWIP_TCP && (TCP_WND > 0xffff))
  #error "If you want to use TCP, TCP_WND must fit in an u16_t, so, you have to reduce it in your lwipopts.h"
#endif
#if (LWIP_TCP && (TCP_SND_QUEUELEN > 0xffff))
  #error "If you want to use TCP, TCP_SND_QUEUELEN must fit in an u16_t, so, you have to reduce it in your lwipopts.h"
#endif
#if (LWIP_TCP && (TCP_SND_QUEUELEN < 2))
  #error "TCP_SND_QUEUELEN must be at least 2 for no-copy TCP writes to work"
#endif
#if (LWIP_TCP && ((TCP_MAXRTX > 12) || (TCP_SYNMAXRTX > 12)))
  #error "If you want to use TCP, TCP_MAXRTX and TCP_SYNMAXRTX must less or equal to 12 (due to tcp_backoff table), so, you have to reduce them in your lwipopts.h"
#endif
#if (LWIP_TCP && TCP_LISTEN_BACKLOG && (TCP_DEFAULT_LISTEN_BACKLOG < 0) || (TCP_DEFAULT_LISTEN_BACKLOG > 0xff))
  #error "If you want to use TCP backlog, TCP_DEFAULT_LISTEN_BACKLOG must fit into an u8_t"
#endif
#if (LWIP_NETIF_API && (NO_SYS==1))
  #error "If you want to use NETIF API, you have to define NO_SYS=0 in your lwipopts.h"
#endif
#if ((LWIP_SOCKET || LWIP_NETCONN) && (NO_SYS==1))
  #error "If you want to use Sequential API, you have to define NO_SYS=0 in your lwipopts.h"
#endif
#if (!LWIP_NETCONN && LWIP_SOCKET)
  #error "If you want to use Socket API, you have to define LWIP_NETCONN=1 in your lwipopts.h"
#endif
#if (((!LWIP_DHCP) || (!LWIP_AUTOIP)) && LWIP_DHCP_AUTOIP_COOP)
  #error "If you want to use DHCP/AUTOIP cooperation mode, you have to define LWIP_DHCP=1 and LWIP_AUTOIP=1 in your lwipopts.h"
#endif
#if (((!LWIP_DHCP) || (!LWIP_ARP)) && DHCP_DOES_ARP_CHECK)
  #error "If you want to use DHCP ARP checking, you have to define LWIP_DHCP=1 and LWIP_ARP=1 in your lwipopts.h"
#endif
#if (!LWIP_ARP && LWIP_AUTOIP)
  #error "If you want to use AUTOIP, you have to define LWIP_ARP=1 in your lwipopts.h"
#endif
#if (LWIP_SNMP && (SNMP_CONCURRENT_REQUESTS<=0))
  #error "If you want to use SNMP, you have to define SNMP_CONCURRENT_REQUESTS>=1 in your lwipopts.h"
#endif
#if (LWIP_SNMP && (SNMP_TRAP_DESTINATIONS<=0))
  #error "If you want to use SNMP, you have to define SNMP_TRAP_DESTINATIONS>=1 in your lwipopts.h"
#endif
#if (LWIP_TCP && ((LWIP_EVENT_API && LWIP_CALLBACK_API) || (!LWIP_EVENT_API && !LWIP_CALLBACK_API)))
  #error "One and exactly one of LWIP_EVENT_API and LWIP_CALLBACK_API has to be enabled in your lwipopts.h"
#endif
#if (MEM_LIBC_MALLOC && MEM_USE_POOLS)
  #error "MEM_LIBC_MALLOC and MEM_USE_POOLS may not both be simultaneously enabled in your lwipopts.h"
#endif
#if (MEM_USE_POOLS && !MEMP_USE_CUSTOM_POOLS)
  #error "MEM_USE_POOLS requires custom pools (MEMP_USE_CUSTOM_POOLS) to be enabled in your lwipopts.h"
#endif
#if (PBUF_POOL_BUFSIZE <= MEM_ALIGNMENT)
  #error "PBUF_POOL_BUFSIZE must be greater than MEM_ALIGNMENT or the offset may take the full first pbuf"
#endif
#if (TCP_QUEUE_OOSEQ && !LWIP_TCP)
  #error "TCP_QUEUE_OOSEQ requires LWIP_TCP"
#endif
#if (DNS_LOCAL_HOSTLIST && !DNS_LOCAL_HOSTLIST_IS_DYNAMIC && !(defined(DNS_LOCAL_HOSTLIST_INIT)))
  #error "you have to define define DNS_LOCAL_HOSTLIST_INIT {{'host1', 0x123}, {'host2', 0x234}} to initialize DNS_LOCAL_HOSTLIST"
#endif
#if PPP_SUPPORT && !PPPOS_SUPPORT & !PPPOE_SUPPORT
  #error "PPP_SUPPORT needs either PPPOS_SUPPORT or PPPOE_SUPPORT turned on"
#endif
#if !LWIP_ETHERNET && (LWIP_ARP || PPPOE_SUPPORT)
  #error "LWIP_ETHERNET needs to be turned on for LWIP_ARP or PPPOE_SUPPORT"
#endif
#if LWIP_IGMP && !defined(LWIP_RAND)
  #error "When using IGMP, LWIP_RAND() needs to be defined to a random-function returning an u32_t random value"
#endif
#if LWIP_TCPIP_CORE_LOCKING_INPUT && !LWIP_TCPIP_CORE_LOCKING
  #error "When using LWIP_TCPIP_CORE_LOCKING_INPUT, LWIP_TCPIP_CORE_LOCKING must be enabled, too"
#endif
#if LWIP_TCP && LWIP_NETIF_TX_SINGLE_PBUF && !TCP_OVERSIZE
  #error "LWIP_NETIF_TX_SINGLE_PBUF needs TCP_OVERSIZE enabled to create single-pbuf TCP packets"
#endif
#if IP_FRAG && IP_FRAG_USES_STATIC_BUF && LWIP_NETIF_TX_SINGLE_PBUF
  #error "LWIP_NETIF_TX_SINGLE_PBUF does not work with IP_FRAG_USES_STATIC_BUF==1 as that creates pbuf queues"
#endif
#if LWIP_NETCONN && LWIP_TCP
#if NETCONN_COPY != TCP_WRITE_FLAG_COPY
  #error "NETCONN_COPY != TCP_WRITE_FLAG_COPY"
#endif
#if NETCONN_MORE != TCP_WRITE_FLAG_MORE
  #error "NETCONN_MORE != TCP_WRITE_FLAG_MORE"
#endif
#endif /* LWIP_NETCONN && LWIP_TCP */
#if LWIP_SOCKET
/* Check that the SO_* socket options and SOF_* lwIP-internal flags match */
#if SO_ACCEPTCONN != SOF_ACCEPTCONN
  #error "SO_ACCEPTCONN != SOF_ACCEPTCONN"
#endif
#if SO_REUSEADDR != SOF_REUSEADDR
  #error "WARNING: SO_REUSEADDR != SOF_REUSEADDR"
#endif
#if SO_KEEPALIVE != SOF_KEEPALIVE
  #error "WARNING: SO_KEEPALIVE != SOF_KEEPALIVE"
#endif
#if SO_BROADCAST != SOF_BROADCAST
  #error "WARNING: SO_BROADCAST != SOF_BROADCAST"
#endif
#if SO_LINGER != SOF_LINGER
  #error "WARNING: SO_LINGER != SOF_LINGER"
#endif
#endif /* LWIP_SOCKET */


/* Compile-time checks for deprecated options.
 */
#ifdef MEMP_NUM_TCPIP_MSG
  #error "MEMP_NUM_TCPIP_MSG option is deprecated. Remove it from your lwipopts.h."
#endif
#ifdef MEMP_NUM_API_MSG
  #error "MEMP_NUM_API_MSG option is deprecated. Remove it from your lwipopts.h."
#endif
#ifdef TCP_REXMIT_DEBUG
  #error "TCP_REXMIT_DEBUG option is deprecated. Remove it from your lwipopts.h."
#endif
#ifdef RAW_STATS
  #error "RAW_STATS option is deprecated. Remove it from your lwipopts.h."
#endif
#ifdef ETHARP_QUEUE_FIRST
  #error "ETHARP_QUEUE_FIRST option is deprecated. Remove it from your lwipopts.h."
#endif
#ifdef ETHARP_ALWAYS_INSERT
  #error "ETHARP_ALWAYS_INSERT option is deprecated. Remove it from your lwipopts.h."
#endif

#ifndef LWIP_DISABLE_TCP_SANITY_CHECKS
#define LWIP_DISABLE_TCP_SANITY_CHECKS  1
#endif
#ifndef LWIP_DISABLE_MEMP_SANITY_CHECKS
#define LWIP_DISABLE_MEMP_SANITY_CHECKS 0
#endif

/* MEMP sanity checks */
#if !LWIP_DISABLE_MEMP_SANITY_CHECKS
#if LWIP_NETCONN
#if MEMP_MEM_MALLOC
#if !MEMP_NUM_NETCONN && LWIP_SOCKET
#error "lwip_sanity_check: WARNING: MEMP_NUM_NETCONN cannot be 0 when using sockets!"
#endif
#else /* MEMP_MEM_MALLOC */
#if MEMP_NUM_NETCONN > (MEMP_NUM_TCP_PCB+MEMP_NUM_TCP_PCB_LISTEN+MEMP_NUM_UDP_PCB+MEMP_NUM_RAW_PCB)
#error "lwip_sanity_check: WARNING: MEMP_NUM_NETCONN should be less than the sum of MEMP_NUM_{TCP,RAW,UDP}_PCB+MEMP_NUM_TCP_PCB_LISTEN. If you know what you are doing, define LWIP_DISABLE_MEMP_SANITY_CHECKS to 1 to disable this error."
#endif
#endif /* MEMP_MEM_MALLOC */
#endif /* LWIP_NETCONN */
#endif /* !LWIP_DISABLE_MEMP_SANITY_CHECKS */

/* TCP sanity checks */
#if !LWIP_DISABLE_TCP_SANITY_CHECKS
#if LWIP_TCP
#if !MEMP_MEM_MALLOC && (MEMP_NUM_TCP_SEG < TCP_SND_QUEUELEN)
  #error "lwip_sanity_check: WARNING: MEMP_NUM_TCP_SEG should be at least as big as TCP_SND_QUEUELEN. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if TCP_SND_BUF < (2 * TCP_MSS)
  #error "lwip_sanity_check: WARNING: TCP_SND_BUF must be at least as much as (2 * TCP_MSS) for things to work smoothly. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if TCP_SND_QUEUELEN < (2 * (TCP_SND_BUF / TCP_MSS))
  #error "lwip_sanity_check: WARNING: TCP_SND_QUEUELEN must be at least as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if TCP_SNDLOWAT >= TCP_SND_BUF
  #error "lwip_sanity_check: WARNING: TCP_SNDLOWAT must be less than TCP_SND_BUF. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if TCP_SNDQUEUELOWAT >= TCP_SND_QUEUELEN
  #error "lwip_sanity_check: WARNING: TCP_SNDQUEUELOWAT must be less than TCP_SND_QUEUELEN. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if !MEMP_MEM_MALLOC && (TCP_WND > (PBUF_POOL_SIZE * (PBUF_POOL_BUFSIZE - (PBUF_LINK_HLEN + PBUF_IP_HLEN + PBUF_TRANSPORT_HLEN))))
  #error "lwip_sanity_check: WARNING: TCP_WND is larger than space provided by PBUF_POOL_SIZE * (PBUF_POOL_BUFSIZE - protocol headers). If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#if TCP_WND < TCP_MSS
  #error "lwip_sanity_check: WARNING: TCP_WND is smaller than MSS. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."
#endif
#endif /* LWIP_TCP */
#endif /* !LWIP_DISABLE_TCP_SANITY_CHECKS */


#if !NO_SYS
void tcpip_init_done(void *p)
{
	*((unsigned int *)p) = 1;
}

void
lwip_sock_init()
{
    volatile unsigned tcpip_init_done_signal = 0;

    tcpip_init(tcpip_init_done, (void *)&tcpip_init_done_signal);

    /* wait until tcpip initialization is complete */
    while (!tcpip_init_done_signal)
        ;
}
#endif
/**
 * Perform Sanity check of user-configurable values, and initialize all modules.
 */


void
lwip_init(void)
{
  /* Modules initialization */
  stats_init();
#if !NO_SYS
  sys_init();
#endif /* !NO_SYS */
  mem_init();
  memp_init();
  pbuf_init();
  netif_init();
#if LWIP_SOCKET
  lwip_socket_init();
#endif /* LWIP_SOCKET */
  ip_init();
#if LWIP_ARP
  etharp_init();
#endif /* LWIP_ARP */
#if LWIP_RAW
  raw_init();
#endif /* LWIP_RAW */
#if LWIP_UDP
  udp_init();
#endif /* LWIP_UDP */
#if LWIP_TCP
  tcp_init();
#endif /* LWIP_TCP */
#if LWIP_SNMP
  snmp_init();
#endif /* LWIP_SNMP */
#if LWIP_AUTOIP
  autoip_init();
#endif /* LWIP_AUTOIP */
#if LWIP_IGMP
  igmp_init();
#endif /* LWIP_IGMP */
#if LWIP_DNS
  dns_init();
#endif /* LWIP_DNS */

#if LWIP_TIMERS
  sys_timeouts_init();
#endif /* LWIP_TIMERS */

#if !NO_SYS
  /* in the Xilinx lwIP 1.2.0 port, lwip_init() was added as a convenience utility function
     to initialize all the lwIP layers. lwIP 1.3.0 introduced lwip_init() in the base lwIP
     itself. However a user cannot use lwip_init() regardless of whether it is raw or socket
     modes. The following call to lwip_sock_init() is made to make sure that lwIP is properly
     initialized in both raw & socket modes with just a call to lwip_init().
   */
  lwip_sock_init();
#endif
}
