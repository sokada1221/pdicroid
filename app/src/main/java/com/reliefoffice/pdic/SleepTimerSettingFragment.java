package com.reliefoffice.pdic;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.text.SpannableStringBuilder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.EditText;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link SleepTimerSettingFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class SleepTimerSettingFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public SleepTimerSettingFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment SleepTimerSettingFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static SleepTimerSettingFragment newInstance(String param1, String param2) {
        SleepTimerSettingFragment fragment = new SleepTimerSettingFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_sleep_timer_setting, container, false);
    }

    View myView;

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        SleepTimerConfig config = loadConfig();
        setCheckBox(view, R.id.checkEnableSleepTimer, config.EnableSleepTimer);
        setIntText(view, R.id.editSLPAfter, config.SLPAfter);
        setIntText(view, R.id.editSLPTransTime, config.SLPTransTime);
        setIntText(view, R.id.editSLPTargetVolume, config.SLPTargetVolume);
        setCheckBox(view, R.id.checkEnableWakeupTimer, config.EnableWakeupTimer);
        setIntText(view, R.id.editWKUFrom, config.WKUFrom);
        setIntText(view, R.id.editWKUTransTime, config.WKUTransTime);
        setIntText(view, R.id.editWKUTargetVolume, config.WKUTargetVolume);

        myView = view;
    }

    @Override
    public void onStop() {
        SleepTimerConfig config = new SleepTimerConfig();
        config.EnableSleepTimer = getCheckBox(myView, R.id.checkEnableSleepTimer);
        config.SLPAfter = getIntText(myView, R.id.editSLPAfter);
        config.SLPTransTime = getIntText(myView, R.id.editSLPTransTime);
        config.SLPTargetVolume = getIntText(myView, R.id.editSLPTargetVolume);
        config.EnableWakeupTimer = getCheckBox(myView, R.id.checkEnableWakeupTimer);
        config.WKUFrom = getIntText(myView, R.id.editWKUFrom);
        config.WKUTransTime = getIntText(myView, R.id.editWKUTransTime);
        config.WKUTargetVolume = getIntText(myView, R.id.editWKUTargetVolume);

        saveConfig(config);

        config = loadConfig();

        super.onStop();
    }

    SleepTimerConfig loadConfig()
    {
        SleepTimerConfig config = new SleepTimerConfig();
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(getContext());
        config.load(pref);
        return config;
    }

    void saveConfig(SleepTimerConfig config)
    {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(getContext());
        config.save(pref);
    }

    // Helpers //
    void setCheckBox(View view, int id, boolean value)
    {
        CheckBox chkbox = view.findViewById(id);
        chkbox.setChecked(value);
    }
    void setIntText(View view, int id, int value)
    {
        EditText edit = view.findViewById(id);
        edit.setText(Integer.toString(value));
    }
    boolean getCheckBox(View view, int id)
    {
        CheckBox chkbox = view.findViewById(id);
        return chkbox.isChecked();
    }
    int getIntText(View view, int id)
    {
        EditText edit = view.findViewById(id);
        SpannableStringBuilder sb = (SpannableStringBuilder)edit.getText();
        String str = sb.toString();
        return Integer.parseInt(str);
    }
}