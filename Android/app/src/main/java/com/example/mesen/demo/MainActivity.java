package com.example.mesen.demo;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.Button;
import com.example.mesen.demo.databinding.ActivityMainBinding;
import com.example.mesen.demo.utils.CopyExternalFileToLocalFilesDir;
import com.example.mesen.demo.utils.FilePicker;
import com.example.mesen.demo.utils.Tip;
import com.onehilltech.promises.Promise;

import android.content.Intent;
public class MainActivity extends AppCompatActivity implements MesenAPI.NotificationCallback, NesGamepadView.NesGamepadStateCallback {
	// Used to load the 'demo' library on application startup.
	static {
		System.loadLibrary("MesenRT");
		System.loadLibrary("MesenAndroid");
	}

	private static final String TAG = MainActivity.class.getSimpleName();

	private SurfaceViewRenderer surfaceViewRenderer;

	private Button loadRomButton;
	private Button loadSaveButton;
	private Button resetButton;

	private Button selectButton;
	private Button startButton;

	private FilePicker filePicker;
	private CopyExternalFileToLocalFilesDir copyExternalFileToLocalFilesDir;
	private NesGamepadView nesGamepadView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Tip.setContext(this);

		ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
		setContentView(binding.getRoot());

		SurfaceView surfaceView = binding.surface;
		surfaceViewRenderer = new SurfaceViewRenderer(surfaceView);

		loadRomButton = binding.loadRom;
		loadSaveButton = binding.loadSave;
		resetButton = binding.reset;
		nesGamepadView = binding.nesGamepad;
		selectButton = binding.selectBtn;
		startButton = binding.startBtn;

		registerClickListeners();

		filePicker = new FilePicker(this);
		copyExternalFileToLocalFilesDir = new CopyExternalFileToLocalFilesDir(this, filePicker);

		initMesen();
	}

	void initMesen() {
		MesenAPI.init();
		MesenAPI.initializeEmu(getFilesDir().getAbsolutePath(), true, false, false);
		MesenAPI.registerNotificationCallback(this);
	}

	void registerClickListeners(){
		loadRomButton.setOnClickListener(view -> loadRom());
		loadSaveButton.setOnClickListener(view -> {
		});
		resetButton.setOnClickListener(view -> {
		});
		selectButton.setOnClickListener(view->{
			MesenAPI.setKeyState(NesGamepadView.KEY_SELECT, true);
			new Handler(getMainLooper()).postDelayed(new Runnable() {
				@Override
				public void run() {
					MesenAPI.setKeyState(NesGamepadView.KEY_SELECT, false);
				}
			}, 100);
		});
		startButton.setOnClickListener(view->{
			MesenAPI.setKeyState(NesGamepadView.KEY_START, true);
			new Handler(getMainLooper()).postDelayed(new Runnable() {
				@Override
				public void run() {
					MesenAPI.setKeyState(NesGamepadView.KEY_START, false);
				}
			}, 100);
		});
		nesGamepadView.setNesGamepadStateCallback(this);
	}

	void loadRom() {
	 	this.copyExternalFileToLocalFilesDir.pickFile().then((value)->{
			MesenAPI.loadROM(value, null);
			return Promise.resolve(Void.class);

		})._catch((err)->{
			Log.d(TAG, "loadRom err: " + err.toString());
			Tip.showTip(err.getLocalizedMessage());
			return Promise.reject(err);
		});
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		filePicker.handleActivityResult(requestCode, resultCode, data);
	}

	@Override
	public void notifyRefreshSoftwareRenderer(int[] buffer, int width, int height) {
		surfaceViewRenderer.updateFrameBuffer(buffer, width, height);
	}

	@Override
	public void notifyNesGamepadStateChange(NesGamepadView.GamepadState gamepadState) {
		for(int i = 0; i < 10; i ++) {
			MesenAPI.setKeyState(i, gamepadState.keyStates[i]);
		}
	}
}