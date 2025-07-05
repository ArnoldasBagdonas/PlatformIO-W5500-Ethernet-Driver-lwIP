/**
 * @file
 * @brief Ethernet interface implementation using lwIP.
 *
 * Provides initialization, polling, and packet transmission logic.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#include "ethif.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'


/**
 * @brief Polls the Ethernet interface for link status and incoming packets.
 *
 * Checks link state and receives a frame if available. Uses lwIP's `netif->input`
 * function to pass packets up the stack.
 *
 * @param netif Pointer to the lwIP network interface.
 */
void ethif_poll(struct netif *netif)
{
  struct ethif *ethif = (struct ethif *)netif->state;
  struct ethif_driver *driver = (struct ethif_driver *)ethif->driver;

  bool connected = driver->poll(ethif, true);
  if (connected != netif_is_link_up(netif)) {
    if (connected) {
      LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_poll: Link is UP\n"));
      netif_set_link_up(netif);
    }
    else {
      LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_poll: Link is DOWN\n"));
      netif_set_link_down(netif);
    }
  }

  struct pbuf *p = pbuf_alloc(PBUF_RAW, PBUF_POOL_BUFSIZE, PBUF_POOL);
  if (p && p->next == NULL) {
    size_t len = driver->rx(p->payload, p->len, ethif);
    if (len > 0) {
      p->len = p->tot_len = len;
      LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_poll: received %u bytes\n", len));

      if (netif->input(p, netif) == ERR_OK) return;

      LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ethif_poll: netif->input() failed\n"));
    }
    pbuf_free(p);
  } else {
    if (p) {
      pbuf_free(p);  // Free if it's chained or failed
      LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("pbuf_alloc failed or chained pbuf not supported!\n"));
    }
  }
}

/**
 * @brief Transmits a packet over the network.
 *
 * Called by lwIP to send a frame. Updates SNMP and link stats.
 *
 * @param netif lwIP network interface.
 * @param p Pointer to the packet buffer (may be chained).
 * @return ERR_OK on success, otherwise lwIP error code.
 */
static err_t ethif_output(struct netif *netif, struct pbuf *p)
{
  struct ethif *ethif = (struct ethif *)netif->state;
  struct ethif_driver *driver = (struct ethif_driver *)ethif->driver;

  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_output: sending %u bytes\n", p->tot_len));

  LINK_STATS_INC(link.xmit);
  MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
  
  const uint8_t *payload = (const uint8_t *)p->payload;
  int unicast = ((payload[0] & 0x01) == 0);
  if (unicast) {
    MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
  } else {
    MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
  }
  
  sys_prot_t irq_state = sys_arch_protect();
  size_t sent = driver->tx(p->payload, p->tot_len, ethif);
  sys_arch_unprotect(irq_state);

  if (sent != p->tot_len) {
    LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
      ("ethif_output: TX failed, sent %u instead of %u\n",(unsigned int)sent, (unsigned int)p->tot_len));
    return ERR_IF;
  }

  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_output: TX successful\n"));
  return ERR_OK;
}


/**
 * @brief Initializes the Ethernet interface.
 *
 * Sets up the driver, MAC address, MTU, link output, and other lwIP settings.
 *
 * @param netif lwIP network interface structure.
 * @return ERR_OK on success, ERR_IF on driver failure, or another lwIP error.
 */
err_t ethif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

  LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_TRACE, ("ethif_init: initializing interface\n"));

#if LWIP_NETIF_HOSTNAME
  netif->hostname = "lwip";
  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_init: hostname set to '%s'\n", netif->hostname));
#endif /* LWIP_NETIF_HOSTNAME */

  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_init: interface name = %c%c\n", IFNAME0, IFNAME1));
  
#if LWIP_IPV4
  netif->output = etharp_output;
  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_init: IPv4 output set\n"));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
  LWIP_DEBUGF(NETIF_DEBUG, ("ethif_init: IPv6 output set\n"));
#endif /* LWIP_IPV6 */
  netif->linkoutput = ethif_output;

  netif->mtu        = ETHERNET_MTU;
  netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

  LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_init: MTU = %d, flags = 0x%02X\n", netif->mtu, netif->flags));

  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);

  struct ethif *ethif = (struct ethif *)netif->state;
  struct ethif_driver *driver = (struct ethif_driver *)ethif->driver;
  
  /* Note: Configure netif's MAC hardware address (netif->hwaddr) before calling ethif_init */
  ethif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  netif->hwaddr_len = sizeof(netif->hwaddr);

  bool success = driver->init(ethif);
  if (success) {
    LWIP_DEBUGF(ETHIF_DEBUG, ("ethif_init: driver initialization successful\n"));
    return ERR_OK;
  } else {
    LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ethif_init: driver initialization failed\n"));
    return ERR_IF;
  }
}
