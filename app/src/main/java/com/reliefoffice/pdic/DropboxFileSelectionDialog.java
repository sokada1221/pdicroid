package com.reliefoffice.pdic;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.exception.DropboxException;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import static java.lang.Thread.sleep;

/**
 * Created by tnishi on 2015/07/01.
 */
public class DropboxFileSelectionDialog extends NetDriveFileSelectionDialog {
    protected DropboxFileSelectionDialog( Context context,
                                OnFileSelectListener listener, INetDriveFileManager dbxFM, boolean modeForSave ){
        super(context, listener, dbxFM, modeForSave);
    }

    public static DropboxFileSelectionDialog createInstance(Context context,
                                                     OnFileSelectListener listener, INetDriveFileManager dbxFM, boolean modeForSave){
        if (DropboxUtils.useV2)
            return new Dropbox2FileSelectionDialog(context, listener, dbxFM, modeForSave);
        else
            return new Dropbox1FileSelectionDialog(context, listener, dbxFM, modeForSave);
    }

}
