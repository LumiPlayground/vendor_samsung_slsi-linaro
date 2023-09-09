/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "AVIExtractor"
#define AVI_INDEX_OF_INDEXES  0x00
#define AVI_INDEX_OF_CHUNKS   0x01
#include <utils/Log.h>

#include "AVIExtractor.h"
#include <binder/ProcessState.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AUtils.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ByteUtils.h>
#include <media/stagefright/foundation/ColorUtils.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaBufferBase.h>
#include <media/stagefright/foundation/avc_utils.h>
#include <media/stagefright/MetaDataUtils.h>
//#include <media/DataSourceBase.h>
//#include <media/stagefright/MetaDataBase.h>

namespace android {

struct AVIExtractor::AVISource : public MediaTrackHelper {
    AVISource(AVIExtractor *extractor, size_t trackIndex);

    virtual media_status_t start();
    virtual media_status_t stop();

    virtual media_status_t getFormat(AMediaFormat *meta);

    virtual media_status_t read(
            MediaBufferHelper **buffer, const ReadOptions *options);

protected:
    virtual ~AVISource();

private:
    AVIExtractor* mExtractor;
    size_t mTrackIndex;
    const AVIExtractor::Track &mTrack;
    size_t mSampleIndex;

    sp<MP3Splitter> mSplitter;

    DISALLOW_EVIL_CONSTRUCTORS(AVISource);
};

////////////////////////////////////////////////////////////////////////////////

struct AVIExtractor::MP3Splitter : public RefBase {
    MP3Splitter();

    void clear();
    void append(MediaBufferHelper *buffer);

    virtual media_status_t read(MediaBufferHelper **buffer);

protected:
    virtual ~MP3Splitter();

private:
    bool mFindSync;
    int64_t mBaseTimeUs;
    int64_t mNumSamplesRead;
    sp<ABuffer> mBuffer;

    bool resync();

    DISALLOW_EVIL_CONSTRUCTORS(MP3Splitter);
};

////////////////////////////////////////////////////
AVIExtractor::AVISource::AVISource(
        AVIExtractor *extractor,
        size_t        trackIndex)
    : mExtractor(extractor),
      mTrackIndex(trackIndex),
      mTrack(mExtractor->mTracks.itemAt(trackIndex)) {
        mSampleIndex = 0;
}

AVIExtractor::AVISource::~AVISource() {
    if (mBufferGroup) {
        stop();
    }
}

media_status_t AVIExtractor::AVISource::start()
{
    ALOGV("AVISource::start");

    mBufferGroup->add_buffer(mTrack.mMaxSampleSize);
    mBufferGroup->add_buffer(mTrack.mMaxSampleSize);

    const char *mime;
    if (!AMediaFormat_getString(mTrack.mMeta, AMEDIAFORMAT_KEY_MIME, &mime)) {
        return AMEDIA_ERROR_UNKNOWN;
    }

    if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG)) {
        mSplitter = new MP3Splitter();
    } else {
        if (mSplitter != NULL)
            mSplitter->clear();
    }

    return AMEDIA_OK;
}

media_status_t AVIExtractor::AVISource::stop()
{
    ALOGV("AVISource::stop");

    CHECK(mBufferGroup);

    delete mBufferGroup;
    mBufferGroup = NULL;

    if (mSplitter != NULL)
        mSplitter->clear();

    return AMEDIA_OK;
}

media_status_t AVIExtractor::AVISource::getFormat(AMediaFormat *meta)
{
    ALOGV("AVISource::getFormat");
    const char *mime = NULL;
    AMediaFormat_copy(meta, mTrack.mMeta);
    AMediaFormat_getString(meta, AMEDIAFORMAT_KEY_MIME, &mime);
    return AMEDIA_OK;
}

media_status_t AVIExtractor::AVISource::read(
    MediaBufferHelper **buffer,
    const ReadOptions  *options)
{
    CHECK(mBufferGroup);

    *buffer = NULL;

    int64_t seekTimeUs;
    ReadOptions::SeekMode seekMode;
    if (options && options->getSeekTo(&seekTimeUs, &seekMode)) {
        status_t err =
            mExtractor->getSampleIndexAtTime(
                    mTrackIndex, seekTimeUs, seekMode, &mSampleIndex);

        if (err != OK) {
            return AMEDIA_ERROR_END_OF_STREAM;
        }

        if (mSplitter != NULL) {
            mSplitter->clear();
        }
    }

    for (int i = 0;; i++) {
        if (mSplitter != NULL) {
            status_t err = mSplitter->read(buffer);

            if (err == OK)
                break;
            else if (err != AMEDIA_ERROR_UNKNOWN)
                return AMEDIA_ERROR_MALFORMED;
        }

        off64_t offset;
        size_t  size;
        bool    isKey;
        int64_t timeUs;

        status_t err = mExtractor->getSampleInfo(mTrackIndex, mSampleIndex, &offset, &size, &isKey, &timeUs);

        ++mSampleIndex;

        if (err != OK) {
            return AMEDIA_ERROR_END_OF_STREAM;
        }

        MediaBufferHelper *out;
        CHECK_EQ(mBufferGroup->acquire_buffer(&out), (media_status_t)AMEDIA_OK);

        ssize_t n = mExtractor->mDataSource->readAt(offset, out->data(), size);

        if (n < (ssize_t)size) {
            return n < 0 ? (media_status_t)n : (media_status_t)AMEDIA_ERROR_MALFORMED;
        }

        out->set_range(0, size);

        AMediaFormat_setInt64(out->meta_data(), AMEDIAFORMAT_KEY_TIME_US, timeUs);

        if (isKey) {
            AMediaFormat_setInt32(out->meta_data(), AMEDIAFORMAT_KEY_IS_SYNC_FRAME, 1);
        }

        if (mSplitter == NULL) {
            *buffer = out;
            break;
        }

        mSplitter->append(out);
        out->release();
        out = NULL;
    }

    return AMEDIA_OK;
}

//////////////////////////////////////////////////////////

