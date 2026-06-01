/**
 * pose_dashboard.c — IMX6ULL 姿态仪表盘
 *
 * 架构:
 *   sensor_thread.c   → 后台线程: ICM20608 硬件 I/O + 互补滤波
 *   styles.c          → 样式
 *   ui_dashboard.c    → LVGL UI 构建
 *   pose_dashboard.c  → main()
 *
 */

#include "lvgl/lvgl.h"
#include "sensor_thread.h"
#include "styles.h"
#include "ui_dashboard.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <string.h>

/* =======================================================
 * 硬件参数
 * ======================================================= */
#define SCREEN_W   1024
#define SCREEN_H   600
#define COLOR_BPP  2

/* 帧缓冲句柄 */
static int   fb_fd        = -1;
static char *fb_mem       = NULL;
static long  fb_size      = 0;
static int   fb_line_len  = 0;

/* =======================================================
 * LVGL flush 回调
 * ======================================================= */
static void flush_cb(lv_display_t *disp, const lv_area_t *area,
                     uint8_t *px_map) {
    uint16_t *fb = (uint16_t *)fb_mem;
    uint16_t *src = (uint16_t *)px_map;
    int w = area->x2 - area->x1 + 1;

    for (int y = area->y1; y <= area->y2; y++) {
        long loc = area->x1 + (y * fb_line_len / 2);
        memcpy(&fb[loc], src, w * 2);
        /* 双缓冲保护 */
        if (fb_size > SCREEN_W * SCREEN_H * COLOR_BPP) {
            memcpy(&fb[loc + (SCREEN_W * SCREEN_H)],
                   &fb[loc], w * 2);
        }
        src += w;
    }
    lv_display_flush_ready(disp);
}

/* =======================================================
 * main LVGL 渲染
 * ======================================================= */
int main(void) {
    printf("=== IMX6ULL Pose Dashboard (Multi-File Architecture) ===\n");

    /* ---- 1. 互斥锁初始化 ---- */
    if (pthread_mutex_init(&data_lock, NULL) != 0) {
        printf("FATAL: mutex init failed\n");
        return -1;
    }

    /* ---- 2. 启动后台传感器线程 ---- */
    pthread_t tid;
    if (pthread_create(&tid, NULL, sensor_thread_func, NULL) != 0) {
        printf("FATAL: failed to create sensor thread\n");
        return -1;
    }

    /* ---- 3. 映射帧缓冲 ---- */
    fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0) {
        printf("FATAL: cannot open /dev/fb0\n");
        return -1;
    }
    {
        struct fb_var_screeninfo vi;
        ioctl(fb_fd, FBIOGET_VSCREENINFO, &vi);
        fb_line_len = vi.xres_virtual * COLOR_BPP;
        fb_size     = vi.xres_virtual * vi.yres_virtual * COLOR_BPP;
    }
    fb_mem = (char *)mmap(0, fb_size, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fb_fd, 0);
    if (fb_mem == MAP_FAILED) {
        printf("FATAL: mmap framebuffer failed\n");
        return -1;
    }

    /* ---- 4. LVGL 初始化 ---- */
    lv_init();
    static uint8_t draw_buf[SCREEN_W * 40 * COLOR_BPP];
    lv_display_t *disp = lv_display_create(SCREEN_W, SCREEN_H);
    lv_display_set_buffers(disp, draw_buf, NULL,
                           sizeof(draw_buf),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, flush_cb);

    /* ---- 5. 样式 + UI 构建 ---- */
    styles_init();
    ui_dashboard_create();
    printf("[Main] UI initialized. Entering render loop...\n\n");

    /* ---- 6. 主循环 ---- */
    while (1) {
        lv_timer_handler();
        usleep(5000);
        lv_tick_inc(5);
    }

    pthread_mutex_destroy(&data_lock);
    munmap(fb_mem, fb_size);
    close(fb_fd);
    return 0;
}
