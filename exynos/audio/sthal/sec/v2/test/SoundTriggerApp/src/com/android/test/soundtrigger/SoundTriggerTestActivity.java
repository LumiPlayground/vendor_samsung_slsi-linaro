/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.test.soundtrigger;

import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import android.Manifest;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.text.Editable;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;

import com.android.test.soundtrigger.SoundTriggerTestService.SoundTriggerTestBinder;

public class SoundTriggerTestActivity extends Activity implements SoundTriggerTestService.UserActivity {
    private static final String TAG = "SoundTriggerTest";
    private static final int AUDIO_PERMISSIONS_REQUEST = 1;

    private SoundTriggerTestService mService = null;

    private static UUID mSelectedModelUuid = null;

    private Map<RadioButton, UUID> mButtonModelUuidMap;
    private Map<UUID, RadioButton> mModelButtons;
    private Map<UUID, String> mModelNames;
    private List<RadioButton> mModelRadioButtons;

    private TextView mDebugView = null;
    private ScrollView mScrollView = null;
    private Button mPlayTriggerButton = null;
    private PowerManager.WakeLock mScreenWakelock;
    private Handler mHandler;
    private RadioGroup mRadioGroup;
    private CheckBox mCaptureAudioCheckBox;
    private Button mPlayCapturedAudioButton = null;
    private CheckBox mAutoTestCheckBox;

    private Button mLoadButton = null;
    private Button mStartButton = null;
    private Button mStopButton = null;
    private Button mUnloadButton = null;
    private Button mReloadButton = null;
    private Button mVtsRecStartButton = null;
    private Button mVtsRecStopButton = null;

    private ToggleButton mAutoStopStartButton = null;
    private ToggleButton mAutoVtsRecButton = null;

    private boolean isAutoTesting = false;
    private boolean isVtsRecording = false;
    private boolean isAutoStopStartTesting = false;
    private boolean isAutoVtsRecTesting = false;

    private final String BROADCAST_MSG = "com.android.test.soundtrigger.AUTO_TEST";
    private BroadcastReceiver mReceiver = null;

