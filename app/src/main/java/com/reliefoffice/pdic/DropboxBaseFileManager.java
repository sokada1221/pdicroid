package com.reliefoffice.pdic;

import android.content.Context;

/**
 * Created by nishikawat on 2016/09/29.
 */

public abstract class DropboxBaseFileManager extends INetDriveFileManager {
    protected Context context;
    public void setContext(Context context){
        this.context = context;
    }
    //public void lockApi(){ }
    //public void unlockApi(){ }
}
