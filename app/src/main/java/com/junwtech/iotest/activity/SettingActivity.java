package com.junwtech.iotest.activity;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.junwtech.iotest.R;
import com.junwtech.iotest.event.ReceiveEvent;
import com.junwtech.iotest.thread.ReceiveThread;
import com.junwtech.iotest.thread.SendThread;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.UnknownHostException;

public class SettingActivity extends AppCompatActivity {

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
                    if (obj instanceof String[]){
                        String[] args = (String[]) obj;
                        confirm_row1_content.setText(args[0]);
                        confirm_row2_content.setText(args[1]);
                        confirm_row3_content.setText(args[2]);
                        confirm_row4_content.setText(args[3]);
                    }
                    break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

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

        confirm_row1_title.setText("IP地址");

        config_row1_title.setText("IP地址");
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
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
        String s = "1,1,1,1";
        byte[] bytes = s.getBytes();
        try {
            InetAddress ip = InetAddress.getByName("192.168.0.112");
            DatagramPacket dp = new DatagramPacket(bytes,bytes.length);
            dp.setAddress(ip);
            dp.setPort(50000);
            SendThread sendThread = new SendThread(dp);
            sendThread.start();
        } catch (UnknownHostException e) {
            e.printStackTrace();
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
            String[] args = s.split(",");
            Message message = mHandler.obtainMessage();
            message.what = 0 ;
            message.obj = args;
            mHandler.sendMessage(message);
        }
    }
}
