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
template <typename SampleType>
void Limiter::process( SampleType** outputBuffer, int bufferSize, int numOutChannels )
{
//    if ( gain > 0.9999f && outputBuffer->isSilent )
//    {
//        // don't process if input is silent
//        return;
//    }

    SampleType g, at, re, tr, th, lev, ol, or_;

    th = thresh;
    g = gain;
    at = att;
    re = rel;
    tr = trim;

    bool hasRight = ( numOutChannels > 1 );

    SampleType* leftBuffer  = outputBuffer[ 0 ];
    SampleType* rightBuffer = hasRight ? outputBuffer[ 1 ] : 0;

    if ( pKnee > 0.5 )
    {
        // soft knee

        for ( int i = 0; i < bufferSize; ++i ) {

            ol  = leftBuffer[ i ];
            or_ = hasRight ? rightBuffer[ i ] : 0;

            lev = ( SampleType ) ( 1.f / ( 1.f + th * fabs( ol + or_ )));

            if ( g > lev ) {
                g = g - at * ( g - lev );
            }
            else {
                g = g + re * ( lev - g );
            }

            leftBuffer[ i ] = ( ol * tr * g );

            if ( hasRight )
                rightBuffer[ i ] = ( or_ * tr * g );
        }
    }
    else
    {
        for ( int i = 0; i < bufferSize; ++i ) {

            ol  = leftBuffer[ i ];
            or_ = hasRight ? rightBuffer[ i ] : 0;

            lev = ( SampleType ) ( 0.5 * g * fabs( ol + or_ ));

            if ( lev > th ) {
                g = g - ( at * ( lev - th ));
            }
            else {
                // below threshold
                g = g + ( SampleType )( re * ( 1.f - g ));
            }

            leftBuffer[ i ] = ( ol * tr * g );

            if ( hasRight )
                rightBuffer[ i ] = ( or_ * tr * g );
        }
    }
    gain = g;
}
