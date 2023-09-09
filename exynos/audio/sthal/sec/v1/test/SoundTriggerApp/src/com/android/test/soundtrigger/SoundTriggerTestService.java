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

import android.Manifest;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.media.soundtrigger.SoundTriggerDetector;
import android.media.soundtrigger.SoundTriggerManager;
import android.net.Uri;
import android.os.Binder;
import android.os.IBinder;
import android.os.SystemClock;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Random;
import java.util.UUID;
import java.util.Date;
import java.util.ArrayList;
import java.text.SimpleDateFormat;

public class SoundTriggerTestService extends Service {
    private static final String TAG = "SoundTriggerTestSrv";
    private static final String INTENT_ACTION = "com.android.intent.action.MANAGE_SOUND_TRIGGER";

    private static final String LOG_FILE = "log.txt";
    private static final String DET_LOG_FILE = "det_log.txt";
    private static final String ERROR_LOG_FILE = "error_log.txt";

    private static final int AUTO_TEST_MAX_WAITING_TIME = 3000;
    private static final int AUTO_TEST_MIN_WAITING_TIME = 500;

    // Binder given to clients.
    private final IBinder mBinder;
    private final Map<UUID, ModelInfo> mModelInfoMap;
    private SoundTriggerUtil mSoundTriggerUtil;
    private Random mRandom;
    private UserActivity mUserActivity;

    private Thread mVtsRecThread = null;
    private Thread mAutoStopStartThread = null;
    private Thread mAutoVtsRecThread = null;
    private boolean isRecording = false;
    private String recType = "NORMAL";
    private boolean isAutoStopStartTesting = false;
    private boolean isAutoVtsRecTesting = false;
    private String testCaseItem = "";

    public interface UserActivity {
        void addModel(UUID modelUuid, String state);
        void setModelState(UUID modelUuid, String state);
        void showMessage(String msg, boolean showToast);
        void handleDetection(UUID modelUuid);
        boolean getAutoTesting();
    }

    public SoundTriggerTestService() {
        super();
        mRandom = new Random();
        mModelInfoMap = new HashMap();
        mBinder = new SoundTriggerTestBinder();
    }

