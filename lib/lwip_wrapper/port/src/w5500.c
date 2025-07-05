/**
 * @file
 * @brief W5500 driver low-level SPI interface and register definitions.
 */

#include "ethif.h"

/**
 * @brief W5500 Register Block Selectors
 *
 * Used to select different memory/register blocks.
 */
enum
{
    COMMON_REGISTER = 0,   /**< Common register block */
    SOCKET0_REGISTER = 1,  /**< Socket 0 register block */
    SOCKET0_TX_BUFFER = 2, /**< Socket 0 TX buffer */
    SOCKET0_RX_BUFFER = 3  /**< Socket 0 RX buffer */
};

/**
 * @brief Common Register Addresses
 *
 * These addresses apply to the W5500's common register space.
 */
enum
{
    MR = 0x0000,       /**< Mode Register (R/W) */
    SHAR = 0x0009,     /**< Source Hardware (MAC) Address Register (R/W) */
    INTLEVEL = 0x0013, /**< Interrupt Low-Level Timer Register (R/W) */
    IR = 0x0015,       /**< Interrupt Register (R/W) */
    _IMR_ = 0x0016,    /**< Interrupt Mask Register (R/W) */
    SIR = 0x0017,      /**< Socket Interrupt Register (R/W) */
    SIMR = 0x0018,     /**< Socket Interrupt Mask Register (R/W) */
    _RTR_ = 0x0019,    /**< Retry Timeout Register (R/W), unit = 100us */
    _RCR_ = 0x001B,    /**< Retry Count Register (R/W) */
    UIPR = 0x0028,     /**< Unreachable IP Register in UDP Mode (Read-only) */
    UPORTR = 0x002C,   /**< Unreachable Port Register in UDP Mode (Read-only) */
    PHYCFGR = 0x002E,  /**< PHY Configuration Register (R/W) */
    VERSIONR = 0x0039  /**< Chip Version Register (Read-only) */
};

/**
 * @brief Socket Register Addresses
 *
 * These apply to a socket's dedicated register space.
 */
enum
{
    Sn_MR = 0x0000,         /**< Socket Mode Register (R/W) */
    Sn_CR = 0x0001,         /**< Socket Command Register (R/W) */
    Sn_IR = 0x0002,         /**< Socket Interrupt Register (R) */
    Sn_SR = 0x0003,         /**< Socket Status Register (R) */
    Sn_PORT = 0x0004,       /**< Source Port Register (R/W) */
    Sn_DHAR = 0x0006,       /**< Destination MAC Register (R/W) */
    Sn_DIPR = 0x000C,       /**< Destination IP Register (R/W) */
    Sn_DPORT = 0x0010,      /**< Destination Port Register (R/W) */
    Sn_MSSR = 0x0012,       /**< Maximum Segment Size Register (R/W) */
    Sn_TOS = 0x0015,        /**< Type of Service Register (R/W) */
    Sn_TTL = 0x0016,        /**< Time To Live Register (R/W) */
    Sn_RXBUF_SIZE = 0x001E, /**< RX Buffer Size Register (R/W) */
    Sn_TXBUF_SIZE = 0x001F, /**< TX Buffer Size Register (R/W) */
    Sn_TX_FSR = 0x0020,     /**< TX Free Size Register (R) */
    Sn_TX_RD = 0x0022,      /**< TX Read Pointer Register (R) */
    Sn_TX_WR = 0x0024,      /**< TX Write Pointer Register (R/W) */
    Sn_RX_RSR = 0x0026,     /**< RX Received Size Register (R) */
    Sn_RX_RD = 0x0028,      /**< RX Read Pointer Register (R/W) */
    Sn_RX_WR = 0x002A,      /**< RX Write Pointer Register (R) */
    Sn_IMR = 0x002C,        /**< Socket Interrupt Mask Register (R) */
    Sn_FRAG = 0x002D,       /**< Fragment Field Register (R/W) */
    Sn_KPALVTR = 0x002F     /**< Keep-Alive Timer Register (R/W) */
};

