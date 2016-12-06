package com.reliefoffice.pdic;

import android.content.Context;

/**
 * Created by tnishi on 2016/09/08.
 */
public class Dropbox2Utils extends DropboxUtils {
    private Dropbox2Utils(Context context) {
        super(context);
    }

    public static Dropbox2Utils getInstance(Context context){
        if (This==null){
            This = new Dropbox2Utils(context);
        } else {
            if (context != null)
                This.context = context;
        }
        return (Dropbox2Utils)This;
    }
}
