package com.example.mesen.demo;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class SurfaceViewRenderer implements SurfaceHolder.Callback, Runnable {

	SurfaceHolder surfaceHolder;
	Thread thread;
	boolean isRunning;

	Object frameMutex = new Object();
	int [] frameBuffer;
	int frameWidth, frameHeight;

	public SurfaceViewRenderer(SurfaceView surfaceView) {
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(this);
		frameWidth = 0;
		frameHeight = 0;
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
				// 创建Bitmap
				Bitmap bitmap = null;
				synchronized (this.frameMutex){
					bitmap =Bitmap.createBitmap(frameBuffer, frameWidth, frameHeight, Bitmap.Config.ARGB_8888);
				}

				int viewWidth = canvas.getWidth();
				int viewHeight = canvas.getHeight();

				// 计算缩放比例
				float scale = Math.min(
					(float) viewWidth / bitmap.getWidth(),
					(float) viewHeight / bitmap.getHeight()
				);

				// 计算目标绘制尺寸
				float drawWidth = bitmap.getWidth() * scale;
				float drawHeight = bitmap.getHeight() * scale;

				// 计算居中坐标
				float left = (viewWidth - drawWidth) / 2;
				float top = (viewHeight - drawHeight) / 2;
				float right = left + drawWidth;
				float bottom = top + drawHeight;


				// 绘制到目标区域（自动缩放与居中）
				Rect srcRect = null; // 全部源图像
				RectF dstRect = new RectF(left, top, right, bottom);
				canvas.drawBitmap(bitmap, srcRect, dstRect, null);

				// 回收bitmap（如需）
				bitmap.recycle();

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

	public void updateFrameBuffer(int []buffer, int width, int height) {
		synchronized (this.frameMutex) {
			frameBuffer = buffer;
			frameWidth = width;
			frameHeight = height;
		}
	}
}
