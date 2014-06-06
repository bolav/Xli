import android.annotation.SuppressLint;
import android.app.NativeActivity;
import android.content.Context;
import android.os.Build;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;

public class KeyboardHelper {
	static ViewTreeObserver.OnGlobalLayoutListener layoutListener;
	static Hidden hiddenText;
    static int keyboardSize;
    static ViewGroup hiddenLayout;
    
    public static int GetKeyboardSize() { return (int)keyboardSize; }
	
    public static void ShowKeyboard(final NativeActivity activity) {
        if (hiddenText == null)
        {
            Log.e("XliApp","ShowKeyboard:Hidden View not available");
            return;
        }
        DummyEditable.PopulateDummyString();
        activity.runOnUiThread(new Runnable() { public void run() {
            try {
                hiddenText.setFocusableInTouchMode(true);
                hiddenText.setFocusable(true);
                InputMethodManager imm = (InputMethodManager)activity.getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.showSoftInput(hiddenText, 0);
                hiddenText.requestFocus();
            } catch (Exception e) {
                Log.e("XliApp","Unable show keyboard");
            }
        }});
    }
    public static void HideKeyboard() {
        if (hiddenText == null)
        {
            Log.e("XliApp","HideKeyboard: Hidden View not available");
            return;
        }
        XliJ.nActivity.runOnUiThread(new Runnable() { public void run() {
            InputMethodManager imm = (InputMethodManager)XliJ.nActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(XliJ.nActivity.getWindow().getDecorView().getWindowToken(), 0);
            hiddenText.setFocusableInTouchMode(false);
            hiddenText.setFocusable(false);
            hiddenText.setVisibility(View.VISIBLE);
        }});
    }
        
    public static void AttachHiddenView(final NativeActivity activity)
    {    	
        Log.d("XliApp","Attempting to attach hidden view");
        if (hiddenLayout == null)
        {
            hiddenLayout = new FrameLayout(activity);
            activity.runOnUiThread(new Runnable() { public void run() {
                try {
                    activity.setContentView(hiddenLayout);                    
                    hiddenText = new Hidden(activity, activity);
                    hiddenLayout.addView(hiddenText);                    
                    hiddenText.setVisibility(View.VISIBLE);
                    hiddenText.requestFocus();
                    HideKeyboard();
                    Log.i("XliApp","Successfully created input capture View.");                    
                } catch (Exception e) {
                    Log.e("XliApp","Unable to create Layout or View for input capture.");
                    XliJ.XliJ_JavaThrowError(-1, "Unable to create Layout or View for input capture.");
                }

                try {
                	keyboardSize = 0;
                	layoutListener = new ViewTreeObserver.OnGlobalLayoutListener(){
                        public void onGlobalLayout() {
                            int rootViewHeight = hiddenLayout.getRootView().getHeight();
                            int location[] = new int[2];
                            hiddenLayout.getLocationOnScreen(location);
                            int hiddenHeight = (int)(location[1] + hiddenLayout.getMeasuredHeight());
                            keyboardSize = rootViewHeight - hiddenHeight;
                        }
                    };
                	hiddenLayout.getViewTreeObserver().addOnGlobalLayoutListener(layoutListener);
                    Log.i("XliApp","Successfully attached View Tree Observer.");
                } catch (Exception e) {
                    Log.e("XliApp","Unable to attach keyboard height monitor.");
                    XliJ.XliJ_JavaThrowError(-1, "Unable to attach keyboard height monitor.");                    
                }                
            }});
        }
    }
    
    @SuppressLint("NewApi")
	@SuppressWarnings("deprecation")
	public static void DetachHiddenView()
    {
    	if (hiddenLayout!=null)
    	{
    		if (layoutListener!=null)
    		{
	    	    if (Build.VERSION.SDK_INT < 16) {
	    	    	hiddenLayout.getViewTreeObserver().removeGlobalOnLayoutListener(layoutListener);
	    	    } else {
	    	    	hiddenLayout.getViewTreeObserver().removeOnGlobalLayoutListener(layoutListener);
	    	    }
    		}
    	    hiddenLayout.removeAllViews();
	    	hiddenLayout = null;
    	}
    	if (hiddenText!=null)
    	{
	    	hiddenText.Disable(XliJ.nActivity, XliJ.nActivity);
	    	hiddenText = null;
    	}
    	keyboardSize = 0;
    }
    
    public static void KHOnPause()
    {
    	DetachHiddenView();
    	//HideKeyboard();
    }
    public static void KHOnResume()
    {
    	AttachHiddenView(XliJ.nActivity);
    }
}
