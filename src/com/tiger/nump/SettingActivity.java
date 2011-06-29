package com.tiger.nump;

import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.preference.Preference.OnPreferenceClickListener;
import android.view.Window;
import android.view.WindowManager;
import com.tiger.utils.AssetSoundPlayer;

public class SettingActivity extends PreferenceActivity implements OnPreferenceClickListener {

	private final static boolean FULL_SETTINGS=false;
	private static final String SEARCH_ROM_URI =
			"http://www.freeroms.com/gameboy_color_roms_popular.htm";

	private Uri ABOUT_URI ;
	
	private static final Uri ABOUT_URI_EN = Uri.parse(
	"file:///android_asset/about.html");
	
	private static final Uri ABOUT_URI_CN = Uri.parse(
	"file:///android_asset/about_cn.html");
	
	private static final String MARKET_URI = "market://details?id=";

	//private static final String GAME_GRIPPER_URI = 
	//		"https://sites.google.com/site/gamegripper";


	private SharedPreferences settings;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// fullscreen mode
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		super.onCreate(savedInstanceState);
		

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		setTitle(R.string.settings);
		addPreferencesFromResource(R.xml.preferences);

		settings = PreferenceManager.getDefaultSharedPreferences(this);
		
		loadSoundTrackNameFromAssets();
		
		
		Preference about =findPreference("about");
		if(about!=null) {
			about.setOnPreferenceClickListener(this);
		}		
	}

	@Override
	public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
			Preference preference) {
		String key = preference.getKey();

		/*
		if ("loadKeyProfile".equals(key)) {
			Intent intent = new Intent(this, KeyProfilesActivity.class);
			intent.putExtra(KeyProfilesActivity.EXTRA_TITLE,
					getString(R.string.load_profile));
			startActivityForResult(intent, REQUEST_LOAD_KEY_PROFILE);
			return true;
		}
		
		if ("saveKeyProfile".equals(key)) {
			Intent intent = new Intent(this, KeyProfilesActivity.class);
			intent.putExtra(KeyProfilesActivity.EXTRA_TITLE,
					getString(R.string.save_profile));
			startActivityForResult(intent, REQUEST_SAVE_KEY_PROFILE);
			return true;
		}
		*/
		return super.onPreferenceTreeClick(preferenceScreen, preference);
	}
	
	
	public boolean onPreferenceClick(Preference preference) {
		String key= preference.getKey();
		if("about".equals(key)) {
			showAbout();
		}
		return true;
	}	

	void loadSoundTrackNameFromAssets(){
		
		AssetSoundPlayer mp = new AssetSoundPlayer(this);
		
		String[] values= mp.getSounds();
		String[] keys=values;

		ListPreference soundTrackPref =  (ListPreference)findPreference("soundTrack");
		soundTrackPref.setEntries(keys);
		soundTrackPref.setEntryValues(values);
	}
	
	private void showMessage(String title, String message)
	{
		//Show Message dialog
		 new AlertDialog.Builder(this)
        .setTitle(title)
        .setMessage(message)
        .setPositiveButton(android.R.string.ok, null)
        .show();
	}

	private void showAbout() {
			showMessage( getString(R.string.about_content_msg), 
					//getString(R.string.about_content_msg) + "\n" +
					getString(R.string.about_content_ver) + AppConfig.VERSION+"\n" +
					getString(R.string.about_content_release) + AppConfig.RELEASE_DATE +"\n\n" +				
					getString(R.string.about_content_author) + AppConfig.AUTHOR +"\n" 
					//+getString(R.string.about_content_bug) + AppConfig.EMAIL +"\n"
					);
	}
}
