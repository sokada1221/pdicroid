package com.reliefoffice.pdic;

import android.content.Context;
import android.content.SharedPreferences;

import java.io.File;

/**
 * Created by nishikawat on 2016/05/25.
 */
public class GoogleDriveUtils extends INetDriveUtils {
    private static final String PREF_NAME = "googledrive";

    private static final String PFS_GDV_INITIAL_DIR = "GoogleDriveInitialDir";

    static GoogleDriveUtils This;
    private Context context;    // should be application context
    SharedPreferences pref;

    private GoogleDriveUtils(Context context){
        super("gdv:");
        pref = context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
    }

    public static GoogleDriveUtils getInstance(Context context){
        if (This==null){
            This = new GoogleDriveUtils(context);
        }
        return This;
    }

    @Override
    public void setAppKeys(AppKeys keys) {

    }

    @Override
    public String getInitialDir() { return pref.getString(PFS_GDV_INITIAL_DIR, "/"); }

    @Override
    public void setInitialDir(String dir) {
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_GDV_INITIAL_DIR, dir);
        editor.commit();
    }

    @Override
    public boolean isLocalFileForRemote(String name) {
        return false;
    }

    @Override
    public String convertToLocalName(String remoteName) {
        return null;
    }

    @Override
    public String convertToRemoteName(String localName) {
        return null;
    }

    @Override
    public boolean makeDir(Context context, File file) {
        return false;
    }
}
