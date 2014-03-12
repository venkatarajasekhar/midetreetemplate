// File: ContinuaManager.h  
#ifndef ANDROID_CONTINUA_MANAGER_H  
#define ANDROID_CONTINUA_MANAGER_H  
 
#include <utils/RefBase.h>  

#define INITIAL_STRONG_VALUE (1<<28)

void onCallbackFunct();
  
namespace android {  

class Mutex;

enum {
    CONTINUA_MANAGER_MSG_NOTIFY1 = 0x0001,            // notifyCallback
};

class ContinuaManagerListener : public RefBase
{
public:    
    virtual void notify(int32_t msgType) = 0;    
        
};

class ContinuaManager : public RefBase  
{  
public:
    static  int isCreate;
    static  sp<ContinuaManager>  instance;
    static  sp<ContinuaManager>  create();  
      
    virtual ~ContinuaManager(); 
    
    void function1();
    
    void setListener(const sp<ContinuaManagerListener>& listener);
    void notifyCallback(int32_t msgType);
    
    //debug
    void printRefCount();
   
    
private:
    ContinuaManager();
    
    sp<ContinuaManagerListener>  mListener;
    static Mutex mLock;     
};  

}; //namespace  
#endif  