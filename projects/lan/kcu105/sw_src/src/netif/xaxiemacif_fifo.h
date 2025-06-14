#ifndef __XAXIEMACIF_FIFO_H_
#define __XAXIEMACIF_FIFO_H_

#include "xparameters.h"
#include "xaxiemacif.h"
#include "../lwip/xlwipconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

XStatus init_axi_fifo(struct xemac_s *xemac);
XStatus axififo_send(xaxiemacif_s *xaxiemacif, struct pbuf *p);

#ifdef __cplusplus
}
#endif

#endif
