#ifndef __DEBUG_H
#define __DEBUG_H

//调试信息开关，关闭直接注释
/*************/



#define DEBUG 1

#ifdef DEBUG
#define Debug(...) 	printf(__VA_ARGS__)
#else
#define Debug(...)
#endif



#endif



