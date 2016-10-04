package com.junwtech.iotest.bean;

/**
 * Created by AdamStone on 2016/7/6.
 * ASCII字符串说明：
 * 第一位是：报文类型
 * 1-SmartConfig请求；
 * 2- SmartConfig响应；
 * 3-查询用户状态请求；
 * 4-查询用户状态响应；
 * 5-配置用户权限。
 * 第二位：全球用户唯一ID
 * GB2312字符串说明：
 * 第一位是：WiFi SSID配置信息，SSID:名称_1@Key:密码
 */
public class SmartConfigParam  {

    public int type;
    public String uuid;

    public String ssid;
    public String key;

    public String getAsciiString() {
        return type + ","
                + uuid;
    }

    public String getGb2312String() {
        return "SSID:" + ssid + "@KEY:"
                + key;
    }
}