AVIExtractor::MP3Splitter::MP3Splitter()
    : mFindSync(true),
      mBaseTimeUs(-1ll),
      mNumSamplesRead(0),
      mBuffer(nullptr) {
}

AVIExtractor::MP3Splitter::~MP3Splitter() {

}

void AVIExtractor::MP3Splitter::clear() {
    mFindSync = true;
    mBaseTimeUs = -1ll;
    mNumSamplesRead = 0;

    if (mBuffer != NULL) {
        mBuffer->setRange(0, 0);
    }
}

void AVIExtractor::MP3Splitter::append(MediaBufferHelper *buffer)
{
    size_t prevCapacity = (mBuffer != NULL) ? mBuffer->capacity() : 0;

    if (mBaseTimeUs < 0) {
        CHECK(mBuffer == NULL || mBuffer->size() == 0);
        AMediaFormat_getInt64(buffer->meta_data(), AMEDIAFORMAT_KEY_TIME_US, &mBaseTimeUs);

        mNumSamplesRead = 0;
    }

    if (mBuffer != NULL && mBuffer->offset() > 0) {
        memmove(mBuffer->base(), mBuffer->data(), mBuffer->size());
        mBuffer->setRange(0, mBuffer->size());
    }

    if (mBuffer == NULL
            || mBuffer->size() + buffer->range_length() > prevCapacity) {
        size_t newCapacity =
            (prevCapacity + buffer->range_length() + 1023) & ~1023;

        sp<ABuffer> newBuffer = new ABuffer(newCapacity);
        if (mBuffer != NULL) {
            memcpy(newBuffer->data(), mBuffer->data(), mBuffer->size());
            newBuffer->setRange(0, mBuffer->size());
        } else {
            newBuffer->setRange(0, 0);
        }
        mBuffer = newBuffer;
    }

    memcpy(mBuffer->data() + mBuffer->size(),
           (const uint8_t *)buffer->data() + buffer->range_offset(),
           buffer->range_length());

    mBuffer->setRange(0, mBuffer->size() + buffer->range_length());
}

bool AVIExtractor::MP3Splitter::resync()
{
    if (mBuffer == NULL)
        return false;

    bool foundSync = false;
    for (size_t offset = 0; offset + 3 < mBuffer->size(); ++offset) {
        uint32_t firstHeader = U32_AT(mBuffer->data() + offset);

        size_t frameSize;
        if (!GetMPEGAudioFrameSize(firstHeader, &frameSize)) {
            continue;
        }

        size_t subsequentOffset = offset + frameSize;
        size_t i = 3;
        while (i > 0) {
            if (subsequentOffset + 3 >= mBuffer->size()) {
                break;
            }

            static const uint32_t kMask = 0xfffe0c00;

            uint32_t header = U32_AT(mBuffer->data() + subsequentOffset);
            if ((header & kMask) != (firstHeader & kMask)) {
                break;
            }

            if (!GetMPEGAudioFrameSize(header, &frameSize)) {
                break;
            }

            subsequentOffset += frameSize;
            --i;
        }

        if (i == 0) {
            foundSync = true;
            memmove(mBuffer->data(),
                    mBuffer->data() + offset,
                    mBuffer->size() - offset);

            mBuffer->setRange(0, mBuffer->size() - offset);
            break;
        }
    }

    return foundSync;
}

media_status_t AVIExtractor::MP3Splitter::read(MediaBufferHelper **out)
{
    *out = NULL;

    if (mFindSync) {
        if (!resync()) {
            ALOGW("[%s] resync() failed", __FUNCTION__);
            return AMEDIA_ERROR_UNKNOWN;
        }

        mFindSync = false;
    }

    if (mBuffer->size() < 4) {
        ALOGW("[%s] mBuffer->size(%d) is small", __FUNCTION__, (int)mBuffer->size());
        return AMEDIA_ERROR_UNKNOWN;
    }

    uint32_t header = U32_AT(mBuffer->data());
    size_t   frameSize;
    int      sampleRate;
    int      numSamples;
    if (!GetMPEGAudioFrameSize(
                header, &frameSize, &sampleRate, NULL, NULL, &numSamples)) {
        return AMEDIA_ERROR_MALFORMED;
    }

    if (mBuffer->size() < frameSize) {
        ALOGW("[%s] mBuffer->size(%d) is smaller than frameSize(%d)", __FUNCTION__, (int)mBuffer->size(), (int)frameSize);
        return AMEDIA_ERROR_UNKNOWN;
    }

    MediaBufferBase   *mbuf       = MediaBufferBase::Create(frameSize);
    CMediaBuffer      *mCMediaBuf = mbuf->wrap();
    MediaBufferHelper *mBufHelper = new MediaBufferHelper(mCMediaBuf);

    memcpy(mBufHelper->data(), mBuffer->data(), frameSize);

    int64_t timeUs   = mBaseTimeUs + (mNumSamplesRead * 1000000ll) / sampleRate;
    mNumSamplesRead += numSamples;
    AMediaFormat_setInt64(mBufHelper->meta_data(), AMEDIAFORMAT_KEY_TIME_US, timeUs);

    mBuffer->setRange(
            mBuffer->offset() + frameSize, mBuffer->size() - frameSize);

    *out = mBufHelper;

    return AMEDIA_OK;
}

////////////////////////////////////////////////////////

AVIExtractor::AVIExtractor(DataSourceHelper *source)
    : mDataSource(source) {
    mInitCheck = parseHeaders();

    if (mInitCheck != OK) {
        ALOGV("parseHeaders failed, lead to mInitCheck != OK，mTracks will be clear");
        mTracks.clear();
    }
}

AVIExtractor::~AVIExtractor() {

}

size_t AVIExtractor::countTracks() {
    ALOGV("AVIExtractor::countTracks");

    return mTracks.size();
}

MediaTrackHelper *AVIExtractor::getTrack(size_t index) {
    ALOGV("AVIExtractor::getTrack");

    return index < mTracks.size() ? new AVISource(this, index) : NULL;
}

