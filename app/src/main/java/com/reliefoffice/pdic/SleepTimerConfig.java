package com.reliefoffice.pdic;

import android.content.SharedPreferences;

import com.reliefoffice.pdic.text.pfs;

public class SleepTimerConfig {
    public boolean EnableSleepTimer;
    public int SLPAfter;        // [min]
    public int SLPTransTime;    // [min]
    public int SLPTargetVolume;
    public boolean EnableWakeupTimer;
    public int WKUFrom;         // [time]
    public int WKUTransTime;    // [min]
    public int WKUTargetVolume;

    public void load(SharedPreferences pref)
    {
        EnableSleepTimer = pref.getBoolean(pfs.ENABLE_SLEEP_TIMER, false);
        SLPAfter = pref.getInt(pfs.SLP_AFTER, 0);
        SLPTransTime = pref.getInt(pfs.SLP_TRANS_TIME, 0);
        SLPTargetVolume = pref.getInt(pfs.SLP_TARGET_VOLUME, 0);
        EnableWakeupTimer = pref.getBoolean(pfs.ENABLE_WAKEUP_TIMER, false);
        WKUFrom = pref.getInt(pfs.WKU_FROM, 0);
        WKUTransTime = pref.getInt(pfs.WKU_TRANS_TIME, 0);
        WKUTargetVolume = pref.getInt(pfs.WKU_TARGET_VOLUME, 0);
    }

    public void save(SharedPreferences pref)
    {
        SharedPreferences.Editor edit = pref.edit();
        edit.putBoolean(pfs.ENABLE_SLEEP_TIMER, EnableSleepTimer);
        edit.putInt(pfs.SLP_AFTER, SLPAfter);
        edit.putInt(pfs.SLP_TRANS_TIME, SLPTransTime);
        edit.putInt(pfs.SLP_TARGET_VOLUME, SLPTargetVolume);
        edit.putBoolean(pfs.ENABLE_WAKEUP_TIMER, EnableWakeupTimer);
        edit.putInt(pfs.WKU_FROM, WKUFrom);
        edit.putInt(pfs.WKU_TRANS_TIME, WKUTransTime);
        edit.putInt(pfs.WKU_TARGET_VOLUME, WKUTargetVolume);
        edit.commit();
    }

    // Wakeupの開始時刻をHM形式で取得
    public int getWKUFromByHM()
    {
        return WKUFrom / 100 * 60 + WKUFrom % 100;
    }
}
