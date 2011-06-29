#ifdef ANDROID

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <jni.h>
#include <android/log.h>
#include "SDL_thread.h"
#include "SDL_main.h"

#include "ndk_log.h"

/* JNI-C wrapper stuff */

#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif

//#define LOGI(fmt,args...) __android_log_print(ANDROID_LOG_INFO, "libSDL_main", fmt, ##args)

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_tiger_nump"

#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)

extern void SDL_ANDROID_MultiThreadedVideoLoopInit();
extern void SDL_ANDROID_MultiThreadedVideoLoop();

extern void setOption(const char*key, const char *value);
extern int  getCurrentLevel();

static int argc = 0;
static char ** argv = NULL;

static int threadedMain(void * unused)
{
	LOGI("libSDL_main", "Enter threadMain");
	SDL_main( argc, argv );
	LOGI("libSDL_main", "Application closed, calling exit(0)");
	exit(0);
}

extern C_LINKAGE void
JAVA_EXPORT_NAME(DemoRenderer_nativeInit) ( JNIEnv*  env, jobject thiz, jstring jcurdir, jstring cmdline, jint multiThreadedVideo )
{
	int i = 0;
	char curdir[PATH_MAX] = "";
	const jbyte *jstr;
	const char * str = "sdl";

	strcpy(curdir, "/sdcard/app-data/");
	strcat(curdir, SDL_CURDIR_PATH);

	LOGI("ENTER DemoRender_nativeInit...");
	jstr = (*env)->GetStringUTFChars(env, jcurdir, NULL);
	if (jstr != NULL && strlen(jstr) > 0)
		strcpy(curdir, jstr);
	(*env)->ReleaseStringUTFChars(env, jcurdir, jstr);

	chdir(curdir);
	setenv("HOME", curdir, 1);
	__android_log_print(ANDROID_LOG_INFO, "libSDL", "Changing curdir to \"%s\"", curdir);

	jstr = (*env)->GetStringUTFChars(env, cmdline, NULL);

	if (jstr != NULL && strlen(jstr) > 0)
		str = jstr;

	{
		char * str1, * str2;
		str1 = strdup(str);
		str2 = str1;
		while(str2)
		{
			argc++;
			str2 = strchr(str2, ' ');
			if(!str2)
				break;
			str2++;
		}

		argv = (char **)malloc(argc*sizeof(char *));
		str2 = str1;
		while(str2)
		{
			argv[i] = str2;
			i++;
			str2 = strchr(str2, ' ');
			if(str2)
				*str2 = 0;
			else
				break;
			str2++;
		}
	}

	__android_log_print(ANDROID_LOG_INFO, "libSDL", "Calling SDL_main(\"%s\")", str);

	(*env)->ReleaseStringUTFChars(env, cmdline, jstr);

	for( i = 0; i < argc; i++ )
		__android_log_print(ANDROID_LOG_INFO, "libSDL", "param %d = \"%s\"", i, argv[i]);

	if( ! multiThreadedVideo )
		SDL_main( argc, argv );
	else
	{	
		LOGI("SDL video init");
		SDL_ANDROID_MultiThreadedVideoLoopInit();
		SDL_CreateThread(threadedMain, NULL);
		SDL_ANDROID_MultiThreadedVideoLoop();
		LOGI("SDL video init done");
	}
	LOGI("LEAVE demoRenderNativeInit");
};


extern C_LINKAGE void
JAVA_EXPORT_NAME(Settings_nativeSetEnv) ( JNIEnv*  env, jobject thiz, jstring j_name, jstring j_value )
{
    jboolean iscopy;
    const char *name = (*env)->GetStringUTFChars(env, j_name, &iscopy);
    const char *value = (*env)->GetStringUTFChars(env, j_value, &iscopy);

    LOGD("Settings_nativeSetEnv, key=%s, value=%s", name, value);

	if(strcmp(name, "RESET") == 0){
		setOption(name,value);
	}
	else if(strcmp(name, "GOTO_LEVEL") == 0){
		setOption(name,value);
	}
	else if(strcmp(name, "PREV_LEVEL") == 0){
		setOption(name,value);
	}
	else if(strcmp(name, "NEXT_LEVEL") == 0){
		setOption(name,value);
	}
	else {
	   setenv(name, value, 1);
	}
    (*env)->ReleaseStringUTFChars(env, j_name, name);
    (*env)->ReleaseStringUTFChars(env, j_value, value);
}

extern C_LINKAGE int
JAVA_EXPORT_NAME(Settings_nativeGetCurrentLevel) ( JNIEnv*  env, jobject thiz )
{
    return getCurrentLevel();
}

#endif
