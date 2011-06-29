package com.tiger.utils;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.WebChromeClient;
import android.webkit.WebView;

public class HelpActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		//no title
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		super.onCreate(savedInstanceState);
		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		WebView view = new WebView(this);
		view.setWebChromeClient(new WebChromeClient() {
			public void onReceivedTitle(WebView view, String title) {
				HelpActivity.this.setTitle(title);
			}
		});
		setContentView(view);

		view.loadUrl(getIntent().getData().toString());
	}
}