media_status_t AVIExtractor::getTrackMetaData(
    AMediaFormat *meta,
    size_t        index,
    uint32_t)
{
    ALOGV("AVIExtractor::getTrackMetaData");

    if (index < mTracks.size()) {
        AMediaFormat_copy(meta, mTracks.editItemAt(index).mMeta);
        return AMEDIA_OK;
    } else {
        return AMEDIA_ERROR_UNKNOWN;
    }
}

media_status_t AVIExtractor::getMetaData(AMediaFormat *meta)
{
    ALOGV("AVIExtractor::getMetaData");

    if (mInitCheck == OK) {
        AMediaFormat_setString(meta, AMEDIAFORMAT_KEY_MIME, MEDIA_MIMETYPE_CONTAINER_AVI);
    }

    return AMEDIA_OK;
}

uint32_t AVIExtractor::flags() const {
    if(mIndexType == NO_INDEX)
    {
        return CAN_PAUSE;
    }
    else
    {
        return (CAN_PAUSE | CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_SEEK);
    }
}

status_t AVIExtractor::parseHeaders()
{
    ALOGV("AVIExtractor::parseHeaders");

    mTracks.clear();
    mMovieOffset        = 0;
    mFoundIndex         = false;
    mOffsetsAreAbsolute = false;
    mIndexType          = NO_INDEX;

    off64_t dataSize = 0;
    status_t err = mDataSource->getSize(&dataSize);
    if(err == ERROR_UNSUPPORTED) {
        dataSize = -1;
    } else if (err != OK) {
        return err;
    }
    ssize_t res = parseChunk(0ll, dataSize);

    ALOGV("[%s] parseHeaders res: %zd, mMovieOffset: %d, mFoundIndex: %d",
                __FUNCTION__, res, (int)mMovieOffset, mFoundIndex);

    if (res < 0) {
        return (status_t)res;
    }

   if(mIndexType == NO_INDEX)
    {
        ALOGV("[%s] NO index existed!", __FUNCTION__);
        mOffsetsAreAbsolute = true;
        //init all track's first sample position to movi's first sample position.
        for (size_t i = 0; i < mTracks.size(); ++i) {
            Track *track = &mTracks.editItemAt(i);
            track->mCurSamplePos = mMovieOffset + 12;
        }
    }
    //from parseindex()
    for (size_t i = 0; i < mTracks.size(); ++i) {
        Track *track = &mTracks.editItemAt(i);
        const char *tmp;
        AMediaFormat_getString(track->mMeta, AMEDIAFORMAT_KEY_MIME, &tmp);
        AString mime = tmp;
        ALOGV("[%s] mime from meta: %s", __FUNCTION__, tmp);

        if (!strncasecmp("video/", mime.c_str(), 6)) {
            if (track->mThumbnailSampleIndex >= 0) {
                int64_t thumbnailTimeUs;

                if ((status_t)OK !=
                        (err = getSampleTime(i, track->mThumbnailSampleIndex,
                                             &thumbnailTimeUs))) {
                    ALOGE("[%s] getSampleTime failed(%d)", __FUNCTION__, err);
                    return err;
                }

                AMediaFormat_setInt64(track->mMeta, AMEDIAFORMAT_KEY_THUMBNAIL_TIME, thumbnailTimeUs);
            }

            err = OK;

            if (!strcasecmp(mime.c_str(), MEDIA_MIMETYPE_VIDEO_MPEG4)) {
                if (OK != addMPEG4CodecSpecificData(i)) {
                    ALOGE("[%s] parseHeaders, addMPEG4CodecSpecificData failed, trackidx=%zd", __FUNCTION__, i);
                }
            } else if (!strcasecmp(mime.c_str(), MEDIA_MIMETYPE_VIDEO_AVC)) {
                err = addH264CodecSpecificData(i);
            }

            if (err != OK) {
                return err;
            }
        }
    }

    if (mMovieOffset == 0ll) {
        return ERROR_MALFORMED;
    }

    return OK;
}
ssize_t AVIExtractor::parseChunk(off64_t offset, off64_t size, int depth)
{
    ALOGV("AVIExtractor::parseChunk");

    if (size >= 0 && size < 8) {
        return ERROR_MALFORMED;
    }

    uint8_t tmp[12];
    ssize_t n = mDataSource->readAt(offset, tmp, 8);

    if (n < 8) {
        return (n < 0) ? n : (ssize_t)ERROR_MALFORMED;
    }

    uint32_t fourcc = U32_AT(tmp);
    uint32_t chunkSize = U32LE_AT(&tmp[4]);

    if (size >= 0 && chunkSize + 8 > size) {
        return ERROR_MALFORMED;
    }

    static const char kPrefix[] = "                              ";
    const char       *prefix    = &kPrefix[strlen(kPrefix) - 2 * depth];

    // It's a list of chunks
    if (fourcc == FOURCC('L', 'I', 'S', 'T')
            || fourcc == FOURCC('R', 'I', 'F', 'F')) {

        if (size >= 0 && size < 12) {
            return ERROR_MALFORMED;
        }

        n = mDataSource->readAt(offset + 8, &tmp[8], 4);

        if (n < 4) {
            return (n < 0) ? n : (ssize_t)ERROR_MALFORMED;
        }

        uint32_t subFourcc = U32_AT(&tmp[8]);

        ALOGV("[%s] %s offset 0x%08llx LIST of '%c%c%c%c', size %d", __FUNCTION__,
             prefix,
             (unsigned long long)offset,
             (char)(subFourcc >> 24),
             (char)((subFourcc >> 16) & 0xff),
             (char)((subFourcc >> 8) & 0xff),
             (char)(subFourcc & 0xff),
             chunkSize - 4);

        if (subFourcc == FOURCC('m', 'o', 'v', 'i')) {
            // We're not going to parse this, but will take note of the
            // offset.

            mMovieOffset = offset;
        } else {
            off64_t subOffset = offset + 12;
            off64_t subOffsetLimit = subOffset + chunkSize - 4;
            while (subOffset < subOffsetLimit) {
                ssize_t res =
                    parseChunk(subOffset, subOffsetLimit - subOffset, depth + 1);

                if (res < 0) {
                    return res;
                }

                subOffset += res;
            }
        }
    } else {
        ALOGV("[%s] %s offset 0x%08llx CHUNK '%c%c%c%c'", __FUNCTION__,
             prefix,
             (unsigned long long)offset,
             (char)(fourcc >> 24),
             (char)((fourcc >> 16) & 0xff),
             (char)((fourcc >> 8) & 0xff),
             (char)(fourcc & 0xff));

        status_t err = OK;

        switch (fourcc) {
        case FOURCC('s', 't', 'r', 'h'):
            err = parseStreamHeader(offset + 8, chunkSize);
            break;
        case FOURCC('s', 't', 'r', 'f'):
            err = parseStreamFormat(offset + 8, chunkSize);
            break;
        case FOURCC('i', 'd', 'x', '1'):
            if(mIndexType == NO_INDEX) {
                err = parseIdx1(offset + 8, chunkSize);
                mIndexType = IDX1;
            }
            break;
        case FOURCC('i', 'n', 'd', 'x'):
            parseIndx(offset, chunkSize + 8);
            mIndexType = INDX;
            break;
        default:
            break;
        }

        if (err != OK)
            return err;
    }

    if (chunkSize & 1)
        ++chunkSize;

    return chunkSize + 8;
}

