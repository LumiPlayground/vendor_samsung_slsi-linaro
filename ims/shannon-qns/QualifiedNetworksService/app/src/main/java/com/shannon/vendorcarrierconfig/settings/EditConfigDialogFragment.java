package com.shannon.vendorcarrierconfig.settings;

import android.app.Dialog;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.fragment.app.DialogFragment;
import androidx.appcompat.app.AlertDialog;

import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.widget.EditText;
import android.widget.FrameLayout.LayoutParams;

import com.shannon.vendorcarrierconfig.settings.IwlanSettingActivity.SettingItem;

public class EditConfigDialogFragment extends DialogFragment {

    public interface DialogEventListener {

        void onPositiveEventClick();

        void onNegativeEventClick();
    }

    private SettingItem mSettingItem = null;
    private DialogEventListener mListener = null;
    private EditText mEditText = null;

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        mEditText = new EditText(getActivity());

        builder.setTitle(mSettingItem.getTitle())
                .setView(mEditText)
                .setPositiveButton("OK", (dialog, id) -> {
                    mSettingItem.updateHandler(mEditText.getText().toString());
                    mListener.onPositiveEventClick();
                })
                .setNegativeButton("CANCEL", (dialog, id) -> {
                    EditConfigDialogFragment.this.getDialog().cancel();
                    mListener.onNegativeEventClick();
                });

        if (!TextUtils.isEmpty(mSettingItem.getDescription())) {
            builder.setMessage(mSettingItem.getDescription());
        }
        return builder.create();
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mEditText != null) {
            LayoutParams lp = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            lp.setMargins(convertDpToPixel(16), convertDpToPixel(4), convertDpToPixel(16), convertDpToPixel(4));

            mEditText.setSingleLine(true);
            mEditText.setLayoutParams(lp);
            mEditText.setText(mSettingItem.getValue());
        }
    }

    private int convertDpToPixel(int dp) {
        return dp * (getContext().getResources().getDisplayMetrics().densityDpi / DisplayMetrics.DENSITY_DEFAULT);
    }

    public void setSettingItem(SettingItem settingItem) {
        mSettingItem = settingItem;
    }

    public void setListener(DialogEventListener listener) {
        mListener = listener;
    }
}
