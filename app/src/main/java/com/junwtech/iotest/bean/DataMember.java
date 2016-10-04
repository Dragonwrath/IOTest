package com.junwtech.iotest.bean;

import java.io.Serializable;

public class DataMember implements Serializable {
    public String address;
    public String command;
    public String qualifier;
    public float data;

    public DataMember(){
    }

    public DataMember(String command, String address, String qualifier, float data) {
        this.command = command;
        this.address = address;
        this.qualifier = qualifier;
        this.data = data;
    }


}
