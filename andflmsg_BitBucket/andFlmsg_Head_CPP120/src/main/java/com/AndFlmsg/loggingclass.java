package com.AndFlmsg;

import android.util.Log;

public class loggingclass {


    private static String Application = "";


    public loggingclass(String app) {
        Application = app;
    }

    /**
     * @param b
     * @param e
     * @param args
     */
//Android allow for different log levels
    public static void writelog(String msg, Exception e, boolean b) {
//Android For the moment send this to the terminal
        if (e == null) {
            Log.e(Application, msg);
        } else {
            Log.e(Application, msg, e);
        }

        Processor.TermWindow += msg;
        AndFlmsg.mHandler.post(AndFlmsg.addtoterminal);

    }

}
