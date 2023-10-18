#ifndef __FM_RADIO_CONTROLLER_TEST_H__
#define __FM_RADIO_CONTROLLER_TEST_H__

extern "C" int PowerOn(void);
extern "C" int PowerOff(void);
extern "C" int SetTune(long freqkhz);
extern "C" int GetRSSI(void);

#endif /* __FM_RADIO_CONTROLLER_TEST_H__ */
