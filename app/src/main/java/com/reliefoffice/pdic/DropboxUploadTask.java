package com.reliefoffice.pdic;

import android.os.AsyncTask;
import android.util.Log;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by nishikawat on 2015/08/20.
 */
public class DropboxUploadTask extends AsyncTask {
    protected DropboxFileManager dbxFM;
    protected File from;
    protected String to;
    protected String revision;
    protected boolean uploaded;
    public DropboxUploadTask(DropboxFileManager dbxFM, File from, String to){
        super();
        this.dbxFM = dbxFM;
        this.from = from;
        this.to = to;
        uploaded = false;
    }
    @Override
    protected Object doInBackground(Object[] params) {
        dbxFM.lockApi();
        try {
            FileInputStream inputStream = new FileInputStream(from);
            final DropboxAPI.Entry entry = dbxFM.getApi().putFileOverwrite(to, inputStream, from.length(), null);
            revision = entry.rev;
            uploaded = true;
            Log.i("PDP", "The file's rev is: " + entry.rev);
        } catch(IOException e){
            e.printStackTrace();
        } catch(DropboxException e){
            e.printStackTrace();
        }
        dbxFM.unlockApi();
        return null;
    }
}
