#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <log/log.h>
#include "ringbuf.h"

#define LOG_TAG "soundtrigger_vts_ringbuf"

typedef struct {
    int ptr;
    unsigned char buffer[KW_FRAME_SIZE];
} StreamBuffer;

StreamBuffer sBuffer;
static int stop = 0;

int deinitRbuf(ringbuf* cq);

int initRbuf(ringbuf* cq) {
    ALOGD("%s\n", __func__);
    int i = 0;
    stop = 0;
    cq->Q = NULL;
    cq->fptr = 0;
    cq->rptr = 0;
    cq->qsize = 0;
    sBuffer.ptr = KW_FRAME_SIZE;
    cq->deinitRbuf = deinitRbuf;
    ALOGD("%s RBUFSIZE = %d\n", __func__, RBUFSIZE);

    pthread_mutex_init(&cq->strm_lock, (const pthread_mutexattr_t *) NULL);
    pthread_cond_init(&cq->strm_cond, (const pthread_condattr_t *) NULL);

    cq->Q = (unsigned char **)malloc(RBUFSIZE * sizeof(unsigned char*));
    if (NULL == cq->Q)
        return -1;

    memset(cq->Q, 0, RBUFSIZE * sizeof(unsigned char*));
    for (i = 0; i < RBUFSIZE; i ++) {
        cq->Q[i] = (unsigned char *)malloc(KW_FRAME_SIZE);
        if (NULL == cq->Q[i]) {
            ALOGE("%s alloc failed %d\n", __func__, i);
            return -1;
        }
    }
    ALOGD("%s finished\n", __func__);
    return 0;
}

int deinitRbuf(ringbuf* cq) {
    ALOGD("%s\n", __func__);
    int i = 0;
    stop = 1;
    cq->fptr = 0;
    cq->rptr = 0;
    cq->qsize = 0;
    sBuffer.ptr = KW_FRAME_SIZE;
    cq->deinitRbuf = NULL;
    pthread_cond_destroy(&cq->strm_cond);
    pthread_mutex_destroy(&cq->strm_lock);
    for (i = 0; i < RBUFSIZE; i ++) {
        if (cq->Q[i]) {
            free(cq->Q[i]);
            cq->Q[i] = NULL;
        }
    }

    if (cq->Q) {
        free(cq->Q);
        cq->Q = NULL;
    }

    ALOGD("%s finished\n", __func__);
    return 0;
}

int writeRbuf(ringbuf* cq, unsigned char *buff, int size) {
    pthread_mutex_lock( &cq->strm_lock );

    if (size) {
        if (RBUFSIZE == cq->qsize) {
            memcpy(cq->Q[cq->rptr], buff, size);
            cq->rptr = (cq->rptr + 1) % RBUFSIZE;
            cq->fptr = cq->rptr;
            //ALOGE("%s Q over flow\n",__func__);
        } else {
            memcpy(cq->Q[cq->rptr], buff, size);
            cq->rptr = (cq->rptr + 1) % RBUFSIZE;
            cq->qsize++;
        }
    } else {
        stop = 1;
    }

    pthread_cond_signal( &cq->strm_cond );
    pthread_mutex_unlock( &cq->strm_lock );
    return 0;
}

static int copybuf(ringbuf* cq, unsigned char *buff, int size) {
    if (0 == cq->qsize) {
        return -1;
    }

    memcpy(buff, cq->Q[cq->fptr], size);
    cq->fptr = (cq->fptr + 1) % RBUFSIZE;
    cq->qsize --;
    return 0;
}

int readRbuf(ringbuf* cq, unsigned char *buff, int size) {
    int ret = 0;
    static int cnt = 0;

    while(size > 0) {
        if (size > (KW_FRAME_SIZE - sBuffer.ptr)) {
            int read_size = KW_FRAME_SIZE - sBuffer.ptr;
            memcpy(buff, sBuffer.buffer + sBuffer.ptr, read_size);
            buff += read_size;
            size -= read_size;

            pthread_mutex_lock(&cq->strm_lock);
            while(!stop) {
                if (copybuf(cq, sBuffer.buffer, KW_FRAME_SIZE)) {
                    ALOGD("No buffer waitining %d\n", ++ cnt);
                    pthread_cond_wait(&cq->strm_cond, &cq->strm_lock);
                    ALOGD("Lock released\n");
                } else {
                    break;
                }
            }
            pthread_mutex_unlock(&cq->strm_lock);
            sBuffer.ptr = 0;
        } else {
            memcpy(buff, sBuffer.buffer + sBuffer.ptr, size);
            sBuffer.ptr += size;
            size = 0;
        }
    }
    return ret;
}

int availSize(ringbuf* cq) {
    int avail = 0;
    pthread_mutex_lock(&cq->strm_lock);
    if (cq->qsize > 0)
        avail = cq->qsize * KW_FRAME_SIZE;
    pthread_mutex_unlock(&cq->strm_lock);
    return avail;
}

int rollbackRbuf(ringbuf* cq, int milliseconds, int frame_rate) {
    ALOGD("%s adjusting %d milliseconds frame_rate(%d)\n", __func__, milliseconds, frame_rate);
    int qsizebefore = 0;
    int qsizeafter = 0;
    int noOfDataPages = ((BYTES_PER_SAMPLE * (frame_rate / 1000) * milliseconds) / KW_FRAME_SIZE);

    pthread_mutex_lock( &cq->strm_lock );
    if (noOfDataPages<=cq->qsize) {
        qsizebefore = cq->qsize;
        cq->fptr = (cq->rptr + (RBUFSIZE - noOfDataPages)) % RBUFSIZE;
        cq->qsize = noOfDataPages;
        qsizeafter = cq->qsize;
    }

    pthread_mutex_unlock( &cq->strm_lock );
    ALOGD("%s noOfDataPages(%d) qsize(%d %d)\n", __func__, noOfDataPages, qsizebefore, qsizeafter);
    return 0;
}
