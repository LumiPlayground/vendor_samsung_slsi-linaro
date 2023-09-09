/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import java.util.Arrays;

import static com.shannon.dataservice.DnsResolver.DNS_HEADER_LENGTH;
import static com.shannon.dataservice.DnsResolver.DNS_QUERY_LENGTH;

public class DnsResponse {
    private static final int DNS_COMPRESSED = 0xc0;

    private final byte[] mData;
    private final int mId;
    private final int[] mQuestions;
    private int mNextQuestion;
    private final int[] mAnswers;
    private int mNextAnswer;

    DnsResponse(byte[] input) {
        mData = input;
        mNextQuestion = mNextAnswer = 0;

        mId = StringUtils.getShortAsInt(input, 0);
        int questions = StringUtils.getShortAsInt(input, 4);
        int answers = StringUtils.getShortAsInt(input ,6);
        answers += StringUtils.getShortAsInt(input ,8);
        answers += StringUtils.getShortAsInt(input ,10);

        mQuestions = new int[questions];
        if (questions > 0) {
            mQuestions[0] = DNS_HEADER_LENGTH;
            for (int i = 1; i < questions; i++) {
                mQuestions[i] = moveToNextQuestion(mQuestions[i - 1]);
            }
        }
        mAnswers = new int[answers];
        if (answers > 0) {
            if (questions > 0) {
                mAnswers[0] = moveToNextQuestion(mQuestions[questions - 1]);
            } else {
                mAnswers[0] = DNS_HEADER_LENGTH;
            }
            for (int i = 1; i < answers; i++) {
                mAnswers[i] = moveToNextAnswer(mAnswers[i - 1]);
            }
        }
    }

    private int moveToNextQuestion(int offset) {
        /* goto domain end */
        while (mData[offset] != 0) {
            if ((mData[offset] & DNS_COMPRESSED) != 0) {
                return (offset + 2 + DNS_QUERY_LENGTH);
            } else {
                offset += (StringUtils.getByteAsInt(mData, offset) + 1);
            }
        }
        return (offset + 1 + DNS_QUERY_LENGTH);
    }

    private int moveToNextAnswer(int offset) {
        int inc = 1;

        /* goto domain end */
        while (mData[offset] != 0) {
            if ((mData[offset] & DNS_COMPRESSED) != 0) {
                offset += 2; inc = 0; break;
            } else {
                offset += (StringUtils.getByteAsInt(mData, offset) + 1);
            }
        }
        offset += (8 + inc);

        int length = StringUtils.getShortAsInt(mData, offset);
        return (offset + 2 + length);
    }

    public byte[] getNextAnswer() {
        if (mNextAnswer < mAnswers.length) {
            int start = mAnswers[mNextAnswer++];
            int end = (mNextAnswer < mAnswers.length) ? mAnswers[mNextAnswer] : mData.length;
            return Arrays.copyOfRange(mData, start, end);
        }
        return null;
    }

    public String getDomainName(byte[] input, int offset) {
        StringBuilder sb = new StringBuilder(512);

        try {
            int length = StringUtils.getByteAsInt(input, offset);
            while (length != 0) {
                if ((length & DNS_COMPRESSED) != 0) {
                    int compOffset = (((StringUtils.getByteAsInt(input, offset) & ~DNS_COMPRESSED) << 8) |
                            StringUtils.getByteAsInt(input, offset + 1)) & 0xFFFF;
                    sb.append(getDomainName(mData, compOffset)); break;
                } else {
                    for (int i = 0; i < length; i++) {
                        sb.append((char)input[++offset]);
                    }
                    length = StringUtils.getByteAsInt(input, ++offset);
                    if (length != 0) {
                        sb.append('.');
                    }
                }
            }
        } catch (Exception e) {
            return "X";
        }
        return sb.toString();
    }

    public int getTypeOffset(byte[] input, int offset) {
        int length = StringUtils.getByteAsInt(input, offset);
        while (length != 0) {
            if ((length & DNS_COMPRESSED) != 0) {
                return (offset + 2);
            } else {
                offset += (length + 1);
            }
            length = StringUtils.getByteAsInt(input, offset);
        }
        return (offset + 1);
    }

    public boolean isExpected(int id) {
        return (mId == id);
    }
}
