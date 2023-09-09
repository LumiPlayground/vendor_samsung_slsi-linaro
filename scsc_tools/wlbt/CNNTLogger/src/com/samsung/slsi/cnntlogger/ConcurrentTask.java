/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.cnntlogger;

import android.content.Context;
import android.os.AsyncTask;

public class ConcurrentTask extends AsyncTask<String, Integer, Boolean> {
    private ConcurrentTaskListener mListener;

    private final Context mContext;

    public ConcurrentTask(Context context, ConcurrentTaskListener listener) {
        mContext = context;
        this.mListener = listener;
    }

    public interface ConcurrentTaskListener {
        void taskCompleted(boolean result);
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
    }

    @Override
    protected Boolean doInBackground(String... params) {
        boolean result = new CmdRunner().startScript(mContext, params[0], params[1], params[2], params[3]);
        return result;
    }

    @Override
    protected void onProgressUpdate(Integer... progress) {
    }

    @Override
    protected void onPostExecute(Boolean aBoolean) {
        super.onPostExecute(aBoolean);
        if (this.mListener != null) {
            this.mListener.taskCompleted(aBoolean);
        }
    }
}
