/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's lpm interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "arch/lpm_arch.h"

#include "stm32l1xx.h"

#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)

/* Ultra Low Power mode definitions */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define ULP_BitNumber           0x09
#define CR_ULP_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ULP_BitNumber * 4))

void lpm_arch_init(void)
{
    /* TODO */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    switch (target) {
        case LPM_ON:                /* Fully running */
            break;

        case LPM_IDLE:
            break;

        case LPM_SLEEP:;             /* Sleep mode */
            uint32_t tmpreg = PWR->CR;

            tmpreg &= CR_DS_MASK;       /* Clear PDDS and LPDSR bits */
            tmpreg |= PWR_CR_LPSDSR;    /* Set voltage regulator into low power mode */

            PWR->CR = tmpreg;

            SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP); /* Clear SLEEPDEEP bit of Cortex System Control Register */

            /* Request Wait For Interrupt */
            __WFI();
            break;

        case LPM_POWERDOWN:         /* Stop mode */
            /* Regulator in LP mode */
            PWR->CR = (PWR->CR & CR_DS_MASK) | PWR_CR_LPSDSR;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= SCB_SCR_SLEEPDEEP;

            /* Wait in sleep mode until interrupt */
            __WFI();

            /* Clear SLEEPDEEP bit */
            SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);
            break;

        case LPM_OFF:               /* Standby mode */
            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= SCB_SCR_SLEEPDEEP;

            /* Enable Ultra Low Power mode */
            *(__IO uint32_t *) CR_ULP_BB = 1;

            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif
            /* Request Wait For Interrupt */
            __WFI();
            break;

        /* do nothing here */
        case LPM_UNKNOWN:
        default:
            break;
    }

    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO */
    return 0;
}

void lpm_arch_awake(void)
{
    /* Disable Ultra Low Power mode */
    *(__IO uint32_t *) CR_ULP_BB = 0;

    PWR->CR &= (uint32_t) ~((uint32_t)PWR_CR_LPRUN);
    PWR->CR &= (uint32_t) ~((uint32_t)PWR_CR_LPSDSR);
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}
