package com.reliefoffice.pdic;

import android.app.ProgressDialog;
import android.content.Context;

/**
 * Created by nishikawat on 2016/05/25.
 */
public class NetDriveFileSelectionDialog extends FileSelectionDialog {
    INetDriveFileManager ndvFM;
    public NetDriveFileSelectionDialog( Context context,
                                       OnFileSelectListener listener, INetDriveFileManager ndvFM, boolean modeForSave ){
        super(context, listener, modeForSave);

        this.ndvFM = ndvFM;
    }

    // Progress Dialog //
    ProgressDialog progress;
    void createProgressDialog(){
        progress = new ProgressDialog(context);
        progress.setTitle(super.context.getString(R.string.title_prog_dirlsit));
        progress.setMessage(super.context.getString(R.string.msg_prog_dirlist));
        progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progress.show();
    }
    void deleteProgressDialog(){
        if (progress==null) return;
        progress.dismiss();
        progress = null;
    }

}
