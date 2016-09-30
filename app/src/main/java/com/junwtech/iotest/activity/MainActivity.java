package com.junwtech.iotest.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import com.junwtech.iotest.R;
import com.junwtech.iotest.event.ReceiveEvent;
import com.junwtech.iotest.thread.ReceiveThread;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;

import java.net.DatagramPacket;

public class MainActivity extends AppCompatActivity {

    private boolean relay;

    public Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        toolbar.setTitle(R.string.main);
        ActionBar bar = getSupportActionBar();

        EventBus.getDefault().register(this);

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
                startActivity(new Intent(this,SettingActivity.class));

                return  true;
        }
        return super.onOptionsItemSelected(item);
    }


    @Subscribe
    public void onReceiveEvent(ReceiveEvent event){
        if (event != null){
            DatagramPacket dp = event.getDp();
            byte[] data = dp.getData();
            String s = new String(data);
            if (s.contains(" >")) {


            }
        }
    }


}
