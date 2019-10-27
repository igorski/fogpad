/**
 * Ported from mdaLimiterProcessor.cpp
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
#include "limiter.h"
#include "global.h"
#include <math.h>

// constructors / destructor

Limiter::Limiter()
{
    init( 0.15, 0.50, 0.60 );
}

Limiter::Limiter( float attackMs, float releaseMs, float thresholdDb )
{
    init( attackMs, releaseMs, thresholdDb );
}

Limiter::~Limiter()
{
    // nowt...
}

/* public methods */

void Limiter::setAttack( float attackMs )
{
    pAttack = ( float ) attackMs;
    recalculate();
}

void Limiter::setRelease( float releaseMs )
{
    pRelease = ( float ) releaseMs;
    recalculate();
}

void Limiter::setThreshold( float thresholdDb )
{
    pTresh = ( float ) thresholdDb;
    recalculate();
}

float Limiter::getLinearGR()
{
    return gain > 1.f ? 1.f / gain : 1.f;
}

/* protected methods */

void Limiter::init( float attackMs, float releaseMs, float thresholdDb )
{
    pAttack  = ( float ) attackMs;
    pRelease = ( float ) releaseMs;
    pTresh   = ( float ) thresholdDb;
    pTrim    = ( float ) 0.60;
    pKnee    = ( float ) 0.40;

    gain = 1.f;

    recalculate();
}

void Limiter::recalculate()
{
    if ( pKnee > 0.5 ) {
        // soft knee
        thresh = ( float ) pow( 10.0, 1.f - ( 2.0 * pTresh ));
    }
    else {
        // hard knee
        thresh = ( float ) pow( 10.0, ( 2.0 * pTresh ) - 2.0 );
    }
    trim = ( float )( pow( 10.0, ( 2.0 * pTrim) - 1.f ));
    att  = ( float )  pow( 10.0, -2.0 * pAttack );
    rel  = ( float )  pow( 10.0, -2.0 - ( 3.0 * pRelease ));
}
