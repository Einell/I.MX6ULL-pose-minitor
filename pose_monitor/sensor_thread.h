#ifndef SENSOR_THREAD_H
#define SENSOR_THREAD_H

#include <pthread.h>

/* 全局互斥锁 */
extern pthread_mutex_t data_lock;

/* 最新姿态角 */
extern float g_pitch;
extern float g_roll;

/* 传感器线程入口函数 */
void *sensor_thread_func(void *arg);

#endif /* SENSOR_THREAD_H */
