#ifndef ndk_log_h
#define ndk_log_h

#ifdef __cplusplus
extern "C"
{
#endif


#undef LOG_ENABLE
//#define LOG_ENABLE



#ifdef LOG_ENABLE
#include <utils/Log.h>
#else
#define LOGE(fmt, args...)  do {} while(0)
#define LOGW(fmt, args...)  do {} while(0)
#define LOGI(fmt, args...)    do {} while(0)
#define LOGD(fmt, args...)  do {} while(0)
#define LOGV(fmt, args...) do {} while(0)

// END -- console log
#endif
#define LOG_ENTER()           LOGD("ENTER %s", __FUNCTION__)
#define LOG_LEAVE()            LOGD("LEAVE %s", __FUNCTION__)
#define LOG_STEP()             LOGV("%s : %d", __FUNCTION__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif

