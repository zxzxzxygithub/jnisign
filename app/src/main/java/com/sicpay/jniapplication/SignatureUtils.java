package com.sicpay.jniapplication;

import android.content.Context;

public class SignatureUtils {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    public static native String signatureParam(String params);
    public static native void signatureVerify(Context context);
}
