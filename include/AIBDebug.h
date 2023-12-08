#ifndef __AIB_DBEUG_H__
#define __AIB_DBEUG_H__

// 宏定义打印时显示的颜色
#define COLOR_NONE  "\033[0m"
#define RED         "\033[1;31m"
#define BLUE        "\033[1;34m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"

// 设置打印宏
#ifndef __DMS_DEBUG__
#include <stdio.h>
#define AIBPrint(format, ...) do {\
    printf("%s to line %d: \n", __FILE__, __LINE__);\
    printf(GREEN);\
    printf(format, ##__VA_ARGS__);\
    printf(COLOR_NONE);\
}while(0)
#else
#define DMSPrint(format, ...)
#endif // __MM_DEBUG__

#endif // __AIB_DBEUG_H__