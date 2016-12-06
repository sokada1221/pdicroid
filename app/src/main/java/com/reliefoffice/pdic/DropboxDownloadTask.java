package com.reliefoffice.pdic;

import android.os.AsyncTask;
import android.util.Log;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by nishikawat on 2015/08/20.
 */
public class DropboxDownloadTask extends NetDriveDownloadTask {
    public DropboxDownloadTask(DropboxFileManager dbxFM, String from, File to){
        super(dbxFM, from, to);
    }
    @Override
    protected Object doInBackground(Object[] params) {
		Log.d("PDD", "DropboxDownloadTask.doInBackground");
        DropboxFileManager dbxFM = (DropboxFileManager)ndvFM;
        dbxFM.lockApi();
        try {
            FileOutputStream outputStream = new FileOutputStream(to);
            DropboxAPI.DropboxFileInfo info = dbxFM.getApi().getFile(from, null, outputStream, null);
            revision = info.getMetadata().rev;
            downloaded = true;
            Log.i("PDP", "Downloaded - file: "+from+" rev: " + info.getMetadata().rev);
        } catch(IOException e){
            Log.i("PDD", "Download failed(1) file:"+from);
            e.printStackTrace();
        } catch(DropboxException e){
            Log.i("PDD", "Download failed(2) file:"+from);
            e.printStackTrace();
        }
        dbxFM.unlockApi();
        return null;
    }
}
