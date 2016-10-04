package com.junwtech.iotest.thread;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Handler;

import com.junwtech.iotest.event.ReceiveEvent;

import org.greenrobot.eventbus.EventBus;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.MulticastSocket;

/**
 * Created by JunWei on 2016/9/29.
 */

public class ReceiveThread extends Thread {

    private Handler mHandler;
    private DatagramSocket receiveSocket;
    private DatagramPacket receviePacket;
    private WifiManager.MulticastLock mWifiLock;

    private Context mContext;
    private  static ReceiveThread mInstance;

    public ReceiveThread(Context context) {
        mContext = context;
        WifiManager manager = (WifiManager) mContext
                .getSystemService(Context.WIFI_SERVICE);
        mWifiLock = manager.createMulticastLock("UDPwifi");
    }

    @Override
    public void run() {
        //接收
        while (true) {
            try {
                mWifiLock.acquire();
                receiveSocket = new DatagramSocket(50002);
                receiveSocket.setSoTimeout(10000);
                byte[] data = new byte[1024];
                receviePacket = new DatagramPacket(data,data.length);
                receiveSocket.receive(receviePacket);
                if (receviePacket != null)
                     EventBus.getDefault().post(new ReceiveEvent(receviePacket));
            } catch (Exception e){
                e.printStackTrace();
            } finally {
                if (mWifiLock.isHeld()) {
                    mWifiLock.release();
                }
                if (receiveSocket != null){
                    if (!receiveSocket.isClosed()){
                        receiveSocket.close();
                    }
                    receiveSocket = null;
                }
            }
        }

    }
}
