package com.reliefoffice.pdic;

import android.content.SharedPreferences;
import android.util.Log;

import com.reliefoffice.pdic.text.config;
import com.reliefoffice.pdic.text.pfs;

import java.util.ArrayList;

public class MarkPositioinHistory {
    ArrayList<MarkPositionItem> history = new ArrayList<MarkPositionItem>();
    SharedPreferences pref;
    boolean refresh = false;
    SharedPreferences.Editor edit = null;
    public MarkPositioinHistory(SharedPreferences pref)
    {
        this.pref = pref;
        loadAll();
    }
    public void save(String filename, int markA, int markB) {
        int index = find(filename);
        if (index == -1) {
            // new item
            index = add(filename, markA, markB);
        } else {
            if (isLastIndex(index)) {
                // update last item
                if (!setItem(index, filename, markA, markB)) {
                    // 同じだった場合は保存しない
                    return;
                }
            } else {
                // need exchange
                remove(index);
                index = add(filename, markA, markB);
            }
        }
        saveItem(index);
        commit();
    }
    public MarkPositionItem load(String filename)
    {
        int index = find(filename);
        if (index == -1) return null;
        return history.get(index);
    }
    public void remove(String filename)
    {
        int index = find(filename);
        if (index == -1) return;
        history.remove(index);
        eraseItem(history.size());
        saveAll();
        commit();
    }

    // private members
    final boolean isLastIndex(int index)
    {
        return index == history.size()-1;
    }
    int find(String filename)
    {
        for (int i=0;i<history.size();i++){
            if (history.get(i).filename.equals(filename)){
                return i;
            }
        }
        return -1;
    }
    int add(String filename, int markA, int markB)
    {
        if (history.size() >= config.MaxAudioMarkPositionHistory){
            history.remove(0);
            refresh = true;
        }
        MarkPositionItem item = new MarkPositionItem();
        item.filename = filename;
        item.markA = markA;
        item.markB = markB;
        history.add(item);
        return history.size()-1;
    }
    boolean setItem(int index, String filename, int markA, int markB)
    {
        MarkPositionItem item = history.get(index);
        if (item.filename.equals((filename)) && item.markA == markA && item.markB == markB)
            return false;   // completely same
        item.filename = filename;
        item.markA = markA;
        item.markB = markB;
        return true;    // updated
    }
    void remove(int index)
    {
        history.remove(index);
        refresh = true;
    }

    //Note: you need commit() finally.
    void saveItem(int index)
    {
        if (refresh){
            saveAll();
            return;
        }

        MarkPositionItem item = history.get(index);
        if (edit == null)
            edit = pref.edit();
        edit.putString(itemName(pfs.LAST_AUDIOFILE, index), item.filename);
        edit.putInt(itemName(pfs.LAST_AUDIO_MARK_A, index), item.markA);
        edit.putInt(itemName(pfs.LAST_AUDIO_MARK_B, index), item.markB);
    }
    //Note: you need commit() finally.
    void saveAll()
    {
        refresh = false;
        for (int i=0;i<history.size();i++){
            saveItem(i);
        }
    }
    void commit()
    {
        if (edit != null){
            edit.commit();
            edit = null;
        }
    }
    void loadAll()
    {
        for (int i=0;i<config.MaxAudioMarkPositionHistory;i++){
            if (!loadItem(i))
                break;
        }
    }
    boolean loadItem(int index)
    {
        MarkPositionItem item = new MarkPositionItem();
        item.filename = pref.getString(itemName(pfs.LAST_AUDIOFILE, index), "");
        if (Utility.isNotEmpty(item.filename)) {
            item.markA = pref.getInt(itemName(pfs.LAST_AUDIO_MARK_A, index), -1);
            item.markB = pref.getInt(itemName(pfs.LAST_AUDIO_MARK_B, index), -1);
            history.add(item);
            return true;
        }
        return false;
    }
    void eraseItem(int index)
    {
        if (edit == null)
            edit = pref.edit();
        edit.remove(itemName(pfs.LAST_AUDIOFILE, index));
        edit.remove(itemName(pfs.LAST_AUDIO_MARK_A, index));
        edit.remove(itemName(pfs.LAST_AUDIO_MARK_B, index));
    }

    final static String itemName(String prefix, int number)
    {
        return prefix + number;
    }
}
