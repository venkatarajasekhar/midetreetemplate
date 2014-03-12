#define LOG_TAG "MidetreeJNI"
//#include "utils/Log.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <android/log.h>

#include <ContinuaManager.h>


using namespace android;

#define __DEBUG__

#ifdef __DEBUG__
#define LOGV(...) __android_log_print( ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__ )
#define LOGD(...) __android_log_print( ANDROID_LOG_DEBUG,  LOG_TAG, __VA_ARGS__ )
#define LOGI(...) __android_log_print( ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__ )
#define LOGW(...) __android_log_print( ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__ )
#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR,  LOG_TAG, __VA_ARGS__ )
#else
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

#define JNI_FALSE  0 
#define JNI_TRUE   1


#define INITIAL_STRONG_VALUE (1<<28)

static const char* const kClassName =
        "com/midetree/server/MidetreeService";
        
struct fields_t {
  jfieldID    context;  
  jfieldID    fidSData1;
  jmethodID   midSfunct1;  
};            

static fields_t fields;
static Mutex sLock;

class JNIContinuaManagerContext: public ContinuaManagerListener
{
public:
     JNIContinuaManagerContext(JNIEnv* env , jobject weak_this, jclass clazz);
     virtual ~JNIContinuaManagerContext() { release(); }
     
     virtual void notify(int32_t msgType);
     
     
     void release();
     
     //debug
    void printRefCount();  
     
    jobject mContinuaManagerJObjectWeak;     // weak reference to java object
    jclass  mContinuaManagerJClass;          // strong reference to java class
    
    Mutex       mLock;     
    
};    


JNIContinuaManagerContext::JNIContinuaManagerContext(JNIEnv* env , jobject weak_this, jclass clazz )
{
    LOGI("[%s] enter\n", __FUNCTION__);

    mContinuaManagerJObjectWeak = env->NewGlobalRef(weak_this);
    LOGI("[%s]: mContinuaManagerJObjectWeak[%p]\n", __FUNCTION__, mContinuaManagerJObjectWeak);

    mContinuaManagerJClass = (jclass)env->NewGlobalRef(clazz);
    LOGI("[%s]: mContinuaManagerJClass[%p]\n", __FUNCTION__, mContinuaManagerJClass);
}

void JNIContinuaManagerContext::release()
{
    LOGI("[%s] enter\n", __FUNCTION__);
    Mutex::Autolock _l(mLock);
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if (mContinuaManagerJObjectWeak != NULL) {
        env->DeleteGlobalRef(mContinuaManagerJObjectWeak);
        mContinuaManagerJObjectWeak = NULL;
    }

    if (mContinuaManagerJClass != NULL) {
        env->DeleteGlobalRef(mContinuaManagerJClass);
        mContinuaManagerJClass = NULL;
    }
 
}

void JNIContinuaManagerContext::notify(int32_t msgType)
{
    LOGI("[%s] enter\n", __FUNCTION__);

    // VM pointer will be NULL if object is released
    Mutex::Autolock _l(mLock);
    if (mContinuaManagerJObjectWeak == NULL) {
        LOGW("callback on dead continua manager object");
        return;
    }
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    /*
     * If the notification or msgType is CAMERA_MSG_RAW_IMAGE_NOTIFY, change it
     * to CAMERA_MSG_RAW_IMAGE since CAMERA_MSG_RAW_IMAGE_NOTIFY is not exposed
     * to the Java app.
     */

    env->CallStaticVoidMethod(mContinuaManagerJClass, fields.midSfunct1, msgType);
}

//debug
void JNIContinuaManagerContext::printRefCount()
{
    int32_t strong = getStrongCount();
    weakref_type *ref = getWeakRefs();
    
    LOGV("------------------------------------------------------\n");
    LOGV("[%s]: Strong Ref Count[%d].\n", __FUNCTION__, (strong == INITIAL_STRONG_VALUE ? 0 : strong));
    LOGV("[%s]: Weak Ref Count[%d].\n", __FUNCTION__, ref->getWeakCount());
    LOGV("------------------------------------------------------\n");    
        
}        
//=================================================================
// function
      
        
        
//=================================================================
//Common information