/**
 * @brief Mode Register Values
 */
enum
{
    MR_RST = 0x80,   /**< Software reset */
    MR_WOL = 0x20,   /**< Wake on LAN */
    MR_PB = 0x10,    /**< Ping block */
    MR_PPPOE = 0x08, /**< Enable PPPoE mode */
    MR_FARP = 0x02   /**< Force ARP in UDP */
};

/**
 * @brief Interrupt Register Bit Definitions
 */
enum
{
    IR_CONFLICT = 0x80, /**< IP address conflict detected */
    IR_UNREACH = 0x40,  /**< Destination unreachable (UDP) */
    IR_PPPoE = 0x20,    /**< PPPoE close received */
    IR_MP = 0x10        /**< Magic packet received (Wake-on-LAN) */
};

/**
 * @brief Interrupt Mask Register Bits
 */
enum
{
    IM_IR7 = 0x80, /**< Mask: IP conflict */
    IM_IR6 = 0x40, /**< Mask: Unreachable destination */
    IM_IR5 = 0x20, /**< Mask: PPPoE close */
    IM_IR4 = 0x10  /**< Mask: Magic packet */
};

/**
 * @brief Socket Mode Register Values (Sn_MR)
 */
enum
{
    Sn_MR_CLOSE = 0x00,  /**< Socket not used */
    Sn_MR_TCP = 0x01,    /**< TCP mode */
    Sn_MR_UDP = 0x02,    /**< UDP mode */
    Sn_MR_MACRAW = 0x04, /**< MAC RAW mode */
    Sn_MR_UCASTB = 0x10, /**< Block unicast in multicast mode */
    Sn_MR_ND = 0x20,     /**< No delay ACK / Multicast flag */
    Sn_MR_BCASTB = 0x40, /**< Block broadcast in multicast mode */
    Sn_MR_MULTI = 0x80,  /**< Enable multicast */
    Sn_MR_MIP6B = 0x10,  /**< Block IPv6 packets in MACRAW */
    Sn_MR_MMB = 0x20,    /**< Block multicast in MACRAW */
    Sn_MR_MFEN = 0x80    /**< Enable MAC filtering in MACRAW */
};

/**
 * @brief Socket Command Register Values (Sn_CR)
 */
enum
{
    Sn_CR_OPEN = 0x01,      /**< Open socket */
    Sn_CR_LISTEN = 0x02,    /**< Listen (TCP server) */
    Sn_CR_CONNECT = 0x04,   /**< Connect (TCP client) */
    Sn_CR_DISCON = 0x08,    /**< Disconnect (TCP) */
    Sn_CR_CLOSE = 0x10,     /**< Close socket */
    Sn_CR_SEND = 0x20,      /**< Send data */
    Sn_CR_SEND_MAC = 0x21,  /**< Send data with MAC address only */
    Sn_CR_SEND_KEEP = 0x22, /**< Send keep-alive packet */
    Sn_CR_RECV = 0x40       /**< Receive data */
};

/**
 * @brief Socket Interrupt Register Values (Sn_IR)
 */
enum
{
    Sn_IR_CON = 0x01,     /**< Connection established */
    Sn_IR_DISCON = 0x02,  /**< Disconnected */
    Sn_IR_RECV = 0x04,    /**< Data received */
    Sn_IR_TIMEOUT = 0x08, /**< Timeout occurred */
    Sn_IR_SENDOK = 0x10   /**< Send operation completed */
};

/**
 * @brief Socket Status Register Values (Sn_SR)
 */
