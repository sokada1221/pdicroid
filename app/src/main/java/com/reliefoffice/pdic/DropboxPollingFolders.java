package com.reliefoffice.pdic;

import android.os.AsyncTask;
import android.util.Log;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;

import java.util.Map;

/**
 * Created by nishikawat on 2015/08/19.
 */

public class DropboxPollingFolders extends AsyncTask<Void, Void, Void> {
    final int PollingInterval = 5000;

    private DropboxFileManager dbxFM;
    private DropboxUtils dbxUtils;
    private String dbCursor;
    private String path = "/";

    public DropboxPollingFolders(DropboxFileManager dbxFileMan){
        this.dbxFM = dbxFileMan;
        dbxUtils = DropboxUtils.getInstance(null);
        dbCursor = dbxUtils.getCursor();
    }

    @Override
    protected Void doInBackground(Void... voids) {
        if (Utility.isEmpty(dbCursor)){
            dbxFM.lockApi();
            try {
                dbCursor = dbxFM.getApi().latest_cursor(path);
            } catch (DropboxException e){
                e.printStackTrace();
            }
            dbxFM.unlockApi();
        }
        while (!isCancelled()) {
            if (checkRevision()){
                checkDownload();
            }
            checkUpload();
            if (dbxFM.needRemoteCheck()) {
                dbxFM.lockApi();
                try {
                    //Log.d("PDD", "get delta cursor="+dbCursor);
                    //Log.d("PDD", "/delta path="+path);
                    DropboxAPI.DeltaPage<DropboxAPI.Entry> dbPage = dbxFM.getApi().delta(dbCursor, path);
                    //Log.d("PDD", "delta returned");
                    if (dbPage != null) {
                        dbCursor = dbPage.cursor;
                        if (dbPage.reset) {
                            // cancel all operation for this path.
                            //Log.d("PDD", "reset operations");
                            //break;
                        } else {
                            for (DropboxAPI.DeltaEntry<DropboxAPI.Entry> entry : dbPage.entries) {
                                Log.d("PDD", "delta: path=" + entry.lcPath);
                                dbxFM.requestDownload(entry.lcPath, entry.metadata.rev);
                            }
                        }
                    }
                } catch (DropboxException e) {
                    e.printStackTrace();
                } catch (Exception e){
                    e.printStackTrace();
                }
                dbxFM.unlockApi();

                checkDownload();
            }

            try {
                Thread.sleep(PollingInterval);
            } catch (InterruptedException e){
                e.printStackTrace(); break;
            }
        }
        return null;
    }

    @Override
    protected void onPostExecute(Void res){
        dbxUtils.setCursor(dbCursor);
    }

    boolean checkRevision(){
        DbxFileInfo info = dbxFM.findRevisionCheck();
        if (info==null) return false;

        boolean needDownload = false;

        dbxFM.lockApi();
        try {
            DropboxAPI.Entry entry = dbxFM.getApi().metadata(info.remoteName, 0, null, false, null);
            if (entry != null) {
                if (info.remoteRevision == null || !info.remoteRevision.equals(entry.rev)){
                    // revision is updated, we need to download
		            Log.d("PDD", "revision differ!: remoteName="+info.remoteName+" rev="+info.remoteRevision+" srev="+entry.rev);
                    info.downloadRequest = true;
                    needDownload = true;
                }
            }
        } catch (DropboxException e) {
            e.printStackTrace();
        }
        dbxFM.unlockApi();

        return needDownload;
    }

    void checkDownload(){
        DbxFileInfo file = null;
        do {
            file = dbxFM.getDownloadRequestedFile(file);
            if (file == null) break;
            downloadFile(file);
        } while (true);
    }
    void downloadFile(DbxFileInfo file){
        DownloadTask downloadTask = new DownloadTask(dbxFM, file);
        file.downloading = true;
        downloadTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }
    class DownloadTask extends DropboxDownloadTask {
        DbxFileInfo dbxFile;
        public DownloadTask(DropboxFileManager dbxFM, DbxFileInfo file){
            super(dbxFM, file.remoteName, file.getLocalFile());
            dbxFile = file;
            Log.d("PDD", "Start download: file="+dbxFile.remoteName);
        }
        @Override
        protected void onPostExecute(Object result) {
            Log.d("PDD", "Download onPost file:"+dbxFile.getLocalName());
            if (downloaded){
                dbxFile.downloadRequest = false;
                dbxFile.needRevisionCheck = false;
                dbxFile.remoteRevision = super.revision;
                Log.d("PDD", "Polling Task downloaded - " + to + " rev: " + super.revision);
                INetDriveFileInfo info2 = dbxFM.findByLocalName( dbxFile.getLocalName() );
                Log.d("PDD", "info2.rev="+info2.remoteRevision);
                dbxFile.updateLocalFileTime();
                dbxFile.downloadNotify();
            }
            dbxFile.downloading = false;
        }
    }

    void checkUpload(){
        dbxFM.checkUpdate();
        DbxFileInfo file = null;
        do {
            //Log.d("PDD", "Checking upload");
            file = dbxFM.getUploadRequestedFile(file);
            if (file == null) break;
            uploadFile(file);
        } while (true);
    }
    void uploadFile(DbxFileInfo file){
        Log.d("PDD", "uploadFile: "+file.getLocalName());
        UploadTask uploadTask = new UploadTask(dbxFM, file);
        file.uploading = true;
        uploadTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }
    class UploadTask extends DropboxUploadTask {
        DbxFileInfo dbxFile;
        public UploadTask(DropboxFileManager dbxFM, DbxFileInfo file){
            super(dbxFM, file.getLocalFile(), file.remoteName);
            dbxFile = file;
        }
        @Override
        protected void onPostExecute(Object result) {
            if (uploaded){
                dbxFile.uploadRequest = false;
                dbxFile.remoteRevision = super.revision;
                dbxFile.updateLocalFileTime();
                dbxFile.uploadNotify();
                Log.d("PDD", "Polling Task uploaded - "+from);
            }
            dbxFile.uploading = false;
        }
    }
}