    @Override
    public synchronized int onStartCommand(Intent intent, int flags, int startId) {
        if (mModelInfoMap.isEmpty()) {
            mSoundTriggerUtil = new SoundTriggerUtil(this);
            loadModelsInDataDir();
        }

        // If we get killed, after returning from here, restart
        return START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        IntentFilter filter = new IntentFilter();
        filter.addAction(INTENT_ACTION);
        registerReceiver(mBroadcastReceiver, filter);

        // Make sure the data directory exists, and we're the owner of it.
        try {
            getFilesDir().mkdir();
        } catch (Exception e) {
            // Don't care - we either made it, or it already exists.
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopAllRecognitionsAndUnload();
        unregisterReceiver(mBroadcastReceiver);
    }

    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null && INTENT_ACTION.equals(intent.getAction())) {
                String command = intent.getStringExtra("command");
                if (command == null) {
                    Log.e(TAG, "No 'command' specified in " + INTENT_ACTION);
                } else {
                    try {
                        if (command.equals("load")) {
                            loadModel(getModelUuidFromIntent(intent));
                        } else if (command.equals("unload")) {
                            unloadModel(getModelUuidFromIntent(intent));
                        } else if (command.equals("start")) {
                            startRecognition(getModelUuidFromIntent(intent));
                        } else if (command.equals("stop")) {
                            stopRecognition(getModelUuidFromIntent(intent));
                        } else if (command.equals("play_trigger")) {
                            playTriggerAudio(getModelUuidFromIntent(intent));
                        } else if (command.equals("play_captured")) {
                            playCapturedAudio(getModelUuidFromIntent(intent));
                        } else if (command.equals("set_capture")) {
                            setCaptureAudio(getModelUuidFromIntent(intent),
                                    intent.getBooleanExtra("enabled", true));
                        } else if (command.equals("set_capture_timeout")) {
                            setCaptureAudioTimeout(getModelUuidFromIntent(intent),
                                    intent.getIntExtra("timeout", 5000));
                        } else {
                            Log.e(TAG, "Unknown command '" + command + "'");
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to process " + command, e);
                    }
                }
            }
        }
    };

    private UUID getModelUuidFromIntent(Intent intent) {
        // First, see if the specified the UUID straight up.
        String value = intent.getStringExtra("modelUuid");
        if (value != null) {
            return UUID.fromString(value);
        }

        // If they specified a name, use that to iterate through the map of models and find it.
        value = intent.getStringExtra("name");
        if (value != null) {
            for (ModelInfo modelInfo : mModelInfoMap.values()) {
                if (value.equals(modelInfo.name)) {
                    return modelInfo.modelUuid;
                }
            }
            Log.e(TAG, "Failed to find a matching model with name '" + value + "'");
        }

        // We couldn't figure out what they were asking for.
        throw new RuntimeException("Failed to get model from intent - specify either " +
                "'modelUuid' or 'name'");
    }

    /**
     * Will be called when the service is killed (through swipe aways, not if we're force killed).
     */
    @Override
    public void onTaskRemoved(Intent rootIntent) {
        super.onTaskRemoved(rootIntent);
        stopAllRecognitionsAndUnload();
        stopSelf();
    }

    @Override
    public synchronized IBinder onBind(Intent intent) {
        return mBinder;
    }

    public class SoundTriggerTestBinder extends Binder {
        SoundTriggerTestService getService() {
            // Return instance of our parent so clients can call public methods.
            return SoundTriggerTestService.this;
        }
    }

    public synchronized void setUserActivity(UserActivity activity) {
        mUserActivity = activity;
        if (mUserActivity != null) {
            for (Map.Entry<UUID, ModelInfo> entry : mModelInfoMap.entrySet()) {
                mUserActivity.addModel(entry.getKey(), entry.getValue().name);
                mUserActivity.setModelState(entry.getKey(), entry.getValue().state);
            }
        }
    }

    private synchronized void stopAllRecognitionsAndUnload() {
        Log.e(TAG, "Stop all recognitions");
        for (ModelInfo modelInfo : mModelInfoMap.values()) {
            Log.e(TAG, "Loop " + modelInfo.modelUuid);
            if (modelInfo.detector != null) {
                Log.i(TAG, "Stopping recognition for " + modelInfo.name);
                try {
                    modelInfo.detector.stopRecognition();
                } catch (Exception e) {
                    Log.e(TAG, "Failed to stop recognition", e);
                }
                try {
                    mSoundTriggerUtil.deleteSoundModel(modelInfo.modelUuid);
                    modelInfo.detector = null;
                } catch (Exception e) {
                    Log.e(TAG, "Failed to unload sound model", e);
                }
            }
        }
    }

    // Helper struct for holding information about a model.
    public static class ModelInfo {
        public String name;
        public String state;
        public UUID modelUuid;
        public UUID vendorUuid;
        public MediaPlayer triggerAudioPlayer;
        public SoundTriggerDetector detector;
        public byte modelData[];
        public boolean captureAudio;
        public int captureAudioMs;
        public AudioTrack captureAudioTrack;
    }

    private SoundTriggerManager.Model createNewSoundModel(ModelInfo modelInfo) {
        return SoundTriggerManager.Model.create(modelInfo.modelUuid, modelInfo.vendorUuid,
                modelInfo.modelData);
    }

    public synchronized void loadModel(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }

        postMessage("Loading model: " + modelInfo.name);

        SoundTriggerManager.Model soundModel = createNewSoundModel(modelInfo);

        boolean status = mSoundTriggerUtil.addOrUpdateSoundModel(soundModel);
        if (status) {
            postToast("Successfully loaded " + modelInfo.name + ", UUID="
                    + soundModel.getModelUuid());
            setModelState(modelInfo, "Loaded");
            saveTestResult("LOAD", true);
        } else {
            postErrorToast("Failed to load " + modelInfo.name + ", UUID="
                    + soundModel.getModelUuid() + "!");
            setModelState(modelInfo, "Failed to load");
        }
    }

    public synchronized void unloadModel(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }

        postMessage("Unloading model: " + modelInfo.name);

        SoundTriggerManager.Model soundModel = mSoundTriggerUtil.getSoundModel(modelUuid);
        if (soundModel == null) {
            postErrorToast("Sound model not found for " + modelInfo.name + "!");
            return;
        }
        modelInfo.detector = null;
        boolean status = mSoundTriggerUtil.deleteSoundModel(modelUuid);
        if (status) {
            postToast("Successfully unloaded " + modelInfo.name + ", UUID="
                    + soundModel.getModelUuid());
            setModelState(modelInfo, "Unloaded");
            saveTestResult("UNLOAD", true);
        } else {
            postErrorToast("Failed to unload " +
                    modelInfo.name + ", UUID=" + soundModel.getModelUuid() + "!");
            setModelState(modelInfo, "Failed to unload");
        }
    }

    public synchronized void reloadModel(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }
        postMessage("Reloading model: " + modelInfo.name);
        SoundTriggerManager.Model soundModel = mSoundTriggerUtil.getSoundModel(modelUuid);
        if (soundModel == null) {
            postErrorToast("Sound model not found for " + modelInfo.name + "!");
            return;
        }
        SoundTriggerManager.Model updated = createNewSoundModel(modelInfo);
        boolean status = mSoundTriggerUtil.addOrUpdateSoundModel(updated);
        if (status) {
            postToast("Successfully reloaded " + modelInfo.name + ", UUID="
                    + modelInfo.modelUuid);
            setModelState(modelInfo, "Reloaded");
        } else {
            postErrorToast("Failed to reload "
                    + modelInfo.name + ", UUID=" + modelInfo.modelUuid + "!");
            setModelState(modelInfo, "Failed to reload");
        }
    }

    public synchronized void startRecognition(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }

        if (modelInfo.detector == null) {
            postMessage("Creating SoundTriggerDetector for " + modelInfo.name);
            modelInfo.detector = mSoundTriggerUtil.createSoundTriggerDetector(
                    modelUuid, new DetectorCallback(modelInfo));
        }

        postMessage("Starting recognition for " + modelInfo.name + ", UUID="
                + modelInfo.modelUuid);
        if (modelInfo.detector.startRecognition(modelInfo.captureAudio ?
                SoundTriggerDetector.RECOGNITION_FLAG_CAPTURE_TRIGGER_AUDIO :
                SoundTriggerDetector.RECOGNITION_FLAG_ALLOW_MULTIPLE_TRIGGERS)) {
            setModelState(modelInfo, "Started");
            saveTestResult("START", true);
        } else {
            postErrorToast("Fast failure attempting to start recognition for " +
                    modelInfo.name + ", UUID=" + modelInfo.modelUuid);
            setModelState(modelInfo, "Failed to start");
        }
    }

    public synchronized void stopRecognition(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }

        if (modelInfo.detector == null) {
            postErrorToast("Stop called on null detector for " +
                    modelInfo.name + ", UUID=" + modelInfo.modelUuid);
            return;
        }
        postMessage("Triggering stop recognition for " +
                modelInfo.name + ", UUID=" + modelInfo.modelUuid);
        if (modelInfo.detector.stopRecognition()) {
            setModelState(modelInfo, "Stopped");
            saveTestResult("STOP", true);
        } else {
            postErrorToast("Fast failure attempting to stop recognition for " +
                    modelInfo.name + ", UUID=" + modelInfo.modelUuid);
            setModelState(modelInfo, "Failed to stop");
        }
    }

    public synchronized void playTriggerAudio(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }
        Context context = getApplicationContext();
        AudioManager mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC,
                mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);

        if (modelInfo.triggerAudioPlayer != null) {
            postMessage("Playing trigger audio for " + modelInfo.name);
            modelInfo.triggerAudioPlayer.start();
        } else {
            postMessage("No trigger audio for " + modelInfo.name);
        }
    }

    public synchronized void playCapturedAudio(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }
        if (modelInfo.captureAudioTrack != null) {
            postMessage("Playing captured audio for " + modelInfo.name);
            modelInfo.captureAudioTrack.stop();
            modelInfo.captureAudioTrack.reloadStaticData();
            modelInfo.captureAudioTrack.play();
        } else {
            postMessage("No captured audio for " + modelInfo.name);
        }
    }

    public synchronized void startRecord(int sec, String type) {
        Log.e(TAG, "enter start" + type + "Record");
        postMessage("start " + type + "Record");
        isRecording = true;
        recType = type;

        mVtsRecThread = new Thread(new VtsAudioRecorder(sec));
        mVtsRecThread.start();
        Log.e(TAG, "exit startRecord");
    }

    public synchronized void stopRecord() {
        Log.e(TAG, "enter stopRecord, isRecording: " + isRecording);
        postMessage("stop VTSRecord");
        if (isRecording) {
            isRecording = false;
            try {
                mVtsRecThread.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            Log.d(TAG, "Already stopped");
        }
    }

    public synchronized void autoStopStartTest(boolean enable) {
        Log.e(TAG, "enter autoStopStartTest, enable: " + enable);
        if (enable) {
            isAutoStopStartTesting = true;
            mAutoStopStartThread = new Thread(new AutoStopStartThread());
            mAutoStopStartThread.start();
        } else {
            isAutoStopStartTesting = false;
            /*
            try {
                mAutoStopStartThread.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
            */
        }
    }

    private class AutoStopStartThread implements Runnable {
        @Override
        public void run() {
            Random random = new Random();

            while(isAutoStopStartTesting) {
                int onTime = random.nextInt(AUTO_TEST_MAX_WAITING_TIME - AUTO_TEST_MIN_WAITING_TIME)
                    + AUTO_TEST_MIN_WAITING_TIME;
                int offTime = random.nextInt(AUTO_TEST_MAX_WAITING_TIME - AUTO_TEST_MIN_WAITING_TIME)
                    + AUTO_TEST_MIN_WAITING_TIME;
                Log.e(TAG, "onTime: " + onTime + ", offTime: " + offTime);

                for (ModelInfo modelInfo : mModelInfoMap.values()) {
                    Log.e(TAG, "Loop " + modelInfo.modelUuid);
                    if (modelInfo.detector != null) {
                        Log.i(TAG, "Stopping recognition for " + modelInfo.name);
                        stopRecognition(modelInfo.modelUuid);
                    }
                }

                try {
                    Thread.sleep(offTime);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                for (ModelInfo modelInfo : mModelInfoMap.values()) {
                    Log.e(TAG, "Loop " + modelInfo.modelUuid);
                    if (modelInfo.detector != null) {
                        Log.i(TAG, "Starting recognition for " + modelInfo.name);
                        startRecognition(modelInfo.modelUuid);
                    }
                }
                try {
                    Thread.sleep(onTime);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

    }

    public synchronized void autoVtsRecTest(boolean enable) {
        Log.e(TAG, "enter autoVtsRecTest, enable: " + enable);
        if (enable) {
            postMessage("Start Auto VTS Record");
            isAutoVtsRecTesting = true;
            mAutoVtsRecThread = new Thread(new AutoVtsRecThread());
            mAutoVtsRecThread.start();
        } else {
            postMessage("Stop Auto VTS Record");
            isAutoVtsRecTesting = false;
            try {
                mAutoVtsRecThread.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private class AutoVtsRecThread implements Runnable {
        @Override
        public void run() {
            Random random = new Random();

            while(isAutoVtsRecTesting) {
                int onTime = random.nextInt(AUTO_TEST_MAX_WAITING_TIME - AUTO_TEST_MIN_WAITING_TIME)
                    + AUTO_TEST_MIN_WAITING_TIME;
                int offTime = random.nextInt(AUTO_TEST_MAX_WAITING_TIME - AUTO_TEST_MIN_WAITING_TIME)
                    + AUTO_TEST_MIN_WAITING_TIME;
                Log.e(TAG, "onTime: " + onTime + ", offTime: " + offTime);
                startRecord(0, "VTS");

                try {
                    Thread.sleep(onTime);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                stopRecord();

                try {
                    Thread.sleep(offTime);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

    }

    public synchronized void setCaptureAudioTimeout(UUID modelUuid, int captureTimeoutMs) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }
        modelInfo.captureAudioMs = captureTimeoutMs;
        Log.i(TAG, "Set " + modelInfo.name + " capture audio timeout to " +
                captureTimeoutMs + "ms");
    }

    public synchronized void setCaptureAudio(UUID modelUuid, boolean captureAudio) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo == null) {
            postError("Could not find model for: " + modelUuid.toString());
            return;
        }
        modelInfo.captureAudio = captureAudio;
        Log.i(TAG, "Set " + modelInfo.name + " capture audio to " + captureAudio);
    }

    public synchronized boolean hasMicrophonePermission() {
        return getBaseContext().checkSelfPermission(Manifest.permission.RECORD_AUDIO)
                == PackageManager.PERMISSION_GRANTED;
    }

    public synchronized boolean modelHasTriggerAudio(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        return modelInfo != null && modelInfo.triggerAudioPlayer != null;
    }

    public synchronized boolean modelWillCaptureTriggerAudio(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        return modelInfo != null && modelInfo.captureAudio;
    }

    public synchronized boolean isStarted(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        if (modelInfo.state == "Started" || modelInfo.state == "Resumed" || modelInfo.state == "Recording")
            return true;
        else
            return false;
    }

    public synchronized boolean modelHasCapturedAudio(UUID modelUuid) {
        ModelInfo modelInfo = mModelInfoMap.get(modelUuid);
        return modelInfo != null && modelInfo.captureAudioTrack != null;
    }

    private void loadModelsInDataDir() {
        // Load all the models in the data dir.
        boolean loadedModel = false;
        for (File file : getFilesDir().listFiles()) {
            // Find meta-data in .properties files, ignore everything else.
            if (!file.getName().endsWith(".properties")) {
                continue;
            }

            try (FileInputStream in = new FileInputStream(file)) {
                Properties properties = new Properties();
                properties.load(in);
                createModelInfo(properties);
                loadedModel = true;
            } catch (Exception e) {
                Log.e(TAG, "Failed to load properties file " + file.getName());
            }
        }

        // Create a few dummy models if we didn't load anything.
        if (!loadedModel) {
            Properties dummyModelProperties = new Properties();
            for (String name : new String[]{"1", "2", "3"}) {
                dummyModelProperties.setProperty("name", "Model " + name);
                createModelInfo(dummyModelProperties);
            }
        }
    }

    /** Parses a Properties collection to generate a sound model.
     *
     * Missing keys are filled in with default/random values.
     * @param properties Has the required 'name' property, but the remaining 'modelUuid',
     *                   'vendorUuid', 'triggerAudio', and 'dataFile' optional properties.
     *
     */
    private synchronized void createModelInfo(Properties properties) {
        try {
            ModelInfo modelInfo = new ModelInfo();

            if (!properties.containsKey("name")) {
                throw new RuntimeException("must have a 'name' property");
            }
            modelInfo.name = properties.getProperty("name");

            if (properties.containsKey("modelUuid")) {
                modelInfo.modelUuid = UUID.fromString(properties.getProperty("modelUuid"));
            } else {
                modelInfo.modelUuid = UUID.randomUUID();
            }

            if (properties.containsKey("vendorUuid")) {
                modelInfo.vendorUuid = UUID.fromString(properties.getProperty("vendorUuid"));
            } else {
                modelInfo.vendorUuid = UUID.randomUUID();
            }

            if (properties.containsKey("triggerAudio")) {
                modelInfo.triggerAudioPlayer = MediaPlayer.create(this, Uri.parse(
                        getFilesDir().getPath() + "/" + properties.getProperty("triggerAudio")));
                if (modelInfo.triggerAudioPlayer.getDuration() == 0) {
                    modelInfo.triggerAudioPlayer.release();
                    modelInfo.triggerAudioPlayer = null;
                }
            }

            if (properties.containsKey("dataFile")) {
                File modelDataFile = new File(
                        getFilesDir().getPath() + "/"
                                + properties.getProperty("dataFile"));
                modelInfo.modelData = new byte[(int) modelDataFile.length()];
                FileInputStream input = new FileInputStream(modelDataFile);
                input.read(modelInfo.modelData, 0, modelInfo.modelData.length);
            } else {
                modelInfo.modelData = new byte[1024];
                mRandom.nextBytes(modelInfo.modelData);
            }

            modelInfo.captureAudioMs = Integer.parseInt((String) properties.getOrDefault(
                    "captureAudioDurationMs", "5000"));

            // TODO: Add property support for keyphrase models when they're exposed by the
            // service.

            // Update our maps containing the button -> id and id -> modelInfo.
            mModelInfoMap.put(modelInfo.modelUuid, modelInfo);
            if (mUserActivity != null) {
                mUserActivity.addModel(modelInfo.modelUuid, modelInfo.name);
                mUserActivity.setModelState(modelInfo.modelUuid, modelInfo.state);
            }
        } catch (IOException e) {
            Log.e(TAG, "Error parsing properties for " + properties.getProperty("name"), e);
        }
    }

    private class VtsAudioRecorder implements Runnable {
        private int frame = 0;
        private int recording_sec = 0;

        VtsAudioRecorder() {};
        VtsAudioRecorder(int sec) {
            recording_sec = sec;
        };
            @Override
            public void run() {
                AudioRecord mAudioRecord;
                AudioManager mAudioManager;

                AudioAttributes.Builder attributesBuilder = new AudioAttributes.Builder();
                attributesBuilder.setInternalCapturePreset(6);
                AudioAttributes attributes = attributesBuilder.build();

                Context context = getApplicationContext();
                mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
                if (recType.equals("VTS"))
                    mAudioManager.setParameters("vts_record_enable=true");

                int mBufferSize = AudioRecord.getMinBufferSize(16000,
                        AudioFormat.CHANNEL_IN_MONO,
                        AudioFormat.ENCODING_PCM_16BIT);
                Log.e(TAG, "mBufferSize:" + mBufferSize);
                byte[] readData = new byte[mBufferSize];
                String filePath = getFilesDir().getPath() + "/pcm";
                File file = new File(filePath);
                if (!file.exists())
                    file.mkdir();
                filePath += "/";

                SimpleDateFormat dateFormat = new SimpleDateFormat("_yyyy-MM-dd_HH.mm.ss");
                Date time = new Date();
                String fileName =   filePath + File.separator
                                    + recType + "Rec_1ch_16000hz_16bit"
                                    + dateFormat.format(time) + ".pcm";

                FileOutputStream fos = null;
                try {
                    fos = new FileOutputStream(new File(fileName));
                } catch(IOException e) {
                    e.printStackTrace();
                }

                mAudioRecord = new AudioRecord.Builder()
                    .setAudioFormat(new AudioFormat.Builder()
                        .setSampleRate(16000)
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setChannelMask(AudioFormat.CHANNEL_IN_MONO).build())
                    .setAudioSource(MediaRecorder.AudioSource.VOICE_RECOGNITION)
                    .setBufferSizeInBytes(mBufferSize)
                    .setAudioAttributes(attributes)
                    .build();

                mAudioRecord.startRecording();

                while(isRecording) {
                    int ret = mAudioRecord.read(readData, 0, mBufferSize);
                    //Log.d(TAG, "read bytes is " + ret);

                    try {
                        fos.write(readData, 0, ret);
                    }catch (IOException e){
                        e.printStackTrace();
                    }
                    frame++;
                    if (frame % 125 == 0) { // 5sec (25frame for 1sec * 5sec)
                        postMessage(recType + " Recording for " + frame / 25 + " sec");
                    }

                    if (recording_sec != 0 && (frame / 25 >= recording_sec)) {
                        Log.d(TAG, "recording_sec: " + recording_sec + ", frame: " + frame);
                        isRecording = false;
                    }
                }

                mAudioRecord.stop();
                mAudioRecord.release();
                mAudioRecord = null;
                if (recType.equals("VTS"))
                    mAudioManager.setParameters("vts_record_enable=false");

                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                file = new File(fileName);
                Log.d(TAG, "End of " + recType + " AudioRecorder, file length : " + file.length());

                if (file.length() > 0) {
                    if (recType.equals("VTS"))
                        saveTestResult("VTS_REC", true);
                    else
                        saveTestResult("NORMAL_REC", true);
                }
            }
    }

    private class CaptureAudioRecorder implements Runnable {
        private final ModelInfo mModelInfo;
        private final SoundTriggerDetector.EventPayload mEvent;

        public CaptureAudioRecorder(ModelInfo modelInfo, SoundTriggerDetector.EventPayload event) {
            mModelInfo = modelInfo;
            mEvent = event;
        }

        @Override
        public void run() {
            AudioFormat format = mEvent.getCaptureAudioFormat();
            if (format == null) {
                postErrorToast("No audio format in recognition event.");
                return;
            }

            AudioRecord audioRecord = null;
            AudioTrack playbackTrack = null;
            try {
                // Inform the audio flinger that we really do want the stream from the soundtrigger.
                AudioAttributes.Builder attributesBuilder = new AudioAttributes.Builder();
                attributesBuilder.setInternalCapturePreset(1999);
                AudioAttributes attributes = attributesBuilder.build();

                // Make sure we understand this kind of playback so we know how many bytes to read.
                String encoding;
                int bytesPerSample;
                switch (format.getEncoding()) {
                    case AudioFormat.ENCODING_PCM_8BIT:
                        encoding = "8bit";
                        bytesPerSample = 1;
                        break;
                    case AudioFormat.ENCODING_PCM_16BIT:
                        encoding = "16bit";
                        bytesPerSample = 2;
                        break;
                    case AudioFormat.ENCODING_PCM_FLOAT:
                        encoding = "float";
                        bytesPerSample = 4;
                        break;
                    default:
                        throw new RuntimeException("Unhandled audio format in event");
                }

                int bytesRequired = format.getSampleRate() * format.getChannelCount() *
                        bytesPerSample * mModelInfo.captureAudioMs / 1000;
                int minBufferSize = AudioRecord.getMinBufferSize(
                        format.getSampleRate(), format.getChannelMask(), format.getEncoding());
                if (minBufferSize > bytesRequired) {
                    bytesRequired = minBufferSize;
                }

                // Make an AudioTrack so we can play the data back out after it's finished
                // recording.
                try {
                    int channelConfig = AudioFormat.CHANNEL_OUT_MONO;
                    if (format.getChannelCount() == 2) {
                        channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
                    } else if (format.getChannelCount() >= 3) {
                        throw new RuntimeException(
                                "Too many channels in captured audio for playback");
                    }

                    playbackTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                            format.getSampleRate(), channelConfig, format.getEncoding(),
                            bytesRequired, AudioTrack.MODE_STATIC);
                } catch (Exception e) {
                    Log.e(TAG, "Exception creating playback track", e);
                    postErrorToast("Failed to create playback track: " + e.getMessage());
                }

                audioRecord = new AudioRecord(attributes, format, bytesRequired,
                        mEvent.getCaptureSession());

                byte[] buffer = new byte[bytesRequired];

                // Create a file so we can save the output data there for analysis later.
                FileOutputStream fos  = null;

                String filePath = getFilesDir().getPath() + "/pcm";
                File file = new File(filePath);
                if (!file.exists())
                    file.mkdir();
                filePath += "/";

                SimpleDateFormat dateFormat = new SimpleDateFormat("_yyyy-MM-dd_HH.mm.ss");
                Date time = new Date();
                String fileName =   filePath + File.separator
                                    + mModelInfo.name.replace(' ', '_')
                                    + "_capture_" + format.getChannelCount() + "ch_"
                                    + format.getSampleRate() + "hz_" + encoding + dateFormat.format(time) + ".pcm";

                try {
                    fos = new FileOutputStream( new File(fileName));
                } catch (IOException e) {
                    Log.e(TAG, "Failed to open output for saving PCM data", e);
                    postErrorToast("Failed to open output for saving PCM data: "
                            + e.getMessage());
                }

                // Inform the user we're recording.
                setModelState(mModelInfo, "Recording");
                audioRecord.startRecording();
                long startTime = SystemClock.elapsedRealtime();
                long recordingTime = SystemClock.elapsedRealtime();
                while (bytesRequired > 0) {
                    int bytesRead = audioRecord.read(buffer, 0, buffer.length);
                    if (bytesRead == -1) {
                        break;
                    }
                    if (fos != null) {
                        fos.write(buffer, 0, bytesRead);
                    }
                    if (playbackTrack != null) {
                        playbackTrack.write(buffer, 0, bytesRead);
                    }
                    bytesRequired -= bytesRead;
                    recordingTime = SystemClock.elapsedRealtime();
                    if (recordingTime - startTime > mModelInfo.captureAudioMs + 1000) {
                        postError(mModelInfo.name + " Recording Time is over. recordingTime : " + (recordingTime - startTime) + "msec");
                        break;
                    }
                }
                audioRecord.stop();

                file = new File(fileName);
                Log.d(TAG, "End of CaptureAudio, file length : " + file.length());

                if (file.length() > 0) {
                    saveTestResult("TRIGGER", true);
                }
            } catch (Exception e) {
                Log.e(TAG, "Error recording trigger audio", e);
                postErrorToast("Error recording trigger audio: " + e.getMessage());
            } finally {
                if (audioRecord != null) {
                    audioRecord.release();
                }
                synchronized (SoundTriggerTestService.this) {
                    if (mModelInfo.captureAudioTrack != null) {
                        mModelInfo.captureAudioTrack.release();
                    }
                    mModelInfo.captureAudioTrack = playbackTrack;
                }
                setModelState(mModelInfo, "Recording finished");
                startRecognition(mModelInfo.modelUuid);
            }
        }
    }

    // Implementation of SoundTriggerDetector.Callback.
    private class DetectorCallback extends SoundTriggerDetector.Callback {
        private final ModelInfo mModelInfo;

        public DetectorCallback(ModelInfo modelInfo) {
            mModelInfo = modelInfo;
        }

        public void onAvailabilityChanged(int status) {
            postMessage(mModelInfo.name + " availability changed to: " + status);
        }

        public void onDetected(SoundTriggerDetector.EventPayload event) {
            postMessage(mModelInfo.name + " onDetected(): " + eventPayloadToString(event));
            printLog(mModelInfo, "onDetected()", DET_LOG_FILE);
            synchronized (SoundTriggerTestService.this) {
                if (mUserActivity != null && mUserActivity.getAutoTesting() == false) {
                    Context context = getApplicationContext();
                    Intent intent = new Intent(context, SoundTriggerTestActivity.class);
                    intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TOP|Intent.FLAG_ACTIVITY_SINGLE_TOP);

                    context.startActivity(intent);

                    if (mUserActivity != null) {
                        mUserActivity.handleDetection(mModelInfo.modelUuid);
                    }

                    if (mModelInfo.captureAudio) {
                        new Thread(new CaptureAudioRecorder(mModelInfo, event)).start();
                    }
                } else {
                    showMessage("detected", Log.INFO, true);
                    if (mModelInfo.captureAudio) {
                        new Thread(new CaptureAudioRecorder(mModelInfo, event)).start();
                    }
                }
            }
        }

        public void onError() {
            postMessage(mModelInfo.name + " onError()");
            setModelState(mModelInfo, "Error");
        }

        public void onRecognitionPaused() {
            postMessage(mModelInfo.name + " onRecognitionPaused()");
            setModelState(mModelInfo, "Paused");
        }

        public void onRecognitionResumed() {
            postMessage(mModelInfo.name + " onRecognitionResumed()");
            setModelState(mModelInfo, "Resumed");
        }
    }

    /* TC */
    public synchronized void setTestItem(String testItem) {
        testCaseItem = testItem;
    }

    private void saveTestResult(String testItem, boolean isPass) {
        Log.e(TAG, "testItem: " + testItem + ", testCaseItem: " + testCaseItem);
        if (!testItem.equals(testCaseItem)) {
            return;
        }

        File file = new File(getFilesDir().getPath() + "/" + testItem + "_result.txt");

        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(file));
            if (isPass)
                writer.write(testItem + " PASS\n");
            else
                writer.write(testItem + " FAIL\n");

            writer.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    private String eventPayloadToString(SoundTriggerDetector.EventPayload event) {
        String result = "EventPayload(";
        AudioFormat format =  event.getCaptureAudioFormat();
        result = result + "AudioFormat: " + ((format == null) ? "null" : format.toString());
        byte[] triggerAudio = event.getTriggerAudio();
        result = result + ", TriggerAudio: "
                + (triggerAudio == null ? "null" : triggerAudio.length);
        byte[] data = event.getData();
        result = result + ", Data: " + (data == null ? "null" : data.length);
        if (data != null) {
          try {
            String decodedData = new String(data, "UTF-8");
            if (decodedData.chars().allMatch(c -> (c >= 32 && c < 128) || c == 0)) {
                result = result + ", Decoded Data: '" + decodedData + "'";
            }
          } catch (Exception e) {
            Log.e(TAG, "Failed to decode data");
          }
        }
        result = result + ", CaptureSession: " + event.getCaptureSession();
        result += " )";
        return result;
    }

    private void postMessage(String msg) {
        showMessage(msg, Log.INFO, false);
        printLog(null, msg, null);
    }

    private void postError(String msg) {
        showMessage(msg, Log.ERROR, false);
        printLog(null, msg, ERROR_LOG_FILE);
    }

    private void postToast(String msg) {
        showMessage(msg, Log.INFO, true);
    }

    private void postErrorToast(String msg) {
        showMessage(msg, Log.ERROR, true);
    }

    /** Logs the message at the specified level, then forwards it to the activity if present. */
    private synchronized void showMessage(String msg, int logLevel, boolean showToast) {
        Log.println(logLevel, TAG, msg);
        if (mUserActivity != null) {
            mUserActivity.showMessage(msg, showToast);
        }
    }

    private synchronized void setModelState(ModelInfo modelInfo, String state) {
        modelInfo.state = state;
        if (mUserActivity != null) {
            mUserActivity.setModelState(modelInfo.modelUuid, modelInfo.state);
        }
    }

    private void printLog(ModelInfo modelInfo, String msg, String fileName) {
        ArrayList<File> fileList = new ArrayList<File>();

        String filePath = getFilesDir().getPath() + "/logs";

        File file = new File(filePath);
        if (!file.exists())
            file.mkdir();

        /* log.txt */
        file = new File(filePath + "/"
                                + LOG_FILE);
        fileList.add(file);
        if (fileName != null) {
            /* filename */
            file = new File(filePath + "/"
                                + fileName);
            fileList.add(file);
        }
        if (modelInfo != null) {
            /* modelname.txt */
            file = new File(filePath + "/"
                                + modelInfo.name + ".txt");
            fileList.add(file);
        }

        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");
        Date time = new Date();
        String log_msg = dateFormat.format(time) + "\t\t";
        if (modelInfo != null)
            log_msg = log_msg + modelInfo.name + "\t: ";

        log_msg = log_msg + msg;
        try {
            for(int i = 0; i < fileList.size(); i++) {
                PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(fileList.get(i), true)));
                pw.println(log_msg);
                pw.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
