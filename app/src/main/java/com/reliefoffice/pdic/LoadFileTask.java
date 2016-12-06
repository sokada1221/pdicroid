package com.reliefoffice.pdic;

import android.os.AsyncTask;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by tnishi on 2015/08/28.
 */
public class LoadFileTask extends AsyncTask {
    InputStream is;
    public String text;
    boolean ok = false;
    public LoadFileTask(InputStream is){
        super();
        this.is = is;
    }
    @Override
    protected Object doInBackground(Object[] param){
        try {
            ByteArrayOutputStream bs = new ByteArrayOutputStream();
            byte[] bytes = new byte[4096];
            int len;
            while ((len = is.read(bytes)) > 0) {
                bs.write(bytes, 0, len);
            }
            text = new String(bs.toByteArray(), "UTF8");
            ok = true;
        } catch (IOException e){
            e.printStackTrace();
        }
        return null;
    }
}
