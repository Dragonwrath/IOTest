package com.junwtech.iotest.util;

import android.support.annotation.NonNull;

import com.junwtech.iotest.bean.DataMember;
import com.junwtech.iotest.bean.DataModel;
import com.junwtech.iotest.bean.SmartConfigParam;


public class DataUtil {
    public static  String DataGenerate(String address,String command,String qualifier,float data) throws Exception {
        return  "<"+address+"/"+command.toUpperCase()+qualifier+data+"\\r";
    }

    public static DataMember DataParse(String string){
        string = string.trim();
        if (string.startsWith(">")){
            string = string.substring(1,string.length());
            DataMember dataMember = new DataMember();
            dataMember.address = string.substring(0,4);
            string = string.substring(4,string.length());
            if (string.startsWith("/")){
                string = string.substring(1);
                dataMember.command = string.substring(0,4);
                dataMember.qualifier =string.substring(3,4);
                string = string.substring(4, string.length());
                if (string.contains("\n\r"))
                    dataMember.data= Float.parseFloat(string.replace("\n\r",""));
                return dataMember;
            }
        }
        return null;
    }

    /**
     * SmartConfig请求
     *
     * @param param
     * @param flag
     * @return
     */
    public static DataModel getSmartConfigMsg(SmartConfigParam param, int flag) {
        DataModel dm = new DataModel();
        dm.fixHeader = "0," + 0 + "," + flag;
        dm.ascii = param.getAsciiString();
        dm.gb2312 = param.getGb2312String();
        return dm;
    }
}
