package com.reliefoffice.pdic;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;

/**
 * Created by nishikawat on 2015/07/28.
 */
public class PSBookmarkListAdapter extends ArrayAdapter<PSBookmarkItem>  {
    LayoutInflater layoutInflater;
    int resource;

    PSBookmarkListAdapter(Context context, int resource) {
        super(context, resource);

        layoutInflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        this.resource = resource;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent){
        final PSBookmarkItemLayout view;
        if (convertView==null){
            view = (PSBookmarkItemLayout)layoutInflater.inflate(resource, null);
        } else {
            view = (PSBookmarkItemLayout)convertView;
        }
        view.bindView(getItem(position));
        return view;
    }
}
