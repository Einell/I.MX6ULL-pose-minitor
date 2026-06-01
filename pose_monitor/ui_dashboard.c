/**
 * ui_dashboard.c — 姿态仪表盘 UI
 */
#include "ui_dashboard.h"
#include "sensor_thread.h"
#include "styles.h"
#include <stdio.h>

#define SCREEN_W       1024
#define SCREEN_H       600
#define HEADER_H       48
#define FOOTER_H       48
#define MAIN_Y         48
#define MAIN_H         504
#define SIDE_W         180
#define GAP            4

#define CENTER_W       (SCREEN_W - 2 * SIDE_W - 2 * GAP)
#define CENTER_H       (MAIN_H - 2 * GAP)

#define HOR_PAD        16
#define HORIZ_W        (CENTER_W - 2 * HOR_PAD)
#define HORIZ_H        (CENTER_H - 2 * HOR_PAD - 20)
#define HORIZ_MID      (HORIZ_H / 2)
#define HORIZ_PX_DEG   ((float)HORIZ_MID / 90.0f)

/*动态控件指针*/
static lv_obj_t *pitch_bar;
static lv_obj_t *pitch_val_label;
static lv_obj_t *roll_arc;
static lv_obj_t *roll_val_label;
static lv_obj_t *sky_panel;
static lv_obj_t *ground_panel;
static lv_obj_t *horizon_line;
static lv_obj_t *cross_h;
static lv_obj_t *cross_v;
static lv_obj_t *footer_text;

static lv_obj_t *mk_label(lv_obj_t *parent, const char *txt,
                           const lv_style_t *s, lv_align_t a,
                           int32_t ox, int32_t oy) {
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    if (s) lv_obj_add_style(l, s, 0);
    lv_obj_align(l, a, ox, oy);
    return l;
}

/* =======================================================
 * 顶栏
 * ======================================================= */
