package com.reliefoffice.pdic;

import java.util.List;

/**
 * Created by tnishi on 2016/10/09.
 */

public abstract class FileSelectionInterface {
    FileListListener listener;
    public FileSelectionInterface(FileListListener listener){
        this.listener = listener;
    }
    public abstract void startFileList();
    void onFileListReady(List<FileInfo> listFile){
        if (listener!=null){
            listener.onFileListReady(listFile);
        }
    }
    public interface FileListListener {
        void onFileListReady(List<FileInfo> listFile);
    }
}
