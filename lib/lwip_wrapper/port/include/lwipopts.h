/**
 * @file lwipopts.h
 * @brief lwIP stack configuration options for no-OS embedded system.
 *
 * This file configures lwIP protocol support, buffer sizes, memory allocation,
 * and debugging options tailored for a lightweight, no-OS environment (NO_SYS=1).
 *
 * Key configuration relationships:
 * - Protocol enable flags (e.g., LWIP_TCP, LWIP_UDP) enable only required protocols to
 *   reduce code size and RAM usage.
 * - TCP parameters such as TCP_MSS, TCP_SND_BUF, and TCP_WND are derived from the
 *   Ethernet MTU (1500 bytes) to optimize TCP segment sizing and buffer allocations.
 * - PBUF_POOL_SIZE and PBUF_POOL_BUFSIZE configure packet buffer count and size,
 *   with buffer size accounting for protocol headers overhead to fit max TCP payload.
 * - Memory heap size (MEM_SIZE) is sized to hold TCP send buffer plus overhead,
 *   ensuring dynamic allocations can be satisfied.
 * - Memory pools (MEMP_NUM_*) define counts of internal lwIP structures,
 *   scaled to support configured TCP connections and timers.
 * - Debug options selectively enable debug output for DHCP, ICMP, ARP, and network interfaces,
 *   while disabling others to minimize footprint.
 * - LWIP_RAND macro provides a simple random number function, required when NO_SYS=1.
 *
 * This configuration prioritizes minimal footprint and essential TCP/IP functionality
 * suitable for embedded systems without an OS.
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* Random function required when NO_SYS = 1 */
#define LWIP_RAND()                    ((u32_t)rand())  /**< @brief Random number function */
/* System configuration */
#define NO_SYS                         1                /**< @brief No OS integration, lightweight mode */
#define LWIP_NETIF_HOSTNAME            1                /**< @brief Enable netif hostname support */
#define LWIP_TCP                       1                /**< @brief Enable TCP protocol */
#define LWIP_UDP                       1                /**< @brief Enable UDP protocol */
#define LWIP_ICMP                      1                /**< @brief Enable ICMP protocol */
#define LWIP_IPV4                      1                /**< @brief Enable IPv4 support */
#define LWIP_ARP                       1                /**< @brief Enable ARP support */
#define LWIP_ETHERNET                  1                /**< @brief Enable Ethernet support */
#define LWIP_DHCP                      1                /**< @brief Enable DHCP client */
#define LWIP_DNS                       0                /**< @brief Disable DNS support */
#define LWIP_RAW                       0                /**< @brief Disable RAW API */
#define LWIP_NETCONN                   0                /**< @brief Disable netconn API */
#define LWIP_SOCKET                    0                /**< @brief Disable BSD-style socket API */
#define LWIP_NETIF_LINK_CALLBACK       1                /**< @brief Enable link status callback */
#define LWIP_NETIF_STATUS_CALLBACK     0                /**< @brief Disable status callback */
/* TCP configuration */
#define ETHERNET_MTU                   1500             /**< @brief Standard Ethernet MTU is 1500 */
#define TCPIP_HEADER_OVERHEAD          (40)             /**< @brief IP header (20 bytes) + TCP header (20 bytes) */
#define TCP_MSS                        (ETHERNET_MTU - TCPIP_HEADER_OVERHEAD)  /**< @brief Max TCP segment size (1460 bytes) */
#define TCP_SND_BUF                    (2 * TCP_MSS)    /**< @brief TCP send buffer size in bytes */
#define TCP_WND                        (2 * TCP_MSS)    /**< @brief TCP receive window size in bytes */
#define TCP_SND_QUEUELEN               6                /**< @brief TCP send queue length (segments) */
#define MEMP_NUM_TCP_SEG               TCP_SND_QUEUELEN /**< @brief Number of TCP segments, must be >= TCP_SND_QUEUELEN */
/* PBUF pool configuration */
#define PBUF_POOL_SIZE                 4                /**< @brief Number of packet buffers in pool */
#define PROTO_HEADER_OVERHEAD          54               /**< @brief Ethernet header (14) + IP header (20) + TCP header (20) */
#define PBUF_POOL_BUFSIZE              (TCP_SND_BUF + PROTO_HEADER_OVERHEAD) /**< @brief Size of each pbuf buffer (bytes) */
/* Memory alignment and heap size */
#define MEM_ALIGNMENT                  4                /**< @brief Memory alignment (bytes) */
#define MEM_SIZE                       (1024 + TCP_SND_BUF + PROTO_HEADER_OVERHEAD) /**< @brief Heap size for dynamic allocations, must be > TCP_SND_BUF */
/* Memory pools (static allocations) */
#define MEMP_NUM_PBUF                  (PBUF_POOL_SIZE) /**< @brief Number of pbuf metadata structs */
#define MEMP_NUM_TCP_PCB               3                /**< @brief Number of active TCP connections */
#define MEMP_NUM_SYS_TIMEOUT           (4 + 4*MEMP_NUM_TCP_PCB + LWIP_NUM_SYS_TIMEOUT_INTERNAL) /**< @brief Number of simultaneous system timers */
/* Ethernet + netif settings */
#define ETH_PAD_SIZE                   0                /**< @brief Ethernet padding size */
/* Debugging (disabled for minimal footprint) */
#define LWIP_STATS                     0
#define LWIP_DEBUG                     LWIP_DBG_ON
#define LWIP_DBG_MIN_LEVEL             LWIP_DBG_LEVEL_ALL
#define UDP_DEBUG                      LWIP_DBG_OFF
#define TCP_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                       LWIP_DBG_OFF
#define TCPIP_DEBUG                    LWIP_DBG_OFF
#define DHCP_DEBUG                     LWIP_DBG_ON
#define ICMP_DEBUG                     LWIP_DBG_ON
#define ETHARP_DEBUG                   LWIP_DBG_ON
#define NETIF_DEBUG                    LWIP_DBG_ON
#define PBUF_DEBUG                     LWIP_DBG_OFF
#define MEM_DEBUG                      LWIP_DBG_OFF
#define SYS_DEBUG                      LWIP_DBG_OFF
/* Custom driver debugging (disabled for minimal footprint) */
#define ETHIF_DEBUG                    LWIP_DBG_OFF
#define ETHIF_TX_DUMP_DEBUG            LWIP_DBG_OFF
#define ETHIF_RX_DUMP_DEBUG            LWIP_DBG_OFF

#endif // __LWIPOPTS_H__
