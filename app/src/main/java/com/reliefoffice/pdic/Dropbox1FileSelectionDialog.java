package com.reliefoffice.pdic;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;
import com.reliefoffice.pdic.text.config;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by nishikawat on 2016/09/29.
 */

public class Dropbox1FileSelectionDialog extends DropboxFileSelectionDialog {
    public Dropbox1FileSelectionDialog( Context context,
                                       OnFileSelectListener listener, INetDriveFileManager dbxFM, boolean modeForSave ){
        super(context, listener, dbxFM, modeForSave);
    }

    MetadataTask metadataTask;

    @Override
    protected List<FileInfo> getListFileInfo(FileInfo fileDirectory) {
        String path = fileDirectory.getAbsolutePath();
        metadataTask = new MetadataTask((DropboxFileManager)ndvFM, path);
        metadataTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        createProgressDialog();
        return null;    // pending
    }

    @Override
    protected void showErrorMessage()
    {
        Toast.makeText(context, metadataTask.exceptionMessage, Toast.LENGTH_LONG).show();
    }

    void onMetadataTaskEnd(){
        deleteProgressDialog();
        if( null == metadataTask.diEntry ) {
            if (metadataTask.exception){
                // illegal directory?
                retryFromRoot();
            }
            return;
        }

        List<FileInfo> listFileInfo = new ArrayList<FileInfo>();
        List<DropboxAPI.Entry>  fileList = metadataTask.diEntry.contents;
        for( DropboxAPI.Entry fileTemp : fileList ){
            if (m_exts!=null && m_exts.length!=0 && !fileTemp.isDir){
                String lcName = fileTemp.path.toLowerCase();
                boolean found = false;
                for ( String ext : m_exts ){
                    if (lcName.endsWith(ext)){
                        found = true;
                        break;
                    }
                }
                if (!found) continue;
            }
            listFileInfo.add( new FileInfo( fileTemp.fileName(), fileTemp.isDir, fileTemp.parentPath() ) );
        }
        Collections.sort(listFileInfo);

        showPost(m_fileDirectory, listFileInfo);
    }

    class MetadataTask extends AsyncTask {
        DropboxFileManager dbxFM;
        String path;
        public DropboxAPI.Entry diEntry;
        boolean exception;
        String exceptionMessage;
        public MetadataTask(DropboxFileManager dbxFM, String path){
            super();
            this.dbxFM = dbxFM;
            this.path = path;
        }
        @Override
        protected Object doInBackground(Object[] params){
            dbxFM.lockApi();
            try {
                final int filelimit = 0;  // Default is 10,000 if you pass in 0.
                String revision = null;
                diEntry = dbxFM.getApi().metadata(path, filelimit, null, true, revision);
                //List<DropboxAPI.Entry>  fileList = diEntry.contents;
                //for (final DropboxAPI.Entry entry : fileList){ Log.d("PDD", "path=" + entry.path + " rev=" + entry.rev);}
            } catch (DropboxException e){
                //Note: ネットに接続していないとここで例外(EAI_NODATA - No address associated with hostname)
                exception = true;
                e.printStackTrace();
                exceptionMessage = e.getMessage();
                Log.e("PDD", "DropboxException: " + e.getMessage());
            } catch (Exception e){
                e.printStackTrace();
            }
            dbxFM.unlockApi();
            return null;
        }
        @Override
        protected void onPostExecute(Object result) {
            onMetadataTaskEnd();
        }
    }
}
