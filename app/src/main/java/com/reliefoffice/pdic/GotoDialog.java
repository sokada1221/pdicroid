package com.reliefoffice.pdic;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioGroup;

/**
 * Created by tnishi on 2016/10/18.
 */

public class GotoDialog extends DialogFragment {

    private Listener listener;
    public interface Listener {
        void onGotoClicked(Bundle args);
    }

    public void setListener(Listener listener){
        this.listener = listener;
    }

    int checkedId;
    EditText editText;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState){
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        LayoutInflater inflater = (LayoutInflater)getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View content = inflater.inflate(R.layout.goto_dialog, null);

        builder.setView(content);

        builder.setMessage(R.string.move_cursor)
                .setNegativeButton(R.string.action_close, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        // User cancelled the dialog
                        dismiss();
                    }
                });

        editText = (EditText)content.findViewById(R.id.value_text);

        // Setup OK/Cancel button //
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                // 呼び出し元フラグメントのビューを更新
                Bundle arg = new Bundle();
                arg.putInt("selection", checkedId);
                arg.putString("value", editText.getText().toString());
                // MyFragmentのonOkClickedをコール
                listener.onGotoClicked(arg);
            }
        });

        builder.setNegativeButton("Cancel", null);

        // Setup raio button //
        RadioGroup gotoRadioGroup = (RadioGroup)content.findViewById(R.id.goto_radio_group);
        gotoRadioGroup.check(R.id.btn_percent);
        gotoRadioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            public void onCheckedChanged(RadioGroup group, int _checkedId) {
                checkedId = _checkedId;
            }
        });

        // Create the AlertDialog object and return it
        return builder.create();
    }
}