    @Override
    public boolean getAutoTesting() {
        return isAutoTesting;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Make sure that this activity can punch through the lockscreen if needed.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                             WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mDebugView = findViewById(R.id.console);
        mScrollView = findViewById(R.id.scroller_id);
        mRadioGroup = findViewById(R.id.model_group_id);
        mPlayTriggerButton = findViewById(R.id.play_trigger_id);
        mDebugView.setText(mDebugView.getText(), TextView.BufferType.EDITABLE);
        mDebugView.setMovementMethod(new ScrollingMovementMethod());
        mCaptureAudioCheckBox = findViewById(R.id.caputre_check_box);
        mPlayCapturedAudioButton = findViewById(R.id.play_captured_id);
        mAutoTestCheckBox = findViewById(R.id.auto_test_check_box);

        mLoadButton = findViewById(R.id.load_id);
        mStartButton = findViewById(R.id.start_id);
        mStopButton = findViewById(R.id.stop_id);
        mUnloadButton = findViewById(R.id.unload_id);
        mReloadButton = findViewById(R.id.reload_id);
        mVtsRecStartButton = findViewById(R.id.vts_rec_start_id);
        mVtsRecStopButton = findViewById(R.id.vts_rec_stop_id);
        mAutoStopStartButton = findViewById(R.id.auto_stop_start_id);
        mAutoVtsRecButton = findViewById(R.id.auto_vts_rec_id);

        mVtsRecStopButton.setEnabled(false);
        mAutoStopStartButton.setEnabled(false);

        mHandler = new Handler();
        mButtonModelUuidMap = new HashMap();
        mModelButtons = new HashMap();
        mModelNames = new HashMap();
        mModelRadioButtons = new LinkedList();

        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        // Make sure that the service is started, so even if our activity goes down, we'll still
        // have a request for it to run.
        startService(new Intent(getBaseContext(), SoundTriggerTestService.class));

        // Bind to SoundTriggerTestService.
        Intent intent = new Intent(this, SoundTriggerTestService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Unbind from the service.
        if (mService != null) {
            mService.setUserActivity(null);
            unbindService(mConnection);
        }
    }

    @Override
    public void addModel(UUID modelUuid, String name) {
        // Create a new widget for this model, and insert everything we'd need into the map.
        RadioButton button = new RadioButton(this);
        mModelRadioButtons.add(button);
        button.setText(name);
        button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onRadioButtonClicked(v);
            }
        });
        mButtonModelUuidMap.put(button, modelUuid);
        mModelButtons.put(modelUuid, button);
        mModelNames.put(modelUuid, name);

        // Sort all the radio buttons by name, then push them into the group in order.
        Collections.sort(mModelRadioButtons, new Comparator<RadioButton>(){
            @Override
            public int compare(RadioButton button0, RadioButton button1) {
                return button0.getText().toString().compareTo(button1.getText().toString());
            }
        });
        mRadioGroup.removeAllViews();
        for (View v : mModelRadioButtons) {
            mRadioGroup.addView(v);
        }

        // If we don't have something selected, select this first thing.
        if (mSelectedModelUuid == null || mSelectedModelUuid.equals(modelUuid)) {
            button.setChecked(true);
            onRadioButtonClicked(button);
        }
    }

    @Override
    public void setModelState(UUID modelUuid, String state) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String newButtonText = mModelNames.get(modelUuid);
                if (state != null) {
                    newButtonText += ": " + state;
                }
                mModelButtons.get(modelUuid).setText(newButtonText);
                updateSelectModelSpecificUiElements();
            }
        });
    }

    @Override
    public void showMessage(String msg, boolean showToast) {
        // Append the message to the text field, then show the toast if requested.
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isAutoStopStartTesting || isAutoVtsRecTesting) {
                    mDebugView.setText("auto testing...\n");
                }
                ((Editable) mDebugView.getText()).append(msg + "\n");
                mScrollView.post(new Runnable() {
                    public void run() {
                        mScrollView.smoothScrollTo(0, mDebugView.getBottom());
                    }
                });
                if (showToast) {
                    Toast.makeText(SoundTriggerTestActivity.this, msg, Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    @Override
    public void handleDetection(UUID modelUuid) {
        screenWakeup();
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                screenRelease();
            }
        }, 1000L);
    }

    private void screenWakeup() {
        if (mScreenWakelock == null) {
            PowerManager pm = ((PowerManager)getSystemService(POWER_SERVICE));
            mScreenWakelock =  pm.newWakeLock(
                    PowerManager.SCREEN_DIM_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, TAG);
        }
        mScreenWakelock.acquire();
    }

    private void screenRelease() {
        mScreenWakelock.release();
    }

    public void onLoadButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Could not load sound model: not bound to SoundTriggerTestService");
        } else {
            mService.loadModel(mSelectedModelUuid);
        }
    }

    public void onUnloadButtonClicked(View v) {
        if (mService == null) {
           Log.e(TAG, "Can't unload model: not bound to SoundTriggerTestService");
        } else {
            mService.unloadModel(mSelectedModelUuid);
        }
    }

    public void onReloadButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't reload model: not bound to SoundTriggerTestService");
        } else {
            mService.reloadModel(mSelectedModelUuid);
        }
    }

    public void onStartRecognitionButtonClicked(View v) {

        if (mService == null) {
            Log.e(TAG, "Can't start recognition: not bound to SoundTriggerTestService");
        } else {
            mService.startRecognition(mSelectedModelUuid);
        }
    }

    public void onStopRecognitionButtonClicked(View v) {

        if (mService == null) {
            Log.e(TAG, "Can't stop recognition: not bound to SoundTriggerTestService");
        } else {
            mService.stopRecognition(mSelectedModelUuid);
        }
    }

    public synchronized void onPlayTriggerButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play trigger audio: not bound to SoundTriggerTestService");
        } else {
            mService.playTriggerAudio(mSelectedModelUuid);
        }
    }

    public synchronized void onCaptureAudioCheckboxClicked(View v) {
        // See if we have the right permissions
        if (!mService.hasMicrophonePermission()) {
            requestPermissions(new String[]{Manifest.permission.RECORD_AUDIO},
                    AUDIO_PERMISSIONS_REQUEST);
            return;
        } else {
            mService.setCaptureAudio(mSelectedModelUuid, mCaptureAudioCheckBox.isChecked());
        }
    }

    public synchronized void onAutoTestCheckboxClicked(View v) {
        AlertDialog.Builder mDialog = new AlertDialog.Builder(this);

        mDialog.setMessage("All controls will be disabled. Do you really start auto test?")
               .setTitle("Warning")
               .setPositiveButton("No", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(TAG, "Cancel Auto Test");
                        mAutoTestCheckBox.setChecked(false);
                    }
                })
               .setNeutralButton("Yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(TAG, "Clicked AutoTest CheckBox. Disable all buttons");
                        isAutoTesting = true;
                        updateSelectModelSpecificUiElements();
                    }
                })
                .setCancelable(false)
                .show();
    }

    public void onVtsRecStartButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play trigger audio: not bound to SoundTriggerTestService");
        } else {
            mService.startVtsRecord(0);

            isVtsRecording = true;
            updateSelectModelSpecificUiElements();
        }
    }

    public void onVtsRecStopButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play trigger audio: not bound to SoundTriggerTestService");
        } else {
            mService.stopVtsRecord();

            isVtsRecording = false;
            updateSelectModelSpecificUiElements();
        }
    }

    public void onAutoStopStartClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play trigger audio: not bound to SoundTriggerTestService");
        } else {
            boolean on = ((ToggleButton) v).isChecked();

            if (on) {
                mService.autoStopStartTest(true);
                isAutoStopStartTesting = true;
                updateSelectModelSpecificUiElements();
            } else {
                mService.autoStopStartTest(false);
                isAutoStopStartTesting = false;
                updateSelectModelSpecificUiElements();
            }
        }
    }

    public void onAutoVtsRecClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play trigger audio: not bound to SoundTriggerTestService");
        } else {
            boolean on = ((ToggleButton) v).isChecked();

            if (on) {
                mService.autoVtsRecTest(true);
                isAutoVtsRecTesting = true;
                updateSelectModelSpecificUiElements();
            } else {
                mService.autoVtsRecTest(false);
                isAutoVtsRecTesting = false;
                updateSelectModelSpecificUiElements();
            }
        }
    }

    @Override
    public synchronized void onRequestPermissionsResult(int requestCode, String permissions[],
                                                        int[] grantResults) {
        if (requestCode == AUDIO_PERMISSIONS_REQUEST) {
            if (grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                // Make sure that the check box is set to false.
                mCaptureAudioCheckBox.setChecked(false);
            }
            mService.setCaptureAudio(mSelectedModelUuid, mCaptureAudioCheckBox.isChecked());
        }
    }

    public synchronized void onPlayCapturedAudioButtonClicked(View v) {
        if (mService == null) {
            Log.e(TAG, "Can't play captured audio: not bound to SoundTriggerTestService");
        } else {
            mService.playCapturedAudio(mSelectedModelUuid);
        }
    }

    public synchronized void onRadioButtonClicked(View view) {
        // Is the button now checked?
        boolean checked = ((RadioButton) view).isChecked();
        if (checked) {
            mSelectedModelUuid = mButtonModelUuidMap.get(view);
            showMessage("Selected " + mModelNames.get(mSelectedModelUuid), false);
            updateSelectModelSpecificUiElements();
        }
    }

    public void setTriggerUIEnabled(boolean enable) {
            mLoadButton.setEnabled(enable);
            mStartButton.setEnabled(enable);
            mStopButton.setEnabled(enable);
            mUnloadButton.setEnabled(enable);
            mReloadButton.setEnabled(enable);
            mPlayTriggerButton.setEnabled(enable);
            mPlayCapturedAudioButton.setEnabled(enable);
    }

    private synchronized void updateSelectModelSpecificUiElements() {
        // Similar logic for the captured audio.
        mCaptureAudioCheckBox.setChecked(
                mService.modelWillCaptureTriggerAudio(mSelectedModelUuid));
        mCaptureAudioCheckBox.setEnabled(!mService.isStarted(mSelectedModelUuid));

        if (isAutoTesting) {
            setTriggerUIEnabled(false);
            mAutoTestCheckBox.setEnabled(false);
            mAutoStopStartButton.setEnabled(false);
            mAutoVtsRecButton.setEnabled(false);
        } else {
            // Set the play trigger button to be enabled only if we actually have some audio.
            mPlayTriggerButton.setEnabled(mService.modelHasTriggerAudio((mSelectedModelUuid)));
            mCaptureAudioCheckBox.setEnabled(!mService.isStarted(mSelectedModelUuid));
            mPlayCapturedAudioButton.setEnabled(mService.modelHasCapturedAudio((mSelectedModelUuid)));
            mAutoVtsRecButton.setEnabled(!isVtsRecording);
            mAutoStopStartButton.setEnabled(mService.isStarted(mSelectedModelUuid));
            mVtsRecStartButton.setEnabled(!isVtsRecording);
            mVtsRecStopButton.setEnabled(isVtsRecording);

            if (isAutoStopStartTesting || isAutoVtsRecTesting) {
                setTriggerUIEnabled(false);
                mAutoStopStartButton.setEnabled(true);
                mCaptureAudioCheckBox.setEnabled(false);
                mVtsRecStartButton.setEnabled(false);
                mVtsRecStopButton.setEnabled(false);
                if (isAutoStopStartTesting)
                    mAutoVtsRecButton.setEnabled(false);
                else if (isAutoVtsRecTesting)
                    mAutoStopStartButton.setEnabled(false);

                mPlayCapturedAudioButton.setEnabled(false);
            } else {
                setTriggerUIEnabled(true);
            }
        }
    }

    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            synchronized (SoundTriggerTestActivity.this) {
                // We've bound to LocalService, cast the IBinder and get LocalService instance
                SoundTriggerTestBinder binder = (SoundTriggerTestBinder) service;
                mService = binder.getService();
                mService.setUserActivity(SoundTriggerTestActivity.this);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            synchronized (SoundTriggerTestActivity.this) {
                mService.setUserActivity(null);
                mService = null;
            }
        }
    };

    @Override
    public void onBackPressed() {
        Log.d(TAG, "onBackPressed Called");
        Intent setIntent = new Intent(Intent.ACTION_MAIN);
        setIntent.addCategory(Intent.CATEGORY_HOME);
        setIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(setIntent);
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver();
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver();
    }

    private void registerReceiver() {
        if(mReceiver != null) return;

        final IntentFilter mFilter = new IntentFilter();
        mFilter.addAction(BROADCAST_MSG);

        this.mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String testItem = intent.getStringExtra("TEST_ITEM");
                if (intent.getAction().equals(BROADCAST_MSG)){
                    Toast.makeText(context, "test item : " + testItem, Toast.LENGTH_SHORT).show();
                    Log.d(TAG, "test item : " + testItem);
                    if (mService == null) {
                        Log.e(TAG, "Could not load sound model: not bound to SoundTriggerTestService");
                        /* add error function */
                    }

                    mService.setTestItem(testItem);

                    if (testItem.equals("LOAD")) {
                        mService.setCaptureAudio(mSelectedModelUuid, true);
                        mService.loadModel(mSelectedModelUuid);
                    } else if (testItem.equals("START")) {
                        mService.startRecognition(mSelectedModelUuid);
                    } else if (testItem.equals("STOP")) {
                        mService.stopRecognition(mSelectedModelUuid);
                    } else if (testItem.equals("UNLOAD")) {
                        mService.unloadModel(mSelectedModelUuid);
                    } else if (testItem.equals("VTS_REC")) {
                        mService.startVtsRecord(5);
                    } else if (testItem.equals("TRIGGER")) {
                        /* Nothing to do */
                    }
                }
            }
        };

        this.registerReceiver(this.mReceiver, mFilter);
    }

    private void unregisterReceiver() {
        if(mReceiver != null){
            this.unregisterReceiver(mReceiver);
            mReceiver = null;
        }
    }
}
