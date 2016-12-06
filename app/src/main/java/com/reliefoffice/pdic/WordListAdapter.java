package com.reliefoffice.pdic;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;

/**
 * Created by tnishi on 2015/07/12.
 */
public class WordListAdapter extends ArrayAdapter<WordItem> {
    LayoutInflater layoutInflater;
    int resource;

    WordListAdapter(Context context, int resource) {
        super(context, resource);

        layoutInflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        this.resource = resource;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent){
        final WordItemLayout view;
        if (convertView==null){
            view = (WordItemLayout)layoutInflater.inflate(resource, null);
        } else {
            view = (WordItemLayout)convertView;
        }
        view.bindView(getItem(position));
        return view;
    }
}
