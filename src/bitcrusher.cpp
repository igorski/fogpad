/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Igor Zinken - https://www.igorski.nl
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
#include "bitcrusher.h"
#include "global.h"
#include "calc.h"
#include <limits.h>
#include <math.h>

namespace Igorski {

/* constructor */

BitCrusher::BitCrusher( float amount, float inputMix, float outputMix )
{
    setAmount   ( amount );
    setInputMix ( inputMix );
    setOutputMix( outputMix );

    _tempAmount = _amount;

    lfo = new LFO();
    hasLFO = false;
}

BitCrusher::~BitCrusher()
{
    delete lfo;
}

/* public methods */

void BitCrusher::setLFO( float LFORatePercentage, float LFODepth )
{
    bool wasEnabled = hasLFO;
    bool enabled    = LFORatePercentage > 0.f;

    hasLFO = enabled;

    bool hadChange = ( wasEnabled != enabled ) || _lfoDepth != LFODepth;

    if ( enabled )
        lfo->setRate(
            VST::MIN_LFO_RATE() + (
                LFORatePercentage * ( VST::MAX_LFO_RATE() - VST::MIN_LFO_RATE() )
            )
        );

    // turning LFO off
    if ( !hasLFO && wasEnabled ) {
        _tempAmount = _amount;
        calcBits();
    }

    if ( hadChange ) {
        _lfoDepth = LFODepth;
        cacheLFO();
    }
}

void BitCrusher::process( float* inBuffer, int bufferSize )
{
    // sound should not be crushed ? do nothing
    if ( _bits == 16 && !hasLFO )
        return;

    int bitsPlusOne = _bits + 1;

    for ( int i = 0; i < bufferSize; ++i )
    {
        short input = ( short ) (( inBuffer[ i ] * _inputMix ) * SHRT_MAX );
        short prevent_offset = ( short )( -1 >> bitsPlusOne );
        input &= ( -1 << ( 16 - _bits ));
        inBuffer[ i ] = (( input + prevent_offset ) * _outputMix ) / SHRT_MAX;

        if ( hasLFO ) {
            // multiply by .5 and add .5 to make the LFO's bipolar waveform unipolar
            float lfoValue = lfo->peek() * .5f  + .5f;
            _tempAmount = std::min( _lfoMax, _lfoMin + _lfoRange * lfoValue );

            // recalculate the current resolution
            calcBits();
            bitsPlusOne = _bits + 1;
        }
    }
}

/* setters */

void BitCrusher::setAmount( float value )
{
    // invert the range 0 == max bits (no distortion), 1 == min bits (severely distorted)
    float scaledValue = abs(value - 1.f);

    float tempRatio = _tempAmount / std::max( 0.000000001f, _amount );

    _amount = scaledValue;

    // in case BitCrusher is attached to oscillator, keep relative offset
    // of currently moving bit resolution in place
    _tempAmount = ( hasLFO ) ? _amount * tempRatio : _amount;

    cacheLFO();
    calcBits();
}

void BitCrusher::setInputMix( float value )
{
    _inputMix = Calc::cap( value );
}

void BitCrusher::setOutputMix( float value )
{
    _outputMix = Calc::cap( value );
}

/* private methods */

void BitCrusher::cacheLFO()
{
    _lfoRange = ( float ) _amount * _lfoDepth;
    _lfoMax   = std::min( 1.f, ( float ) _amount + _lfoRange / 2.f );
    _lfoMin   = std::max( 0.f, ( float ) _amount - _lfoRange / 2.f );
}

void BitCrusher::calcBits()
{
    // scale float to 1 - 16 bit range
    _bits = ( int ) floor( Calc::scale( _tempAmount, 1, 15 )) + 1;
}

}
