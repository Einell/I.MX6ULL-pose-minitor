#ifndef STYLES_H
#define STYLES_H

#include "lvgl/lvgl.h"

/* =======================================================
 * 全局色彩
 * ======================================================= */

/* 背景层级 */
#define COLOR_BG_DEEP     0x0B0E14   /* 最深底色 */
#define COLOR_BG_PANEL    0x151A24   /* 卡片 / 面板底色 */
#define COLOR_BG_HEADER   0x111722   /* 顶栏底色 */

/* 强调色 */
#define COLOR_ACCENT      0x00E5A0   /* 青绿 —— 俯仰 / 标题 */
#define COLOR_ACCENT2     0xFFB300   /* 琥珀金 —— 横滚 */
#define COLOR_ACCENT3     0x4DA8FF   /* 天蓝 —— 辅助点缀 */

/* 地平线 */
#define COLOR_SKY         0x1E6091   /* 天空蓝 */
#define COLOR_SKY_TOP     0x0D3B66   /* 天顶深蓝 */
#define COLOR_GROUND      0x7B5B2A   /* 大地棕 */
#define COLOR_GROUND_BOT  0x4A3518   /* 地底深棕 */
#define COLOR_HORIZON     0xFFFFFF   /* 地平线白 */

/* 文字 */
#define COLOR_TEXT_PRIME  0xE8E8E8   /* 主文字 */
#define COLOR_TEXT_DIM    0x6B7280   /* 次级文字 */
#define COLOR_TEXT_WHITE  0xFFFFFF   /* 纯白 */

/* 俯仰条梯度分区 */
#define COLOR_PITCH_SAFE  0x00E676   /* 安全区 (绿) */
#define COLOR_PITCH_WARN  0xFFD600   /* 警告区 (黄) */
#define COLOR_PITCH_DANGER 0xFF1744  /* 危险区 (红) */

/* 边框 / 分割线 */
#define COLOR_BORDER      0x2A3040   /* 面板边框 */
#define COLOR_DIVIDER     0x1E2535   /* 暗分割线 */

/* 状态灯 */
#define COLOR_LED_ON      0x00FF88   /* 运行中 */
#define COLOR_LED_OFF     0x442222   /* 未连接 */

/* =======================================================
 * 共享样式对象
 * ======================================================= */
extern lv_style_t style_panel;       /* 通用卡片面板 */
extern lv_style_t style_panel_inner; /* 无边框内面板 */
extern lv_style_t style_title;       /* 标题文字 */
extern lv_style_t style_value;       /* 数值大字 */
extern lv_style_t style_label_dim;   /* 次级标签 */
extern lv_style_t style_label_white; /* 白色标签 */
extern lv_style_t style_header;      /* 顶栏容器 */

/* 一次性初始化所有样式 */
void styles_init(void);

#endif /* STYLES_H */