static const char *GetMIMETypeForHandler(uint32_t handler)
{
    ALOGV("GetMIMETypeForHandler");
    switch (handler) {
        // Wow... shamelessly copied from
        // http://wiki.multimedia.cx/index.php?title=ISO_MPEG-4

        case FOURCC('3', 'I', 'V', '2'):
        case FOURCC('3', 'i', 'v', '2'):
        case FOURCC('B', 'L', 'Z', '0'):
        case FOURCC('D', 'I', 'G', 'I'):
        case FOURCC('D', 'I', 'V', '1'):
        case FOURCC('d', 'i', 'v', '1'):
        /* This type of stream can not be played for license issue */
        // case FOURCC('D', 'I', 'V', 'X'):
        // case FOURCC('d', 'i', 'v', 'x'):
        // case FOURCC('D', 'X', '5', '0'):
        // case FOURCC('d', 'x', '5', '0'):
        case FOURCC('D', 'X', 'G', 'M'):
        case FOURCC('E', 'M', '4', 'A'):
        case FOURCC('E', 'P', 'H', 'V'):
        case FOURCC('F', 'M', 'P', '4'):
        case FOURCC('f', 'm', 'p', '4'):
        case FOURCC('F', 'V', 'F', 'W'):
        case FOURCC('H', 'D', 'X', '4'):
        case FOURCC('h', 'd', 'x', '4'):
        case FOURCC('M', '4', 'C', 'C'):
        case FOURCC('M', '4', 'S', '2'):
        case FOURCC('m', '4', 's', '2'):
        case FOURCC('M', 'P', '4', 'S'):
        case FOURCC('m', 'p', '4', 's'):
        case FOURCC('M', 'P', '4', 'V'):
        case FOURCC('m', 'p', '4', 'v'):
        case FOURCC('M', 'V', 'X', 'M'):
        case FOURCC('R', 'M', 'P', '4'):
        case FOURCC('S', 'E', 'D', 'G'):
        case FOURCC('S', 'M', 'P', '4'):
        case FOURCC('U', 'M', 'P', '4'):
        case FOURCC('W', 'V', '1', 'F'):
        case FOURCC('X', 'V', 'I', 'D'):
        case FOURCC('X', 'v', 'i', 'D'):
        case FOURCC('x', 'v', 'i', 'd'):
        case FOURCC('X', 'V', 'I', 'X'):
            return MEDIA_MIMETYPE_VIDEO_MPEG4;

        // from http://wiki.multimedia.cx/index.php?title=H264
        case FOURCC('a', 'v', 'c', '1'):
        case FOURCC('d', 'a', 'v', 'c'):
        case FOURCC('x', '2', '6', '4'):
        case FOURCC('X', '2', '6', '4'):
        case FOURCC('h', '2', '6', '4'):
        case FOURCC('H', '2', '6', '4'):
        case FOURCC('v', 's', 's', 'h'):
            return MEDIA_MIMETYPE_VIDEO_AVC;

        default:
            return NULL;
    }
}

status_t AVIExtractor::parseStreamHeader(off64_t offset, size_t size)
{
    ALOGV("AVIExtractor::parseStreamHeader");

    if (size != 56) {
        return ERROR_MALFORMED;
    }

    if (mTracks.size() > 99) {
        return -ERANGE;
    }

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    const uint8_t *data = buffer->data();

    uint32_t type       = U32_AT(data);
    uint32_t handler    = U32_AT(&data[4]);
    AMediaFormat *meta  = AMediaFormat_new();

    uint32_t rate       = U32LE_AT(&data[20]);
    uint32_t scale      = U32LE_AT(&data[24]);
    uint32_t length     = U32LE_AT(&data[32]);
    uint32_t sampleSize = U32LE_AT(&data[44]);

    const char *mime      = NULL;
    Track::Kind kind      = Track::OTHER;
    int32_t maxSampleSize = 0;

    if (type == FOURCC('v', 'i', 'd', 's')) {
        mime = GetMIMETypeForHandler(handler);
        ALOGV("[%s] mime from fourcc: %s", __FUNCTION__, mime);
        if (mime && strncasecmp(mime, "video/", 6)) {
            ALOGE("[%s] invalid mime from fourcc: %s", __FUNCTION__, mime);
            return ERROR_MALFORMED;
        }

        if (mime == NULL) {
            ALOGW("[%s] Unsupported video format '%c%c%c%c'", __FUNCTION__,
                 (char)(handler >> 24),
                 (char)((handler >> 16) & 0xff),
                 (char)((handler >> 8) & 0xff),
                 (char)(handler & 0xff));
        }

        kind          = Track::VIDEO;
        maxSampleSize = AVI_VIDEO_SAMPLE_MAX_SIZE;
    } else if (type == FOURCC('a', 'u', 'd', 's')) {
        if (mime && strncasecmp(mime, "audio/", 6)) {
            ALOGE("[%s] invalid mime from fourcc: %s", __FUNCTION__, mime);
            return ERROR_MALFORMED;
        }

        kind = Track::AUDIO;
    }

    if (!mime) {
        mime = "application/octet-stream";
    }

    AMediaFormat_setString(meta, AMEDIAFORMAT_KEY_MIME, mime);
    ALOGV("[%s] mimes to meta: %s", __FUNCTION__, mime);

    if (scale == 0) {
        scale = 1;
    }
    if (rate == 0) {
        rate = 1;
    }

    AMediaFormat_setInt64(meta, AMEDIAFORMAT_KEY_DURATION, (int64_t)(length * 1000000ll * rate / scale));
    AMediaFormat_setInt32(meta, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, maxSampleSize);

    mTracks.push();
    Track *track = &mTracks.editItemAt(mTracks.size() - 1);

    track->mMeta            = meta;
    track->mRate            = rate;
    track->mScale           = scale;
    track->mBytesPerSample  = sampleSize;
    track->mKind            = kind;
    track->mNumSyncSamples  = 0;
    track->mThumbnailSampleSize  = 0;
    track->mThumbnailSampleIndex = -1;
    track->mMaxSampleSize   = 0;
    track->mAvgChunkSize    = 1.0;
    track->mFirstChunkSize  = 0;

    return OK;
}

