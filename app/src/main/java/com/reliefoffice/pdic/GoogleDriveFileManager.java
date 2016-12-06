package com.reliefoffice.pdic;

import android.app.Activity;
import android.content.Context;
import android.content.IntentSender;
import android.support.annotation.NonNull;
import android.util.Log;

import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.drive.Drive;
import com.google.android.gms.drive.DriveApi;
import com.google.android.gms.drive.DriveFolder;
import com.google.android.gms.drive.Metadata;
import com.google.android.gms.drive.MetadataBuffer;
import com.google.android.gms.drive.MetadataChangeSet;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;

/**
 * Created by nishikawat on 2016/05/25.
 */

//Note:
// activity must have GoogleApiClient.ConnectionCallbacks and GoogleApiClient.OnConnectionFailedListener methods.
public class GoogleDriveFileManager extends INetDriveFileManager {
    //private static GoogleDriveFileManager This;
    private Activity activity;

    private GoogleApiClient gdApi;

    private GoogleDriveFileManager(Activity activity){
        this.activity = activity;
        gdApi = new GoogleApiClient.Builder(activity)
                .addApi(Drive.API)
                .addScope(Drive.SCOPE_FILE)
                .addConnectionCallbacks((GoogleApiClient.ConnectionCallbacks)activity)
                .addOnConnectionFailedListener((GoogleApiClient.OnConnectionFailedListener)activity)
                .build();
    }
    public static GoogleDriveFileManager getInstance(Activity activity){
        return new GoogleDriveFileManager(activity);
    }

    @Override
    public boolean startAuth(boolean easyMode) {
        gdApi.connect();
        return false;
    }

    @Override
    public boolean authComplete(Context context) {
        return false;
    }

    @Override
    public void disconnect() {
        gdApi.disconnect();
    }

    @Override
    public void add(DicInfo dicInfo, INetDriveFileInfo.UpdateNotifier notifier) {

    }

    @Override
    public void add(String localName, String remoteName, String remoteRevision, INetDriveFileInfo.UpdateNotifier notifier) {

    }

    @Override
    public void remove(String localName) {

    }

    @Override
    public void removeAll() {

    }

    @Override
    public void removeDicInfo() {

    }

    @Override
    public INetDriveFileInfo findByLocalName(String filename) {
        return null;
    }

    @Override
    public void executeDownload(String from, File to, OnExecuteListener listener) {

    }

    public List<FileInfo> getDirListWait(String path, int limit) throws InterruptedException {
        DriveFolder folder = findFolder(path);
        if (folder==null) return null;  // no such path

        final boolean await = true;
        if (await){
            getDirListWait(folder);
        } else {
            getDirList(folder);
            metaSem.acquire();
        }

        if (metaBuffer==null) return null;   // error or pending

        List<FileInfo> list = new ArrayList<>();
        for (int i = 0; i<metaBuffer.getCount(); i++) {
            Metadata meta = metaBuffer.get(i);
            if (meta==null) break;
            String title = meta.getTitle() + " " + meta.getCreatedDate().toString();
            list.add(new FileInfo(title, meta.isFolder(), path));
        }
        return list;
    }

    DriveFolder findFolder(String path){
        if (!path.startsWith("/")) return null; // should start with "/"

        DriveFolder rootFolder = Drive.DriveApi.getRootFolder(gdApi);
        path = path.substring(1);   // remove "/"
        //createFolder(rootFolder);
        DriveFolder curFolder = rootFolder;
        while (path.length()>0) {
            int index = path.indexOf('/');
            String subdir;
            if (index>=0){
                subdir = path.substring(0, index);
                path = path.substring(index+1);
            } else {
                subdir = path;
                path = "";
            }

            getDirList(curFolder);
            DriveFolder subFolder = null;
            if (metaBuffer != null) {
                for (int i = 0; ; i++) {
                    Metadata meta = metaBuffer.get(i);
                    if (meta.getOriginalFilename() == subdir){
                        // found subdir folder
                        subFolder = meta.getDriveId().asDriveFolder();
                        break;
                    }
                }
            }
            if (subFolder==null)
                break;
            curFolder = subFolder;
        }
        return curFolder;
    }

    void getDirListWait(DriveFolder folder){
        metaBuffer = null;
        metaBuffer = folder.listChildren(gdApi).await().getMetadataBuffer();
    }

    void getDirList(DriveFolder folder){
        metaBuffer = null;
        metaSem = new Semaphore(0);
        folder.listChildren(gdApi).setResultCallback(childrenRetrieveCallback);
    }

    void createFolder(DriveFolder folder){
        MetadataChangeSet changeSet = new MetadataChangeSet.Builder().setTitle("New folder").build();
        folder.createFolder(gdApi, changeSet).await();
    }

    MetadataBuffer metaBuffer;
    Semaphore metaSem;

    private ResultCallback<DriveApi.MetadataBufferResult> childrenRetrieveCallback = new ResultCallback<DriveApi.MetadataBufferResult>() {
        @Override
        public void onResult(@NonNull DriveApi.MetadataBufferResult result) {
            if (!result.getStatus().isSuccess()) {
                showMessage("Problem while retrieving files");
                return;
            }
            //mResultsAdapter.clear();
            //mResultsAdapter.append(result.getMetadataBuffer());
            for (int i=0;;i++) {
                Metadata meta = result.getMetadataBuffer().get(i);
                if (meta==null) break;
                Log.d("PDD", "meta: "+meta.getOriginalFilename());
            }
            metaBuffer = result.getMetadataBuffer();
            metaSem.release();

            showMessage("Successfully listed files.");
        }
    };

    private void parserMetadataBuffer(MetadataBuffer metaBuffer) {
        for (int i=0;;i++) {
            Metadata meta = metaBuffer.get(i);
            if (meta==null) break;
            Log.d("PDD", "meta: "+meta.getOriginalFilename());
        }
    }

    void showMessage(final String msg){

    }

    private static final String MIME_TYPE_TEXT = "text/plain";

    // GoogleDriveFileManager unique //
    public IntentSender createFileSelectionIntent(){
        return Drive.DriveApi
                .newOpenFileActivityBuilder()
                .setMimeType(new String[] { MIME_TYPE_TEXT })
                .build(gdApi);
    }
    public GoogleApiClient getApi(){
        return gdApi;
    }

}
