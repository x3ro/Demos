#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "vtimer.h"
#include <smb380-board.h>

#include "acc.h"


#define SAMPLING_INTERVAL       5           /* interval to sample data in ms */
#define ACC_SENSITIVITY         256         /* resolution per G */

/* define default data threshholds for the event detection */
#define UPPER_TH                275
#define LOWER_TH                250
#define AXIS_TH                 225
#define REP_COUNT               3           /* accept new state only after 3 consecutive measurements */


acc_pos_t acc_current_pos = ACC_POS_0;


int16_t math_modulus(int16_t *v, int dim);


void acc_thread(void)
{
    int16_t acc_data[6];
    int16_t norm;
    int state = ACC_POS_0;
    int count = 0;

    /* initialize the acceleration sensor */
    puts("acc: will initialize SMB380 driver now");
    SMB380_init_simple(100, SMB380_BAND_WIDTH_375HZ, SMB380_RANGE_2G);
    puts("acc: SMB380 initialized");

    /* detect data */
    while (1) {
        /* wait according to the sampling interval */
        vtimer_usleep(SAMPLING_INTERVAL * 1000);

        /* get acceleration values */
        SMB380_getAcceleration(SMB380_X_AXIS, &acc_data[SMB380_X_AXIS], &acc_data[SMB380_X_AXIS + 3]);
        SMB380_getAcceleration(SMB380_Y_AXIS, &acc_data[SMB380_Y_AXIS], &acc_data[SMB380_Y_AXIS + 3]);
        SMB380_getAcceleration(SMB380_Z_AXIS, &acc_data[SMB380_Z_AXIS], &acc_data[SMB380_Z_AXIS + 3]);

        /* evaluate the new data */
        norm = math_modulus(acc_data, 3);
        if (norm > LOWER_TH && norm < UPPER_TH) {

            /* sensor is stable, evaluate all three axes */
            for (int i = 0; i < 3; i++) {
                if (acc_data[i] > AXIS_TH) {
                    if (state == i) {
                        if (count <= REP_COUNT) {
                            count++;
                        }
                    }
                    else {
                        state = i;
                        count = 0;
                    }
                }
                else if (acc_data[i] < -AXIS_TH) {
                    if (state == (i + 3)) {
                        if (count <= REP_COUNT) {
                            count++;
                        }
                    }
                    else {
                        state = i + 3;
                        count = 0;
                    }
                }
            }
        }
        if (count == REP_COUNT && acc_current_pos != state) {
            printf("acc: new state: %i\n", state);
            acc_current_pos = state;
        }
    }
}

int16_t math_modulus(int16_t *v, int dim)
{
    float mod = 0.0f;
    for (int i = 0; i < dim; i++) {
        mod += powf(v[i], 2.0f);
    }
    return (int16_t)sqrtf(mod);
}
