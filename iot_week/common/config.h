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

#define CONFIG_CHANNEL              1
#define CONFIG_OWN_ADDRESS          1

#define CONFIG_NEIGHBORS            5

#define CONFIG_NEIGHBOR_P1_TOKEN    "fe80::ff:fe00:1"
#define CONFIG_NEIGHBOR_P2_TOKEN    "fe80::ff:fe00:2"
#define CONFIG_NEIGHBOR_P1_LED      "fe80::ff:fe00:5"
#define CONFIG_NEIGHBOR_P2_LED      "fe80::ff:fe00:6"
#define CONFIG_NEIGHBOR_PORTAL      "fe80::ff:fe00:9"

#endif /* CONFIG_H_ */
