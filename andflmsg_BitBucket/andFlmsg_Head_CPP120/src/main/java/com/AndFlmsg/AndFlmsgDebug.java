/*
 * This is the error reporting section based on Google's ACRA documentation.
 * It allows the sending of an email, under user's control and review, to the 
 * author of the application with stack trace and Logcat information for 
 * debugging purposes.
 */

package com.AndFlmsg;

//error reporting

import org.acra.*;
import org.acra.annotation.*;

import android.app.Application;


@ReportsCrashes(formKey = "",
        mailTo = "vk2eta@gmail.com",
        customReportContent = {ReportField.ANDROID_VERSION, ReportField.PHONE_MODEL, ReportField.CUSTOM_DATA, ReportField.STACK_TRACE, ReportField.LOGCAT},
        mode = ReportingInteractionMode.TOAST,
        forceCloseDialogAfterToast = true, // optional, default false
        resToastText = R.string.txt_crash_toast_text)


public class AndFlmsgDebug extends Application {


    @Override
    public void onCreate() {
        // Initialization of ACRA error reporting
        super.onCreate();
        ACRA.init(this);
    }

}
