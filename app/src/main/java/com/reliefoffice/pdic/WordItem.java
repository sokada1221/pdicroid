package com.reliefoffice.pdic;

/**
 * Created by tnishi on 2015/07/13.
 */
public class WordItem {
    public String word;
    public String trans;
    WordItem(String word, String trans){
        this.word = word;
        this.trans = trans;
    }
    @Override
    public String toString(){
        return word + " " + trans;
    }
}
