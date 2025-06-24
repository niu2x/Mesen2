package com.example.mesen.demo;

public class MesenAPI {

	interface NotificationCallback{
		void notifyRefreshSoftwareRenderer(int []buffer, int width, int height);
	};
	public static native void init();
	public static native void initializeEmu(String dir, boolean noAudio, boolean noVideo, boolean noInput);
	public static native void registerNotificationCallback(NotificationCallback callback);
	public static native boolean loadROM(String romFile, String patchFile);
}