status_t AVIExtractor::parseStreamFormat(off64_t offset, size_t size)
{
    ALOGV("AVIExtractor::parseStreamFormat");

    if (mTracks.isEmpty()) {
        ALOGE("[%s] mTracks is empty", __FUNCTION__);
        return ERROR_MALFORMED;
    }

    Track *track = &mTracks.editItemAt(mTracks.size() - 1);

    if (track->mKind == Track::OTHER) {
        // We don't support this content, but that's not a parsing error.
        return OK;
    }

    bool isVideo = (track->mKind == Track::VIDEO);

    if ((isVideo && size < 40) || (!isVideo && size < 16)) {
        // Expected a BITMAPINFO or WAVEFORMAT(EX) structure, respectively.
        return ERROR_MALFORMED;
    }

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    const uint8_t *data = buffer->data();

    if (isVideo) {
        const char *mime    = NULL;
        uint32_t    width   = U32LE_AT(&data[4]);
        uint32_t    height  = U32LE_AT(&data[8]);
        uint32_t    handler = U32_AT(&data[16]);

        mime = GetMIMETypeForHandler(handler);
        ALOGV("[%s] mime from fourcc: %s", __FUNCTION__, mime);
        if ((!mime)||(mime && strncasecmp(mime, "video/", 6))) {
            ALOGW("[%s] strh Unsupported video format '%c%c%c%c'", __FUNCTION__,
                 (char)(handler >> 24),
                 (char)((handler >> 16) & 0xff),
                 (char)((handler >> 8) & 0xff),
                 (char)(handler & 0xff));
        } else {
            AMediaFormat_setString(track->mMeta, AMEDIAFORMAT_KEY_MIME, mime);
            ALOGV("[%s] mime set to meta: %s", __FUNCTION__, mime);
        }

        AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_WIDTH,  (int32_t)width);
        AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_HEIGHT, (int32_t)height);
    } else {
        uint32_t format = U16LE_AT(data);

        if (format == 0x55) {
            AMediaFormat_setString(track->mMeta, AMEDIAFORMAT_KEY_MIME, MEDIA_MIMETYPE_AUDIO_MPEG);
        } else {
            ALOGW("[%s] Unsupported audio format = 0x%04x", __FUNCTION__, format);
        }

        uint32_t numChannels = U16LE_AT(&data[2]);
        uint32_t sampleRate  = U32LE_AT(&data[4]);

        AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_CHANNEL_COUNT, numChannels);
        AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_SAMPLE_RATE, sampleRate);
    }

    return OK;
}

// static
bool AVIExtractor::IsCorrectChunkType(
    ssize_t     trackIndex,
    Track::Kind kind,
    uint32_t    chunkType)
{
    ALOGV("AVIExtractor::IsCorrectChunkType");

    uint32_t chunkBase = chunkType & 0xffff;

    switch (kind) {
    case Track::VIDEO:
        if (chunkBase != FOURCC(0, 0, 'd', 'c') && chunkBase != FOURCC(0, 0, 'd', 'b')) {
            return false;
        }
        break;
    case Track::AUDIO:
        if (chunkBase != FOURCC(0, 0, 'w', 'b')) {
            return false;
        }
        break;
    default:
        break;
    }

    if (trackIndex < 0) {
        return true;
    }

    uint8_t hi = chunkType >> 24;
    uint8_t lo = (chunkType >> 16) & 0xff;

    if (hi < '0' || hi > '9' || lo < '0' || lo > '9') {
        return false;
    }

    if (trackIndex != (10 * (hi - '0') + (lo - '0'))) {
        return false;
    }

    return true;
}


