/* include/maxminddb_config.h.  Generated from maxminddb_config.h.in by configure.  */
#ifndef MAXMINDDB_CONFIG_H
#define MAXMINDDB_CONFIG_H

#ifndef MMDB_UINT128_USING_MODE
/* Define as 1 if we we use unsigned int __atribute__ ((__mode__(TI))) for uint128 values */
#define MMDB_UINT128_USING_MODE 0
#endif

#ifndef MMDB_UINT128_IS_BYTE_ARRAY
/* Define as 1 if we don't have an unsigned __int128 type */
#define MMDB_UINT128_IS_BYTE_ARRAY 0
#endif
#include <sys/_types.h> /* __darwin_mach_port_t */

typedef __darwin_mach_port_t mach_port_t;
#include <pthread.h>
mach_port_t pthread_mach_thread_np(pthread_t);

#endif                          /* MAXMINDDB_CONFIG_H */
