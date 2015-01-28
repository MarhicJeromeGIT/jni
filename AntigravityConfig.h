#ifndef CONFIG_H
	#define CONFIG_H

#ifdef __ANDROID__

#ifdef NDEBUG
#define __DEBUG__ true
#endif

#define EXECUTABLE_PATH ""
#define MUSIC_PATH  ""
#define SHADER_PATH ""
#define DATA_PATH  ""
#define LEVEL_PATH  ""

#elif defined( __linux__ )

#define EXECUTABLE_PATH "/home/jerome/workspace_android/AlienSurf/build/"
#define MUSIC_PATH "/media/E/projets/build/Data/"
#define SHADER_PATH "/home/jerome/workspace_android/AlienSurf/assets/"
#define DATA_PATH  "/media/E/projets/build/Data/"
#define LEVEL_PATH  "/Levels/"

#elif defined( WIN32 )

#define MUSIC_PATH "C:\\Users\\XiongBaoBao\\Portfolio\\Data\\Data\\"
#define SHADER_PATH "C:\\Users\\XiongBaoBao\\Portfolio\\jni\\shaders\\"
#define DATA_PATH  "C:\\Users\\XiongBaoBao\\Portfolio\\Data\\Data\\"

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

#else // linux, win32

//#define __BUILD_DATA__ true

#define LOGE(...) printf(__VA_ARGS__);
#define LOGT(TAG,...) printf(__VA_ARGS__);
#endif

#ifdef WIN32
#define _USE_MATH_DEFINES // M_PI
#define NOMINMAX // seriously...
#endif

#endif // CONFIG_H
