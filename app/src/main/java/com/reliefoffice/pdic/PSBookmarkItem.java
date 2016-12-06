package com.reliefoffice.pdic;

/**
 * Created by nishikawat on 2015/07/28.
 */
public class PSBookmarkItem {
    public String filename;
    public int position;
    public int length;
    public int style;
    public int color;
    public String markedWord;
    public String comment;
    //public String remotename;
    PSBookmarkItem(PSBookmarkItem item){
        filename = item.filename;
        position = item.position;
        length = item.length;
        style = item.style;
        color = item.color;
        markedWord = item.markedWord;
        comment = item.comment;
        //remotename = item.remotename;
    }
    PSBookmarkItem(String filename, int position, int length, int style, int color, String makedWord, String comment/*, String remotename*/){
        this.filename = filename;
        this.position = position;
        this.length = length;
        this.style = style;
        this.color = color;
        this.markedWord = makedWord;
        this.comment = comment;
        //this.remotename = remotename;
    }
    PSBookmarkItem(String filename){
        this.filename = filename;
    }
    PSBookmarkItem(){}
}
