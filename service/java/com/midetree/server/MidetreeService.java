package com.midetree.server;

/**
 * Class that manages the device's sensors. It register clients and activate
 * the needed sensors. The sensor events themselves are not broadcasted from
 * this service, instead, a file descriptor is provided to each client they
 * can read events from.
 */
import android.content.Context;
import android.util.Log;


public final class MidetreeService {
    static final String TAG = MidetreeService.class.getSimpleName();
    private static final boolean DEBUG = true;


    static {
//        System.load("/system/lib/libatkdroid_runtime.so");
        System.loadLibrary("midetree_jni");
    }

   
    @SuppressWarnings("unused")
    public MidetreeService(Context context) {
       printI("MidetreeService startup");

    }
    
    
    public void function1() {
       printI("function1 ");
       
       _function1();    
        
    }
    
    
    private void printI(String str) {
     
        if(DEBUG) Log.i(TAG, str);
        
    }    
    
    
     // ---------------------------------------------------------
    // Java methods called from the native side
    // --------------------
    
    
    
    
    // ---------------------------------------------------------
    // native methods called from the java side
    // --------------------    
    private native final void _function1();
}