status_t AVIExtractor::parseIdx1(off64_t offset, size_t size)
{
    ALOGV("AVIExtractor::parseIdx1");

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        //return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    const uint8_t *data = buffer->data();

    while (n >= 16) {
        uint32_t chunkType = U32_AT(data);

        uint8_t hi = chunkType >> 24;
        uint8_t lo = (chunkType >> 16) & 0xff;

        if (hi < '0' || hi > '9' || lo < '0' || lo > '9') {
            data += 16;
            n -= 16;
            continue;
        }

        size_t trackIndex = 10 * (hi - '0') + (lo - '0');

        if (trackIndex >= mTracks.size()) {
            return ERROR_MALFORMED;
        }

        Track *track = &mTracks.editItemAt(trackIndex);

        if (!IsCorrectChunkType(-1, track->mKind, chunkType)) {
            return ERROR_MALFORMED;
        }

        if (track->mKind == Track::OTHER) {
            data += 16;
            size -= 16;
            continue;
        }

        uint32_t flags      = U32LE_AT(&data[4]);
        uint32_t offset     = U32LE_AT(&data[8]);
        uint32_t chunkSize  = U32LE_AT(&data[12]);

        if (chunkSize > track->mMaxSampleSize) {
            track->mMaxSampleSize = chunkSize;
        }

        track->mSamples.push();

        SampleInfo *info =
            &track->mSamples.editItemAt(track->mSamples.size() - 1);

        info->mOffset = offset;
        info->mIsKey  = (flags & 0x10) != 0;
        if(track->mSamples.size() > 1){
            track->mLengthTotal += track->mPreChunkSize;
        }else{
            track->mLengthTotal = 0;
        }

        track->mPreChunkSize = chunkSize;
        info->mLengthTotal   = track->mLengthTotal;

        if (info->mIsKey) {
            static const size_t kMaxNumSyncSamplesToScan = 20;

            if (track->mNumSyncSamples < kMaxNumSyncSamplesToScan) {
                if (chunkSize > track->mThumbnailSampleSize) {
                    track->mThumbnailSampleSize = chunkSize;

                    track->mThumbnailSampleIndex =
                        track->mSamples.size() - 1;
                }
            }

            ++track->mNumSyncSamples;
        }

        data += 16;
        n -= 16;
    }

    if (!mTracks.isEmpty()) {
        off64_t  offset;
        size_t   size;
        bool     isKey;
        int64_t  timeUs;
        status_t err = getSampleInfo(0, 0, &offset, &size, &isKey, &timeUs);

        if (err != OK) {
            mOffsetsAreAbsolute = !mOffsetsAreAbsolute;
            err = getSampleInfo(0, 0, &offset, &size, &isKey, &timeUs);

            if (err != OK) {
                return err;
            }
        }

        ALOGV("Chunk offsets are %s",
             mOffsetsAreAbsolute ? "absolute" : "movie-chunk relative");
    }

    for (size_t i = 0; i < mTracks.size(); ++i) {
        Track *track = &mTracks.editItemAt(i);

        if (Track::AUDIO == track->mKind) {
            if (track->mBytesPerSample > 0) {
            // Assume all chunks are roughly the same size for now.

            // Compute the avg. size of the first 128 chunks (if there are
            // that many), but exclude the size of the first one, since
            // it may be an outlier.
            size_t numSamplesToAverage = track->mSamples.size();
            if (numSamplesToAverage > 256) {
                numSamplesToAverage = 256;
            }

            double avgChunkSize = 0;
            size_t j;
            for (j = 0; j < numSamplesToAverage; ++j) {
                off64_t offset;
                size_t size;
                bool isKey;
                int64_t dummy;

                status_t err = getSampleInfo(i, j, &offset, &size, &isKey, &dummy);
                if (err != OK) {
                    return err;
                }

                if (j == 0) {
                    track->mFirstChunkSize = size;
                    continue;
                }

                avgChunkSize += size;
            }

            if (numSamplesToAverage > 1) {
                avgChunkSize /= (numSamplesToAverage - 1);
            }

            track->mAvgChunkSize = avgChunkSize;
            }else{
                continue;
            }
        }

        int64_t durationUs;
        if (track->mSamples.size() < 1){
            return ERROR_MALFORMED;
        }


        status_t error = getSampleTime(i, track->mSamples.size() - 1, &durationUs);
        if (error != (status_t)OK){
            return error;
        }

        ALOGV("[%s] track %zu duration = %.2f secs", __FUNCTION__, i, durationUs / 1E6);
        AMediaFormat_setInt64(track->mMeta, AMEDIAFORMAT_KEY_DURATION, durationUs);
        AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, track->mMaxSampleSize);
    }

    mFoundIndex = true;

    return OK;
}

status_t AVIExtractor::parseIndx(off64_t offset, size_t size)
{
    ALOGV("AVIExtractor::parseIndx");

    if (size < 32)
        return ERROR_MALFORMED;

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    const uint8_t  *data                = buffer->data();
    Track          *track               = &mTracks.editItemAt(mTracks.size() - 1);
    uint32_t        sizePerIndexEntry   = U16LE_AT(&data[8]) * 4;
    uint8_t         indextype           = data[11];
    uint32_t        entriesInUse        = U32LE_AT(&data[12]);
    off64_t         baseoffset          = U64LE_AT(&data[20]);

    if (!sizePerIndexEntry)
        return ERROR_MALFORMED;

    uint32_t maxEntries = (size - 32)/sizePerIndexEntry;
    if (entriesInUse > maxEntries) {
        entriesInUse = maxEntries;
    }

    mOffsetsAreAbsolute = true;
    data += 32;

    if (indextype == AVI_INDEX_OF_CHUNKS) {
        double avgChunkSize = 0;

        for (size_t i=0; i<entriesInUse; i++) {
            track->mSamples.push();
            SampleInfo *info = &track->mSamples.editItemAt(track->mSamples.size() - 1);
            info->mOffset    = (uint32_t)baseoffset + U32LE_AT(data) - 8;

            size_t chunkSize = U32LE_AT(&data[4]);
            info->mIsKey     = ((int32_t)chunkSize) > 0;

            chunkSize = chunkSize & 0x7fffffff;
            if (chunkSize > track->mMaxSampleSize) {
                ALOGE("[%s] parse Indx track->maxSize:%zd, size:%zu", __FUNCTION__, track->mMaxSampleSize, chunkSize );
                track->mMaxSampleSize = chunkSize;
                AMediaFormat_setInt32(track->mMeta, AMEDIAFORMAT_KEY_MAX_INPUT_SIZE, track->mMaxSampleSize);
            }

            if (track->mSamples.size() > 1) {
                track->mLengthTotal += track->mPreChunkSize;
            } else {
                track->mLengthTotal = 0;
            }

            track->mPreChunkSize = chunkSize;
            info->mLengthTotal   = track->mLengthTotal;

            if (track->mBytesPerSample > 0)
                ALOGV("[%s] parseIndx, num=%zu, chunkSize=%zu, mLengthTotal=%d", __FUNCTION__, i, chunkSize, info->mLengthTotal);

            if (info->mIsKey) {
                static const size_t kMaxNumSyncSamplesToScan = 20;

                if (track->mNumSyncSamples < kMaxNumSyncSamplesToScan) {
                    if (chunkSize > track->mThumbnailSampleSize) {
                        track->mThumbnailSampleSize = chunkSize;

                        track->mThumbnailSampleIndex =
                        track->mSamples.size() - 1;
                    }
                }
                ++track->mNumSyncSamples;
            }

            data += sizePerIndexEntry;
            if (i == 0) {
                track->mFirstChunkSize = size;
                continue;
            }
            avgChunkSize += chunkSize;
        }

        if (entriesInUse > 1) {
            avgChunkSize /= entriesInUse;
        }
        track->mAvgChunkSize = avgChunkSize;
    } else if (indextype == AVI_INDEX_OF_INDEXES) {
        for(size_t i=0; i<entriesInUse; i++) {
            off64_t stdIndexOffset = U64LE_AT(data);
            uint32_t stdIndexSize  = U32LE_AT(&data[8]);
            status_t err = parseIndx(stdIndexOffset, stdIndexSize);

            if(err)
                return err;

            data += sizePerIndexEntry;
        }
    }

    return OK;
}

