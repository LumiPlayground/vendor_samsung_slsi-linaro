/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oem;

import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.URISyntaxException;
import java.util.ArrayList;

import android.content.Context;
import android.content.Intent;
import android.os.ShellCommand;
import android.os.UserHandle;

public class Am {

    private int mUserId;
    private Context mContext;

    final protected ShellCommand mArgs = new ShellCommand() {
        @Override
        public int onCommand(String cmd) {
            return 0;
        }
        @Override
        public void onHelp() {
        }
    };
    private String[] mRawArgs;

    public void run(Context context, String args) {
        // TODO need to split params correctly
        // e.g., --es name "value is a sentence"
        boolean start = false;
        StringBuilder sb = new StringBuilder();
        ArrayList<String> tokens = new ArrayList<String>();
        for (int i = 0; i < args.length(); i++) {
            char ch = args.charAt(i);
            if (ch == '\"') {
                if (start) {
                    String ret = sb.toString().trim();
                    if (ret != null && ret.length() > 0) {
                        tokens.add(ret);
                    }
                    sb = new StringBuilder();
                }
                start = !start;
                continue;
            }
            else if (ch == ' ') {
                if (!start) {
                    String ret = sb.toString().trim();
                    if (ret != null && ret.length() > 0) {
                        tokens.add(ret);
                    }
                    sb = new StringBuilder();
                    continue;
                }
            }
            sb.append(ch);
        }

        if (sb.length() > 0)
            tokens.add(sb.toString());

        if (start) {
            // invalid double quotes pair
            return ;
        }

        String[] newArgs = new String[tokens.size()];
        tokens.toArray(newArgs);
        run(context, newArgs);
    }

    public void run(Context context, String[] args) {
        mContext = context;
        run(args);
    }

    private void run(String[] args) {
        mRawArgs = args;
        mArgs.init(null, null, null, null, args, 0);

        try {
            onRun();
        } catch (IllegalArgumentException e) {
            System.err.println();
            System.err.println("Error: " + e.getMessage());
        } catch (Exception e) {
            e.printStackTrace(System.err);
            System.exit(1);
        }
    }

    public void onRun() throws Exception {
        String op = nextArgRequired();

        if (op.equals("start")) {
            runStart();
        } else if (op.equals("startservice")) {
            runStartService();
        } else if (op.equals("stopservice")) {
            runStopService();
        } else if (op.equals("broadcast")) {
            sendBroadcast();
        } else {
            System.out.println("Error: unknown command '" + op + "'");
        }
    }

    public String nextArgRequired() {
        return mArgs.getNextArgRequired();
    }

    int parseUserArg(String arg) {
        int userId;
        if ("all".equals(arg)) {
            userId = UserHandle.USER_ALL;
        } else if ("current".equals(arg) || "cur".equals(arg)) {
            userId = UserHandle.USER_CURRENT;
        } else {
            userId = Integer.parseInt(arg);
        }
        return userId;
    }

    private Intent makeIntent(int defUser) throws URISyntaxException {
        mUserId = defUser;
        return Intent.parseCommandArgs(mArgs, null);
    }

    private void runStartService() throws Exception {
        Intent intent = makeIntent(UserHandle.USER_CURRENT);
        if (mUserId == UserHandle.USER_ALL) {
            System.err.println("Error: Can't start activity with user 'all'");
            return;
        }
        System.out.println("Starting service: " + intent);
        try {
            mContext.startService(intent);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void runStopService() throws Exception {
        Intent intent = makeIntent(UserHandle.USER_CURRENT);
        if (mUserId == UserHandle.USER_ALL) {
            System.err.println("Error: Can't stop activity with user 'all'");
            return;
        }
        System.out.println("Stopping service: " + intent);
        try {
            mContext.stopService(intent);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void runStart() throws Exception {
        Intent intent = makeIntent(UserHandle.USER_CURRENT);

        if (mUserId == UserHandle.USER_ALL) {
            System.err.println("Error: Can't start service with user 'all'");
            return;
        }

        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        try {
            mContext.startActivity(intent);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void sendBroadcast() throws Exception {
        Intent intent = makeIntent(UserHandle.USER_CURRENT);
        try {
            mContext.sendBroadcast(intent);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
