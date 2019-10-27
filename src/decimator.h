/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Igor Zinken - https://www.igorski.nl
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
#ifndef __DECIMATOR_H_INCLUDED__
#define __DECIMATOR_H_INCLUDED__

#include "audiobuffer.h"

namespace Igorski {
class Decimator {

    public:

        Decimator( int bits, float rate );
        ~Decimator();

        // the output resolution, value between 1 - 32
        int getBits();
        void setBits( int value );

        // decimator has an internal oscillator
        // as the effect is applied at the peak of the cycle
        // the range is 0 - 1 where 1 implies the original sample rate
        float getRate();
        void setRate( float value );

        void process( float* sampleBuffer, int bufferSize );

        // store/restore the processor properties
        // this ensures that multi channel processing for a
        // single buffer uses all properties across all channels

        void store();
        void restore();

    private:
        int _bits;
        long _m;
        float _rate;
        float _accumulator;
        float _accumulatorStored;
};
}

#endif
