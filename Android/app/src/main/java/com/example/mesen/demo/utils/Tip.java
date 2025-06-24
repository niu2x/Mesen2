package com.example.mesen.demo.utils;

import android.app.Activity;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

public class Tip {
	private static final String TAG = Tip.class.getSimpleName();
	private static Activity context;
	public static void setContext(Activity activity) {
		context = activity;
	}
	public static void showTip(String message) {
		new Handler(context.getMainLooper()).post(() -> Toast.makeText(context, message, Toast.LENGTH_SHORT).show());
	}
}
