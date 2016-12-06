package com.reliefoffice.pdic;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by tnishi on 2015/07/12.
 */
public class WordItemLayout extends LinearLayout {
    TextView textView;

    public WordItemLayout(Context context, AttributeSet attrs){
        super(context, attrs);
    }

    @Override
    protected void onFinishInflate(){
        super.onFinishInflate();
        textView  = (TextView)findViewById(R.id.word_list_item_text);
    }

    public void bindView(WordItem item){
        textView.setText(item.word + " " + item.trans);
    }
}
