package com.reliefoffice.pdic;

import android.content.Context;

import java.io.File;

/**
 * Created by tnishi on 2016/04/09.
 */
public abstract class INetDriveFileManager {
    //Note: should call authComplete() in onResume()
    public abstract boolean startAuth(boolean easyMode);
    public abstract boolean authComplete(Context context);
    public abstract void disconnect();

    // add a dictionary file for download monitor
    public abstract void add(DicInfo dicInfo, INetDriveFileInfo.UpdateNotifier notifier);
    // add a file for download monitor
    public abstract void add(String localName, String remoteName, String remoteRevision, INetDriveFileInfo.UpdateNotifier notifier);
    // remove file from download monitor
    public abstract void remove(String localName);
    public abstract void removeAll();
    // remove all files that have DicInfo
    public abstract void removeDicInfo();

    public abstract INetDriveFileInfo findByLocalName(String filename);

    public abstract void executeDownload(String from, File to, final OnExecuteListener listener);
    interface OnExecuteListener {
        void onPostExecute(boolean downloaded, String from, File to, String revision);
    }
}
