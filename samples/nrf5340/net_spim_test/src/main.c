/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <nrf.h>
#include <string.h>

// It does not work to use Net buffers for SPIM DMA
#define RAM_IN_APP_CORE

static char text[] = "Nordic SPIM test";

void main(void)
{
#ifdef RAM_IN_APP_CORE
    char *tx = (void*)0x20070000;
    char *rx = (void*)0x2006fff0;
#else
    char *tx = (void*)0x2100F000;
    char *rx = (void*)0x2100F800;
#endif

    memcpy(tx, text, sizeof(text));

    NRF_SPIM_Type *SPIM = (NRF_SPIM_Type *)0x5000A000;

    SPIM->ENABLE = 7;
    SPIM->PSEL.SCK = 36;
    SPIM->PSEL.MOSI = 37;
    SPIM->PSEL.MISO = 38;
    SPIM->PSEL.CSN = 39;
    SPIM->FREQUENCY = 0x80000000;

    SPIM->RXD.PTR = (uint32_t)rx;
    SPIM->RXD.MAXCNT = 1;
    SPIM->TXD.PTR = (uint32_t)tx;
    SPIM->TXD.MAXCNT = sizeof(text);

    while (1) {
        SPIM->TASKS_START = 1;
        while (!SPIM->EVENTS_END);
        SPIM->EVENTS_END = 0;

        for (volatile int i = 0; i < 10000; i++);
    }
}
