package com.reliefoffice.pdic;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by nishikawat on 2015/08/20.
 */
public class DbxPollingFiles extends INetDrivePollingFiles {
    List<DbxFileInfo> files = new ArrayList<DbxFileInfo>();

    final public boolean exist() {
        return files.size() > 0;
    }

    final public boolean empty() {
        return files.size() == 0;
    }

    final public int size() {
        return files.size();
    }

    final public synchronized DbxFileInfo findLocal(String filename) {
        for (DbxFileInfo info : files) {
            if (info.isLocalName(filename)) {
                return info;
            }
        }
        return null;
    }

    final public synchronized DbxFileInfo findRemote(String filename) {
        for (DbxFileInfo info : files) {
            if (info.remoteName.equalsIgnoreCase(filename)) {
                return info;
            }
        }
        return null;
    }

    final public synchronized boolean needRemoteCheck() {
        return exist()
                &&!isDownloading()
                &&!isAllDownloadRequseted();
    }
    final public synchronized boolean isAllDownloadRequseted(){
        for (DbxFileInfo info : files){
            if (!info.downloadRequest){
                return false;
            }
        }
        return true;
    }
    final public synchronized boolean isDownloading(){
        for (DbxFileInfo info : files){
            if (info.downloading){
                return true;
            }
        }
        return false;
    }
    final public synchronized DbxFileInfo findNeedRevisionCheck(){
        for (DbxFileInfo info : files){
            if (info.needRevisionCheck && !info.downloadRequest){
                return info;
            }
        }
        return null;    // all revision updated
    }
    @Override
    final public void add(String localName, String remoteName, String remoteRevision, DicInfo refInfo, INetDriveFileInfo.UpdateNotifier update){
        addSync(localName, remoteName, remoteRevision, refInfo, update);
    }
    final public synchronized void addSync(String localName, String remoteName, String remoteRevision, DicInfo refInfo, INetDriveFileInfo.UpdateNotifier update){
        DbxFileInfo info = new DbxFileInfo();
        info.setLocalName(localName);
        info.remoteName = remoteName;
        info.remoteRevision = remoteRevision;
        info.dicInfo = refInfo;
        info.needRevisionCheck = true;
        info.updateNotifier = update;
        files.add(info);
    }
    final public synchronized void removeByLocalName(String localName){
        DbxFileInfo info = findLocal(localName);
        if (info!=null){
            files.remove(info);
        }
    }
    final public synchronized void remove(DbxFileInfo info){
        files.remove(info);
    }
    final public synchronized void removeAll(){
        files.clear();
    }
    // remove file that has DicInfo
    final public synchronized void removeDicInfo(){
        _removeDicInfo();
    }
    final private void _removeDicInfo(){
        for (DbxFileInfo info : files){
            if (info.dicInfo !=null) {
                files.remove(info);
                _removeDicInfo();
                return;
            }
        }
    }

    final public synchronized DbxFileInfo getDownloadRequestedFile(DbxFileInfo cur){
        for (DbxFileInfo info : files){
            if (cur!=null){
                if (info==cur) cur = null;
                continue;
            }
            if (info.downloadRequest && !info.downloading) return info;
        }
        return null;
    }

    final public synchronized DbxFileInfo getUploadRequestedFile(DbxFileInfo cur){
        for (DbxFileInfo info : files){
            if (cur!=null){
                if (info==cur) cur = null;
                continue;
            }
            if (info.uploadRequest && !info.uploading) return info;
        }
        return null;
    }

    final public synchronized DbxFileInfo findUpdatedFile(){
        for (DbxFileInfo info : files){
            if (!info.downloadRequest && !info.uploadRequest) {
                if (info.isUpdated()) {
                    info.isUpdated();
                    return info;
                }
            }
        }
        return null;
    }
}
