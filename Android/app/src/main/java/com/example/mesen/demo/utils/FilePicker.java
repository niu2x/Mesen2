package com.example.mesen.demo.utils;

import static android.app.Activity.RESULT_OK;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import com.example.mesen.demo.RequestType;
import com.onehilltech.promises.Promise;


public class FilePicker {
	private static String TAG = FilePicker.class.getSimpleName();
	private final Activity context;
	private Promise<Uri> currentPendingRequest;
	private Promise.Settlement<Uri> currentPendingSettlement;
	public FilePicker(Activity activity) {
		this.context = activity;
	}
	public Promise<Uri> pickFile() {
		if(currentPendingRequest != null){
			return Promise.reject(new RuntimeException("Busy"));
		}
		currentPendingRequest = new Promise<>(settlement -> {
			currentPendingSettlement = settlement;
			Log.d(TAG, "set currentPendingSettlement");
			Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
			intent.addCategory(Intent.CATEGORY_OPENABLE);
			intent.setType("*/*");  // All file types
			FilePicker.this.context.startActivityForResult(intent, RequestType.PICK_FILE_REQUEST);
		});
		return currentPendingRequest;

	}

 	public void handleActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == RequestType.PICK_FILE_REQUEST) {
			Log.d(TAG, "currentPendingSettlement exist?: " + (currentPendingSettlement != null));
			Log.d(TAG, data.toString());

			if(resultCode == RESULT_OK && data != null) {
				Uri uri = data.getData();
				if(currentPendingSettlement != null) {
					currentPendingSettlement.resolve(uri);
					currentPendingSettlement = null;
				}
			}
			if(currentPendingSettlement != null) {
				Log.d(TAG, "reject");
				currentPendingSettlement.reject(new RuntimeException("Pick file fail"));
				currentPendingSettlement = null;
			}
			currentPendingRequest = null;
		}
	}


}
