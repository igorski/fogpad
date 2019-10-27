/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Igor Zinken - https://www.igorski.nl
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
#ifndef __REVERBPROCESS__H_INCLUDED__
#define __REVERBPROCESS__H_INCLUDED__

#include "global.h"
#include "audiobuffer.h"
#include "comb.h"
#include "allpass.h"
#include "bitcrusher.h"
#include "decimator.h"
#include "filter.h"
#include "limiter.h"
#include <vector>

using namespace Steinberg;

namespace Igorski {
class ReverbProcess {

    struct combFilters {
        std::vector<Comb*> filters;
        std::vector<float*> buffers;

        ~combFilters() {
            while ( !filters.empty() ) {
                delete filters.at( 0 );
                filters.erase( filters.begin() );
            }
            while ( !buffers.empty() ) {
                delete buffers.at( 0 );
                buffers.erase( buffers.begin() );
            }
        }
    };

    struct allpassFilters {
        std::vector<AllPass*> filters;
        std::vector<float*> buffers;

        ~allpassFilters() {
            while ( !filters.empty() ) {
                delete filters.at( 0 );
                filters.erase( filters.begin() );
            }
            while ( !buffers.empty() ) {
                delete buffers.at( 0 );
                buffers.erase( buffers.begin() );
            }
        }
    };

    static constexpr float MAX_RECORD_TIME_MS = 5000.f;
    static constexpr float MUTED              = 0;
    static constexpr float FIXED_GAIN         = 0.015f;
    static constexpr float SCALE_WET          = 1.f;
    static constexpr float SCALE_DRY          = 1.f;
    static constexpr float SCALE_DAMP         = 0.4f;
    static constexpr float SCALE_ROOM         = 0.28f;
    static constexpr float OFFSET_ROOM        = 0.7f;
    static constexpr float INITIAL_ROOM       = 0.5f;
    static constexpr float INITIAL_DAMP       = 0.5f;
    static constexpr float INITIAL_WET        = 1 / SCALE_WET;
    static constexpr float INITIAL_DRY        = 0.5;
    static constexpr float INITIAL_WIDTH      = 1;
    static constexpr float INITIAL_MODE       = 0;
    static constexpr float FREEZE_MODE        = 0.5f;
    static constexpr int STEREO_SPREAD        = 23;

    // we allow only a slowdown and speed up of 100 pct

    static constexpr float MIN_PLAYBACK_RATE = 0.5f;
    static constexpr float MAX_PLAYBACK_RATE = 1.5f;

    public:
        ReverbProcess( int amountOfChannels );
        ~ReverbProcess();

        // apply effect to incoming sampleBuffer contents

        template <typename SampleType>
        void process( SampleType** inBuffer, SampleType** outBuffer, int numInChannels, int numOutChannels,
            int bufferSize, uint32 sampleFramesSize
        );

        void mute();
        void setRoomSize( float value );
        float getRoomSize();
        void setDamp( float value );
        float getDamp();
        void setWet( float value );
        float getWet();
        void setDry( float value );
        float getDry();
        float getWidth();
        void setWidth( float value );
        float getMode();
        void setMode( float value );
        float getPlaybackRate();
        void setPlaybackRate( float value );

        BitCrusher* bitCrusher;
        Decimator* decimator;
        Filter* filter;
        Limiter* limiter;

        // whether effects are applied onto the input delay signal or onto
        // the delayed signal itself (false = on input, true = on delay)

        bool bitCrusherPostMix;

    private:
        AudioBuffer* _recordBuffer;  // contains the sample memory for drift mode
        AudioBuffer* _preMixBuffer;  // buffer used for the pre-delay effect mixing
        AudioBuffer* _postMixBuffer; // buffer used for the post-delay effect mixing
        int  _amountOfChannels;
        int  _maxRecordIndex;
        int* _recordIndices;

        void setupFilters();         // generates comb and allpass filter buffers
        void clearFilters();         // frees memory allocated to comb and allpass filter buffers
        void update();

        float _playbackRate;
        float _playbackReadIndex;

        float _gain;
        float _roomSize, _roomSize1;
        float _damp, _damp1;
        float _wet, _wet1, _wet2;
        float _dry;
        float _width;
        float _mode;

        std::vector<combFilters*>    _combFilters;
        std::vector<allpassFilters*> _allpassFilters;

        // ensures the pre- and post mix buffers match the appropriate amount of channels
        // and buffer size. this also clones the contents of given in buffer into the pre-mix buffer
        // the buffers are pooled so this can be called upon each process cycle without allocation overhead

        template <typename SampleType>
        void prepareMixBuffers( SampleType** inBuffer, int numInChannels, int bufferSize );

};
}

#include "reverbprocess.tcc"

#endif
