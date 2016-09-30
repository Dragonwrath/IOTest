package com.junwtech.iotest.event;

import java.net.DatagramPacket;

/**
 * Created by JunWei on 2016/9/30.
 */

public class ReceiveEvent  {
    DatagramPacket dp;

    public ReceiveEvent(DatagramPacket dp) {
        this.dp = dp;
    }

    public DatagramPacket getDp() {
        return dp;
    }
}
