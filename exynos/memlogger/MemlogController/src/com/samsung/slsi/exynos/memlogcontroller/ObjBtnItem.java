package com.samsung.slsi.exynos.memlogcontroller;
import android.graphics.drawable.Drawable;
import android.widget.Button;

public class ObjBtnItem {
    private Drawable iconDrawable;
    private String textStr;
    private String fullName;
    private String type;
    private boolean enabled;
    private byte level;
    public void setIcon(Drawable icon) {
        iconDrawable = icon;
    }
    public void setText(String text) {
        textStr = text;
    }
    public void setFullName(String text) { fullName = text; }
    public void setType(String text) { type = text; }
    public void setEnabled(boolean enable) { enabled = enable; }
    public void setLevel(byte Level) { level = Level; }
    public boolean getEnabled() { return enabled; }
    public byte getLevel() { return level; }

    public Drawable getIcon() {
        return this.iconDrawable;
    }
    public String getText() {
        return this.textStr;
    }
    public String getFullName() { return this.fullName; }
    public String getType() { return this.type; }
}