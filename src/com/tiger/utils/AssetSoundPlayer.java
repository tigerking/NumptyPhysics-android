package com.tiger.utils;

import java.io.IOException;
import java.util.ArrayList;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaPlayer;
import android.util.Log;
import com.tiger.nump.AppConfig;

import com.tiger.nump.R;

public class AssetSoundPlayer {
	final static boolean DBG=!AppConfig.RELEASE;
	final static String LOG_TAG ="AssetSoundPlayer";
	
	Context mCtx;
	MediaPlayer mPlayer;
	ArrayList<String> mSounds;
	
	int mSoundSelected;
	boolean mPaused=false;
	
	public AssetSoundPlayer(Context ctx){
		if(DBG) Log.i(LOG_TAG, "create AssetSoundPlayer...");
		mCtx = ctx;
		mSounds = new ArrayList<String>();
		mPlayer = new MediaPlayer();
		
		mSoundSelected =0;
		if(DBG) Log.i(LOG_TAG, "init AssetSoundPlayer...");
		init();
		if(DBG) Log.i(LOG_TAG, "create AssetSoundPlayer done.");
	}
	
	public void play(int index){
		if(index>=mSounds.size()){
			if(DBG) Log.e(LOG_TAG,"play(id) :index out of bound, ignore " +index);
		}
		mSoundSelected=index;
		play( mSounds.get(index));
	}
	
	public void play(String trackName){
		//to use default track if trackName is null
		if(trackName==null) {
			trackName= getDefaultTrack();
			if(trackName ==null) return;
		}
		
		AssetFileDescriptor afd;
		try {
			if(mPlayer.isPlaying()) {
				mPlayer.stop();
			}
			mPlayer.reset();
			
			//TODO, FIXME
			
			String type = getSoundType(mCtx.getAssets(), trackName);
			if(false) {
				if(type==null) return;
			}else{
				type=".MID";
			}
			
			if(DBG) Log.d(LOG_TAG, "Sound type is " + type);
			
			afd = mCtx.getAssets().openFd("sound/"+trackName+type);
			mPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd
					.getLength());
			mPlayer.prepare();
			mPlayer.setLooping(true);
			mPlayer.start();
		} catch (IOException e) {
			if(DBG) e.printStackTrace();
		}
	}
	
	public void play(){
		if(mSoundSelected>=0 && mSoundSelected< mSounds.size()){
			play(mSoundSelected);
		}else{
			play(0);
		}
	}
	
	public void stop(){
		if(mPlayer.isPlaying())
			mPlayer.stop();
	}
	
	public void pause(){
		if(mPlayer.isPlaying()){
			mPaused=true;
			mPlayer.pause();
		}
	}

	public void resume(){
		if(mPaused) {
			mPaused=false;
			mPlayer.start();
		}
	}

	
	public String[] getSounds(){
		if(DBG) Log.d(LOG_TAG,"mSounds =[" + mSounds+"]");
		String[] result = (String[]) mSounds.toArray(new String[mSounds.size()]);
		if(DBG) Log.d(LOG_TAG,"result =[" + result+"]");

		return result;
	}
	
	public void selectSound(){
		AlertDialog dialog = new AlertDialog.Builder(mCtx)
        .setIcon(android.R.drawable.title_bar)
        .setTitle(R.string.select_sound)
        .setSingleChoiceItems( getSounds(), 0, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            	//play(whichButton);
            	mSoundSelected = whichButton;
            	if(DBG) Log.d(LOG_TAG,"sound (index=" + mSoundSelected +") is selected.");
            }
        })
        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                /* User clicked Yes so do some stuff */
            }
        })
        .setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {

                /* User clicked No so do some stuff */
            }
        })
       .create();	
		
		//dialog.
		
		dialog.show();
	}
	
	private void loadAssets() {
		if (DBG) Log.d(LOG_TAG, "list assets...");
		try {
			String[] allFiles = mCtx.getAssets().list("sound");
			for (String filename : allFiles) {
				if (accept(filename)) {
					mSounds.add(filename.substring(0, filename.length() - 4));
				}
			}
		} catch (Exception e) {
			if (DBG)
				e.printStackTrace();
		}
		if(DBG) Log.d(LOG_TAG,"loadAssets result: [" + mSounds + "]");
	}

	private void init() {
		mSounds.clear();
		loadAssets();
	}

	private boolean accept(String filename) {
		return filename.endsWith(".mp3")
		|| filename.endsWith(".MP3")
		|| filename.endsWith(".mid")
		|| filename.endsWith(".MID")
		;
	}
	
	private String getSoundType(AssetManager asm, String trackName){
		String[] supported={
				".mp3", ".MP3",
				".mid", ".MID"
		};
		try {
			for(String type:supported){
				String[] atmp =asm.list(trackName + type);
				if(atmp!=null){
					if(DBG) Log.d(LOG_TAG, "TIGERKING DBG, "+ trackName+type+","+ atmp);
				}
				if(asm.list(trackName+type).length>0) {
					if(DBG) Log.d(LOG_TAG, "FOUND sound track " + trackName+type);
					return type;
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	private String getDefaultTrack(){
		if(mSounds.size()==0) {
			if(DBG) Log.w(LOG_TAG,"no sound track available");
			return null;
		}
		
		if(mSoundSelected>=0  && mSoundSelected < mSounds.size()){
			return mSounds.get(mSoundSelected);
		}else{
			return mSounds.get(0);
		}
		
		
	}
	
	private void sleep(int ms){
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
