package com.reliefoffice.pdic;

import android.util.Log;

import java.io.File;

/**
 * Created by nishikawat on 2015/08/20.
 */
public class DbxFileInfo extends INetDriveFileInfo {
    private String localName;
    public String remoteName;
    //public String remoteRevision; // moved to suepr
    public boolean needRevisionCheck;
    public boolean downloadRequest;
    public boolean downloading;
    public boolean uploadRequest;
    public boolean uploading;
    public long lastModified;
    //public DicInfo dicInfo;   // moved to super
    private File localFile;
    public UpdateNotifier updateNotifier;

    // local file operations //
    @Override
    public File getLocalFile(){ return localFile; }
    public void setLocalName(String filename){
        localName = filename;
        localFile  = new File(localName);
        lastModified = localFile.lastModified();
    }
    @Override
    public String getLocalName(){ return localName; }
    public boolean isLocalName(String filename){
        return filename.equalsIgnoreCase(localName);
    }
    public void updateLocalFileTime(){
        lastModified = localFile.lastModified();
        Log.d("PDD", "updateLocalFileTime");
    }
    public boolean isUpdated(){
        long modTime = localFile.lastModified();
        return modTime!=lastModified;
    }
    public Long getLastModified(){
        return localFile.lastModified();
    }
    public void downloadNotify(){
        if (updateNotifier !=null) updateNotifier.downloaded(this);
    }
    public void uploadNotify(){
        if (updateNotifier !=null) updateNotifier.uploaded(this);
    }

    // Dropboxでdownload/uploadされたときに呼ばれる
/*    public interface UpdateNotifier {
        void downloaded(DbxFileInfo dbxInfo);
        void uploaded(DbxFileInfo dbxInfo);
    }*/
}
