package com.reliefoffice.pdic;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TableRow;
import android.widget.TextView;

/**
 * Created by nishikawat on 2015/07/28.
 */
public class PSBookmarkItemLayout extends LinearLayout {
    LinearLayout layout;
    TextView headerView;
    TextView textView;
    TextView commentView;

    public PSBookmarkItemLayout(Context context, AttributeSet attrs){
        super(context, attrs);
    }

    @Override
    protected void onFinishInflate(){
        super.onFinishInflate();
        layout = (LinearLayout)findViewById(R.id.psb_list_item_layout);
        headerView  = (TextView)findViewById(R.id.psb_list_item_header);
        textView  = (TextView)findViewById(R.id.psb_list_item_text);
        commentView  = (TextView)findViewById(R.id.psb_list_item_comment);
    }

    public void bindView(PSBookmarkItem item){
        if (Utility.isNotEmpty(item.filename)) {
            if (item.filename.startsWith("dbx:")){
                headerView.setText(item.filename.substring(4));
            } else {
                headerView.setText(item.filename);
            }
        } else {
            Utility.setTextStyle(textView, item.style, item.color, item.markedWord);
            commentView.setText(item.comment);
        }
    }
}
