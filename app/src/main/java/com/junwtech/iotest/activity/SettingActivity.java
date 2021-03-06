package com.junwtech.iotest.activity;

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
import com.junwtech.iotest.event.ReceiveEvent;
import com.junwtech.iotest.thread.ReceiveThread;
import com.junwtech.iotest.thread.SendThread;
import com.junwtech.iotest.util.DataUtil;

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
                    if (obj instanceof DataMember){
                        DataMember args = (DataMember) obj;
                        confirm_row1_content.setText(args.address);
                        confirm_row2_content.setText(args.command);
                        confirm_row3_content.setText(args.qualifier);
                        confirm_row4_content.setText(String.valueOf(args.data));
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

        confirm_row1_title.setText("Mac地址");
        confirm_row2_title.setText("命令");
        confirm_row3_title.setText("限定符");
        confirm_row4_title.setText("数据");

        config_row1_title.setText("Mac地址");
        config_row2_title.setText("命令");
        config_row3_title.setText("限定符");
        config_row4_title.setText("数据");

        config_row2_content.setText("RSW");
        config_row3_content.setText("=");
        config_row4_content.setText("1949.2345");

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
        String s = null;
        try {
            s = DataUtil.DataGenerate(
                    config_row1_content.getText().toString(),
                    config_row2_content.getText().toString(),
                    config_row3_content.getText().toString(),
                    Float.parseFloat(config_row4_content.getText().toString()));
            Log.w(this.getPackageName(),"---->"+s);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (s !=null) {
            byte[] bytes = s.getBytes();
            try {
                if (BaseApplication.hostIp.equals("")) {
                    InetAddress ip = InetAddress.getByName(BaseApplication.hostIp);
                    DatagramPacket dp = new DatagramPacket(bytes, bytes.length,ip,50002);
                    BaseApplication.cachePool.execute(new SendThread(dp));

                } else {
                    Toast.makeText(SettingActivity.this, "没有获得有效的地址", Toast.LENGTH_SHORT).show();
                    throw new UnknownHostException("没有获得有效的地址");
                }
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
            InetAddress address = dp.getAddress();
            BaseApplication.hostIp = address.getHostAddress();
            Toast.makeText(this,BaseApplication.hostIp,Toast.LENGTH_SHORT).show();
            String s = new String(data);
            if (s.startsWith(">")) {
                DataMember dataMember = DataUtil.DataParse(s);
                if (dataMember != null) {
                    Message message = mHandler.obtainMessage();
                    message.what = 0;
                    message.obj = dataMember;
                    mHandler.sendMessage(message);
                }
            }
        }
    }
}
