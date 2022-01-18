package com.reliefoffice.pdic;

import android.content.Context;
import android.media.AudioManager;
import android.util.Log;

import java.util.Calendar;

public class SleepTimer {
    enum State {
        Idle,
        Waiting,
        Started,
        End
    }
    State state = State.Idle;
    Calendar setTime;
    Calendar startTime;
    Calendar endTime;
    int duration;   // endTime - startTime [sec]
    int targetVol;
    int startVol;
    int endVol;
    int curVol;

    AudioManager audioManager;

    public SleepTimer(AudioManager audioManager)
    {
        this.audioManager = audioManager;
    }

    // 直近の時刻hmに開始、wakeup timer用
    // hm: hours * 60 + minutes
    // interval: hours * 60 + minutes
    public boolean startByTime(int hm, int interval, int targetVol)
    {
        if (state != State.Idle)
            return false;

        setTime = Calendar.getInstance();
        startTime = (Calendar)setTime.clone();
        setTime(startTime, hm, setTime);

        startCommon(interval, targetVol);

        return true;
    }
    // 今からhm時間後に開始、sleep timer用
    // hm: hours * 60 + minutes
    // interval: hours * 60 + minutes
    public boolean startByAfter(int hm, int interval, int targetVol)
    {
        if (state != State.Idle)
            return false;

        setTime = Calendar.getInstance();
        startTime = (Calendar)setTime.clone();
        addTime(startTime, hm);

        startCommon(interval, targetVol);

        return true;
    }
    void startCommon(int interval, int targetVol)
    {
        endTime = (Calendar)startTime.clone();
        addTime(endTime, interval);
        this.targetVol = targetVol;

        duration = (int)(endTime.getTimeInMillis() - startTime.getTimeInMillis()) / 1000;
        Log.d("PDX", "duration="+Integer.toString(duration)+" targetVol="+Integer.toString(targetVol));

        setState(State.Waiting);
    }
    void setupStartEndVol()
    {
        curVol = getSystemVolume();

        startVol = curVol;
        endVol = targetVol;
        if (endVol < 0) endVol = 0;
        int maxVol = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        if (endVol > maxVol) endVol = maxVol;
        Log.d("PDX", "start="+startVol+" end="+endVol);
    }
    void restart(int curvol)
    {
        startVol = curvol;
        curVol = curvol;
        startTime = Calendar.getInstance(); // 現在時刻に
        duration = (int)(endTime.getTimeInMillis() - startTime.getTimeInMillis()) / 1000;
    }
    public void stop()
    {
        if (state == State.Waiting || state == State.Started){
            setState(State.Idle);
        }
    }
    public boolean isEnd()
    {
        return state == State.End;
    }
    public void handleTimer()
    {
        if (state == State.Idle || state == State.End)
            return;

        if (state == State.Waiting){
            Calendar now = Calendar.getInstance();
            if (startTime.before(now)){
                setupStartEndVol();
                setState(State.Started);
            }
        } else
        if (state == State.Started){
            Calendar now = Calendar.getInstance();
            if (endTime.before(now)){
                Log.d("PDX", "end time reached");
                setSystemVolume(endVol);
                setState(State.End);
            } else {
                int sysvol = getSystemVolume();
                if (curVol != sysvol){
                    // changed by user intentionally
                    if (endVol <= startVol){
                        // sleep timer
                        if (sysvol > curVol){
                            // 音量を上げられた->中止？
                            setState(State.End);
                            Log.d("PDX", "force to stop");
                        } else {
                            // 再計算
                            restart(sysvol);
                        }
                    } else {
                        // wakeup timer
                        // 再計算（音量を下げられても継続？）
                        restart(sysvol);
                    }
                }
                if (state == State.Started){
                    int elapse = diffDate(now, startTime);
                    // 現在の目標volume
                    int vol = startVol + (endVol - startVol) * elapse / duration;
                    if (curVol != vol){
                        setSystemVolume(vol);
                        curVol = vol;
                    }
                }
            }
        }
    }
    void setState(State stat)
    {
        state = stat;
    }

    // Utilities
    static void setTime(Calendar cal, int hm, Calendar now)
    {
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MINUTE, hm % 60);
        cal.set(Calendar.HOUR_OF_DAY, hm/60);
        if (cal.before(now)){
            nextDay(cal);
        }
    }
    static void addTime(Calendar cal, int hm)
    {
        cal.add(Calendar.MINUTE, hm % 60);
        cal.add(Calendar.HOUR_OF_DAY, hm/60);
    }
    static void nextDay(Calendar cal)
    {
        cal.add(Calendar.DAY_OF_MONTH, 1);
    }
    static void printDate(String tag, Calendar cal)
    {
        Utility.printDate(tag, cal);
    }
    static int diffDate(Calendar cal1, Calendar cal2)
    {
        return (int)(cal1.getTimeInMillis() - cal2.getTimeInMillis()) / 1000;
    }

    int __curvol = 80;
    boolean simu = false;
    int getSystemVolume()
    {
        if (simu) {
            return __curvol;
        } else {
            return audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        }
    }
    void setSystemVolume(int vol)
    {
        Log.d("PDX", "setVol" + Integer.toString(vol));
        if (simu) {
            __curvol = vol;
        } else {
            audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, vol, 0);
        }
    }

}