//Jni function implement
static void 
native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    LOGI("[%s] enter\n", __FUNCTION__);
    
    sp<ContinuaManager> manager = ContinuaManager::create();
    
    if (manager == NULL) {
        jniThrowRuntimeException(env, "Fail to create Continua Manager object");
        return;
    }
    
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        jniThrowRuntimeException(env, "Can't find com/midetree/server/MidetreeService");
        return;
    }
    
    sp<JNIContinuaManagerContext> context = new JNIContinuaManagerContext(env, weak_this, clazz);
    
    context->incStrong(thiz);
    manager->setListener(context);
    
     // save context in opaque field
    LOGI("[%s]: context.get()[%p]\n", __FUNCTION__, context.get()); 
    env->SetIntField(thiz, fields.context, (int)context.get());
    
}

static void 
native_release(JNIEnv *env, jobject thiz)
{
    LOGI("[%s] enter\n", __FUNCTION__);
    JNIContinuaManagerContext* context = NULL;

    {
        Mutex::Autolock _l(sLock);
        context = reinterpret_cast<JNIContinuaManagerContext*>(env->GetIntField(thiz, fields.context));

        // Make sure we do not attempt to callback on a deleted Java object.
        env->SetIntField(thiz, fields.context, 0);
    }

    // clean up if release has not been called before
    if (context != NULL) {
        context->release();
        LOGV("native_release: context=%p ", context);
        // remove context to prevent further Java access
        context->decStrong(thiz);
        context->printRefCount();
    }
    
    //clear ContinuaManager 
    sp<ContinuaManager> manager = ContinuaManager::create();    
    manager->printRefCount();    
    manager.clear();
    
}

static void 
function1(JNIEnv *env, jobject thiz)
{
    LOGI("[%s] enter\n", __FUNCTION__);
    
    sp<ContinuaManager> manager = ContinuaManager::create();
    
    manager->function1();
}

static void 
function2(JNIEnv *env, jobject thiz)
{
    LOGI("[%s] enter\n", __FUNCTION__);
    
    onCallbackFunct();
}


//=====================================================================================
/*
 * Array of methods.
 *
 * Each entry has three fields: the name of the method, the method
 * signature, and a pointer to the native implementation.
 */
static const JNINativeMethod gMethods[] = {
    {"_native_setup", "(Ljava/lang/Object;)V", (void*)native_setup},
    {"_native_release", "()V", (void*)native_release},
    {"_function1", "()V", (void*)function1},
    {"_function2", "()V", (void*)function2},
};

struct field {
    const char *class_name;
    const char *field_name;
    const char *field_type;
    jfieldID   *jfield;
};

static int find_fields(JNIEnv *env, field *fields, int count)
{
    for (int i = 0; i < count; i++) {
        field *f = &fields[i];
        jclass clazz = env->FindClass(f->class_name);
        if (clazz == NULL) {
            LOGE("Can't find %s", f->class_name);
            return -1;
        }

        jfieldID field = env->GetFieldID(clazz, f->field_name, f->field_type);
        if (field == NULL) {
            LOGE("Can't find %s.%s", f->class_name, f->field_name);
            return -1;
        }

        *(f->jfield) = field;
    }

    return 0;
}


static int registerMethods(JNIEnv* env) {
    
    jclass clazz;

    /* look up the class */
    clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        LOGE("Can't find class %s\n", kClassName);
        return -1;
    }


    /* register all the methods */
    if (env->RegisterNatives(clazz, gMethods,
            sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK)
    {
        LOGE("Failed registering methods for %s\n", kClassName);
        return -1;
    }
    
    //get java field id
    field fields_to_find[] = {
        { "com/midetree/server/MidetreeService", "mNativeContext",   "I", &fields.context },

    };

    if (find_fields(env, fields_to_find, NELEM(fields_to_find)) < 0)
        return -1;
    
    
    
    //get java method id
    fields.midSfunct1 = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(I)V");
    if (fields.midSfunct1 == NULL) {
        LOGE("Can't find android/hardware/Camera.postEventFromNative");
        return -1;
    }
    
    
    /* fill out the rest of the ID cache */
    return 0;
}

/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;
	LOGI("JNI_OnLoad");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        goto fail;
    }
    assert(env != NULL);

    if (registerMethods(env) != 0) {
        LOGE("ERROR: PlatformLibrary native registration failed\n");
        goto fail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

fail:
    return result;
}
