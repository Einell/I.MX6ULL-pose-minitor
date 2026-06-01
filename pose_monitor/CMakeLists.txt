cmake_minimum_required(VERSION 3.5)
project(lvgl_dashboard C)

# 1. 开启 C99 标准
set(CMAKE_C_STANDARD 99)

# 2.源文件
set(SOURCES
    pose_dashboard.c
    sensor_thread.c
    styles.c
    ui_dashboard.c
)

# 3. 确保编译器能找到项目根目录下的 lv_conf.h 以及 lvgl/lvgl.h
include_directories(${CMAKE_SOURCE_DIR})

# 4. 将 lvgl 文件夹作为一个子模块
add_subdirectory(lvgl)

# 5. 编译
add_executable(pose_demo ${SOURCES})

# 6. 链接 LVGL、数学库、线程库
target_link_libraries(pose_demo PRIVATE lvgl m pthread)