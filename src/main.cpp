#include <Arduino.h>
#include <SPI.h>

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"

#include "ethif.h"

#define USE_STATIC_IP 0            /**< @brief Set to 1 for static IP, 0 for DHCP */

const int BUILTIN_LED_PIN = 13;    /**< @brief Built-in LED pin number */
const int LED1_PIN = 11;           /**< @brief External LED1 pin */
const int LED2_PIN = 12;           /**< @brief External LED2 pin */
const int W5500_CS_PIN = 7;        /**< @brief W5500 chip select pin */

static struct netif netif;         /**< @brief Network interface structure */

/**
 * @brief Ethernet interface driver instance for W5500 with SPI callbacks.
 */
static struct ethif ethif_w5500 = {
    NULL, /* SPI handler/context not used*/
    [](void *) { digitalWrite(W5500_CS_PIN, LOW); SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); },
    [](void *) { digitalWrite(W5500_CS_PIN, HIGH); SPI.endTransaction(); },
    [](void *, uint8_t c) { return SPI.transfer(c); },
    (struct eth_addr *)&netif.hwaddr,
    &ethif_driver_w5500
};

static struct tcp_pcb *http_pcb;   /**< @brief TCP protocol control block for HTTP server */
static uint32_t view_counter = 0;  /**< @brief Counter for root HTTP GET requests */

bool dhcp_bound = false;           /**< @brief Flag to indicate DHCP IP assignment */
bool http_server_started = false;  /**< @brief Flag to indicate HTTP server running */

/**
 * @brief Called when all sent data has been acknowledged by the client.
 *        Closes the TCP connection.
 * 
 * @param arg User argument pointer (unused)
 * @param tpcb TCP protocol control block
 * @param len Number of bytes acknowledged
 * @return err_t ERR_OK on success
 */
static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  Serial.println("All data sent, closing connection");
  tcp_close(tpcb);
  return ERR_OK;
}

/**
 * @brief Called when data is received on the TCP connection.
 *        Handles HTTP GET requests for the root path and sends the view count response.
 * 
 * @param arg User argument pointer (unused)
 * @param tpcb TCP protocol control block
 * @param p Pointer to received pbuf buffer
 * @param err Error code
 * @return err_t ERR_OK on success
 */
static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (!p) {
    Serial.println("Connection closed by client");
    tcp_close(tpcb);
    return ERR_OK;
  }

  char request[128] = {0};
  pbuf_copy_partial(p, request, sizeof(request) - 1, 0);
  Serial.printf("Received request: %s\n", request);

  if (strncmp(request, "GET / ", 6) == 0) {
    view_counter++;
    Serial.printf("HTTP request #%lu received\n", view_counter);
  } else {
    Serial.println("Non-root request, not incrementing view counter");
  }

  char response_body[64];
  snprintf(response_body, sizeof(response_body), "View Count: %lu", view_counter);

  char http_response[256];
  snprintf(http_response, sizeof(http_response),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %d\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           strlen(response_body),
           response_body);

  err_t wr_err = tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  if (wr_err != ERR_OK) {
    Serial.printf("tcp_write failed: %d\n", wr_err);
  }
  tcp_output(tpcb);

  pbuf_free(p);

  // Set callback to close connection after data is fully sent
  tcp_sent(tpcb, http_sent);

  return ERR_OK;
}

/**
 * @brief Called when a new TCP connection is accepted by the server.
 *        Sets the receive callback to handle incoming data.
 * 
 * @param arg User argument pointer (unused)
 * @param newpcb New TCP protocol control block for accepted connection
 * @param err Error code
 * @return err_t ERR_OK on success
 */
static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  Serial.println("HTTP connection accepted");
  tcp_recv(newpcb, http_recv);
  return ERR_OK;
}

/**
 * @brief Starts the HTTP server listening on port 80.
 *        Creates a new TCP PCB, binds, and listens for incoming connections.
 */
void start_http_server()
{
  http_pcb = tcp_new();
  if (!http_pcb) {
    Serial.println("Failed to create PCB");
    return;
  }

  if (tcp_bind(http_pcb, IP_ADDR_ANY, 80) != ERR_OK) {
    Serial.println("Failed to bind HTTP server");
    return;
  }

  http_pcb = tcp_listen(http_pcb);
  tcp_accept(http_pcb, http_accept);
  Serial.println("HTTP server started on port 80");
}

/**
 * @brief Callback for network interface link status changes.
 *        Starts or stops DHCP as appropriate.
 * 
 * @param netif Pointer to the network interface
 */
static void netif_link_callback(struct netif *netif)
{
  if (netif_is_link_up(netif)) {
    Serial.println("Link is UP");

#if !USE_STATIC_IP
    Serial.println("Restarting DHCP...");
    dhcp_release(netif);  // optional: free existing lease
    dhcp_stop(netif);
    dhcp_bound = false;
    dhcp_start(netif);
#endif

  } else {
    Serial.println("Link is DOWN");

#if !USE_STATIC_IP
    dhcp_stop(netif);     // stop any existing session
    dhcp_bound = false;
#endif
  }
}

/**
 * @brief Arduino setup function.
 *        Initializes serial, SPI, lwIP, network interface and callbacks.
 */
void setup() {
  Serial.begin(115200);              
  while (!Serial) delay(50);         

  SPI.begin();                       
  pinMode(W5500_CS_PIN, OUTPUT);
  digitalWrite(W5500_CS_PIN, HIGH);
  pinMode(BUILTIN_LED_PIN, OUTPUT);  

  Serial.printf("Starting, CPU freq %.2f MHz\n", (double)F_CPU / 1000000);
  
  lwip_init();

  const uint8_t mac[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
  memcpy(netif.hwaddr, mac, 6);

#if USE_STATIC_IP
  ip4_addr_t ipaddr, netmask, gw;
  IP4_ADDR(&ipaddr, 192, 168, 50, 40);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 192, 168, 50, 1);
  netif_add(&netif, &ipaddr, &netmask, &gw, &ethif_w5500, ethif_init, ethernet_input);
#else
  netif_add(&netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY, &ethif_w5500, ethif_init, ethernet_input);
#endif
  netif_set_default(&netif);
  netif_set_link_callback(&netif, netif_link_callback);
  netif_set_up(&netif);
}

/**
 * @brief Arduino main loop.
 *        Polls ethernet, checks DHCP assignment, and starts HTTP server once ready.
 */
void loop() {
  ethif_poll(&netif);
  sys_check_timeouts();

  if (!dhcp_bound && netif_is_up(&netif) && netif.ip_addr.addr != 0) {
    dhcp_bound = true;

    Serial.print("Assigned IP: ");
    Serial.println(ip4addr_ntoa(&netif.ip_addr));
    Serial.print("Netmask: ");
    Serial.println(ip4addr_ntoa(&netif.netmask));
    Serial.print("Gateway: ");
    Serial.println(ip4addr_ntoa(&netif.gw));
  }

  if (!http_server_started && dhcp_bound && netif_is_up(&netif) && netif.ip_addr.addr != 0) {
    http_server_started = true;

    Serial.println("Starting HTTP server...");
    start_http_server();
  }
}
