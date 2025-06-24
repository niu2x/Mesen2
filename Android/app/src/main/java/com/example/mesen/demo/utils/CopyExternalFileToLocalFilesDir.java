package com.example.mesen.demo.utils;

import android.app.Activity;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.OpenableColumns;
import android.util.Log;

import com.onehilltech.promises.Promise;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class CopyExternalFileToLocalFilesDir {
	private static final String TAG = CopyExternalFileToLocalFilesDir.class.getSimpleName();
	private final Activity context;
	private final FilePicker filePicker;
	public CopyExternalFileToLocalFilesDir(Activity activity, FilePicker filePicker) {
		this.context = activity;
		this.filePicker = filePicker;
	}

	private String writeBytesToFilesDir(byte[] data, String fileName) throws IOException {
		File file = new File(context.getFilesDir(), fileName);
		try (FileOutputStream fos = new FileOutputStream(file)) {
			fos.write(data);
			fos.flush();
		}
		return file.getAbsolutePath();
	}

	public Promise<String> pickFile() {
		return this.filePicker.pickFile().then((uri)-> new Promise<>(settlement -> {
			Log.d(TAG, "uri: " + uri.toString());
			byte[] content = readFileAsBinary(uri);
			Log.d(TAG, "content: " + content.length + " bytes");
			String fileName = getFileName(context, uri);
			Log.d(TAG, "fileName: " + fileName);
			settlement.resolve(writeBytesToFilesDir(content, fileName));
		}));
	}


	public static String getFileName(Context context, Uri uri) {
		String[] projection = {OpenableColumns.DISPLAY_NAME};
		try (Cursor cursor = context.getContentResolver().query(uri, projection, null, null, null)) {
			if (cursor != null && cursor.moveToFirst()) {
				int columnIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
				if (columnIndex != -1) {
					return cursor.getString(columnIndex);
				}
			}
		}
		return null;
	}

	private byte[] readFileAsBinary(Uri uri) throws IOException {
		try{
			InputStream in = context.getContentResolver().openInputStream(uri);
			ByteArrayOutputStream buffer = new ByteArrayOutputStream();
			byte[] data = new byte[4096];
			int nRead;
			while ((nRead = in.read(data, 0, data.length)) != -1) {
				buffer.write(data, 0, nRead);
			}
			byte[] fileBytes = buffer.toByteArray();
			in.close();
			return fileBytes;
		} catch (Exception e) {
			e.printStackTrace();
			throw e;
		}
	}
}
