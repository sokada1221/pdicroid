package com.reliefoffice.pdic;

import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;

/**
 * Created by tnishi on 2015/06/24.
 *  bring from http://www.ipentec.com/document/document.aspx?page=android-http-file-download-async
 */
public class ProgressHandler extends Handler {
    public ProgressDialog progressDialog;
    public AsyncFileDownload asyncfiledownload;

    @Override
    public void handleMessage(Message msg){
        super.handleMessage(msg);
        if(asyncfiledownload.isCancelled()){
            progressDialog.dismiss();
        }
        else if(asyncfiledownload.getStatus() == AsyncTask.Status.FINISHED){
            progressDialog.dismiss();
        }else{
            progressDialog.setProgress(asyncfiledownload.getLoadedBytePercent());
            this.sendEmptyMessageDelayed(0, 100);
        }
    }
}
