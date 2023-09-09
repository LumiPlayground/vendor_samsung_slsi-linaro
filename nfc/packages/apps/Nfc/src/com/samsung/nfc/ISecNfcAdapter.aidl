package com.samsung.nfc;

interface ISecNfcAdapter
{
    boolean setRfOptionUpdate(String filePath);
    boolean setDefaultRoute(int route);
    int getDefaultRoute();
    boolean setListenTechMask(int techMask);
    int getListenTechMask();
    boolean setFWDebugLevel(int type, boolean enable);
    void doFactoryRfCalibration();
    String getFactoryRfCalValue();
    void setPowerOffCardEmulation(boolean enable);
    int[] getSecureElementList();
    void setPreferredSimSlot(int slotId);
    int getPreferredSimSlot();
}
