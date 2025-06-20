///*
// * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
// * All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without modification,
// * are permitted provided that the following conditions are met:
// *
// * 1. Redistributions of source code must retain the above copyright notice,
// *    this list of conditions and the following disclaimer.
// * 2. Redistributions in binary form must reproduce the above copyright notice,
// *    this list of conditions and the following disclaimer in the documentation
// *    and/or other materials provided with the distribution.
// * 3. The name of the author may not be used to endorse or promote products
// *    derived from this software without specific prior written permission.
// *
// * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// * OF SUCH DAMAGE.
// *
// * This file is part of the lwIP TCP/IP stack.
// *
// * Author: Adam Dunkels <adam@sics.se>
// *
// */
//#ifndef __LWIP_ICMP_H__
//#define __LWIP_ICMP_H__
//
//#include "../opt.h"
//
//#if LWIP_ICMP /* don't build if not configured for use in lwipopts.h */
//
//#include "../pbuf.h"
//#include "../netif.h"
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#define ICMP6_DUR  1
//#define ICMP6_TE   3
//#define ICMP6_ECHO 128    /* echo */
//#define ICMP6_ER   129      /* echo reply */
//
//
//enum icmp_dur_type {
//  ICMP_DUR_NET = 0,    /* net unreachable */
//  ICMP_DUR_HOST = 1,   /* host unreachable */
//  ICMP_DUR_PROTO = 2,  /* protocol unreachable */
//  ICMP_DUR_PORT = 3,   /* port unreachable */
//  ICMP_DUR_FRAG = 4,   /* fragmentation needed and DF set */
//  ICMP_DUR_SR = 5      /* source route failed */
//};
//
//enum icmp_te_type {
//  ICMP_TE_TTL = 0,     /* time to live exceeded in transit */
//  ICMP_TE_FRAG = 1     /* fragment reassembly time exceeded */
//};
//
//void icmp_input(struct pbuf *p, struct netif *inp);
//
//void icmp_dest_unreach(struct pbuf *p, enum icmp_dur_type t);
//void icmp_time_exceeded(struct pbuf *p, enum icmp_te_type t);
//
//struct icmp_echo_hdr {
//  u8_t type;
//  u8_t icode;
//  u16_t chksum;
//  u16_t id;
//  u16_t seqno;
//};
//
//struct icmp_dur_hdr {
//  u8_t type;
//  u8_t icode;
//  u16_t chksum;
//  u32_t unused;
//};
//
//struct icmp_te_hdr {
//  u8_t type;
//  u8_t icode;
//  u16_t chksum;
//  u32_t unused;
//};
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* LWIP_ICMP */
//
//#endif /* __LWIP_ICMP_H__ */
