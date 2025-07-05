#ifndef __W5500_ETHIF_H__
#define __W5500_ETHIF_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/etharp.h"
#include "lwip/netif.h"
#include "lwip/debug.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ethif_driver;

/**
 * @struct ethif
 * @brief Holds private state and function pointers for the Ethernet interface.
 */
struct ethif {
  void * spi;                       /**< SPI handle or context */
  void (*begin)(void *);            /**< Function to select SPI slave (assert CS low) */
  void (*end)(void *);              /**< Function to deselect SPI slave (deassert CS high) */
  uint8_t (*txn)(void *, uint8_t);  /**< Function to transmit a byte over SPI and receive a response */
  struct eth_addr *ethaddr;         /**< MAC address pointer */
  struct ethif_driver * driver;     /**< Pointer to driver-specific context */
};

/**
 * @struct ethif_driver
 * @brief Driver function pointers for initializing and handling Ethernet frames.
 */
struct ethif_driver {
  bool (*init)(struct ethif *);                         /**< Initialize the driver */
  size_t (*tx)(const void *, size_t, struct ethif *);   /**< Transmit Ethernet frame */
  size_t (*rx)(void *buf, size_t len, struct ethif *);  /**< Receive Ethernet frame */
  bool (*poll)(struct ethif *, bool);                   /**< Poll link status, return up/down */
};

/**
 * @brief Maximum number of loop iterations for wait operations.
 */
#define MAX_LOOP_ITERATIONS 1000

/**
 * @brief Wait for a condition or fail after a number of iterations.
 *
 * @param max_iter Maximum number of iterations.
 * @param condition Expression to evaluate.
 * @param result Result boolean; true if condition ended before max_iter.
 */
#define WAIT_OR_FAIL(max_iter, condition, result)           \
    do {                                                    \
        result = true;                                      \
        int __count = 0;                                    \
        while (condition) {                                 \
            if (++__count > (max_iter)) {                   \
                result = false;                             \
                break;                                      \
            }                                               \
        }                                                   \
    } while(0)

/**
 * @brief Initialize the Ethernet interface with lwIP.
 *
 * @param netif Pointer to lwIP network interface structure.
 * @return ERR_OK on success, or lwIP error code.
 */
err_t ethif_init(struct netif *netif);

/**
 * @brief Poll the Ethernet interface for received frames and link status.
 *
 * @param netif Pointer to lwIP network interface structure.
 */
void ethif_poll(struct netif *netif);

/**
 * @brief W5500 Ethernet driver instance.
 */
extern struct ethif_driver ethif_driver_w5500;

#ifdef __cplusplus
}
#endif

#endif // __W5500_ETHIF_H__