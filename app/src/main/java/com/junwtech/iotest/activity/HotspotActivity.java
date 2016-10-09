package com.junwtech.iotest.activity;

import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.junwtech.iotest.R;
import com.junwtech.iotest.util.WifiApAdmin;

import java.lang.reflect.Method;
import java.util.regex.Pattern;

public class HotspotActivity extends AppCompatActivity {


    private TextView config_row1_title,config_row2_title,config_row3_title,config_row4_title;
    private EditText config_row1_content,config_row2_content,config_row3_content,config_row4_content;
    private WifiManager wifiManager;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hotspot);

        config_row1_title=(TextView)(findViewById(R.id.config_row1).findViewById(R.id.config_row_title_tv));
        config_row2_title=(TextView)(findViewById(R.id.config_row2).findViewById(R.id.config_row_title_tv));
//        config_row3_title=(TextView)(findViewById(R.id.config_row3).findViewById(R.id.config_row_title_tv));
//        config_row4_title=(TextView)(findViewById(R.id.config_row4).findViewById(R.id.config_row_title_tv));

        config_row1_content=(EditText)(findViewById(R.id.config_row1).findViewById(R.id.config_row_content_et));
        config_row2_content=(EditText)(findViewById(R.id.config_row2).findViewById(R.id.config_row_content_et));
//        config_row3_content=(EditText)(findViewById(R.id.config_row3).findViewById(R.id.config_row_content_et));
//        config_row4_content=(EditText)(findViewById(R.id.config_row4).findViewById(R.id.config_row_content_et));

        config_row1_title.setText("SSID");
        config_row2_title.setText("KEY");


        config_row1_content.setText("HotSpot");
        config_row2_content.setText("123456789");
        wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);


    }

    public void config(View view) {
        setWifiApEnabled(true);
    }

    private void setWifiApEnabled() {
        String ssid = config_row1_content.getText().toString().trim();
        String pwd = config_row2_content.getText().toString().trim();
        boolean valited = validateSSID$PWD(ssid, pwd);
        if (valited) {
            WifiApAdmin wifiAp = new WifiApAdmin(this);
            wifiAp.startWifiAp(ssid, pwd);
        }
    }

    private boolean validateSSID$PWD(String ssid,String pwd) {
        Boolean ssidValited = Pattern.matches("\\w{4,}", ssid);
        Boolean pwdValited =  Pattern.matches("[a-zA-Z|0-9]{6,}", pwd);

        if (ssidValited){
            if (pwdValited){
                return  true;
            } else {
                Toast.makeText(this, "密码的长度至少6位，只能包含A到Z或者a到z或者0到9的字符", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "SSID至少4位，只能包含A到Z或者a到z或者0到9的字符", Toast.LENGTH_SHORT).show();
        }

        return false;
    }


    // wifi热点开关
    public boolean setWifiApEnabled(boolean enabled) {
        if (enabled) { // disable WiFi in any case
            //wifi和热点不能同时打开，所以打开热点的时候需要关闭wifi
            wifiManager.setWifiEnabled(false);
        }
        String ssid = config_row1_content.getText().toString().trim();
        String pwd = config_row2_content.getText().toString().trim();
        if(validateSSID$PWD(ssid,pwd))
            try {
                //热点的配置类
                WifiConfiguration apConfig = new WifiConfiguration();
                //配置热点的名称(可以在名字后面加点随机数什么的)
                apConfig.SSID = ssid;
                //配置热点的密码
                apConfig.preSharedKey= pwd;
                //通过反射调用设置热点
                Method method = wifiManager.getClass().getMethod(
                        "setWifiApEnabled", WifiConfiguration.class, Boolean.TYPE);
                //返回热点打开状态

                return (Boolean) method.invoke(wifiManager, apConfig, enabled);
            } catch (Exception e) {
                e.printStackTrace();
            }
        return false;
    }
}
