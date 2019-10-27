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
#ifndef __FILTER_H_INCLUDED__
#define __FILTER_H_INCLUDED__

#include "global.h"
#include "lfo.h"
#include <math.h>

namespace Igorski {
class Filter {

    public:
        Filter();
        ~Filter();

        void  setCutoff( float frequency );
        float getCutoff();
        void  setResonance( float resonance );
        float getResonance();
        void setDepth( float depth );
        float getDepth();
        void setLFO( bool enabled );

        void calculateParameters();

        // update Filter properties, the values here are in normalized 0 - 1 range
        void updateProperties( float cutoffPercentage, float resonancePercentage, float LFORatePercentage, float fLFODepth );

        // apply filter to incoming sampleBuffer contents
        void process( float* sampleBuffer, int bufferSize, int c );

        LFO* lfo;

        // store/restore the processor properties
        // this ensures that multi channel processing for a
        // single buffer uses all properties across all channels

        void store();
        void restore();

    private:
        float _cutoff;
        float _tempCutoff;
        float _resonance;
        float _depth;
        float _lfoMin;
        float _lfoMax;
        float _lfoRange;
        bool  _hasLFO;

        // used internally

        float _accumulatorStored;
        float _tempCutoffStored;

        float _a1;
        float _a2;
        float _a3;
        float _b1;
        float _b2;
        float _c;

        float* _in1;
        float* _in2;
        float* _out1;
        float* _out2;

        void cacheLFOProperties();
};
}

#endif
