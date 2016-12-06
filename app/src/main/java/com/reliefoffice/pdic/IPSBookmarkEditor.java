package com.reliefoffice.pdic;

import android.widget.EditText;
import android.widget.TextView;

/**
 * Created by tnishi on 2015/08/02.
 */
public interface IPSBookmarkEditor {
    int getSelectionStart();
    int getSelectionEnd();
    String getText();
    EditText getEditText();
    TextView getTextView();
}