static size_t GetSizeWidth(size_t x)
{
    size_t n = 1;
    while (x > 127) {
        ++n;
        x >>= 7;
    }
    return n;
}

static uint8_t *EncodeSize(uint8_t *dst, size_t x)
{
    while (x > 127) {
        *dst++ = (x & 0x7f) | 0x80;
        x >>= 7;
    }

    *dst++ = x;

    return dst;
}

sp<ABuffer> MakeMPEG4VideoCodecSpecificData(const sp<ABuffer> &config)
{
    ALOGV("MakeMPEG4VideoCodecSpecificData");

    size_t len1 = config->size() + GetSizeWidth(config->size()) + 1;
    size_t len2 = len1 + GetSizeWidth(len1) + 1 + 13;
    size_t len3 = len2 + GetSizeWidth(len2) + 1 + 3;

    sp<ABuffer> csd = new ABuffer(len3);
    uint8_t *dst = csd->data();
    *dst++ = 0x03;
    dst = EncodeSize(dst, len2 + 3);
    *dst++ = 0x00;  // ES_ID
    *dst++ = 0x00;
    *dst++ = 0x00;  // streamDependenceFlag, URL_Flag, OCRstreamFlag

    *dst++ = 0x04;
    dst = EncodeSize(dst, len1 + 13);
    *dst++ = 0x01;  // Video ISO/IEC 14496-2 Simple Profile
    for (size_t i = 0; i < 12; ++i) {
        *dst++ = 0x00;
    }

    *dst++ = 0x05;
    dst = EncodeSize(dst, config->size());
    memcpy(dst, config->data(), config->size());
    dst += config->size();

    // hexdump(csd->data(), csd->size());

    return csd;
}

status_t AVIExtractor::addMPEG4CodecSpecificData(size_t trackIndex)
{
    ALOGV("addMPEG4CodecSpecificData");

    Track *track = &mTracks.editItemAt(trackIndex);

    off64_t offset;
    size_t size;
    bool isKey;
    int64_t timeUs;

    status_t err = getSampleInfo(trackIndex, 0, &offset, &size, &isKey, &timeUs);

    if (err != OK) {
        return err;
    }

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    // Extract everything up to the first VOP start code from the first
    // frame's encoded data and use it to construct an ESDS with the
    // codec specific data.

    size_t i   = 0;
    bool found = false;
    while (i + 3 < buffer->size()) {
        if (!memcmp("\x00\x00\x01\xb6", &buffer->data()[i], 4)) {
            found = true;
            break;
        }

        ++i;
    }

    if (!found) {
        return ERROR_MALFORMED;
    }

    buffer->setRange(0, i);

    sp<ABuffer> csd = MakeMPEG4VideoCodecSpecificData(buffer);
    AMediaFormat_setBuffer(track->mMeta, AMEDIAFORMAT_KEY_ESDS, csd->data(), csd->size());

    return OK;
}


status_t AVIExtractor::addH264CodecSpecificData(size_t trackIndex)
{
    ALOGV("addH264CodecSpecificData");

    Track *track = &mTracks.editItemAt(trackIndex);

    off64_t offset;
    size_t  size;
    bool    isKey;
    int64_t timeUs;

    // Extract codec specific data from the first non-empty sample.

    size_t sampleIndex = 0;
    for (;;) {
        status_t err = getSampleInfo(trackIndex, sampleIndex, &offset, &size, &isKey, &timeUs);
        if (err != OK) {
            return err;
        }

        if (size > 0) {
            break;
        }

        ++sampleIndex;
    }

    sp<ABuffer> buffer = new ABuffer(size);
    ssize_t n = mDataSource->readAt(offset, buffer->data(), buffer->size());

    if (n < (ssize_t)size) {
        return n < 0 ? (status_t)n : ERROR_MALFORMED;
    }

    if (!MakeAVCCodecSpecificData(track->mMeta, buffer->data(), buffer->size())) {
        ALOGE("[%s] Unable to extract AVC codec specific data", __FUNCTION__);
        return ERROR_MALFORMED;
    }

    return OK;
}

status_t AVIExtractor::getSampleInfo(
    size_t   trackIndex,
    size_t   sampleIndex,
    off64_t *offset,
    size_t  *size,
    bool    *isKey,
    int64_t *sampleTimeUs)
{
    ALOGV("AVIExtractor::getSampleInfo");

    if (trackIndex >= mTracks.size()) {
        return -ERANGE;
    }

    const Track &track = mTracks.itemAt(trackIndex);

    if (sampleIndex >= track.mSamples.size()) {
        return -ERANGE;
    }

    const SampleInfo &info = track.mSamples.itemAt(sampleIndex);

    if (!mOffsetsAreAbsolute) {
        *offset = info.mOffset + mMovieOffset + 8;
    } else {
        *offset = info.mOffset;
    }

    *size = 0;

    uint8_t tmp[8];
    ssize_t n = mDataSource->readAt(*offset, tmp, 8);

    if (n < 8) {
        return n < 0 ? (status_t)n : (status_t)ERROR_MALFORMED;
    }

    uint32_t chunkType = U32_AT(tmp);

    if (!IsCorrectChunkType(trackIndex, track.mKind, chunkType)) {
        return ERROR_MALFORMED;
    }

    *offset += 8;
    *size = U32LE_AT(&tmp[4]);

    *isKey = info.mIsKey;

    if (track.mBytesPerSample > 0) {
        size_t sampleStartInBytes;
        if (sampleIndex == 0) {
            sampleStartInBytes = 0;
        } else {
            sampleStartInBytes =
                track.mFirstChunkSize + track.mAvgChunkSize * (sampleIndex - 1);
        }

        sampleIndex = sampleStartInBytes / track.mBytesPerSample;
    }

    *sampleTimeUs = (sampleIndex * 1000000ll * track.mRate) / track.mScale;

    return OK;
}

