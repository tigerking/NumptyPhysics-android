#
# Android Makefile ( Numpty Physics native libs)
#
#   CREATE on 2011-2-1 by Tiger King  
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

## Generating ARM codes instead of Thumb for optimizations
LOCAL_ARM_MODE := arm

# project source files
LOCAL_SRC_FILES:= \
	src/Path.cpp	\
	src/Canvas.cpp	\
	src/Levels.cpp	\
	src/Game.cpp


LOCAL_SRC_FILES += \
	src/Box2D/Source/Collision/b2Distance.cpp \
	src/Box2D/Source/Collision/b2CollideCircle.cpp \
	src/Box2D/Source/Collision/b2CollidePoly.cpp \
	src/Box2D/Source/Collision/b2PairManager.cpp \
	src/Box2D/Source/Collision/b2Shape.cpp \
	src/Box2D/Source/Collision/b2BroadPhase.cpp \
	src/Box2D/Source/Dynamics/b2WorldCallbacks.cpp \
	src/Box2D/Source/Dynamics/Joints/b2PrismaticJoint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2MouseJoint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2GearJoint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2Joint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2PulleyJoint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2DistanceJoint.cpp \
	src/Box2D/Source/Dynamics/Joints/b2RevoluteJoint.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2CircleContact.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2PolyAndCircleContact.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2Contact.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2PolyContact.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2ContactSolver.cpp \
	src/Box2D/Source/Dynamics/Contacts/b2Conservative.cpp \
	src/Box2D/Source/Dynamics/b2Island.cpp \
	src/Box2D/Source/Dynamics/b2Body.cpp \
	src/Box2D/Source/Dynamics/b2ContactManager.cpp \
	src/Box2D/Source/Dynamics/b2World.cpp \
	src/Box2D/Source/Common/b2BlockAllocator.cpp \
	src/Box2D/Source/Common/b2StackAllocator.cpp \
	src/Box2D/Source/Common/b2Settings.cpp

#addons (common) android JNI interface source
LOCAL_SRC_FILES += \
	android/sdl_main.c			\


LOCAL_CFLAGS:=  \
	-O2  \
	-DLINUX	\
	-DANDROID	\
	-DTIGERKING_FIX \
	-DSDL_JAVA_PACKAGE_PATH=com_tiger_nump \
	-DSDL_CURDIR_PATH=\"com.tiger.nump\"   \
	-DAPP_PACKAGE_NAME=\"com.tiger.nump\"   \
	-DDATA_DIR=\"/sdcard/data/data/.ltiger/nump\"
	

#disable gprof
LOCAL_CFLAGS +=        \
	-ffunction-sections  \
	-fno-omit-frame-pointer  		

LOCAL_CXXFLAGS := $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/src                    \
	$(LOCAL_PATH)/src/Box2D/include		\
	$(LOCAL_PATH)/android


#add stlport cpp head
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../external_deps/include/stlport    \
	$(LOCAL_PATH)/../external_deps/include/sdl        \
	$(LOCAL_PATH)/../external_deps/include/sdl_image  \
	$(LOCAL_PATH)/../external_deps/include/sdl_mixer  \
	$(LOCAL_PATH)/../external_deps/include/sdl_audio  \
	$(LOCAL_PATH)/../external_deps/include/sdl_video  \
	$(LOCAL_PATH)/../external_deps/include/sdl_events \
	$(LOCAL_PATH)/../external_deps/include/sdl_joystick 
	
LOCAL_LDFLAGS += \
	-L$(LOCAL_PATH)/../external_deps/lib \
	-llog -lz -ldl		  	\
	-lstlport  			\
	-lsdl				\
	-lsdl_image	\
	-ljpeg	-lpng \
	-lgcc	
	

LOCAL_LDLIBS :=  -L$(SYSROOT)/usr/lib -llog
#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -lGLESv2

# NDK v3/v4 for opengl v1.1
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -lGLESv1_CM

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libtigernump
include $(BUILD_SHARED_LIBRARY)

### END of Android.mk