static void build_header(lv_obj_t *scr) {
    lv_obj_t *h = lv_obj_create(scr);
    lv_obj_add_style(h, &style_header, 0);
    lv_obj_set_size(h, SCREEN_W, HEADER_H);
    lv_obj_align(h, LV_ALIGN_TOP_MID, 0, 0);

    /* 前置青色小方块 */
    lv_obj_t *dot = lv_obj_create(h);
    lv_obj_set_size(dot, 10, 10);
    lv_obj_set_style_bg_color(dot, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_border_width(dot, 0, 0);
    lv_obj_set_style_radius(dot, 2, 0);
    lv_obj_align(dot, LV_ALIGN_LEFT_MID, 8, 0);

    mk_label(h, "IMX6ULL   DIGITAL ATTITUDE INDICATOR",
             &style_title, LV_ALIGN_LEFT_MID, 24, 0);

    /* 右侧 LED */
    lv_obj_t *led = lv_led_create(h);
    lv_obj_set_size(led, 10, 10);
    lv_obj_align(led, LV_ALIGN_RIGHT_MID, -52, 0);
    lv_led_set_color(led, lv_color_hex(COLOR_LED_ON));
    lv_led_on(led);
    mk_label(h, "ACTIVE", &style_title, LV_ALIGN_RIGHT_MID, -8, 0);
}

/* =======================================================
 * 底栏
 * ======================================================= */
static void build_footer(lv_obj_t *scr) {
    lv_obj_t *f = lv_obj_create(scr);
    lv_obj_add_style(f, &style_header, 0);
    lv_obj_set_style_border_width(f, 1, 0);
    lv_obj_set_style_border_color(f, lv_color_hex(COLOR_BORDER), 0);
    lv_obj_set_style_border_side(f, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_size(f, SCREEN_W, FOOTER_H);
    lv_obj_align(f, LV_ALIGN_BOTTOM_MID, 0, 0);
    footer_text = mk_label(f, "", &style_label_dim, LV_ALIGN_CENTER, 0, 0);
}

/* =======================================================
 * 左侧 —— 俯仰条面板
 * ======================================================= */
static void build_pitch(lv_obj_t *scr) {
    lv_obj_t *pnl = lv_obj_create(scr);
    lv_obj_add_style(pnl, &style_panel, 0);
    lv_obj_set_size(pnl, SIDE_W, MAIN_H - 2 * GAP);
    lv_obj_align(pnl, LV_ALIGN_TOP_LEFT, GAP, MAIN_Y + GAP);

    mk_label(pnl, "PITCH", &style_title, LV_ALIGN_TOP_MID, 0, 6);

    /* 垂直俯仰条 */
    pitch_bar = lv_bar_create(pnl);
    lv_obj_set_size(pitch_bar, 26, 290);
    lv_obj_align(pitch_bar, LV_ALIGN_CENTER, -22, 0);
    lv_bar_set_range(pitch_bar, -90, 90);
    /* 主背景 */
    lv_obj_set_style_bg_color(pitch_bar, lv_color_hex(COLOR_BORDER), LV_PART_MAIN);
    lv_obj_set_style_radius(pitch_bar, 6, LV_PART_MAIN);
    /* 指示条 */
    lv_obj_set_style_bg_color(pitch_bar, lv_color_hex(COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_radius(pitch_bar, 6, LV_PART_INDICATOR);

    /* 右侧刻度标签 */
    const int degs[] = { -90, -60, -30, 0, 30, 60, 90 };
    int bar_h = 290;
    for (int i = 0; i < 7; i++) {
        char b[6];
        snprintf(b, sizeof(b), "%+d", degs[i]);
        int y = i * bar_h / 6 - bar_h / 2;

        lv_obj_t *lbl = mk_label(pnl, b, &style_label_dim,
                                  LV_ALIGN_CENTER, 0, y);
        lv_obj_align_to(lbl, pitch_bar, LV_ALIGN_OUT_RIGHT_MID, 8, y);

        lv_obj_t *tk = lv_obj_create(pnl);
        lv_obj_set_size(tk, 14, 1);
        lv_obj_set_style_bg_color(tk, lv_color_hex(COLOR_TEXT_DIM), 0);
        lv_obj_set_style_border_width(tk, 0, 0);
        lv_obj_set_style_radius(tk, 0, 0);
        lv_obj_align_to(tk, pitch_bar, LV_ALIGN_OUT_RIGHT_MID, 2, y);
    }

    /* 当前值标签*/
    pitch_val_label = mk_label(pnl, "+0.0", &style_value,
                                LV_ALIGN_CENTER, -22, bar_h / 2 + 14);
    mk_label(pnl, "deg", &style_label_dim,
             LV_ALIGN_CENTER, 16, bar_h / 2 + 14);
}

/* =======================================================
 * 中间
 * ======================================================= */
static void build_horizon(lv_obj_t *scr) {
    lv_obj_t *pnl = lv_obj_create(scr);
    lv_obj_add_style(pnl, &style_panel, 0);
    lv_obj_set_size(pnl, CENTER_W, MAIN_H - 2 * GAP);
    lv_obj_align(pnl, LV_ALIGN_TOP_MID, 0, MAIN_Y + GAP);

    mk_label(pnl, "ARTIFICIAL HORIZON", &style_title, LV_ALIGN_TOP_MID, 0, 6);

    /* 裁剪窗 */
    lv_obj_t *clip = lv_obj_create(pnl);
    lv_obj_set_size(clip, HORIZ_W, HORIZ_H);
    lv_obj_align(clip, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_style_bg_color(clip, lv_color_hex(COLOR_BG_DEEP), 0);
    lv_obj_set_style_border_width(clip, 1, 0);
    lv_obj_set_style_border_color(clip, lv_color_hex(COLOR_BORDER), 0);
    lv_obj_set_style_radius(clip, 4, 0);
    lv_obj_set_style_pad_all(clip, 0, 0);
    lv_obj_set_style_clip_corner(clip, true, 0);

    sky_panel = lv_obj_create(clip);
    lv_obj_set_style_bg_color(sky_panel, lv_color_hex(COLOR_SKY), 0);
    lv_obj_set_style_bg_opa(sky_panel, LV_OPA_COVER,0);
    lv_obj_set_style_border_width(sky_panel, 0, 0);
    lv_obj_set_style_radius(sky_panel, 0, 0);
    lv_obj_set_style_pad_all(sky_panel, 0, 0);
    lv_obj_set_size(sky_panel, HORIZ_W, HORIZ_MID);
    lv_obj_set_pos(sky_panel, 0, 0);

    ground_panel = lv_obj_create(clip);
    lv_obj_set_style_bg_color(ground_panel, lv_color_hex(COLOR_GROUND), 0);
    lv_obj_set_style_bg_opa(ground_panel, LV_OPA_COVER,0);
    lv_obj_set_style_border_width(ground_panel, 0, 0);
    lv_obj_set_style_radius(ground_panel, 0, 0);
    lv_obj_set_style_pad_all(ground_panel, 0, 0);
    lv_obj_set_size(ground_panel, HORIZ_W, HORIZ_MID);
    lv_obj_set_pos(ground_panel, 0, HORIZ_MID);

    horizon_line = lv_obj_create(clip);
    lv_obj_set_style_bg_color(horizon_line, lv_color_hex(COLOR_HORIZON), 0);
    lv_obj_set_style_bg_opa(horizon_line, LV_OPA_COVER,0);
    lv_obj_set_style_border_width(horizon_line, 0, 0);
    lv_obj_set_style_radius(horizon_line, 0, 0);
    lv_obj_set_size(horizon_line, HORIZ_W, 2);
    lv_obj_set_pos(horizon_line, 0, HORIZ_MID);

    /* 中心十字标 */
    cross_h = lv_obj_create(clip);
    lv_obj_set_style_bg_color(cross_h, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_bg_opa(cross_h, LV_OPA_COVER,0);
    lv_obj_set_style_border_width(cross_h, 0, 0);
    lv_obj_set_style_radius(cross_h, 0, 0);
    lv_obj_set_size(cross_h, 44, 1);
    lv_obj_set_pos(cross_h, HORIZ_W / 2 - 22, HORIZ_MID);

    cross_v = lv_obj_create(clip);
    lv_obj_set_style_bg_color(cross_v, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_bg_opa(cross_v, LV_OPA_COVER,0);
    lv_obj_set_style_border_width(cross_v, 0, 0);
    lv_obj_set_style_radius(cross_v, 0, 0);
    lv_obj_set_size(cross_v, 1, 30);
    lv_obj_set_pos(cross_v, HORIZ_W / 2, HORIZ_MID - 15);

    /* 俯仰刻度 */
    for (int d = -80; d <= 80; d += 10) {
        if (d == 0) continue;
        lv_obj_t *t = lv_obj_create(clip);
        int len = (d % 30 == 0) ? 28 : 14;
        lv_obj_set_style_bg_color(t, lv_color_hex(COLOR_HORIZON), 0);
        lv_obj_set_style_bg_opa(t, LV_OPA_COVER,0);
        lv_obj_set_style_border_width(t, 0, 0);
        lv_obj_set_style_radius(t, 0, 0);
        lv_obj_set_size(t, len, 1);
        lv_obj_set_pos(t, 8, HORIZ_MID - (int)(d * HORIZ_PX_DEG));
    }
}

/* =======================================================
 * 右侧
 * ======================================================= */
static void build_roll(lv_obj_t *scr) {
    lv_obj_t *pnl = lv_obj_create(scr);
    lv_obj_add_style(pnl, &style_panel, 0);
    lv_obj_set_size(pnl, SIDE_W, MAIN_H - 2 * GAP);
    lv_obj_align(pnl, LV_ALIGN_TOP_RIGHT, -GAP, MAIN_Y + GAP);

    mk_label(pnl, "ROLL", &style_title, LV_ALIGN_TOP_MID, 0, 6);

    /* 弧 */
    roll_arc = lv_arc_create(pnl);
    lv_obj_set_size(roll_arc, 150, 150);
    lv_obj_align(roll_arc, LV_ALIGN_CENTER, 0, -8);
    lv_arc_set_range(roll_arc, -90, 90);
    lv_arc_set_bg_angles(roll_arc, 0, 180);
    lv_arc_set_rotation(roll_arc, 180);
    lv_obj_set_style_arc_color(roll_arc, lv_color_hex(COLOR_ACCENT2),
                               LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(roll_arc, 7, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(roll_arc, lv_color_hex(COLOR_BORDER),
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(roll_arc, 7, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roll_arc, LV_OPA_TRANSP, LV_PART_KNOB);

    /* 弧中心数值 */
    roll_val_label = mk_label(pnl, "+0.0", &style_value,
                               LV_ALIGN_CENTER, 0, 22);
    mk_label(pnl, "deg", &style_label_dim, LV_ALIGN_CENTER, 0, 38);

    /* 弧外侧角度标签 */
    mk_label(pnl, "-90", &style_label_dim, LV_ALIGN_TOP_MID, -112, 84);
    mk_label(pnl, "-45", &style_label_dim, LV_ALIGN_TOP_MID, -88, 50);
    mk_label(pnl, "0",   &style_label_dim, LV_ALIGN_TOP_MID, 0, 10);
    mk_label(pnl, "+45", &style_label_dim, LV_ALIGN_TOP_MID, 88, 50);
    mk_label(pnl, "+90", &style_label_dim, LV_ALIGN_TOP_MID, 112, 84);
}

/* =======================================================
 * 定时器回调
 * ======================================================= */
static void ui_update_timer_cb(lv_timer_t *t) {
    (void)t;
    float p, r;

    pthread_mutex_lock(&data_lock);
    p = g_pitch;
    r = g_roll;
    pthread_mutex_unlock(&data_lock);

    /* 物理限幅 */
    if      (p >  90.0f) p =  90.0f;
    else if (p < -90.0f) p = -90.0f;
    if      (r >  90.0f) r =  90.0f;
    else if (r < -90.0f) r = -90.0f;

    /* ---- 俯仰 ---- */
    lv_bar_set_value(pitch_bar, (int32_t)p, LV_ANIM_OFF);
    lv_label_set_text_fmt(pitch_val_label, "%+.1f", (double)p);

    /* ---- 横滚 ---- */
    lv_arc_set_value(roll_arc, (int32_t)r);
    lv_label_set_text_fmt(roll_val_label, "%+.1f", (double)r);

    /* ---- 地平线 ---- */
    int sk = (int)(HORIZ_MID + p * HORIZ_PX_DEG);
    if (sk < 0) sk = 0;
    if (sk > HORIZ_H) sk = HORIZ_H;

    lv_obj_set_size(sky_panel, HORIZ_W, sk);
    lv_obj_set_pos(sky_panel, 0, 0);
    lv_obj_set_size(ground_panel, HORIZ_W, HORIZ_H - sk);
    lv_obj_set_pos(ground_panel, 0, sk);
    lv_obj_set_pos(horizon_line, 0, sk);
    lv_obj_set_pos(cross_h, HORIZ_W / 2 - 22, sk);
    lv_obj_set_pos(cross_v, HORIZ_W / 2, sk - 15);

    /* ---- 底栏 ---- */
    lv_label_set_text_fmt(footer_text,
        "PITCH: %+6.1f deg    |    ROLL: %+6.1f deg    |    STATUS: STABLE",
        (double)p, (double)r);
}

/* =======================================================
 * 对外入口
 * ======================================================= */
void ui_dashboard_create(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COLOR_BG_DEEP), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    build_header(scr);
    build_footer(scr);
    build_pitch(scr);
    build_horizon(scr);
    build_roll(scr);

    lv_timer_create(ui_update_timer_cb, 40, NULL);
    printf("[UI Dashboard] Layout complete. Update timer: 40ms.\n");
}
