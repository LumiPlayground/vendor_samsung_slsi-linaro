package com.android.exynos.hdrdisplaytune;

import android.annotation.SuppressLint;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;

import android.util.DisplayMetrics;
import android.content.Context;
import android.view.WindowManager;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
//import android.hardware.display.ExynosDisplaySolutionManager;
import java.util.Arrays;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLSurfaceView.EGLWindowSurfaceFactory;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.widget.Toast;

public class FullscreenActivity extends AppCompatActivity {
    /**
     * Whether or not the system UI should be auto-hidden after
     * {@link #AUTO_HIDE_DELAY_MILLIS} milliseconds.
     */
    private static final boolean AUTO_HIDE = true;

    /**
     * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
     * user interaction before hiding the system UI.
     */
    private static final int AUTO_HIDE_DELAY_MILLIS = 3000;

    /**
     * Some older devices needs a small delay between UI widget updates
     * and a change of the status and navigation bar.
     */
    private static final int UI_ANIMATION_DELAY = 300;
    private final Handler mHideHandler = new Handler(Looper.myLooper());
    private View mContentView;
    private final Runnable mHidePart2Runnable = new Runnable() {
        @SuppressLint("InlinedApi")
        @Override
        public void run() {
            // Delayed removal of status and navigation bar
            if (Build.VERSION.SDK_INT >= 30) {
                mContentView.getWindowInsetsController().hide(
                        WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
            } else {
                // Note that some of these constants are new as of API 16 (Jelly Bean)
                // and API 19 (KitKat). It is safe to use them, as they are inlined
                // at compile-time and do nothing on earlier devices.
                mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
            }
        }
    };
    private final Runnable mShowPart2Runnable = new Runnable() {
        @Override
        public void run() {
            // Delayed display of UI elements
            ActionBar actionBar = getSupportActionBar();
            if (actionBar != null) {
                actionBar.show();
            }
        }
    };
    private boolean mVisible;
    private final Runnable mHideRunnable = new Runnable() {
        @Override
        public void run() {
            hide();
        }
    };
    /**
     * Touch listener to use for in-layout UI controls to delay hiding the
     * system UI. This is to prevent the jarring behavior of controls going away
     * while interacting with activity UI.
     */
    private final View.OnTouchListener mDelayHideTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            switch (motionEvent.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    if (AUTO_HIDE) {
                        delayedHide(AUTO_HIDE_DELAY_MILLIS);
                    }
                    break;
                case MotionEvent.ACTION_UP:
                    view.performClick();
                    break;
                default:
                    break;
            }
            return false;
        }
    };

    private static final String TAG = "HDRDisplayTune";
    private WindowManager mWindowManager;
    private int screen_width;
    private int screen_height;

    private GLSurfaceView glView;
    private Toast mToast = null;
    private MessageReceiver mReceiver;

    private int mType = ePattern.GRADATION_X;
    private long mColor = 0xFFFFFFFFL; // white
    private int mFormat = eFormat.RGBA1010102;
    private int mOutFormat = eFormat.RGBA1010102;

    public static final String HDR_TUNE_PATTERN_CHANGED
            = "com.android.server.display.HDR_TUNE_PATTERN_CHANGED";
    public static final String HDR_TUNE_PATTERN_COLOR
            = "com.android.server.display.hdr_tune_color";
    public static final String HDR_TUNE_PATTERN_FORMAT
            = "com.android.server.display.hdr_tune_format";
    public static final String HDR_TUNE_PATTERN_TYPE
            = "com.android.server.display.hdr_tune_type";

    private interface eFormat {
        int RGBA8888 = 8;
        int RGBA1010102 = 10;
    }

    private interface ePattern {
        int CENTER = 0; // 0~6
        int CORNER = 7;
        int GRADATION_Y = 8;
        int GRADATION_X = 9;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        DisplayMetrics metrics = new DisplayMetrics();
        mWindowManager = (WindowManager)this.getSystemService(Context.WINDOW_SERVICE);
        mWindowManager.getDefaultDisplay().getRealMetrics(metrics);
        screen_width = metrics.widthPixels;
        screen_height = metrics.heightPixels;

        mContentView = glView = new GLSurfaceView(this);
        glView.setEGLWindowSurfaceFactory(new CustomSurfaceFactory());
        if (mOutFormat == eFormat.RGBA1010102)
            glView.setEGLConfigChooser(10, 10, 10, 2, 0, 0); // RGBA1010102
        glView.getHolder().setKeepScreenOn(true);
        glView.setRenderer(new CustomRenderer());
        glView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        setContentView(glView);

        // Set up the user interaction to manually show or hide the system UI.
        mContentView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggle();
            }
        });

        mReceiver = new MessageReceiver();

        drawText("waiting for adb command [" + Integer.toString(mOutFormat) + "]");

        Log.d(TAG, "onCreate " + screen_width + " x " + screen_height);
    }

    @Override
    protected void onStart() {
        super.onStart();

        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(HDR_TUNE_PATTERN_CHANGED);
        registerReceiver(mReceiver, intentFilter);
        //Log.d(TAG, "onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();

        unregisterReceiver(mReceiver);
        //Log.d(TAG, "onStop");
    }

    private class CustomSurfaceFactory implements GLSurfaceView.EGLWindowSurfaceFactory {
        public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display,
                                              EGLConfig config, Object nativeWindow) {
            int EGL_GL_COLORSPACE_KHR = 0x309D;
            int EGL_GL_COLORSPACE_BT2020_PQ_EXT = 0x3340;
            int[] attribList = new int[] {
                    EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_BT2020_PQ_EXT,
                    EGL10.EGL_NONE,	EGL10.EGL_NONE,
                    EGL10.EGL_NONE
            };
            return egl.eglCreateWindowSurface(display, config, nativeWindow, attribList);
        }

        public void destroySurface(EGL10 egl, EGLDisplay display,
                                   EGLSurface surface) {
            egl.eglDestroySurface(display, surface);
        }
    }

    private class CustomRenderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            //Log.d(TAG, "onSurfaceCreated");
            gl.glViewport(0, 0, screen_width, screen_height);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            //Log.d(TAG, "onSurfaceChanged");
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            //Log.d(TAG, "onDrawFrame");
            gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
            drawPattern(gl, mFormat, mType, mColor);
        }
    }

    private void drawText(String message) {
        mToast = Toast.makeText(this, message, Toast.LENGTH_LONG);
        mToast.show();
    }

    private void clearText() {
        if (mToast != null)
            mToast.cancel();
        mToast = null;
    }

    private void drawPattern(GL10 gl, int format, int type, long color) {
        Log.d(TAG, "drawPattern " + format + "," + type + "," + Long.toHexString(color));

        float[] types = {1.0f, 0.02f, 0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.025f, 0.0025f, 0.01f};
        float step_x, step_y, ratio;
        int [] shift, mask;
        float [] colors, colors_t;
        Square [] square;
        float [][] center;
        float [] position;

        switch(format) {
            case eFormat.RGBA1010102:
                shift = new int[] {22, 12, 2, 0};
                mask = new int[] {0x3ff, 0x3ff, 0x3ff, 0x3};
                break;
            case eFormat.RGBA8888:
            default:
                shift = new int[] {24, 16, 8, 0};
                mask = new int[] {0xff, 0xff, 0xff, 0xff};
                break;
        }

        /* Convert color to float */
        colors = new float[mask.length];
        for (int i = 0; i < colors.length; i++)
            colors[i] = (float)((color >> shift[i]) & mask[i]) / mask[i];

        ratio = (float)Math.sqrt(types[(type < types.length)? type : 0]);
        switch(type) {
            case ePattern.GRADATION_Y: /* gradation y directional */
                step_x = 1.0f;
                step_y = ratio;
                center = new float [(int) (1.0f / ratio)][];
                for (int i = 0; i < center.length; i++) {
                    center[i] = new float [] {
                            0.0f, // x
                            -1.0f + step_y + i * step_y * 2, // y
                            ratio * (i + 1) // color
                    };
                }
                break;
            case ePattern.GRADATION_X: /* gradation x directional */
                step_x = ratio;
                step_y = 1.0f;
                center = new float [(int) (1.0f / ratio)][];
                for (int i = 0; i < center.length; i++) {
                    center[i] = new float [] {
                            1.0f - step_x - i * step_x * 2, // x
                            0.0f, // y
                            ratio * (i + 1) // color
                    };
                }
                break;
            case ePattern.CORNER: /* corner */
                step_x = step_y = ratio;
                center = new float [][] {
                        {-1.0f + step_x,  1.0f - step_y, 1.0f}, // top-left
                        { 1.0f - step_x,  1.0f - step_y, 1.0f}, // top-right
                        { 1.0f - step_x, -1.0f + step_y, 1.0f},	// bottom-right
                        {-1.0f + step_x, -1.0f + step_y, 1.0f}  // bottom-left
                };
                break;
            case ePattern.CENTER: /* center */
            default:
                step_x = step_y = ratio;
                center = new float [][] {{0.0f, 0.0f, 1.0f}};	// center
                break;
        }

        /* Draw Pattern */
        square = new Square[center.length];
        for(int i = 0; i < square.length; i++) {
            position = new float [] {
                    center[i][0] - step_x, center[i][1] - step_y, 0.0f, // top-left
                    center[i][0] + step_x, center[i][1] - step_y, 0.0f, // top-right
                    center[i][0] + step_x, center[i][1] + step_y, 0.0f, // bottom-right
                    center[i][0] - step_x, center[i][1] + step_y, 0.0f  // bottom-left
            };
            colors_t = colors.clone();
            for (int j = 0; j < (colors.length - 1); j++)
                colors_t[j] *= center[i][2];
            square[i] = new Square(position);
            square[i].draw(gl, colors_t);
        }
    }

    private final class Square{
        private FloatBuffer mVertexbuffer;
        private ByteBuffer mIndicesBuffer;
        private byte [] mIndices = {0, 1, 2, 0, 2, 3};

        public Square(float [] position){
            //Log.d(TAG,	" Square Position " + Arrays.toString(position));
            ByteBuffer byteBuffer = ByteBuffer.allocateDirect(position.length * 4);
            byteBuffer.order(ByteOrder.nativeOrder());
            mVertexbuffer = byteBuffer.asFloatBuffer();
            mVertexbuffer.put(position);
            mVertexbuffer.position(0);

            mIndicesBuffer = ByteBuffer.allocateDirect(mIndices.length);
            mIndicesBuffer.put(mIndices);
            mIndicesBuffer.position(0);
        }

        public void draw(GL10 gl, float [] colors){
            //Log.d(TAG,	" Draw Color" + Arrays.toString(colors));
            gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);

            gl.glVertexPointer(3, GL10.GL_FLOAT, 0, mVertexbuffer);
            gl.glColor4f(colors[0], colors[1], colors[2], colors[3]);
            gl.glDrawElements(GL10.GL_TRIANGLES, mIndices.length, GL10.GL_UNSIGNED_BYTE, mIndicesBuffer);

            gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
        }
    }

    private final class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            try {
                mFormat = intent.getIntExtra(HDR_TUNE_PATTERN_FORMAT, 0);
                mType = intent.getIntExtra(HDR_TUNE_PATTERN_TYPE, 0);
                mColor = intent.getLongExtra(HDR_TUNE_PATTERN_COLOR, -1);
                clearText();
                glView.requestRender();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        // Trigger the initial hide() shortly after the activity has been
        // created, to briefly hint to the user that UI controls
        // are available.
        delayedHide(100);
    }

    private void toggle() {
        if (mVisible) {
            hide();
        } else {
            show();
        }
    }

    private void hide() {
        // Hide UI first
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }
        mVisible = false;

        // Schedule a runnable to remove the status and navigation bar after a delay
        mHideHandler.removeCallbacks(mShowPart2Runnable);
        mHideHandler.postDelayed(mHidePart2Runnable, UI_ANIMATION_DELAY);
        clearText();
    }

    private void show() {
        // Show the system bar
        if (Build.VERSION.SDK_INT >= 30) {
            mContentView.getWindowInsetsController().show(
                    WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
        } else {
            mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
        }
        mVisible = true;

        // Schedule a runnable to display UI elements after a delay
        mHideHandler.removeCallbacks(mHidePart2Runnable);
        mHideHandler.postDelayed(mShowPart2Runnable, UI_ANIMATION_DELAY);
        drawText("waiting for adb command [" + Integer.toString(mOutFormat) + "]");
    }

    /**
     * Schedules a call to hide() in delay milliseconds, canceling any
     * previously scheduled calls.
     */
    private void delayedHide(int delayMillis) {
        mHideHandler.removeCallbacks(mHideRunnable);
        mHideHandler.postDelayed(mHideRunnable, delayMillis);
    }
}