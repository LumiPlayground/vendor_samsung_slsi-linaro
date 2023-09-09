
#ifndef RINGBUF_H
#define RINGBUF_H

#ifdef __cplusplus
extern "C" {
#endif
#define BARGEIN_SAMPLING_RATE 16000
#define BYTES_PER_SAMPLE 2
#define CHANNEL_COUNT 2
#define KW_FRAME_SIZE (4800) // FW frame size. Multiplyit with kw_marker to get total number bytes in the KW
#define RBUFSIZE ((CHANNEL_COUNT * BYTES_PER_SAMPLE * BARGEIN_SAMPLING_RATE * 3) / KW_FRAME_SIZE)

#define CVQ_SAMPLE_RATE 16000
#define DEFAULT_END_MARK_TIME 2 // 2s
#define CVQ_CHANNEL 1
#define BYTE_PER_SAMPLE 2
#define DEFAULT_KW_MARKER_END_SIZE (CVQ_SAMPLE_RATE * DEFAULT_END_MARK_TIME * CVQ_CHANNEL * BYTE_PER_SAMPLE)

typedef struct RingBuf ringbuf;
struct RingBuf {
    pthread_mutex_t strm_lock;
    pthread_cond_t strm_cond;
    unsigned char **Q;
    int fptr;
    int rptr;
    int qsize;
    int (*deinitRbuf)(ringbuf*);
};

int initRbuf(ringbuf* cq);
int writeRbuf(ringbuf* cq, unsigned char *buff, int size);
int readRbuf(ringbuf* cq, unsigned char *buff, int size);
int rollbackRbuf(ringbuf* cq, int milliseconds, int frame_rate);
int availSize(ringbuf* cq);

#ifdef __cplusplus
}
#endif

#endif
