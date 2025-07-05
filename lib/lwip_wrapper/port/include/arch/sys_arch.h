/**
 * @file
 * @brief Architecture-specific system abstraction types for lwIP.
 *
 * Defines basic system types and constants used by the lwIP sys_arch layer,
 * including thread handles, protection state, and null definitions for
 * mailboxes and semaphores.
 */

#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#define SYS_MBOX_NULL   NULL  /**< Null mailbox handle */
#define SYS_SEM_NULL    NULL  /**< Null semaphore handle */

typedef void * sys_prot_t;   /**< Type to hold protection state (e.g. interrupt mask) */

//typedef void * sys_sem_t;  /**< Semaphore handle (commented out) */
//typedef void * sys_mbox_t; /**< Mailbox handle (commented out) */

typedef void * sys_thread_t; /**< Thread handle */

#endif /* __ARCH_SYS_ARCH_H__ */