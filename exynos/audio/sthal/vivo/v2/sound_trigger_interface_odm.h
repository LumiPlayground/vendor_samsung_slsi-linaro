
#ifndef __VOLCANO_STHAL_INTERFACE_H__
#define __VOLCANO_STHAL_INTERFACE_H__

#include <system/sound_trigger.h>

typedef enum {
   VTS_RECOGNIZE_STOP  = 0,  // Stop recognization
   VTS_RECOGNIZE_START = 1,  // Start recognization
} VTS_RECOGNIZE_STATE;

/* ODM interface */
extern struct sound_trigger_phrase_recognition_event *odm_sound_trigger_event_alloc(
    void *stdev_ptr, int index);
extern struct sound_trigger_generic_recognition_event *odm_sound_trigger_generic_event_alloc(
    void *stdev_ptr, int index);
extern int odm_callback_thread_loop(void *stdev_ptr, struct pollfd poll_event);
extern void odm_vts_set_power(void *stdev_ptr, int req);
extern void odm_chk_n_set_rec_call_state(void *stdev_ptr);
extern void odm_chk_n_set_read_status(void *stdev_ptr);
extern void odm_stop_thread(void *stdev_ptr);
extern void odm_stop_recognition_l(void *stdev_ptr, sound_model_handle_t handle,
    int active_bitmask);
extern int odm_open_pcm_for_streaming(void *stdev_ptr);
extern int odm_pcm_read_sample(void *stdev_ptr, void *buffer, size_t  buffer_len);
extern void odm_pcm_close_for_streaming(void *stdev_ptr);
extern void odm_sound_trigger_voicecall_status(void *stdev_ptr, int callstate);
extern void odm_get_ahal_api(void *stdev_ptr);

#endif /* __VOLCANO_STHAL_INTERFACE_H__ */
