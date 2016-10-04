package com.junwtech.iotest.base;

import android.app.Application;

import com.junwtech.iotest.thread.ReceiveThread;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by JunWei on 2016/9/30.
 */

public class BaseApplication extends Application {
    public static String hostIp="192.168.0.103";
    public static boolean relay;
    public static ExecutorService cachePool =  Executors.newSingleThreadExecutor();

    @Override
    public void onCreate() {
        super.onCreate();
    }
}
