package com.example.mesen.demo;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.Button;
import com.example.mesen.demo.databinding.ActivityMainBinding;
import com.example.mesen.demo.utils.CopyExternalFileToLocalFilesDir;
import com.example.mesen.demo.utils.FilePicker;
import com.example.mesen.demo.utils.Tip;
import com.onehilltech.promises.Promise;

import android.content.Intent;
public class MainActivity extends AppCompatActivity {
	// Used to load the 'demo' library on application startup.
	static {
	// System.loadLibrary("demo");
	}

	private static final String TAG = MainActivity.class.getSimpleName();

	private SurfaceViewRenderer surfaceViewRenderer;

	private Button loadRomButton;
	private Button loadSaveButton;
	private Button resetButton;

	private FilePicker filePicker;
	private CopyExternalFileToLocalFilesDir copyExternalFileToLocalFilesDir;

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

		registerClickListeners();

		filePicker = new FilePicker(this);
		copyExternalFileToLocalFilesDir = new CopyExternalFileToLocalFilesDir(this, filePicker);
	}

	void registerClickListeners(){
		loadRomButton.setOnClickListener(view -> loadRom());
		loadSaveButton.setOnClickListener(view -> {
		});
		resetButton.setOnClickListener(view -> {
		});
	}

	void loadRom() {
	 	this.copyExternalFileToLocalFilesDir.pickFile().then((value)->{
			Tip.showTip("bytes: " + value);
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
}