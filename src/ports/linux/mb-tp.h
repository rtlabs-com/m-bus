#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER mb

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./mb-tp.h"

#if !defined(_MB_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _MB_TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT (mb, t1p5, TP_ARGS(), TP_FIELDS())

TRACEPOINT_EVENT (mb, t3p5, TP_ARGS(), TP_FIELDS())

TRACEPOINT_EVENT (mb, rx_hook, TP_ARGS(), TP_FIELDS())

TRACEPOINT_EVENT (
   mb,
   rx_read,
   TP_ARGS (int, size),
   TP_FIELDS (ctf_integer (int, size, size)))

TRACEPOINT_EVENT (
   mb,
   rx_trace,
   TP_ARGS (int, id),
   TP_FIELDS (ctf_integer (int, id, id)))

TRACEPOINT_EVENT (
   mb,
   tx_trace,
   TP_ARGS (int, id),
   TP_FIELDS (ctf_integer (int, id, id)))

#endif /* _MB_TP_H */

#include <lttng/tracepoint-event.h>
