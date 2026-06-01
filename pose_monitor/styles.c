/**
 * styles.c — 样式
 */
#include "styles.h"

/* ---- 样式对象定义 ---- */
lv_style_t style_panel;
lv_style_t style_panel_inner;
lv_style_t style_title;
lv_style_t style_value;
lv_style_t style_label_dim;
lv_style_t style_label_white;
lv_style_t style_header;

void styles_init(void) {
    /* ========== 卡片面板 ========== */
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, lv_color_hex(COLOR_BG_PANEL));
    lv_style_set_bg_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_border_color(&style_panel, lv_color_hex(COLOR_BORDER));
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_border_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_radius(&style_panel, 8);
    lv_style_set_pad_all(&style_panel, 10);

    /* ========== 内面板 ========== */
    lv_style_init(&style_panel_inner);
    lv_style_set_bg_color(&style_panel_inner, lv_color_hex(COLOR_BG_PANEL));
    lv_style_set_bg_opa(&style_panel_inner, LV_OPA_COVER);
    lv_style_set_border_width(&style_panel_inner, 0);
    lv_style_set_radius(&style_panel_inner, 4);
    lv_style_set_pad_all(&style_panel_inner, 6);

    /* ========== 标题 ========== */
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(COLOR_ACCENT));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_14);

    /* ========== 数值 ========== */
    lv_style_init(&style_value);
    lv_style_set_text_color(&style_value, lv_color_hex(COLOR_TEXT_WHITE));
    lv_style_set_text_font(&style_value, &lv_font_montserrat_14);

    /* ========== 次级标签 ========== */
    lv_style_init(&style_label_dim);
    lv_style_set_text_color(&style_label_dim, lv_color_hex(COLOR_TEXT_DIM));
    lv_style_set_text_font(&style_label_dim, &lv_font_montserrat_14);

    /* ========== 白色标签 ========== */
    lv_style_init(&style_label_white);
    lv_style_set_text_color(&style_label_white, lv_color_hex(COLOR_TEXT_WHITE));
    lv_style_set_text_font(&style_label_white, &lv_font_montserrat_14);

    /* ========== 顶栏容器 ========== */
    lv_style_init(&style_header);
    lv_style_set_bg_color(&style_header, lv_color_hex(COLOR_BG_HEADER));
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_border_width(&style_header, 0);
    lv_style_set_radius(&style_header, 0);
    lv_style_set_pad_hor(&style_header, 16);
    lv_style_set_pad_ver(&style_header, 0);
}
