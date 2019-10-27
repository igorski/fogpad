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
#ifndef __BITCRUSHER_H_INCLUDED__
#define __BITCRUSHER_H_INCLUDED__

#include "lfo.h"

namespace Igorski {
class BitCrusher {

    public:
        BitCrusher( float amount, float inputMix, float outputMix );
        ~BitCrusher();

        void setLFO( float LFORatePercentage, float LFODepth );
        void process( float* inBuffer, int bufferSize );

        void setAmount( float value ); // range between -1 to +1
        void setInputMix( float value );
        void setOutputMix( float value );

        LFO* lfo;
        bool hasLFO;

    private:
        int _bits; // we scale the amount to integers in the 1-16 range
        float _amount;
        float _inputMix;
        float _outputMix;

        void cacheLFO();
        void calcBits();
        float _tempAmount;
        float _lfoDepth;
        float _lfoRange;
        float _lfoMax;
        float _lfoMin;
};
}

#endif
