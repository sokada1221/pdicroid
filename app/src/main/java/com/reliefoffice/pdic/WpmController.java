package com.reliefoffice.pdic;

import android.os.Handler;
import android.widget.EditText;

public class WpmController {
    boolean running = false;
    boolean paused = false;
    int wordCount = 0;
    int wpmCounter = 0;
    Handler wpmHandler;
    Runnable wpmRun;
    EditText editText;
    public WpmController(EditText editText)
    {
        this.editText = editText;
    }
    public void start()
    {
        if (!running) {
            if (wordCount == 0){
                wordCount = Utility.getWordCount(editText.getText().toString());
            }
            if (wordCount == 0)
                return;
            running = true;
            paused = true;
        }
        restart();
    }
    public void pause()
    {
        pause(true);
    }
    public void pause(boolean update)
    {
        if (!running)
            return;
        if (paused)
            return;
        if (wpmHandler != null){
            wpmHandler.removeCallbacks(wpmRun);
            wpmHandler = null;
        }
        paused = true;
        if (update)
            update(true);
    }
    public void restart()
    {
        if (!running)
            return;
        if (!paused)
            return;

        // Timer for WPM
        wpmHandler = new Handler();
        wpmRun = new Runnable() {
            @Override
            public void run() {
                // in UI thread
                wpmCounter++;
                update(false);
                wpmHandler.postDelayed(this, 1000);
            }
        };
        wpmHandler.post(wpmRun);
        paused = false;
        update(true);
    }
    public void stop()
    {
        if (!running)
            return;
        stop(true);
    }
    public void stop(boolean update)
    {
        if (!running)
            return;
        pause(update);
        running = false;
        paused = false;
    }
    public void clear()
    {
        stop(false);
        wordCount = 0;
        wpmCounter = 0;
    }
    public void update(boolean force)
    {
        if (wordCount == 0 || wpmCounter == 0){
            return;
        }
        if (!force){
            // ５秒に一回更新
            if (wpmCounter % 5 != 1){
                return;
            }
        }
        int wpm = wordCount * 60 / wpmCounter;
        updateProcess(wpm, wordCount);
    }

    public boolean isRunning()
    {
        return running;
    }
    public boolean isPaused() { return paused; }

    protected void updateProcess(int wpm, int wordCount)
    {

    }
}
