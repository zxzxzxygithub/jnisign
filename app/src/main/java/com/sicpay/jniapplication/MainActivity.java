package com.sicpay.jniapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PackageInfo packageInfo=null;
        try{
            packageInfo=getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
        }
        catch(Exception e){
            e.printStackTrace();
        }
        Signature[] signatures=packageInfo.signatures;
        System.out.println("签名:"+ signatures[0].toCharsString());

        SignatureUtils.signatureVerify(this);
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        String str=SignatureUtils.signatureParam("user=123&password=123");
        tv.setText(str);
    }

}
