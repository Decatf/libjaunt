LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CLANG := false

LOCAL_STRIP_MODULE := keep_symbols

LOCAL_SRC_FILES := \
	jaunt.c

LOCAL_CFLAGS += -Werror -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16
LOCAL_CFLAGS += -fpic

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_MODULE := libjaunt
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES:= libcutils


include $(BUILD_SHARED_LIBRARY)
