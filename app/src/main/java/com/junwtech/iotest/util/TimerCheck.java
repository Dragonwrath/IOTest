package com.junwtech.iotest.util;

import android.support.annotation.WorkerThread;

/**
 * Created by JunWei on 2016/10/9.
 */

public abstract class TimerCheck {
    private int mCount = 0 ;
    private int mTimeOutCount =  1;
    private int mSleepTime = 1000;
    private boolean mExitFlag = false;
    private Thread mThread= null;

    public abstract  void doTimerCheckWork();

    public abstract  void doTimeOutWork();

    public TimerCheck(){
        mThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (!mExitFlag){
                    mCount++;
                    if (mCount < mTimeOutCount){
                        doTimerCheckWork();
                        try {
                            mThread.sleep(mSleepTime);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                            exit();
                        }
                    } else {
                        doTimeOutWork();
                    }
                }
            }
        });
    }

    public void start(int timeOutCount,int sleepTime){
        mTimeOutCount = timeOutCount;
        mSleepTime = sleepTime;
        mThread.start();
    }

    public void exit(){
        mExitFlag = true;
    }
}
