// File: HapticService.cpp
#define LOG_TAG "ContinuaManager"
#include <android/log.h>
#include <utils/threads.h>
  
#include "ContinuaManager.h"

namespace android {
    
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
      

Mutex ContinuaManager::mLock;
int ContinuaManager::isCreate = 0;
sp<ContinuaManager> ContinuaManager::instance = NULL;        

// construct a Continua Manager object
sp<ContinuaManager> ContinuaManager::create()
{
    LOGV("create");   
    if (!isCreate) {
        LOGV("[%s]: construct ContinuaManager.\n", __FUNCTION__); 
//        sp<ContinuaManager> c = new ContinuaManager();
        instance = new ContinuaManager();
        isCreate = 1;
//        instance =  c;
    }
    LOGV("[%s]: instance.get() = %p\n", __FUNCTION__, instance.get());  
    return instance;
}

  
ContinuaManager::ContinuaManager()  
{   
    LOGV("[%s] enter\n", __FUNCTION__);
        
 
          
}  
  
ContinuaManager::~ContinuaManager()  
{  
    LOGV("[%s] enter\n", __FUNCTION__);  
    isCreate = 0;
    instance = NULL;
}  
 
 
void ContinuaManager::function1()
{
    LOGV("[%s] enter\n", __FUNCTION__);    
         
}

void ContinuaManager::setListener(const sp<ContinuaManagerListener>& listener)
{
    LOGV("[%s] enter\n", __FUNCTION__);    
    Mutex::Autolock _l(mLock);
    mListener = listener;
    LOGV("[%s]: mListener[%p]\n", __FUNCTION__, mListener.get());
}

// callback from mindtree lib
void ContinuaManager::notifyCallback(int32_t msgType)
{
    LOGV("[%s] enter\n", __FUNCTION__);
    LOGV("[%s]: msgType[%d]\n", __FUNCTION__, msgType);    
    sp<ContinuaManagerListener> listener;
    {
        Mutex::Autolock _l(mLock);
        listener = mListener;
    }
    if (listener != NULL) {
        listener->notify(msgType);
    }
}

//debug
void ContinuaManager::printRefCount()
{
    int32_t strong = getStrongCount();
    weakref_type *ref = getWeakRefs();
    
    LOGV("------------------------------------------------------\n");
    LOGV("[%s]: Strong Ref Count[%d].\n", __FUNCTION__, (strong == INITIAL_STRONG_VALUE ? 0 : strong));
    LOGV("[%s]: Weak Ref Count[%d].\n", __FUNCTION__, ref->getWeakCount());
    LOGV("------------------------------------------------------\n");    
        
}


}; //namespace

using namespace android;

// callback from mindtree lib
void onCallbackFunct()
{
    LOGV("[%s] enter\n", __FUNCTION__);    
    sp<ContinuaManager> manager = ContinuaManager::create();
    
    manager->notifyCallback(CONTINUA_MANAGER_MSG_NOTIFY1);      
    
}      