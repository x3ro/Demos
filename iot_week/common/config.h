/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       Networking configuration options
 *
 * @author      Christian Mehlis <christian.mehlis@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef I_AM_P1_TOKEN
#define PLAYER                      1
#define CONFIG_OWN_ADDRESS          1
#define CONFIG_N0                   "fe80::ff:fe00:2"
#define CONFIG_N1                   "fe80::ff:fe00:5"
#define CONFIG_N2                   "fe80::ff:fe00:6"
#define CONFIG_N3                   "fe80::ff:fe00:9"
#endif
#ifdef I_AM_P1_LED
#define PLAYER                      1
#define CONFIG_OWN_ADDRESS          5
#define CONFIG_N0                   "fe80::ff:fe00:1"
#define CONFIG_N1                   "fe80::ff:fe00:2"
#define CONFIG_N2                   "fe80::ff:fe00:6"
#define CONFIG_N3                   "fe80::ff:fe00:9"
#endif
#ifdef I_AM_P2_TOKEN
#define PLAYER                      2
#define CONFIG_OWN_ADDRESS          2
#define CONFIG_N0                   "fe80::ff:fe00:1"
#define CONFIG_N1                   "fe80::ff:fe00:5"
#define CONFIG_N2                   "fe80::ff:fe00:6"
#define CONFIG_N3                   "fe80::ff:fe00:9"
#endif
#ifdef I_AM_P2_LED
#define PLAYER                      2
#define CONFIG_OWN_ADDRESS          6
#define CONFIG_N0                   "fe80::ff:fe00:1"
#define CONFIG_N1                   "fe80::ff:fe00:2"
#define CONFIG_N2                   "fe80::ff:fe00:5"
#define CONFIG_N3                   "fe80::ff:fe00:9"
#endif
#ifdef I_AM_PORTAL
#define PLAYER                      0
#define CONFIG_OWN_ADDRESS          9
#define CONFIG_N0                   "fe80::ff:fe00:1"
#define CONFIG_N1                   "fe80::ff:fe00:2"
#define CONFIG_N2                   "fe80::ff:fe00:5"
#define CONFIG_N3                   "fe80::ff:fe00:6"
#endif

#define CONFIG_CHANNEL              10

#endif /* CONFIG_H_ */
