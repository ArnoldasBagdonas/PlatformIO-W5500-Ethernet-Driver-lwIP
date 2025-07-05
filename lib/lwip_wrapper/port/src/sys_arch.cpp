/**
 * @file
 * @brief System architecture abstraction for lwIP on Arduino (AVR and ARM Cortex-M).
 *
 * Provides critical section protection, debug output, assertions, timing,
 * and utility functions required by lwIP.
 */

#include <Arduino.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/arch.h"

#if defined(__AVR__)
/**
 * @brief Enter a critical section by disabling interrupts on AVR.
 *
 * Saves the current interrupt state and disables interrupts.
 *
 * @return The previous interrupt state to be restored later.
 */
extern "C" sys_prot_t sys_arch_protect(void) {
    uint8_t state = SREG;
    cli();
    return (sys_prot_t)state;
}

/**
 * @brief Exit a critical section by restoring interrupts on AVR.
 *
 * Restores the saved interrupt state.
 *
 * @param state The interrupt state to restore.
 */
extern "C" void sys_arch_unprotect(sys_prot_t state) {
    SREG = (uint8_t)state;
}

#elif defined(__arm__) || defined(__ARM_ARCH) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

/**
 * @brief Enter a critical section by disabling interrupts on ARM Cortex-M.
 *
 * Saves the current PRIMASK state and disables interrupts.
 *
 * @return The previous PRIMASK state to be restored later.
 */
extern "C" sys_prot_t sys_arch_protect(void) {
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    return (sys_prot_t)state;
}

/**
 * @brief Exit a critical section by restoring interrupts on ARM Cortex-M.
 *
 * Restores the saved PRIMASK state.
 *
 * @param state The PRIMASK state to restore.
 */
extern "C" void sys_arch_unprotect(sys_prot_t state) {
    __set_PRIMASK((uint32_t)state);
}

#else
#error "Unsupported platform. Only AVR and ARM Cortex-M supported."
#endif

#if defined(LWIP_DEBUG) && LWIP_DEBUG

/**
 * @brief Prints a debug message prefixed by [lwip].
 *
 * @param msg Null-terminated message string.
 */
extern "C" void lwip_debug_print(const char* msg) {
  Serial.print("[lwip] ");
  Serial.print(msg);
}

/**
 * @brief Prints a formatted debug message prefixed by [lwip].
 *
 * @param fmt Format string (printf-style).
 * @param ... Format arguments.
 */
extern "C" void lwip_debug_printf(const char* fmt, ...) {
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  Serial.print("[lwip] ");
  Serial.print(buf);
}

/**
 * @brief Prints a formatted message without prefix.
 *
 * Used mainly for hex dumps and raw data output.
 *
 * @param fmt Format string (printf-style).
 * @param ... Format arguments.
 */
extern "C" void lwip_dump_printf(const char* fmt, ...) {
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  Serial.print(buf);
}

/**
 * @brief Handles lwIP assertion failures.
 *
 * Prints assertion message, source file, and line number, then halts execution.
 *
 * @param msg Assertion message.
 * @param file Source file name.
 * @param line Line number in source file.
 */
extern "C" void lwip_assert(const char* msg, const char* file, int line) {
  Serial.print("ASSERT: ");
  Serial.print(msg);
  Serial.print(" at ");
  Serial.print(file);
  Serial.print(":");
  Serial.println(line);
  while (1);  // Halt
}

/**
 * @brief Dumps a block of memory in hex format.
 *
 * Prints a label, length, and hex dump of the data buffer.
 *
 * @param label Description label for the data.
 * @param data Pointer to the data buffer.
 * @param len Length of the data buffer in bytes.
 */
extern "C" void hex_dump_lwip(const char *label, const void *data, size_t len) {
  const uint8_t *d = (const uint8_t *)data;
  lwip_dump_printf("%s (%u bytes):\n", label, (unsigned int)len);
  for (size_t i = 0; i < len; i++) {
    if (i % 16 == 0) lwip_dump_printf("%04x: ", (unsigned int)i);
    lwip_dump_printf("%02x ", d[i]);
    if (i % 16 == 15 || i == len - 1) lwip_dump_printf("\n");
  }
}

#endif //LWIP_DEBUG

/**
 * @brief Returns the current system time in milliseconds.
 *
 * Uses Arduino's `millis()` function.
 *
 * @return Current time in milliseconds.
 */
extern "C" u32_t sys_now(void) {
    return millis();  // Returns system time in milliseconds
}

#ifdef sys_msleep
#undef sys_msleep
#endif

/**
 * @brief Sleeps/delays for the specified number of milliseconds.
 *
 * Uses Arduino's `delay()` function.
 *
 * @param ms Number of milliseconds to sleep.
 */
extern "C" void sys_msleep(uint32_t ms) {
    delay(ms);
}