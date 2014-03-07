LOCAL_PATH := $(call my-dir)

# the library
# =============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
            $(call all-subdir-java-files)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := midetree_framework

include $(BUILD_JAVA_LIBRARY)



