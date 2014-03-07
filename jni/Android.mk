# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_LDLIBS := -llog -ldl

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libandroid_runtime \
	libcutils \
	libutils \
	libui

# No static libraries.
LOCAL_STATIC_LIBRARIES :=

# Also need the JNI headers.
LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	../native/libContinuaManager/

# No specia compiler flags.
LOCAL_CFLAGS +=


LOCAL_MODULE    := libmidetree_jni

LOCAL_SRC_FILES := midetree_jni.cpp

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true.
# LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
