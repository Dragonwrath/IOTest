package com.junwtech.iotest.thread;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;



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
            if (dp.getAddress().getHostName().contains(".255"))
                mSocket.setBroadcast(true);
            mSocket.setSoTimeout(2000);
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
