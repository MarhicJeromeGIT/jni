

// Statically linked into the "main" shared library

#ifndef COMMON_LIBRARY_H
#define COMMON_LIBRARY_H

#ifdef __ANDROID__

extern int test();

#endif

#ifdef __ANDROID__

#ifndef NDEBUG
#include <android/log.h>
#define  LOG_TAG    "montestgljni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);   \
					__android_log_print(ANDROID_LOG_INFO,LOG_TAG,"%s %d",__FILE__,__LINE__);
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);  \
					__android_log_print(ANDROID_LOG_INFO,LOG_TAG,"%s %d",__FILE__,__LINE__);
#define  LOGT(TAG,...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__);  \
						__android_log_print(ANDROID_LOG_INFO,LOG_TAG,"%s %d",__FILE__,__LINE__);
#else
#define  LOGI(...)
#define  LOGE(...)
#define  LOGT(...)
#endif

#else // linux

#define LOGE(...) printf(__VA_ARGS__);
#define LOGT(TAG,...) printf(__VA_ARGS__);
#endif


#endif
