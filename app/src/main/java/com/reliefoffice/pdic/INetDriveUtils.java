package com.reliefoffice.pdic;

import android.content.Context;

import java.io.File;

/**
 * Created by nishikawat on 2016/05/25.
 */
public abstract class INetDriveUtils {
    // Dropbox
    // pfx = dbx
    // netdrive = dropbox
    // Google Drive
    // pfx = gdv
    // netdrive = gdrive

    public static String prefix;

    INetDriveUtils(String prefix){
        this.prefix = prefix;
    }

    public static class AppKeys {
        public String key;
        public String secret;

        public AppKeys(String key, String secret){
            this.key = key;
            this.secret = secret;
        }
    }

    public boolean appKeysConfirmed = false;

    public abstract void setAppKeys(AppKeys keys);

    public abstract String getInitialDir();
    public abstract void setInitialDir(String dir);

    // has /.../pfx/ ?
    public abstract boolean isLocalFileForRemote(String name);
    final public boolean hasPrefix(String name){
        return name.startsWith(prefix);
    }
    // [pfx:]/netdrive/... => /sdcard/.../pfx/netdrive/...
    public abstract String convertToLocalName(String remoteName);
    // /sdcard/.../pfx/netdrive/... => /netdrive/...
    public abstract String convertToRemoteName(String localName);

    public abstract boolean makeDir(Context context, File file);
}