enum
{
    SOCK_CLOSED = 0x00,      /**< Closed */
    SOCK_INIT = 0x13,        /**< Initialized */
    SOCK_LISTEN = 0x14,      /**< Listening for connections */
    SOCK_SYNSENT = 0x15,     /**< SYN sent */
    SOCK_SYNRECV = 0x16,     /**< SYN received */
    SOCK_ESTABLISHED = 0x17, /**< Connection established */
    SOCK_FIN_WAIT = 0x18,    /**< FIN wait */
    SOCK_CLOSING = 0x1A,     /**< Closing state */
    SOCK_TIME_WAIT = 0x1B,   /**< Time wait */
    SOCK_CLOSE_WAIT = 0x1C,  /**< Close wait */
    SOCK_LAST_ACK = 0x1D,    /**< Last ACK */
    SOCK_UDP = 0x22,         /**< UDP mode */
    SOCK_MACRAW = 0x42       /**< MACRAW mode */
};

/**
 * @brief PHY Configuration Register (PHYCFGR) Bits
 */
enum
{
    PHYCFGR_RST = ~(1 << 7),        /**< Software reset (clear to reset) */
    PHYCFGR_OPMD = (1 << 6),        /**< Use OPMDC for configuration */
    PHYCFGR_OPMDC_ALLA = (7 << 3),  /**< All capable auto-negotiation */
    PHYCFGR_OPMDC_PDOWN = (6 << 3), /**< Power-down mode */
    PHYCFGR_OPMDC_NA = (5 << 3),    /**< Not applicable */
    PHYCFGR_OPMDC_100FA = (4 << 3), /**< 100Mbps Full-duplex auto-negotiation */
    PHYCFGR_OPMDC_100F = (3 << 3),  /**< 100Mbps Full-duplex */
    PHYCFGR_OPMDC_100H = (2 << 3),  /**< 100Mbps Half-duplex */
    PHYCFGR_OPMDC_10F = (1 << 3),   /**< 10Mbps Full-duplex */
    PHYCFGR_OPMDC_10H = (0 << 3),   /**< 10Mbps Half-duplex */
    PHYCFGR_DPX_FULL = (1 << 2),    /**< Full-duplex */
    PHYCFGR_DPX_HALF = (0 << 2),    /**< Half-duplex */
    PHYCFGR_SPD_100 = (1 << 1),     /**< 100Mbps speed */
    PHYCFGR_SPD_10 = (0 << 1),      /**< 10Mbps speed */
    PHYCFGR_LNK_ON = (1 << 0),      /**< Link is up */
    PHYCFGR_LNK_OFF = (0 << 0)      /**< Link is down */
};

/**
 * @brief PHY State Representation Values
 */
enum
{
    PHY_SPEED_10 = 0,    /**< 10 Mbps link speed */
    PHY_SPEED_100 = 1,   /**< 100 Mbps link speed */
    PHY_DUPLEX_HALF = 0, /**< Half-duplex mode */
    PHY_DUPLEX_FULL = 1, /**< Full-duplex mode */
    PHY_LINK_OFF = 0,    /**< Link down */
    PHY_LINK_ON = 1,     /**< Link up */
    PHY_POWER_NORM = 0,  /**< Normal power mode */
    PHY_POWER_DOWN = 1   /**< Power-down mode */
};

/**
 * @brief Perform SPI I/O to read or write W5500 registers.
 *
 * @param s Pointer to Ethernet interface.
 * @param block Register block.
 * @param addr Register address.
 * @param wr Write if true; read if false.
 * @param buf Pointer to data buffer.
 * @param len Length of data.
 */
static void w5500_spi_io(struct ethif *s, uint8_t block, uint16_t addr,
                         bool wr, void *buf, size_t len)
{
    size_t i;
    uint8_t *p = (uint8_t *)buf;
    uint8_t cmd[] = {
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 255),
        (uint8_t)((block << 3) | (wr ? 4 : 0))};

    s->begin(s->spi);

    for (i = 0; i < sizeof(cmd); i++)
        s->txn(s->spi, cmd[i]);

    for (i = 0; i < len; i++)
    {
        uint8_t r = s->txn(s->spi, p[i]);
        if (!wr)
            p[i] = r;
    }

    s->end(s->spi);
}

