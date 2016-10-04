package com.junwtech.iotest.util;


public class JniUtils {

    public static native String getStringFormC();
    public static native byte[] getAmlLoginNetByte(String mStrData);

    public static native byte[] encoder(String fixHeader, String ascii, String gb2312);
    public static native String decoder(byte[] netArry,int len);

    static {
        System.loadLibrary("amludp");
    }
}
