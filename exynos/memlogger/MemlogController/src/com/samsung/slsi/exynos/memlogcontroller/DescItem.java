package com.samsung.slsi.exynos.memlogcontroller;
import android.widget.Button;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.ListIterator;
import java.util.Iterator;
import android.util.Log;

public class DescItem {
    private byte level = -1;
    private boolean enabled;
    public ArrayList<ObjBtnItem> child;
    public String descName;

    DescItem(String name) {
        descName = name;
        enabled = false;
        child = new ArrayList<ObjBtnItem>();
    }

    public String getDescName() {
        return descName;
    }

    public void setEnabled(boolean enable) { enabled = enable; }
    public void setLevel(byte Level) { level = Level; }
    public boolean getEnabled() { return enabled; }
    public byte getLevel() { return level; }

    public boolean removeChild(String Child) {
        for (ListIterator<ObjBtnItem> it = child.listIterator(); it.hasNext(); ) {
            ObjBtnItem objItem = it.next();
            if (objItem.getText().equals(Child)) {
                child.remove(it.nextIndex());
                Log.i("memlogcontroller", "remove " + descName + " " + Child + " size " + child.size());
                return true;
            }
        }
        return false;
    }
}