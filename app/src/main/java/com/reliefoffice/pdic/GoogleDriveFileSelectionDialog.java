package com.reliefoffice.pdic;

import android.content.Context;
import android.os.AsyncTask;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by nishikawat on 2016/05/25.
 */
public class GoogleDriveFileSelectionDialog extends NetDriveFileSelectionDialog {
    public GoogleDriveFileSelectionDialog( Context context,
                                       OnFileSelectListener listener, GoogleDriveFileManager gdvFM, boolean modeForSave ){
        super(context, listener, gdvFM, modeForSave);
    }

    MetadataTask metadataTask;

    @Override
    protected List<FileInfo> getListFileInfo(FileInfo fileDirectory) {
        String path = fileDirectory.getAbsolutePath();
        metadataTask = new MetadataTask((GoogleDriveFileManager)ndvFM, path);
        metadataTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        createProgressDialog();
        return null;    // pending
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
        for( FileInfo fileTemp : metadataTask.diEntry ){
            if (m_exts!=null && m_exts.length!=0 && !fileTemp.isDirectory()){
                String lcName = fileTemp.getPath().toLowerCase();
                boolean found = false;
                for ( String ext : m_exts ){
                    if (lcName.endsWith(ext)){
                        found = true;
                        break;
                    }
                }
                if (!found) continue;
            }
            listFileInfo.add( new FileInfo( fileTemp.getName(), fileTemp.isDirectory(), fileTemp.getParent() ) );
        }
        Collections.sort(listFileInfo);

        showPost(m_fileDirectory, listFileInfo);
    }

    class MetadataTask extends AsyncTask {
        GoogleDriveFileManager gdvFM;
        String path;
        public List<FileInfo> diEntry;
        boolean exception;
        public MetadataTask(GoogleDriveFileManager gdvFM, String path){
            super();
            this.gdvFM = gdvFM;
            this.path = path;
        }
        @Override
        protected Object doInBackground(Object[] params){
            //dbxFM.lockApi();
            try {
                final int filelimit = 0;  // Default is 10,000 if you pass in 0.
                String revision = null;
                diEntry = gdvFM.getDirListWait(path, filelimit);
            } catch (Exception e){
                e.printStackTrace();
            }
            //dbxFM.unlockApi();
            return null;
        }
        @Override
        protected void onPostExecute(Object result) {
            onMetadataTaskEnd();
        }
    }
}
