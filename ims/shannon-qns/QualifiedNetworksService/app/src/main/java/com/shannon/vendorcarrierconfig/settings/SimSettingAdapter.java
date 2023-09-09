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
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Filter;
import android.widget.TextView;

import com.shannon.qualifiednetworksservice.R;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;
import com.shannon.vendorcarrierconfig.settings.IwlanSettingActivity.SettingItem;

import java.util.ArrayList;
import java.util.Locale;

public class SimSettingAdapter extends ArrayAdapter<SettingItem> {
    private static final String TAG = ShannonVendorCarrierConfig.getServiceTAG("-Setting-Sim");
    // List having all settingItems
    protected ArrayList<SettingItem> mOriginalSettingItems;
    // List to be actually displayed
    protected ArrayList<SettingItem> mSettingItems;
    protected Filter mFilter;
    public SimSettingAdapter(@NonNull Context context, int resource, ArrayList<SettingItem> simSettingItem) {
        super(context, resource, simSettingItem);
        mSettingItems = simSettingItem;
    }

    private class SimFilter extends Filter {
        SimFilter() {
            mOriginalSettingItems = new ArrayList<>(mSettingItems);
        }

        @Override
        protected FilterResults performFiltering(CharSequence constraint) {
            FilterResults filterResults = new FilterResults();
            ArrayList<SettingItem> filteredList = new ArrayList<>();

            if (constraint == null || constraint.length() == 0) {
                final ArrayList<SettingItem> list = new ArrayList<>(mOriginalSettingItems);
                filterResults.values = list;
                filterResults.count = list.size();
            } else {
                for (SettingItem simSettingItem : mOriginalSettingItems) {
                    if (simSettingItem.getTitle().toLowerCase(Locale.ROOT)
                            .contains(constraint.toString().toLowerCase(Locale.ROOT))) {
                        filteredList.add(simSettingItem);
                    }
                }
                filterResults.values = filteredList;
                filterResults.count = filteredList.size();
            }

            return filterResults;
        }

        @Override
        protected void publishResults(CharSequence constraint, FilterResults results) {
            Log.i(TAG, "searching [" + constraint + "], results count [" + results.count + "]");
            //noinspection unchecked
            mSettingItems = (ArrayList<SettingItem>) results.values;
            if (results.count > 0) {
                notifyDataSetChanged();
            } else {
                notifyDataSetInvalidated();
            }
        }
    }

    @Override
    public int getCount() {
        return mSettingItems.size();
    }

    @Override
    public SettingItem getItem(int position) {
        try {
            return mSettingItems.get(position);
        } catch (IndexOutOfBoundsException e) {
            Log.d(TAG, "out of bound! at pos " + position);
            return null;
        }
    }

    @Override
    public Filter getFilter() {
        if (mFilter == null) {
            mFilter = new SimFilter();
        }
        return mFilter;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(parent.getContext()).inflate(R.layout.setting_list_item, parent, false);
        }

        TextView title = convertView.findViewById(R.id.textEditorTitle);
        TextView description = convertView.findViewById(R.id.textEditorValue);
        SettingItem item = (SettingItem) getItem(position);

        if (item != null) {
            title.setText(item.getTitle());
            description.setText(item.getValue());
        }

        return convertView;
    }
}
