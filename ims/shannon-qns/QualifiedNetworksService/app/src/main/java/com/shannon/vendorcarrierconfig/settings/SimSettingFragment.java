/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */

package com.shannon.vendorcarrierconfig.settings;

import android.content.Context;
import android.os.Bundle;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ListView;

import com.shannon.qualifiednetworksservice.R;
import com.shannon.vendorcarrierconfig.settings.IwlanSettingActivity.SettingItem;

import java.util.ArrayList;

/**
 * Use the {@link SimSettingFragment#newInstance} factory method to
 * create an instance of this fragment.
 */

public class SimSettingFragment extends Fragment {

    private static final String KEY_PHONEID = "KEY_PHONEID";

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param phoneId Phone ID.
     * @param settingItems List of items in ImsConfig
     * @return A new instance of fragment SimSettingFragment.
     */
    public static SimSettingFragment newInstance(int phoneId, ArrayList<SettingItem> settingItems) {
        SimSettingFragment fragment = new SimSettingFragment();
        Bundle args = new Bundle();

        args.putInt(KEY_PHONEID, phoneId);

        fragment.setArguments(args);
        fragment.setSettingItems(settingItems);

        return fragment;
    }

    private Context mContext;
    private SimSettingAdapter mAdapter;
    private ListView mListView;
    private int mPhoneId;
    private ArrayList<SettingItem> mSettingItems;

    public SimSettingFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            Bundle bundle = getArguments();
            mPhoneId = bundle.getInt(KEY_PHONEID);

        } else {
            mPhoneId = 0;
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_sim_setting, container, false);

        EditText editText = view.findViewById(R.id.search_edittext);
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                mAdapter.getFilter().filter(s);
            }
        });
        return view;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);

        mContext = context;
        mAdapter = new SimSettingAdapter(mContext, R.layout.setting_list_item, mSettingItems);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        mListView = view.findViewById(R.id.sim_config_list_view);
        mListView.setAdapter(mAdapter);
        mListView.setTextFilterEnabled(true);
        IwlanSettingActivity.setDialogOnListView(mListView, mDialogEventListener, "EditConfig",
            getActivity().getSupportFragmentManager());
    }

    private void refreshConfigs() {
        mAdapter.clear();
        for (SettingItem item : mSettingItems) {
            mAdapter.add(item);
        }

        mAdapter.notifyDataSetChanged();
    }

    @Override
    public void onDetach() {
        super.onDetach();
    }

    private EditDialogEventListener mDialogEventListener = new EditDialogEventListener();

    public class EditDialogEventListener implements EditConfigDialogFragment.DialogEventListener {

        @Override
        public void onPositiveEventClick() {
            mAdapter.notifyDataSetChanged();
        }

        @Override
        public void onNegativeEventClick() {

        }
    }

    public void setSettingItems(
            ArrayList<SettingItem> settingItems) {
        mSettingItems = settingItems;
    }
}
