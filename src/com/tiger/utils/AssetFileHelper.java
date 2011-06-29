package com.tiger.utils;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.tiger.nump.AppConfig;

import android.content.Context;
import android.util.Log;

public class AssetFileHelper {
	final static boolean DBG=!AppConfig.RELEASE;
	final static String LOG_TAG ="AssetFileHelper";
	
	private Context mCtx;
	
	public AssetFileHelper(Context contex){
		mCtx =contex;
	}
	
	private boolean copyAssetFile(String assetFile, String targetDir) {
		//Asset file not appliable for below action
		//File file = new File(assetFile);
		//if(!file.exists()) return false;
		if(DBG) Log.d("ASSET COPY", "copy "+ assetFile + " to "+ targetDir );
		InputStream in = null;
		OutputStream out = null;

		try {
			in = mCtx.getAssets().open("game/"+assetFile);
			out = new FileOutputStream( new File(targetDir, assetFile));

			byte[] buf = new byte[8192];
			int len;
			while ((len = in.read(buf)) > 0)
				out.write(buf, 0, len);

		} catch (Exception e) {
			e.printStackTrace();
			return false;

		} finally {
			try {
				if (out != null)
					out.close();
				if (in != null)
					in.close();
			} catch (IOException e) {
			}
		}
		return true;
	}	
	
	public void copyAssets2Data() {
		if(DBG) Log.d(LOG_TAG, "copy assets");
		//copy preset files in assets to data dir
		File dataDir = mCtx.getDir("data", Context.MODE_PRIVATE);
		
		String destDir = dataDir.getAbsolutePath();
		try {
			String[] allFiles = mCtx.getAssets().list("game");
			if(DBG) Log.d(LOG_TAG, "asset file list: " + allFiles);
			for(String filename: allFiles){
				if(accept(filename)) {
				    if(DBG) Log.d(LOG_TAG, "copy " + filename + " -> " + destDir);
				    copyAssetFile(filename, destDir);
				}
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	boolean accept(String filename){
		if(filename.endsWith(".mid") || filename.endsWith(".MID") || filename.endsWith(".mp3") ||
				filename.endsWith(".MP3"))
			return false;
		else
			return true;
	}
}