// clang-format off

/**
 * @brief Write a buffer to W5500.
 */
static  void w5500_write(struct ethif *s, uint8_t block, uint16_t addr, void *buf, size_t len) { w5500_spi_io(s, block, addr, true, buf, len); }

/**
 * @brief Write a byte to W5500.
 */
static  void w5500_write_byte(struct ethif *s, uint8_t block, uint16_t addr, uint8_t val) { w5500_write(s, block, addr, &val, 1); }

/**
 * @brief Write a word (16-bit) to W5500.
 */
static  void w5500_write_word(struct ethif *s, uint8_t block, uint16_t addr, uint16_t val) { uint8_t buf[2] = {(uint8_t) (val >> 8), (uint8_t) (val & 255)}; w5500_write(s, block, addr, buf, sizeof(buf)); }

/**
 * @brief Read a buffer from W5500.
 */
static  void w5500_read(struct ethif *s, uint8_t block, uint16_t addr, void *buf, size_t len) { w5500_spi_io(s, block, addr, false, buf, len); }

/**
 * @brief Read a byte from W5500.
 */
static  uint8_t w5500_read_byte(struct ethif *s, uint8_t block, uint16_t addr) { uint8_t r = 0; w5500_read(s, block, addr, &r, 1); return r; }
/**
 * @brief Read a word (16-bit) from W5500.
 */
static  uint16_t w5500_read_word(struct ethif *s, uint8_t block, uint16_t addr) { uint8_t buf[2] = {0, 0}; w5500_read(s, block, addr, buf, sizeof(buf)); return (uint16_t) ((buf[0] << 8) | buf[1]); }

// clang-format on

/**
 * @brief Read and verify a stable value from the Receive Buffer Size Register (RX_RSR).
 *
 * @param[in]  s    Pointer to the Ethernet interface structure.
 * @param[out] len  Pointer to a variable to store the stable RX buffer size.
 * @return true if the RX_RSR value is stable and stored in @p len, false otherwise.
 */
static bool w5500_read_rx_rsr_stable(struct ethif *s, uint16_t *len)
{
    uint16_t tmp = w5500_read_word(s, SOCKET0_REGISTER, Sn_RX_RSR);
    *len = w5500_read_word(s, SOCKET0_REGISTER, Sn_RX_RSR);
    return (*len == tmp);
}

/**
 * @brief Receive Ethernet frame from W5500.
 *
 * @param buf Pointer to receive buffer.
 * @param buflen Length of buffer.
 * @param s Ethernet interface structure.
 * @return Size of received payload.
 */
static size_t w5500_rx(void *buf, size_t buflen, struct ethif *s)
{
    bool passed;

    uint16_t len = 0;
    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (!w5500_read_rx_rsr_stable(s, &len)), passed);

    if (!passed)
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE,
            ("w5500_rx: Timeout waiting for stable Sn_RX_RSR\n"));
        return 0;
    }

    if (len == 0)
        return 0;

    uint16_t ptr = w5500_read_word(s, SOCKET0_REGISTER, Sn_RX_RD); // SOCKET_RX_RD
    uint8_t header[2];
    w5500_read(s, SOCKET0_RX_BUFFER, ptr, header, 2);
    uint16_t frame_len = ((uint16_t)header[0] << 8) | header[1];
    uint16_t payload_len = frame_len > 2 ? frame_len - 2 : 0;

    if (payload_len > buflen)
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE,
            ("w5500_rx: Frame too large: payload_len=%u > buflen=%u\n", payload_len, (unsigned)buflen));
        payload_len = 0;
    }
    else
    {
        LWIP_DEBUGF(ETHIF_DEBUG, ("w5500_rx: Payload received: len=%u\n", (unsigned)payload_len));
        w5500_read(s, SOCKET0_RX_BUFFER, ptr + 2, buf, payload_len);
    }

    w5500_write_word(s, SOCKET0_REGISTER, Sn_RX_RD, ptr + frame_len);
    w5500_write_byte(s, SOCKET0_REGISTER, Sn_CR, Sn_CR_RECV);

    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (w5500_read_byte(s, SOCKET0_REGISTER, Sn_CR)), passed);

    if ((!passed))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE,
            ("w5500_rx: Sn_CR not cleared after RECV command\n"));
        return 0;
    }

