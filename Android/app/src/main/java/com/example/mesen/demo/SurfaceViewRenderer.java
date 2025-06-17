package com.example.mesen.demo;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class SurfaceViewRenderer implements SurfaceHolder.Callback, Runnable {

	SurfaceHolder surfaceHolder;
	Thread thread;
	boolean isRunning;
	public SurfaceViewRenderer(SurfaceView surfaceView) {
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(this);
	}
	@Override
	public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
		Log.d("Demo", "surfaceCreated");

		isRunning = true;
		thread = new Thread(this);
		thread.start();
	}

	@Override
	public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
		Log.d("Demo", "surfaceChanged");
	}

	@Override
	public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
		Log.d("Demo", "surfaceDestroyed");
		isRunning = false;
		try {
			thread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void run() {
		while (isRunning) {
			draw();
		}
	}
	private void draw() {
		Canvas canvas = null;
		try {
			canvas = surfaceHolder.lockCanvas();
			if (canvas != null) {
				// 在这里绘制内容
				canvas.drawColor(Color.WHITE);
				Paint paint = new Paint();
				paint.setColor(Color.RED);
				canvas.drawCircle(100, 100, 50, paint);
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		finally {
			if (canvas != null) {
				surfaceHolder.unlockCanvasAndPost(canvas);
			}
		}
	}
}
