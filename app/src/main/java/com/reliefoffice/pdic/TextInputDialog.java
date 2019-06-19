package com.reliefoffice.pdic;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.widget.EditText;

/**
 * Created by tnishi on 2018/03/18.
 */

public class TextInputDialog extends DialogFragment {
    TextInputCallback callback;
    EditText editText;
    String titleText;
    String initialText;
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        editText = new EditText(getActivity());
        if (initialText!=null){
            editText.setText(initialText);
            editText.setSelection(initialText.length());
        }
        builder.setMessage(titleText)
                .setView(editText)
                .setPositiveButton("OK",new DialogInterface.OnClickListener(){
                    public void onClick(DialogInterface dialog, int id){
                        if (callback!=null) callback.onTextInputClickOk();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        if (callback!=null) callback.onTextInputClickCancel();
                    }
                });
        return builder.create();
    }
    public void setCallback(TextInputCallback callback){
        this.callback = callback;
    }
    public void setText(String text){
        if (editText==null)
            initialText = text;
        else
            editText.setText(text);
    }
    public String getText(){
        return editText.getText().toString();
    }
}
