package com.junwtech.iotest.activity;

import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.junwtech.iotest.R;
import com.junwtech.iotest.base.BaseApplication;
import com.junwtech.iotest.event.ReceiveEvent;
import com.junwtech.iotest.thread.ReceiveThread;
import com.junwtech.iotest.thread.SendThread;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.w3c.dom.Text;

import java.net.DatagramPacket;
import java.net.InetAddress;

public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {

    private boolean relay;

    private Switch mSwitch;
    private TextView mTextView;
    public Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 0) {
                mTextView.setText(new String((byte[])msg.obj));
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        mTextView = (TextView) findViewById(R.id.text_main);
        setSupportActionBar(toolbar);

        toolbar.setTitle(R.string.main);
        ActionBar bar = getSupportActionBar();

        EventBus.getDefault().register(this);

        mSwitch = (Switch) findViewById(R.id.switch_main);
        mSwitch.setOnCheckedChangeListener(this);
        new ReceiveThread(getApplicationContext()).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id){
            case R.id.action_settings:
                startActivity(new Intent(this,SmartConfigActivity.class));
                return  true;
        }
        return super.onOptionsItemSelected(item);
    }


    @Subscribe
    public void onReceiveEvent(ReceiveEvent event){
        if (event != null){
            if (BaseApplication.relay) {
                DatagramPacket dp = event.getDp();
                InetAddress dest = dp.getAddress();
                BaseApplication.hostIp = dest.getHostAddress();
                byte[] data = dp.getData();
                Message message = mHandler.obtainMessage();
                message.what = 0;
                message.obj = data;
                mHandler.sendMessage(message);
                String s = new String(data);
                if (s.startsWith(">")) {
                    s = s.replace(">", "<");
                    byte[] bytes = s.getBytes();
                    dp = new DatagramPacket(bytes, bytes.length,dest,50001);
                    BaseApplication.cachePool.execute(new SendThread(dp));
                }
            }
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView.getId() == R.id.switch_main ){
            Toast.makeText(this,""+isChecked,Toast.LENGTH_SHORT).show();
            if (isChecked){
                BaseApplication.relay = true ;
            } else {
                BaseApplication.relay = false;
            }
        }
    }
}
