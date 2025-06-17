package com.example.mesen.demo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.SurfaceView;
import android.widget.TextView;

import com.example.mesen.demo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'demo' library on application startup.
    static {
//        System.loadLibrary("demo");
    }

    private ActivityMainBinding binding;
	 private SurfaceViewRenderer surfaceViewRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        TextView tv = binding.sampleText;
        tv.setText("a");

		  SurfaceView surfaceView = binding.surface;
		 surfaceViewRenderer = new SurfaceViewRenderer(surfaceView);
    }
}