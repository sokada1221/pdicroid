package com.reliefoffice.pdic;

import java.util.Arrays;

public class LLMManager {
    int[] timestamps;
    LLMManager()
    {

    }
    public void setup(int[] timestamps)
    {
        this.timestamps = timestamps;
    }
    // LLM fileのdurationを読み込みtimestamp - 行番号 mapを作成
    String setup(String text)
    {
        String[] lines = text.split("\n");
        timestamps = new int[lines.length];
        int index = 0;
        int timestamp = 0;
        int linenum = 0;
        String newtext = "";
        for (String line : lines){
            int pos = line.indexOf(",");
            int dur = 0;
            if (pos>=1){
                dur = Integer.parseInt(line.substring(0, pos));
                newtext += line.substring(pos+1);
            } else {
                newtext += line;
            }
            newtext += "\n";
            timestamps[index] = timestamp;  // msec単位のtimestamp
            timestamp += dur;
            index++;
            linenum++;
        }
        return newtext;
    }
    public void clear()
    {
        timestamps = null;
    }
    public int timestampToLine(int timestamp)
    {
        if (timestamps == null) return -1;
        int index = Arrays.binarySearch(timestamps, timestamp);
        if (index < 0){
            index = ~index;
            if (index >= 1) index--;
        }
        return index;
    }
}
