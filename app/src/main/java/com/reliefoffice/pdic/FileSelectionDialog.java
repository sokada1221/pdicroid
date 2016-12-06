package com.reliefoffice.pdic;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by tnishi on 2015/06/24.
 */
public class FileSelectionDialog implements AdapterView.OnItemClickListener{

    protected Context context;
    private boolean modeForSave;
    private OnFileSelectListener	m_listener;
    private AlertDialog				m_dlg;
    private FileInfoArrayAdapter	m_fileinfoarrayadapter;
    protected  String[] m_exts;
    protected FileInfo m_fileDirectory;

    public FileSelectionDialog( Context context,
                                OnFileSelectListener listener, boolean modeForSave )
    {
        this.context = context;
        this.modeForSave = modeForSave;
        m_listener = listener;
    }

    // Cancel Listener //
    interface OnCancelListener {
        void onCancel();
    }
    OnCancelListener onCancelListener;
    public void setOnCancelListener(OnCancelListener listener){
        onCancelListener = listener;
    }
    // 呼び出し側にcancelをさせる
    protected void cancel(){
        if (onCancelListener!=null) onCancelListener.onCancel();
    }

    public void show(File file, final String[] exts){
        show(new FileInfo(file.getName(), file), exts);
    }

    public void show( FileInfo fileDirectory, final String[] exts )
    {
        m_fileDirectory = fileDirectory;
        m_exts = exts;
        Log.d("PDD", "fileDirectory="+fileDirectory.getPath());

        List<FileInfo> listFileInfo = getListFileInfo(fileDirectory);
        if (listFileInfo == null) {
            return; //TODO: error or pending
        }

        showPost(fileDirectory, listFileInfo);
    }

    protected void showErrorMessage()
    {}

    String getCurrentDir(){
        return m_fileDirectory.getPath();
    }

    protected boolean retryFromRoot(){
        if (m_fileDirectory.getPath().equals("/")) {
            // already rooted. Show the advisable message.
            showErrorMessage();
            return false;
        } else {
            Log.d("PDD", "retryFromRoot");
            m_fileDirectory.setRoot();
            show(m_fileDirectory, m_exts);
            return true;
        }
    }

    protected void showPost(FileInfo fileDirectory, List<FileInfo> listFileInfo){
        ListView listview = new ListView(context);
        listview.setScrollingCacheEnabled(false);
        listview.setOnItemClickListener( this );

        String strTitle = fileDirectory.getAbsolutePath();

        if( null != fileDirectory.getParent() ){
            listFileInfo.add( 0, new FileInfo( "..", true, fileDirectory.getParent() ) );
        }
        m_fileinfoarrayadapter = new FileInfoArrayAdapter(context, listFileInfo );
        listview.setAdapter( m_fileinfoarrayadapter );

        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(strTitle);
        if (modeForSave){
            builder.setPositiveButton( context.getString(R.string.label_select), new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    FileInfo fileinfo = new FileInfo( null, true, getCurrentDir() );    //Note: save時はparentのみ有効。fileinfo.getPath()以外は使用不可
                    m_listener.onFileSelect( fileinfo );
                }
            } );
        }
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (onCancelListener!=null) onCancelListener.onCancel();
            }
        });
        builder.setView(listview);
        builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                if (onCancelListener != null) onCancelListener.onCancel();
            }
        });
        m_dlg = builder.show();
    }

    protected List<FileInfo> getListFileInfo(FileInfo fileDirectory){
        File file = new File(fileDirectory.getAbsolutePath());
        File[] aFile = file.listFiles();
        List<FileInfo> listFileInfo = new ArrayList<FileInfo>();
        if( null != aFile ){
            for( File fileTemp : aFile ){
                if (m_exts!=null && m_exts.length!=0 && !fileTemp.isDirectory()){
                    String lcName = fileTemp.getName().toLowerCase();
                    boolean found = false;
                    for ( String ext : m_exts ){
                        if (lcName.endsWith(ext)){
                            found = true;
                            break;
                        }
                    }
                    if (!found) continue;
                }
                listFileInfo.add( new FileInfo( fileTemp.getName(), fileTemp ) );
            }
            Collections.sort(listFileInfo);
        }
        return listFileInfo;
    }

    public void onItemClick(	AdapterView<?> l,
                                View v,
                                int position,
                                long id )
    {
        if( null != m_dlg ){
            m_dlg.dismiss();
            m_dlg = null;
        }

        FileInfo fileinfo = m_fileinfoarrayadapter.getItem( position );

        if( true == fileinfo.isDirectory() ){
            if (fileinfo.getName().equals("..")) {
                File file = fileinfo.getFile();
                if (file == null) {
                    file = new File(fileinfo.getParent());
                }
                show(file, m_exts);
            } else {
                show(fileinfo, m_exts);
            }
        } else {
            m_listener.onFileSelect( fileinfo );
        }
    }

    public interface OnFileSelectListener
    {
        void onFileSelect( FileInfo file );
    }
}
