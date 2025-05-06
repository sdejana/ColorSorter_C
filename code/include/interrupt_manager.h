/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    interrupt_manager.h
 * @brief   Interrupt controller interface.
 *
 * @details
 *   Provides initialization and control functions for the interrupt system,
 *   including global enable/disable and retrieval of the current interrupt vector.
 *
 *********************************************************************************/

#ifndef _INTERRUPT_MANAGER_H
#define _INTERRUPT_MANAGER_H


/**
 * @brief   Initializes interrupt priorities for configured peripherals.
 *
 * @details
 *   Sets up the default interrupt priority levels and enables any
 *   peripheral-specific interrupts as required. Must be called at system startup.
 */
void INTERRUPT_Initialize(void);

/**
 * @brief   Globally enable interrupts.
 *
 * @details
 *   Enables all unmasked CPU interrupts.
 */
inline static void INTERRUPT_GlobalEnable(void)
{
    __builtin_enable_interrupts();
}

/**
 * @brief   Globally disable interrupts.
 *
 * @details
 *   Disables all CPU interrupts, preventing any interrupt service routines
 *   from executing until re-enabled.
 */
inline static void INTERRUPT_GlobalDisable(void)
{
    __builtin_disable_interrupts();
}

/**
 * @brief   Retrieves the current interrupt vector number.
 *
 * @return  uint16_t  The vector number of the currently serviced interrupt.
 *
 * @details
 *   Reads the _VECNUM register to identify which interrupt source is active.
 */
inline static uint16_t INTERRUPT_VectorNumberGet(void)
{
    return _VECNUM; 
}

#endif /* _INTERRUPT_MANAGER_H */