status_t AVIExtractor::getSampleTime(
    size_t   trackIndex,
    size_t   sampleIndex,
    int64_t *sampleTimeUs)
{
    ALOGV("AVIExtractor::getSampleTime");

    off64_t offset;
    size_t  size;
    bool    isKey;

    return getSampleInfo(trackIndex, sampleIndex, &offset, &size, &isKey, sampleTimeUs);
}

status_t AVIExtractor::getSampleIndexAtTime(
    size_t  trackIndex,
    int64_t timeUs,
    int     mode,
    size_t *sampleIndex) const
{
    ALOGV("AVIExtractor::getSampleIndexAtTime");

    if (trackIndex >= mTracks.size()) {
        return -ERANGE;
    }

    const Track &track = mTracks.itemAt(trackIndex);

    ssize_t closestSampleIndex;

    if (track.mBytesPerSample > 0) {
        size_t closestByteOffset =
            (timeUs * track.mBytesPerSample)
                / track.mRate * track.mScale / 1000000ll;

        if (closestByteOffset <= track.mFirstChunkSize) {
            closestSampleIndex = 0;
        } else {
            closestSampleIndex =
                (closestByteOffset - track.mFirstChunkSize)
                    / track.mAvgChunkSize;
        }
    } else {
        // Each chunk contains a single sample.
        closestSampleIndex = timeUs / track.mRate * track.mScale / 1000000ll;
    }

    ssize_t numSamples = track.mSamples.size();

    if (closestSampleIndex < 0) {
        closestSampleIndex = 0;
    } else if (closestSampleIndex >= numSamples) {
        closestSampleIndex = numSamples - 1;
    }

    if (mode == MediaTrackHelper::ReadOptions::SeekMode::SEEK_CLOSEST) {
        *sampleIndex = closestSampleIndex;

        return OK;
    }

    ssize_t prevSyncSampleIndex = closestSampleIndex;
    while (prevSyncSampleIndex >= 0) {
        const SampleInfo &info =
            track.mSamples.itemAt(prevSyncSampleIndex);

        if (info.mIsKey) {
            break;
        }

        --prevSyncSampleIndex;
    }

    ssize_t nextSyncSampleIndex = closestSampleIndex;
    while (nextSyncSampleIndex < numSamples) {
        const SampleInfo &info =
            track.mSamples.itemAt(nextSyncSampleIndex);

        if (info.mIsKey) {
            break;
        }

        ++nextSyncSampleIndex;
    }

    switch (mode) {
    case MediaTrackHelper::ReadOptions::SEEK_PREVIOUS_SYNC:
    {
        *sampleIndex = prevSyncSampleIndex;

        return prevSyncSampleIndex >= 0 ? OK : UNKNOWN_ERROR;
    }
    case MediaTrackHelper::ReadOptions::SEEK_NEXT_SYNC:
    {
        *sampleIndex = nextSyncSampleIndex;

        return nextSyncSampleIndex < numSamples ? OK : UNKNOWN_ERROR;
    }
    case MediaTrackHelper::ReadOptions::SEEK_CLOSEST_SYNC:
    {
        if (prevSyncSampleIndex < 0 && nextSyncSampleIndex >= numSamples) {
            return UNKNOWN_ERROR;
        }

        if (prevSyncSampleIndex < 0) {
            *sampleIndex = nextSyncSampleIndex;
            return OK;
        }

        if (nextSyncSampleIndex >= numSamples) {
            *sampleIndex = prevSyncSampleIndex;
            return OK;
        }

        size_t dist1 = closestSampleIndex - prevSyncSampleIndex;
        size_t dist2 = nextSyncSampleIndex - closestSampleIndex;

        *sampleIndex = (dist1 < dist2) ? prevSyncSampleIndex : nextSyncSampleIndex;

        return OK;
    }
    default:
    {
        TRESPASS();
        break;
    }
    }
}

/////////////////////////////////////////////////////////

static CMediaExtractor *CreateExtractor(CDataSource *source, void *)
{
   ALOGV("CreateExtractor");

   return wrap(new AVIExtractor(new DataSourceHelper(source)));
}

static CreatorFunc Sniff(
    CDataSource  *source,
    float        *confidence,
    void         **,
    FreeMetaFunc *)
{
    ALOGV("CreatorFunc Sniff");
    char tmp[12];
    if (source->readAt(source->handle, 0, tmp, 12) < 12) {
        return 0;
    }

    if (!memcmp(tmp, "RIFF", 4) && !memcmp(&tmp[8], "AVI ", 4)) {
        // Just a tad over the mp3 extractor's confidence, since
        // these .avi files may contain .mp3 content that otherwise would
        // mistakenly lead to us identifying the entire file as a .mp3 file.
        *confidence = 0.21;
        return CreateExtractor;
    }

    return 0;
}

static const char *extensions[] = {
    "avi",
    "AVI",
    NULL
};

extern "C" {
// This is the only symbol that needs to be exported
__attribute__ ((visibility ("default")))
ExtractorDef GETEXTRACTORDEF() {
    return {
        EXTRACTORDEF_VERSION,
        UUID("7d613858-5837-4a38-84c5-332d1cddee30"),
        1, // version
        "AVI Extractor",
        { .v3 = {Sniff, extensions} },
    };
}

} // extern "C"

}  // namespace android

