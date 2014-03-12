package com.midetree.server;

/**
 * Class that manages the device's sensors. It register clients and activate
 * the needed sensors. The sensor events themselves are not broadcasted from
 * this service, instead, a file descriptor is provided to each client they
 * can read events from.
 */
import android.content.Context;
import android.util.Log;

import java.lang.ref.WeakReference;


public final class MidetreeService {
    static final String TAG = MidetreeService.class.getSimpleName();
    private static final boolean DEBUG = true;

    // These match the enums in vendor\altek\externel\midetree\native\libContinuaManager\ContinuaManager.h
    private static final int CONTINUA_MANAGER_MSG_NOTIFY1 = 0x0001;


    static {
//        System.load("/system/lib/libatkdroid_runtime.so");
        System.loadLibrary("midetree_jni");
    }


    private static onJNIListener mListener = null;
    
    private int mNativeContext; // accessed by native methods
   
    @SuppressWarnings("unused")
    public MidetreeService(Context context) {
       printI("MidetreeService startup");
       _native_setup(new WeakReference<MidetreeService>(this));

    }
    
    public void setOnJNIListener(onJNIListener listener) {
        
        mListener = listener;
    }
    
    
    public void function1() {
       printI("function1 ");
       
       _function1();    
        
    }
    
    public void function2() {
       printI("function2 ");
       
       _function2();    
        
    }
    
    public void release() {
       printI("release ");
       
       _native_release();    
        
    }
    
    
    private static void printI(String str) {
     
        if(DEBUG) Log.i(TAG, str);
        
    }    
    
    
    // ---------------------------------------------------------
    // Java methods called from the native side
    // --------------------
    private static void postEventFromNative(int what, int arg1, int arg2, int arg3, int arg4)
    {
        printI("postEventFromNative ");
        
        switch(what) {
        case CONTINUA_MANAGER_MSG_NOTIFY1:
            mListener.onCallBackFunc1(arg1, arg2, arg3, arg4);
        break;    
        default:
        break;    
        }    
        
    }
    
    
    public interface onJNIListener {
        
        void onCallBackFunc1(int arg1, int arg2, int arg3, int arg4);
        
    }
    
    
    // ---------------------------------------------------------
    // native methods called from the java side
    // --------------------
    private native final void _native_setup(Object mindtree_this);
    private native final void _native_release();    
    private native final void _function1();
    private native final void _function2();
    
}
