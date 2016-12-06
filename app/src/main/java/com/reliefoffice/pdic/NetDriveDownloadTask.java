package com.reliefoffice.pdic;

import android.os.AsyncTask;

import java.io.File;

/**
 * Created by nishikawat on 2016/05/25.
 */
public abstract class NetDriveDownloadTask extends AsyncTask {
    protected INetDriveFileManager ndvFM;
    protected String from;
    protected File to;
    protected boolean downloaded;
    protected String revision;
    public NetDriveDownloadTask(INetDriveFileManager ndvFM, String from, File to){
        super();
        this.ndvFM = ndvFM;
        this.from = from;
        this.to = to;
        downloaded = false;
    }
    // protected Object doInBackground(Object[] params);
}