#if defined(ETHIF_RX_DUMP_DEBUG) && ETHIF_RX_DUMP_DEBUG
    hex_dump_lwip("w5500_rx: Packet", buf, payload_len);
#endif

    return payload_len;
}

/**
 * @brief Read and verify a stable value from the Transmit Free Size Register (TX_FSR).
 *
 * @param[in]  s          Pointer to the Ethernet interface structure.
 * @param[out] freesize   Pointer to a variable to store the stable TX free size.
 * @return true if the TX_FSR value is stable and stored in @p freesize, false otherwise.
 */
static bool w5500_read_tx_rsr_stable(struct ethif *s, uint16_t *freesize)
{
    uint16_t tmp = w5500_read_word(s, SOCKET0_REGISTER, Sn_TX_FSR);
    *freesize = w5500_read_word(s, SOCKET0_REGISTER, Sn_TX_FSR);
    return (*freesize == tmp);
}

/**
 * @brief Read and clear the W5500 interrupt register.
 *
 * @param[in]  s   Ethernet interface context.
 * @param[out] ir  Optional pointer to store the read interrupt value.
 * @return The value of the interrupt register that was read.
 */
static uint8_t w5500_read_ir_and_clear(struct ethif *s, uint8_t *ir)
{
    *ir = w5500_read_byte(s, SOCKET0_REGISTER, Sn_IR) & 0x1F;
    if (*ir)
    {
        w5500_write_byte(s, SOCKET0_REGISTER, Sn_IR, *ir);
    }
    return *ir;
}

/**
 * @brief Transmit Ethernet frame using W5500.
 *
 * @param buf Pointer to buffer to transmit.
 * @param buflen Number of bytes to send.
 * @param s Ethernet interface.
 * @return Number of bytes sent (0 on error).
 */
static size_t w5500_tx(const void *buf, size_t buflen, struct ethif *s)
{
    bool passed;

    size_t len = buflen;
    if (0 == len)
        return 0;

    uint16_t freesize = 0;
    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (!w5500_read_tx_rsr_stable(s, &freesize)), passed);

    if ((!passed))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE,
            ("w5500_tx: Timeout waiting for stable Sn_TX_FSR\n"));
        return 0;
    }

    if (freesize < buflen)
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE,
            ("w5500_tx: Not enough space: freesize=%u, buflen=%u\n", freesize, (unsigned)buflen));
        return 0;
    }

    uint8_t sock_status = w5500_read_byte(s, SOCKET0_REGISTER, Sn_SR);
    if (sock_status == SOCK_CLOSED ||
        sock_status == SOCK_CLOSING ||
        sock_status == SOCK_TIME_WAIT ||
        sock_status == SOCK_CLOSE_WAIT)
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_tx: Socket unexpectedly closed\n"));
        return 0;
    }

    uint16_t ptr = w5500_read_word(s, SOCKET0_REGISTER, Sn_TX_WR);
    w5500_write(s, SOCKET0_TX_BUFFER, ptr, (void *)buf, len);
    w5500_write_word(s, SOCKET0_REGISTER, Sn_TX_WR, ptr + len);
    w5500_write_byte(s, SOCKET0_REGISTER, Sn_CR, Sn_CR_SEND);

    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (0 != w5500_read_byte(s, SOCKET0_REGISTER, Sn_CR)), passed);

    if ((!passed))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_tx: Sn_CR not cleared after SEND\n"));
        return 0;
    }

    uint8_t ir;
    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (0 == (w5500_read_ir_and_clear(s, &ir) & ((Sn_IR_SENDOK | Sn_IR_TIMEOUT | Sn_IR_DISCON)))), passed);

    if (!passed)
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_tx: Send failed: Sn_IR=%02X\n", ir));
    }

    if (ir & (Sn_IR_TIMEOUT | Sn_IR_DISCON))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_tx: Socket unexpectedly timeouted or closed\n"));
        len = 0;
    }

    if (ir & Sn_IR_SENDOK)
    {
        LWIP_DEBUGF(ETHIF_DEBUG, ("w5500_tx: Frame sent: len=%u\n", (unsigned)buflen));
    }

