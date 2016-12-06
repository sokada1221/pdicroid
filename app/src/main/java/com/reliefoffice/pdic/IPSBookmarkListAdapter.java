package com.reliefoffice.pdic;

/**
 * Created by tnishi on 2015/07/26.
 */
public interface IPSBookmarkListAdapter {
    int addItem(int position, int length, int style, int color, String markedWord, String comment);
    int addFile(String filename);
}
