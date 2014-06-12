

#ifndef __ACC_H
#define __ACC_H

typedef enum {
    ACC_POS_0 = 0,
    ACC_POS_1,
    ACC_POS_2,
    ACC_POS_3,
    ACC_POS_4,
    ACC_POS_5
} acc_pos_t;


extern acc_pos_t acc_current_pos;


void acc_thread(void);

#endif /* __ACC_H */