#if defined(ETHIF_TX_DUMP_DEBUG) && ETHIF_TX_DUMP_DEBUG
    hex_dump_lwip("w5500_tx: Packet", buf, len);
#endif

    return len;
}

/**
 * @brief Initialize W5500 chip.
 *
 * @param s Ethernet interface.
 * @return True if initialization succeeded.
 */
static bool w5500_init(struct ethif *s)
{
    bool passed;

    LWIP_DEBUGF(ETHIF_DEBUG, ("w5500_init: Initializing W5500 chip\n"));

    s->end(s->spi);

    w5500_write_byte(s, COMMON_REGISTER, MR, MR_RST);

    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (0 != (w5500_read_byte(s, COMMON_REGISTER, MR) & MR_RST)), passed);

    if ((!passed))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_init: Timeout waiting for MR_RST to clear\n"));
        return false;
    }

    w5500_write_byte(s, COMMON_REGISTER, PHYCFGR, 0);
    w5500_write_byte(s, COMMON_REGISTER, PHYCFGR, (~PHYCFGR_RST) | PHYCFGR_OPMD | PHYCFGR_OPMDC_ALLA);
    w5500_write_byte(s, SOCKET0_REGISTER, Sn_RXBUF_SIZE, 16);
    w5500_write_byte(s, SOCKET0_REGISTER, Sn_TXBUF_SIZE, 16);
    if (NULL != s->ethaddr) {    
        w5500_write(s, COMMON_REGISTER, SHAR, s->ethaddr->addr, 6);
        w5500_write_byte(s, SOCKET0_REGISTER, Sn_MR, Sn_MR_MFEN | Sn_MR_MACRAW);
    } else {
        w5500_write_byte(s, SOCKET0_REGISTER, Sn_MR, Sn_MR_MACRAW);
    }
    w5500_write_byte(s, SOCKET0_REGISTER, Sn_CR, Sn_CR_OPEN);

    WAIT_OR_FAIL(MAX_LOOP_ITERATIONS, (0 != w5500_read_byte(s, SOCKET0_REGISTER, Sn_CR)), passed);

    if ((!passed))
    {
        LWIP_DEBUGF(ETHIF_DEBUG | LWIP_DBG_LEVEL_SEVERE, ("w5500_init: Timeout waiting for Sn_CR to clear\n"));
        return false;
    }

    return w5500_read_byte(s, SOCKET0_REGISTER, Sn_SR) == SOCK_MACRAW;
}

/**
 * @brief Check link status.
 *
 * @param s Ethernet interface.
 * @param s1 Whether to check (true) or skip (false).
 * @return True if link is up.
 */
static bool w5500_poll(struct ethif *s, bool s1)
{
    return s1 ? w5500_read_byte(s, COMMON_REGISTER, PHYCFGR) & PHYCFGR_LNK_ON : false;
}

/**
 * @brief Ethernet driver structure for W5500.
 */
struct ethif_driver ethif_driver_w5500 = {
    w5500_init,
    w5500_tx,
    w5500_rx,
    w5500_poll};
