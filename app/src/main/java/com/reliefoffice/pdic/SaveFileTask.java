package com.reliefoffice.pdic;

import android.os.AsyncTask;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by tnishi on 2015/11/10.
 */
public class SaveFileTask extends AsyncTask {
    OutputStream os;
    String text; // text to save
    boolean ok = false;
    SaveFileTaskDone done;
    public SaveFileTask(OutputStream os, String text, SaveFileTaskDone done){
        super();
        this.os = os;
        this.text = text;
        this.done = done;
    }
    @Override
    protected Object doInBackground(Object[] param){
        try {
            os.write(text.getBytes());
            ok = true;
        } catch (IOException e){
            e.printStackTrace();
        }
        return null;
    }

    @Override
    protected void onPostExecute(Object result) {
        done.onPostSaveFile(this);
    }

    public interface SaveFileTaskDone {
        void onPostSaveFile(SaveFileTask task);
    }
}
