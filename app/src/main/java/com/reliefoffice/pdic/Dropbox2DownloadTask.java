package com.reliefoffice.pdic;

import android.util.Log;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;
import com.dropbox.core.DbxException;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by tnishi on 2016/09/08.
 */
public class Dropbox2DownloadTask extends NetDriveDownloadTask {
    public Dropbox2DownloadTask(Dropbox2FileManager dbxFM, String from, File to){
        super(dbxFM, from, to);
    }
    @Override
    protected Object doInBackground(Object[] params) {
        Log.d("PDD", "Dropbox2DownloadTask.doInBackground");
        Dropbox2FileManager dbxFM = (Dropbox2FileManager)ndvFM;
        dbxFM.lockApi();
        try {
            FileOutputStream outputStream = new FileOutputStream(to);
            dbxFM.getClient().files().download(from).download(outputStream);
            //DropboxAPI.DropboxFileInfo info = dbxFM.getApi().getFile(from, null, outputStream, null);
            //revision = info.getMetadata().rev;
            downloaded = true;
            Log.i("PDP", "Downloaded - file: " + from);
        } catch(DbxException e){
            Log.i("PDD", "Download failed(0) file:"+from);
            e.printStackTrace();
        } catch(IOException e){
            Log.i("PDD", "Download failed(1) file:"+from);
            e.printStackTrace();
        }
        dbxFM.unlockApi();
        return null;
    }
}
