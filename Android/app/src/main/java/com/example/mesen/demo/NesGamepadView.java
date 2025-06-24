package com.example.mesen.demo;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import org.apache.commons.math3.geometry.euclidean.twod.Vector2D;

public class NesGamepadView extends View {
	public final int KEY_A = 0;
	public final int KEY_B = 1;
	public final int KEY_TURBO_A = 2;
	public final int KEY_TURBO_B = 3;
	public final int KEY_LEFT = 4;
	public final int KEY_RIGHT = 5;
	public final int KEY_UP = 6;
	public final int KEY_DOWN = 7;
	public final int KEY_SELECT = 8;
	public final int KEY_START = 9;
	public final int KEY_DIRECTION = 10;


	public class GamepadState {
		public boolean keyStates[] = new boolean[10];

		@Override
		public String toString() {
			StringBuffer sb = new StringBuffer();
			for(int i = 0; i < 10; i ++) {
				sb.append(keyStates[i]);
				sb.append(" ");
			}
			return sb.toString();
		}
	};


	public class KeyTouchingState {
		int pointerId = -1;

		Vector2D targetPos;
		double radius = 0;

	};

	private KeyTouchingState []keyTouchingStates = new KeyTouchingState[10];

	public interface NesGamepadStateCallback {
		void notifyNesGamepadStateChange(GamepadState gamepadState);
	}

	private class DiretionButtonTouchingState {
		int pointerId = -1;
		Vector2D targetPos;
		double radius = 0;

		Vector2D touchingPos;
	};

	DiretionButtonTouchingState diretionButtonTouchingState = new DiretionButtonTouchingState();

	private NesGamepadStateCallback nesGamepadStateCallback;
	private GamepadState gamepadState = new GamepadState();

	private Paint paint;
	private int circleColor;
	private int strokeColor;
	private float strokeWidth;

	public NesGamepadView(Context context) {
		super(context);
		init(null);
	}

