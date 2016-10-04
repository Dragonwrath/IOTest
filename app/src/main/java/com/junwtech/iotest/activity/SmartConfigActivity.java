package com.junwtech.iotest.activity;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.junwtech.iotest.R;
import com.junwtech.iotest.base.BaseApplication;
import com.junwtech.iotest.bean.DataMember;
import com.junwtech.iotest.bean.DataModel;
import com.junwtech.iotest.bean.SmartConfigParam;
import com.junwtech.iotest.event.ReceiveEvent;
import com.junwtech.iotest.thread.SendThread;
import com.junwtech.iotest.util.DataUtil;
import com.junwtech.iotest.util.JniUtils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class SmartConfigActivity extends AppCompatActivity {


    private TextView confirm_row1_title,confirm_row2_title,confirm_row3_title,confirm_row4_title;
    private TextView confirm_row1_content,confirm_row2_content,confirm_row3_content,confirm_row4_content;
    private TextView config_row1_title,config_row2_title,config_row3_title,config_row4_title;
    private EditText config_row1_content,config_row2_content,config_row3_content,config_row4_content;

    public Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            int what = msg.what;
            Object obj = msg.obj;
            switch (what){
                case 0:
                    if (obj instanceof SmartConfigParam ){
                        SmartConfigParam param =  (SmartConfigParam)obj;
                        confirm_row1_content.setText(param.ssid);
                        confirm_row2_content.setText(param.key);
                    }
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_smart_config);


        ActionBar bar = getSupportActionBar();
        if (bar != null) {
            bar.setDisplayHomeAsUpEnabled(true);
            bar.setHomeButtonEnabled(true);
        }


        EventBus.getDefault().register(this);
        init();
    }


    private void init(){

        confirm_row1_title=(TextView)(findViewById(R.id.confirm_row1).findViewById(R.id.confirm_row_title_tv));
        confirm_row2_title=(TextView)(findViewById(R.id.confirm_row2).findViewById(R.id.confirm_row_title_tv));
        confirm_row3_title=(TextView)(findViewById(R.id.confirm_row3).findViewById(R.id.confirm_row_title_tv));
        confirm_row4_title=(TextView)(findViewById(R.id.confirm_row4).findViewById(R.id.confirm_row_title_tv));

        confirm_row1_content=(TextView)(findViewById(R.id.confirm_row1).findViewById(R.id.confirm_row_content_tv));
        confirm_row2_content=(TextView)(findViewById(R.id.confirm_row2).findViewById(R.id.confirm_row_content_tv));
        confirm_row3_content=(TextView)(findViewById(R.id.confirm_row3).findViewById(R.id.confirm_row_content_tv));
        confirm_row4_content=(TextView)(findViewById(R.id.confirm_row4).findViewById(R.id.confirm_row_content_tv));

        config_row1_title=(TextView)(findViewById(R.id.config_row1).findViewById(R.id.config_row_title_tv));
        config_row2_title=(TextView)(findViewById(R.id.config_row2).findViewById(R.id.config_row_title_tv));
        config_row3_title=(TextView)(findViewById(R.id.config_row3).findViewById(R.id.config_row_title_tv));
        config_row4_title=(TextView)(findViewById(R.id.config_row4).findViewById(R.id.config_row_title_tv));

        config_row1_content=(EditText)(findViewById(R.id.config_row1).findViewById(R.id.config_row_content_et));
        config_row2_content=(EditText)(findViewById(R.id.config_row2).findViewById(R.id.config_row_content_et));
        config_row3_content=(EditText)(findViewById(R.id.config_row3).findViewById(R.id.config_row_content_et));
        config_row4_content=(EditText)(findViewById(R.id.config_row4).findViewById(R.id.config_row_content_et));

        confirm_row1_title.setText("SSID");
        confirm_row2_title.setText("KEY");



        config_row1_title.setText("SSID");
        config_row2_title.setText("KEY");


        config_row1_content.setText("123456");
        config_row2_content.setText("123456");
        config_row3_content.setText("=");


    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id){
            case android.R.id.home:
                this.finish();
                break;
        }
        return super.onOptionsItemSelected(item);
    }



    @Override
    public void onBackPressed() {
        super.onBackPressed();
        this.finish();
    }

    public void config(View view) {
        String s = null;
        DataModel dm = null;
        try {
            SmartConfigParam param = new SmartConfigParam();
            param.type =1 ;
            param.uuid = "10007" ;
            param.ssid = config_row1_content.getText().toString().trim();
            param.key = config_row2_content.getText().toString().trim();
            dm =  DataUtil.getSmartConfigMsg(param, 0);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (dm != null) {
            Log.w(this.getPackageName(),"发送---->"+dm.fixHeader+"---"+dm.ascii+"-----"+dm.gb2312);
            byte[] bytes = JniUtils.encoder(dm.fixHeader,dm.ascii,dm.gb2312);
            Log.w(this.getPackageName(),"发送--->"+new String(bytes));
            try {
                String hostIp = getLocalIpAddress(this);
                int end = hostIp.lastIndexOf(".");
                String broacast = hostIp.substring(0,end)+".255";
                InetAddress ip = InetAddress.getByName(broacast);
                DatagramPacket dp = new DatagramPacket(bytes, bytes.length,ip,50001);
                BaseApplication.cachePool.execute(new SendThread(dp));
            } catch (UnknownHostException e) {
                e.printStackTrace();
            }
        }
    }

    public void confirm(View view) {

    }

    @Subscribe
    public void onReceiveEvent(ReceiveEvent event){
        if (event != null){
            DatagramPacket dp = event.getDp();
            byte[] data = dp.getData();
            String s = new String(data);
            Log.w(this.getPackageName(),"接收---->"+s);
            if (s.contains("SSID")) {
                SmartConfigParam param = new SmartConfigParam();
                int x= s.indexOf(":");
                int y = s.indexOf("@");
                int z = s.lastIndexOf(":");
                param.ssid = s.substring(x+1,y);
                param.key = s.substring(z+1,s.length());
                Message message = mHandler.obtainMessage();
                message.what = 0;
                message.obj = param ;
                mHandler.sendMessage(message);
            }
        }
    }

    private String getLocalIpAddress(Context context) {
        try {
            WifiManager wifiManager = (WifiManager) context
                    .getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            int i = wifiInfo.getIpAddress();
            return int2ip(i);
        } catch (Exception ex) {
            return " 获取IP出错!!!!请保证是WIFI,或者请重新打开网络!\n" + ex.getMessage();
        }
        // return null;
    }

    private String int2ip(int ipInt) {
        StringBuilder sb = new StringBuilder();
        sb.append(ipInt & 0xFF).append(".");
        sb.append((ipInt >> 8) & 0xFF).append(".");
        sb.append((ipInt >> 16) & 0xFF).append(".");
        sb.append((ipInt >> 24) & 0xFF);
        return sb.toString();
    }

}
