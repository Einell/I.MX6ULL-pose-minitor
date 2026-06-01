/**
 * sensor_thread.c — ICM20608 传感器后台线程
 */
#include "sensor_thread.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

pthread_mutex_t data_lock;
float g_pitch = 0.0f;
float g_roll  = 0.0f;

void *sensor_thread_func(void *arg) {
    (void)arg;

    int imu_fd = open("/dev/icm20608", O_RDWR);
    if (imu_fd < 0) {
        printf("Error: Background thread failed to open /dev/icm20608!\n");
        return NULL;
    }

    int databuf[7];
    float ax, ay, az, gx, gy, gz;
    float pitch_acc, roll_acc;
    float pitch = 0.0f, roll = 0.0f;
    float dt = 0.02f; /* 50Hz 采样 */
    int loop_cnt = 0;

    printf("[Sensor Thread] Started successfully. Running in background...\n");

    while (1) {
        int ret = read(imu_fd, databuf, sizeof(databuf));

        if (ret == (int)sizeof(databuf)) {
            gx = (float)((short)databuf[0]) / 16.4f;
            gy = (float)((short)databuf[1]) / 16.4f;
            gz = (float)((short)databuf[2]) / 16.4f;
            ax = (float)((short)databuf[3]) / 2048.0f;
            ay = (float)((short)databuf[4]) / 2048.0f;
            az = (float)((short)databuf[5]) / 2048.0f;

            pitch_acc = atan2f(ax, sqrtf(ay * ay + az * az)) * 57.29578f;
            roll_acc  = atan2f(ay, az) * 57.29578f;

            pitch = 0.95f * (pitch + gx * dt) + 0.05f * pitch_acc;
            roll  = 0.95f * (roll  + gy * dt) + 0.05f * roll_acc;

            pthread_mutex_lock(&data_lock);
            g_pitch = pitch;
            g_roll  = roll;
            pthread_mutex_unlock(&data_lock);

            if (loop_cnt % 50 == 0) {
                printf("\r[Background IMU] Pitch: %6.1f | Roll: %6.1f", pitch, roll);
                fflush(stdout);
            }
        }
        loop_cnt++;
        usleep(20000);
    }

    close(imu_fd);
    return NULL;
}
