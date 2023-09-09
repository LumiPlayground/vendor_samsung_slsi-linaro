package com.samsung.slsi.exynos.memlogcontroller;

import android.widget.BaseExpandableListAdapter;
import android.app.Activity;
import android.app.Dialog;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Switch;

import com.samsung.slsi.exynos.memlogcontroller.R;

import java.util.ArrayList;
import java.util.Iterator;

public class DescBtnAdapter extends BaseExpandableListAdapter {
    private static final String TAG = "MemlogDescBtnAdapter";
    private Context context;
    private int groupLayout = 0;
    private int childLayout = 0;
    static ArrayList<DescItem> DescList = new ArrayList<DescItem>();
    private LayoutInflater myinf = null;

    public DescBtnAdapter(Context context, int groupLayout, int childLayout) {
        this.groupLayout = groupLayout;
        this.childLayout = childLayout;
        this.context = context;
        this.myinf = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public void setContext(Context context) {
        this.context = context;
    }

    @Override
    public int getGroupCount() {
        return DescList.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        return DescList.get(groupPosition).child.size();
    }

    @Override
    public DescItem getGroup(int groupPosition) {
        return DescList.get(groupPosition);
    }

    public DescItem getGroup(String group) {
        Iterator<DescItem> descItemIterator = DescList.iterator();
        while(descItemIterator.hasNext()) {
            DescItem descItem = descItemIterator.next();
            if (descItem.getDescName().equals(group))
                return descItem;
        }
        return null;
    }

    public DescItem getGroup(String group, boolean create) {
        DescItem descItem = getGroup(group);
        if (descItem != null)
            return descItem;
        else if (create) {
            Log.i(TAG, "getGroup: create new group: " + group);
            descItem = new DescItem(group);
            DescList.add(descItem);
            return descItem;
        } else
            return null;
    }

    @Override
    public ObjBtnItem getChild(int groupPosition, int childPosition) {
        return DescList.get(groupPosition).child.get(childPosition);
    }

    public ObjBtnItem getChild(String group, String child) {
        DescItem descItem = getGroup(group);
        if (descItem != null) {
            Iterator<ObjBtnItem> objBtnItemIterator = descItem.child.iterator();
            while (objBtnItemIterator.hasNext()) {
                ObjBtnItem objItem = objBtnItemIterator.next();
                if (objItem.getText().equals(child))
                    return objItem;
            }
        }
        return null;
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return true;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = myinf.inflate(this.groupLayout, parent, false);
        }
        final DescItem group = getGroup(groupPosition);
        TextView groupView = (TextView) convertView.findViewById(R.id.descView);
        groupView.setText(DescList.get(groupPosition).descName);

        final Switch switch1 = (Switch) convertView.findViewById(R.id.descswitch1);
        final Button button2 = (Button) convertView.findViewById(R.id.descbutton2);

        //switch1.setText(group.enabled ? "all on" : "all off");

        boolean child_enabled = false;
        for (Iterator<ObjBtnItem> it = group.child.iterator(); it.hasNext();) {
            ObjBtnItem objItem = it.next();
            if (group.getLevel() == -1)
                group.setLevel(objItem.getLevel());
            if (objItem.getEnabled() == true) {
                child_enabled = true;
                break;
            }
        }
        button2.setText("level " + group.getLevel());

        switch1.setChecked(child_enabled);
        button2.setEnabled(child_enabled);

        switch1.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                if (switch1.isChecked()) {
                    group.setEnabled(true);
                    //switch1.setText("all on");
                    for (Iterator<ObjBtnItem> it = group.child.iterator(); it.hasNext();) {
                        ObjBtnItem objItem = it.next();
                        objItem.setEnabled(true);
                        MemlogControllerControlInterface.getInstance().enableItem(true, objItem.getFullName());
                    }
                } else {
                    group.setEnabled(false);
                    //switch1.setText("all off");
                    for (Iterator<ObjBtnItem> it = group.child.iterator(); it.hasNext();) {
                        ObjBtnItem objItem = it.next();
                        objItem.setEnabled(false);
                        MemlogControllerControlInterface.getInstance().enableItem(false, objItem.getFullName());
                    }
                }
                notifyDataSetChanged();
            }
        });

        button2.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Activity activity = (Activity) context;
                final int[] level = {-1};
                AlertDialog.Builder builder = new AlertDialog.Builder(context);
                builder.setTitle("Select Group Log Level")
                        .setSingleChoiceItems(R.array.levels, -1,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    level[0] = which;
                                }
                            })
                        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.i(TAG, "level select ok " + level[0]);

                                if (level[0] >= 0 && level[0] < 6) {
                                    group.setLevel((byte) level[0]);
                                    button2.setText("level " + group.getLevel());
                                    for (Iterator<ObjBtnItem> it = group.child.iterator(); it.hasNext();) {
                                        ObjBtnItem objItem = it.next();
                                        objItem.setLevel((byte) level[0]);
                                        MemlogControllerControlInterface.getInstance().setLevel(objItem.getLevel(), objItem.getFullName());
                                    }
                                } else
                                    Log.i(TAG, "level invalid!");
                                notifyDataSetChanged();
                                }
                            })
                        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.i(TAG, "level select cancel");
                            }
                        });
                if (!activity.isFinishing()) {
                    AlertDialog dialog = builder.create();
                    dialog.show();
                }
            }
        });

        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = myinf.inflate(this.childLayout, parent, false);
        }
        TextView childName = convertView.findViewById(R.id.objView);
        TextView childType = convertView.findViewById(R.id.objType);

        final ObjBtnItem objItem = (ObjBtnItem) getChild(groupPosition, childPosition);

        childName.setText(DescList.get(groupPosition).child.get(childPosition).getText());
        childType.setText(DescList.get(groupPosition).child.get(childPosition).getType());

        final Switch switch1 = (Switch) convertView.findViewById(R.id.objswitch1);
        final Button button2 = (Button) convertView.findViewById(R.id.objbutton2);

        final DescItem group = getGroup(groupPosition);

        //android.util.Log.i(TAG, "getChildView: " + objItem.getLevel() + " " + objItem.getEnabled());
        switch1.setChecked(objItem.getEnabled());
        button2.setEnabled(objItem.getEnabled());
        button2.setText("level " + objItem.getLevel());
        switch1.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                if (switch1.isChecked()) {
                    objItem.setEnabled(true);
                    MemlogControllerControlInterface.getInstance().enableItem(true, objItem.getFullName());
                    group.setEnabled(true);
                } else {
                    objItem.setEnabled(false);
                    button2.setEnabled(false);
                    MemlogControllerControlInterface.getInstance().enableItem(false, objItem.getFullName());
                    boolean flag = false;
                    for (Iterator<ObjBtnItem> it = group.child.iterator(); it.hasNext();) {
                        ObjBtnItem objItem = it.next();
                        if (objItem.getEnabled() == true) {
                            flag = true;
                            break;
                        }
                    }
                    if (flag == false)
                        group.setEnabled(false);
                }
                notifyDataSetChanged();
            }
        });

        button2.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Activity activity = (Activity) context;
                final int[] level = {-1};
                AlertDialog.Builder builder = new AlertDialog.Builder(context);
                builder.setTitle("Select File Log Level")
                        .setSingleChoiceItems(R.array.levels, objItem.getLevel(),
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    level[0] = which;
                                }
                        })
                        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.i(TAG, "level select ok " + level[0]);
                                if (level[0] >= 0 && level[0] < 6) {
                                    objItem.setLevel((byte) level[0]);
                                    MemlogControllerControlInterface.getInstance().setLevel(objItem.getLevel(), objItem.getFullName());
                                    button2.setText("level " + objItem.getLevel());
                                } else
                                    Log.i(TAG, "level invalid!");
                                notifyDataSetChanged();
                            }
                        })
                        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.i(TAG, "level select cancel");
                            }
                        });
                if (!activity.isFinishing()) {
                    AlertDialog dialog = builder.create();
                    dialog.show();
                }
            }
        });

        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

    public boolean addChild(String group, String child, boolean enabled, byte level, byte type, String str) {
        DescItem descItem = getGroup(group, true);

        ObjBtnItem objBtnItem = getChild(group, child);

        if (objBtnItem != null) {
            return false;
        } else {
            objBtnItem = new ObjBtnItem();
            objBtnItem.setEnabled(enabled);
            objBtnItem.setLevel(level);
            objBtnItem.setFullName(str);
            objBtnItem.setType(MemlogControllerConstant.objType[type]);
            objBtnItem.setText(child);
            descItem.child.add(objBtnItem);
            notifyDataSetChanged();
            return true;
        }
    }
    //not used for now, just use refresh button!
    public boolean removeChild(String group, String child) {
        DescItem descItem = getGroup(group);

        if (descItem == null) {
            Log.i(TAG, "group not found " + group);
            return false;
        }

        if (descItem.removeChild(child))
            notifyDataSetChanged();
        return true;
    }
}
