/**
 * The MIT License (MIT)
 *
 * Based on freeverb by Jezar at Dreampoint (June 2000)
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
#ifndef __ALLPASS_H_INCLUDED__
#define __ALLPASS_H_INCLUDED__

#include "global.h"
#include "calc.h"

using namespace Steinberg;

namespace Igorski {
class AllPass
{
    public:
        AllPass();
        void setBuffer( float *buf, int size );
        inline float process( float input )
        {
            float output;
            float bufout = _buffer[ _bufIndex ];
            undenormalise( bufout );

            output = -input + bufout;
            _buffer[ _bufIndex ] = input + ( bufout * _feedback );

            if ( ++_bufIndex >= _bufSize ) {
                _bufIndex = 0;
            }
            return output;
        }
        void mute();
        float getFeedback();
        void setFeedback( float val );

    private:
        float  _feedback;
        float* _buffer;
        int _bufSize;
        int _bufIndex;
};
}
#endif
