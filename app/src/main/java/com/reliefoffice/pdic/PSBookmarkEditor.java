package com.reliefoffice.pdic;

import android.widget.EditText;
import android.widget.TextView;

public class PSBookmarkEditor implements IPSBookmarkEditor {
    EditText editText;
    TextView textView;

    public PSBookmarkEditor(EditText editText){
        this.editText = editText;
        this.textView = editText;
    }
    public PSBookmarkEditor(TextView textView){
        this.textView = textView;
    }

    @Override
    public int getSelectionStart() {
        return textView.getSelectionStart();
    }

    @Override
    public int getSelectionEnd() {
        return textView.getSelectionEnd();
    }

    @Override
    public String getText() {
        return editText!=null ?
                editText.getText().toString().substring( editText.getSelectionStart(), editText.getSelectionEnd()) :
                textView.getText().toString();
    }

    @Override
    public EditText getEditText() {
        return editText;
    }

    @Override
    public TextView getTextView() {
        return textView;
    }
}
