package com.junwtech.iotest.thread;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

/**
 * Created by JunWei on 2016/9/30.
 */

public class SendThread extends Thread {

    private DatagramPacket dp;
    private DatagramSocket mSocket;

    public SendThread(DatagramPacket dp) {
        this.dp = dp;
    }

    @Override
    public void run() {
        try {
            mSocket = new DatagramSocket();
            if (dp != null)
                mSocket.send(dp);

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (mSocket != null){
                mSocket.close();
                mSocket = null;
            }
        }
    }
}