	public NesGamepadView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(attrs);
	}

	public NesGamepadView(Context context, AttributeSet attrs, int defStyleAttr) {
		super(context, attrs, defStyleAttr);
		init(attrs);
	}

	private void init(AttributeSet attrs) {
		// 初始化画笔
		paint = new Paint(Paint.ANTI_ALIAS_FLAG);

		// 处理自定义属性
		if (attrs != null) {
			TypedArray a = getContext().obtainStyledAttributes(attrs, R.styleable.NesGamepadView);
			circleColor = a.getColor(R.styleable.NesGamepadView_circleColor, Color.RED);
			strokeColor = a.getColor(R.styleable.NesGamepadView_strokeColor, Color.BLACK);
			strokeWidth = a.getDimension(R.styleable.NesGamepadView_strokeWidth, 5f);
			a.recycle();
		}

		setClickable(true);
		for(int i = 0; i < 10; i ++) {
			keyTouchingStates[i] = new KeyTouchingState();
			gamepadState.keyStates[i] = false;
		}


	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		// 计算圆心和半径
		double directionPadRadius = calculateDirectionButtonRadius();
		Vector2D directionPadPos = calculateDirectionButtonPosition();
		if(diretionButtonTouchingState.touchingPos != null) {
			directionPadPos = diretionButtonTouchingState.touchingPos;
		}

		if(diretionButtonTouchingState.touchingPos != null)
		Log.d("nnnn", diretionButtonTouchingState.touchingPos.toString());

		// 绘制填充圆
		paint.setStyle(Paint.Style.FILL);
		paint.setColor(circleColor);
		canvas.drawCircle((int)directionPadPos.getX(), (int)directionPadPos.getY(), (int) directionPadRadius, paint);

		Vector2D []otherButtonPositions = calculateOtherButtonPositions();
		double otherButtonRadius = calculateOtherButtonRadius();
		for (Vector2D pos	:otherButtonPositions) {
			canvas.drawCircle((int)pos.getX(), (int)pos.getY(), (int) otherButtonRadius, paint);
		}
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		super.onSizeChanged(w, h, oldw, oldh);


		diretionButtonTouchingState.targetPos = calculateDirectionButtonPosition();
		diretionButtonTouchingState.radius = calculateDirectionButtonRadius();

		Vector2D[] positions = calculateOtherButtonPositions();
		double otherButtonRadius = calculateOtherButtonRadius();

		keyTouchingStates[KEY_TURBO_B].targetPos = positions[0];
		keyTouchingStates[KEY_TURBO_B].radius = otherButtonRadius;

		keyTouchingStates[KEY_TURBO_A].targetPos = positions[1];
		keyTouchingStates[KEY_TURBO_A].radius = otherButtonRadius;

		keyTouchingStates[KEY_B].targetPos = positions[2];
		keyTouchingStates[KEY_B].radius = otherButtonRadius;

		keyTouchingStates[KEY_A].targetPos = positions[3];
		keyTouchingStates[KEY_A].radius = otherButtonRadius;
	}

	private Vector2D calculateDirectionButtonPosition() {
		return new Vector2D(getWidth() / 4.0, getHeight()/2.0);
	}

	private double calculateDirectionButtonRadius() {
		return Math.min(getWidth(), getHeight()) / 8.0;
	}

	private Vector2D[] calculateOtherButtonPositions() {
		Vector2D[] positions = new Vector2D[4];

		Vector2D center = new Vector2D(getWidth() / 4.0*3.0, getHeight()/2.0);

		double step = calculateOtherButtonDistance();
		positions[0] = center.add(new Vector2D(-1.0, 1.0).normalize().scalarMultiply(step));
		positions[1] = center.add(new Vector2D(1.0, 1.0).normalize().scalarMultiply(step));
		positions[2] = center.add(new Vector2D(-1.0, -1.0).normalize().scalarMultiply(step));
		positions[3] = center.add(new Vector2D(1.0, -1.0).normalize().scalarMultiply(step));
		return positions;
	}

	private double calculateOtherButtonRadius() {
		return Math.min(getWidth(), getHeight()) / 16.0;
	}
	private double calculateOtherButtonDistance() {
		return Math.min(getWidth(), getHeight()) / 6.0;
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		int action = event.getActionMasked();
		int pointerIndex = event.getActionIndex(); // 获取触发事件的指针索引
		int pointerId = event.getPointerId(pointerIndex); // 获取指针ID

		switch (action) {
			case MotionEvent.ACTION_DOWN:
			case MotionEvent.ACTION_POINTER_DOWN: // 额外的指针按下
				handlePointerDown(pointerId, event.getX(pointerIndex), event.getY(pointerIndex));
				return true;

			case MotionEvent.ACTION_MOVE:
				// 遍历所有活动指针
				for (int i = 0; i < event.getPointerCount(); i++) {
					int currentPointerId = event.getPointerId(i);
					handlePointerMove(currentPointerId, event.getX(i), event.getY(i));
				}
				return true;

			case MotionEvent.ACTION_UP:
			case MotionEvent.ACTION_POINTER_UP: // 非最后一个指针抬起
				handlePointerUp(pointerId, event.getX(pointerIndex), event.getY(pointerIndex));
				return true;

			case MotionEvent.ACTION_CANCEL:
				handleAllPointersUp();
				return true;
		}
		// 处理触摸事件
		return super.onTouchEvent(event);
	}

	private void fireGamepadState() {
		if(nesGamepadStateCallback != null) {
			nesGamepadStateCallback.notifyNesGamepadStateChange(gamepadState);
			invalidate();
		}
	}

	private void handleAllPointersUp() {
		for(int i = 0; i < 10; i ++) {
			keyTouchingStates[i].pointerId = -1;
			gamepadState.keyStates[i] = false;
		}

		diretionButtonTouchingState.touchingPos = null;
		diretionButtonTouchingState.pointerId = -1;
		gamepadState.keyStates[KEY_UP] = false;
		gamepadState.keyStates[KEY_DOWN] = false;
		gamepadState.keyStates[KEY_LEFT] = false;
		gamepadState.keyStates[KEY_RIGHT] = false;

		fireGamepadState();
	}

	private void handlePointerMove(int currentPointerId, float x, float y) {
		for(int i = 0; i < 10; i ++) {
			if(keyTouchingStates[i].pointerId == currentPointerId) {
				if(keyTouchingStates[i].targetPos.distance(new Vector2D(x, y)) > keyTouchingStates[i].radius) {
					keyTouchingStates[i].pointerId = -1;
					gamepadState.keyStates[i] = false;
					fireGamepadState();
				}
				break;
			}
		}

		if(diretionButtonTouchingState.pointerId == currentPointerId) {
			if(diretionButtonTouchingState.targetPos.distance(new Vector2D(x, y)) > diretionButtonTouchingState.radius) {
			}
			else {
				diretionButtonTouchingState.touchingPos = new Vector2D(x, y);
			}
			updateDirectionKeyStates(x, y);
			fireGamepadState();
		}
	}

	private void handlePointerUp(int pointerId, float x, float y) {
		for(int i = 0; i < 10; i ++) {
			if(keyTouchingStates[i].pointerId == pointerId) {
				keyTouchingStates[i].pointerId = -1;
				gamepadState.keyStates[i] = false;
				fireGamepadState();
				break;
			}
		}

		if(diretionButtonTouchingState.pointerId == pointerId) {
			diretionButtonTouchingState.pointerId = -1;
			diretionButtonTouchingState.touchingPos = null;
			gamepadState.keyStates[KEY_UP] = false;
			gamepadState.keyStates[KEY_DOWN] = false;
			gamepadState.keyStates[KEY_LEFT] = false;
			gamepadState.keyStates[KEY_RIGHT] = false;
			fireGamepadState();
		}
	}

	public void setNesGamepadStateCallback(NesGamepadStateCallback callback) {
		nesGamepadStateCallback = callback;
	}

	private void handlePointerDown(int pointerId, float x, float y) {
		for(int i = 0; i < 10; i ++) {
			if(keyTouchingStates[i].pointerId == -1) {
				if(keyTouchingStates[i].radius > 0) {
					if(keyTouchingStates[i].targetPos.distance(new Vector2D(x, y)) < keyTouchingStates[i].radius) {
						keyTouchingStates[i].pointerId = pointerId;
						gamepadState.keyStates[i] = true;
						fireGamepadState();
						break;
					}
				}
			}
		}

		if(diretionButtonTouchingState.pointerId == -1) {
			if(diretionButtonTouchingState.radius > 0) {
				if(diretionButtonTouchingState.targetPos.distance(new Vector2D(x, y)) < diretionButtonTouchingState.radius) {
					diretionButtonTouchingState.pointerId = pointerId;
					diretionButtonTouchingState.touchingPos = new Vector2D(x, y);
					updateDirectionKeyStates(x, y);
					fireGamepadState();
				}
			}
		}
	}

	private void updateDirectionKeyStates(float x, float y) {
		gamepadState.keyStates[KEY_RIGHT] = x > diretionButtonTouchingState.targetPos.getX();
		gamepadState.keyStates[KEY_LEFT] = x < diretionButtonTouchingState.targetPos.getX();
		gamepadState.keyStates[KEY_DOWN] = y > diretionButtonTouchingState.targetPos.getY();
		gamepadState.keyStates[KEY_UP] = y < diretionButtonTouchingState.targetPos.getY();
	}
}
