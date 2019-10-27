/**
 * Ported from mdaLimiterProcessor.h
 * Created by Arne Scheffler on 6/14/08.
 *
 * mda VST Plug-ins
 *
 * Copyright (c) 2008 Paul Kellett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __LIMITER_H_INCLUDED__
#define __LIMITER_H_INCLUDED__

#include "audiobuffer.h"

class Limiter
{
    public:
        Limiter();
        Limiter( float attackMs, float releaseMs, float thresholdDb );
        ~Limiter();

        template <typename SampleType>
        void process( SampleType** outputBuffer, int bufferSize, int numOutChannels );

        void setAttack( float attackMs );
        void setRelease( float releaseMs );
        void setThreshold( float thresholdDb );

        float getLinearGR();

    protected:
        void init( float attackMs, float releaseMs, float thresholdDb );
        void recalculate();

        float pTresh;   // in dB, -20 - 20
        float pTrim;
        float pAttack;  // in microseconds
        float pRelease; // in ms
        float pKnee;

        float thresh, gain, att, rel, trim;
};

#include "limiter.tcc"

#endif
