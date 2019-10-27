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
#include "decimator.h"
#include "calc.h"
#include <math.h>

namespace Igorski {

/* constructor / destructor */

Decimator::Decimator( int bits, float rate )
{
    setBits( bits );
    setRate( rate );

    _accumulator = 0.0;
}

Decimator::~Decimator()
{
    // nowt...
}

/* getters / setters */

int Decimator::getBits()
{
    return _bits;
}

void Decimator::setBits( int value )
{
    // cap in 1 - 32 range
    _bits = std::min( 32, std::max( 1, value ));
    _m    = 1 << ( _bits - 1 );
}

float Decimator::getRate()
{
    return _rate;
}

void Decimator::setRate( float value )
{
    _rate = Calc::cap( value );
}

void Decimator::store()
{
    _accumulatorStored = _accumulator;
}

void Decimator::restore()
{
    _accumulator = _accumulatorStored;
}

/* public methods */

void Decimator::process( float* sampleBuffer, int bufferSize )
{
    float sample;
    bool doProcess = ( _bits < 32 );

    for ( int i = 0; i < bufferSize; ++i )
    {
        sample = sampleBuffer[ i ];
        _accumulator += _rate;

        if ( _accumulator >= 1.f )
        {
            _accumulator -= 1.f;

            if ( doProcess )
                sample = ( float ) _m * floor( sample / ( float ) _m + 0.5f );
        }
        sampleBuffer[ i ] = sample;
    }
}

}
