/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       Game configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __GAME_H
#define __GAME_H

#ifndef PLAYER
#define PLAYER                  1
#endif

#define MSG_GAME_START          0
#define MSG_GAME_OVER           1
#define MSG_GAME_STATE          2
#define MSG_GAME_GO             3
#define MSG_GAME_SCORE          4

#define GAME_START_SEQ_LEN      3
#define GAME_START_SEQ          {ACC_POS_0, ACC_POS_1, ACC_POS_2}

#define GAME_LED_COUNTDOWN_LEN  7
#define GAME_LED_COUNTDOWN      {{0, 0, 0}, {0, 0xff, 0}, \
                                 {0, 0, 0}, {0, 0, 0xff}, \
                                 {0, 0, 0}, {0xff, 0, 0}, {0, 0, 0}}

#define GAME_LED_WIN_LEN        5
#define GAME_LED_WIN            {{0, 0, 0}, {0, 0xff, 0}, \
                                 {0, 0, 0}, {0, 0xff, 0}, {0, 0, 0}}
#define GAME_LED_LOSE_LEN       5
#define GAME_LED_LOSE           {{0, 0, 0}, {0xff, 0, 0}, \
                                 {0, 0, 0}, {0xff, 0, 0}, {0, 0, 0}}

#endif /* __GAME_H */
