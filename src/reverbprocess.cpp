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
#include "reverbprocess.h"
#include "calc.h"
#include <math.h>

namespace Igorski {

ReverbProcess::ReverbProcess( int amountOfChannels ) {
    _amountOfChannels = amountOfChannels;

    _maxRecordIndex = Calc::millisecondsToBuffer( MAX_RECORD_TIME_MS );
    _recordBuffer   = new AudioBuffer( amountOfChannels, _maxRecordIndex );
    _recordIndices  = new int[ amountOfChannels ];
    for ( int i = 0; i < amountOfChannels; ++i ) {
        _recordIndices[ i ] = 0;
    }
    _playbackReadIndex = 0.f;

    bitCrusher = new BitCrusher( 8, .5f, .5f );
    decimator  = new Decimator( 32, 0.f );
    filter     = new Filter();
    limiter    = new Limiter( 10.f, 500.f, .6f );

    setupFilters();

    setWet     ( INITIAL_WET );
    setRoomSize( INITIAL_ROOM );
    setDry     ( INITIAL_DRY );
    setDamp    ( INITIAL_DAMP );
    setWidth   ( INITIAL_WIDTH );
    setMode    ( INITIAL_MODE );

    // this will initialize the buffers with silence
    mute();

    // will be lazily created in the process function
    _preMixBuffer  = nullptr;
    _postMixBuffer = nullptr;
    _playbackRate  = 1.f;
}

ReverbProcess::~ReverbProcess() {
    delete _recordIndices;
    delete _recordBuffer;
    delete _postMixBuffer;
    delete _preMixBuffer;
    delete bitCrusher;
    delete decimator;
    delete filter;
    delete limiter;
    clearFilters();
}

void ReverbProcess::mute()
{
    if ( getMode() >= FREEZE_MODE )
        return;

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        auto combData = _combFilters.at( c );
        for ( int i = 0; i < VST::NUM_COMBS; i++ ) {
            combData->filters.at( i )->mute();
        }

        auto allPassData = _allpassFilters.at( c );
        for ( int i = 0; i < VST::NUM_ALLPASSES; i++ ) {
            allPassData->filters.at( i )->mute();
        }
    }
}

float ReverbProcess::getRoomSize()
{
    return ( _roomSize - OFFSET_ROOM ) / SCALE_ROOM;
}

void ReverbProcess::setRoomSize( float value )
{
    _roomSize = ( value * SCALE_ROOM ) + OFFSET_ROOM;
    update();
}

float ReverbProcess::getDamp()
{
    return _damp / SCALE_DAMP;
}

void ReverbProcess::setDamp( float value )
{
    _damp = value * SCALE_DAMP;
    update();
}

float ReverbProcess::getWet()
{
    return _wet / SCALE_WET;
}

void ReverbProcess::setWet( float value )
{
    _wet = value * SCALE_WET;
    update();
}

float ReverbProcess::getDry()
{
    return _dry / SCALE_DRY;
}

void ReverbProcess::setDry( float value )
{
    _dry = value * SCALE_DRY;
}

float ReverbProcess::getWidth()
{
    return _width;
}

void ReverbProcess::setWidth( float value )
{
    _width = value;
    update();
}

float ReverbProcess::getPlaybackRate()
{
    return Calc::scale( _playbackRate - MIN_PLAYBACK_RATE, 1.0f, 1.0f );
}

void ReverbProcess::setPlaybackRate( float value )
{
    _playbackRate = MIN_PLAYBACK_RATE + Calc::scale(value, 1.0f, 1.0f );
}

float ReverbProcess::getMode()
{
    return ( _mode >= FREEZE_MODE ) ? 1 : 0;
}

void ReverbProcess::setMode( float value )
{
    _mode = value;
    update();
}

void ReverbProcess::setupFilters()
{
    clearFilters();

    // create filters and buffers per output channel

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        combFilters* combData = new combFilters();
        _combFilters.push_back( combData );

        // comb filters

        for ( int i = 0; i < VST::NUM_COMBS; ++i ) {
            // tune the comb to the host environments sample rate
            int tuning = ( int ) ((( float ) VST::COMB_TUNINGS[ i ] / 44100.f ) * VST::SAMPLE_RATE );
            int size = tuning + ( c * STEREO_SPREAD );
            float* buffer = new float[ size ];

            Comb* comb = new Comb();
            comb->setBuffer( buffer, size );
            combData->filters.push_back( comb );
            combData->buffers.push_back( buffer );
        }

        // all pass filters

        allpassFilters* allpassData = new allpassFilters();
        _allpassFilters.push_back( allpassData );

        for ( int i = 0; i < VST::NUM_ALLPASSES; ++i ) {
            // tune the comb to the host environments sample rate
            int tuning = ( int ) ((( float ) VST::ALLPASS_TUNINGS[ i ] / 44100.f ) * VST::SAMPLE_RATE );
            int size = tuning + ( c * STEREO_SPREAD );
            float* buffer = new float[ size ];

            AllPass* allPass = new AllPass();
            allPass->setBuffer( buffer, size );
            allpassData->filters.push_back( allPass );
            allpassData->buffers.push_back( buffer );
        }
    }
}

void ReverbProcess::clearFilters()
{
    while ( !_combFilters.empty() ) {
        delete _combFilters.at( 0 );
        _combFilters.erase( _combFilters.begin() );
    }
    while ( !_allpassFilters.empty() ) {
        delete _allpassFilters.at( 0 );
        _allpassFilters.erase( _allpassFilters.begin() );
    }
}

void ReverbProcess::update()
{
    // Recalculate internal values after parameter change

    _wet1 = _wet * ( _width / 2 + 0.5f );
    _wet2 = _wet * (( 1 - _width ) / 2 );

    if ( _mode >= FREEZE_MODE ){
        _roomSize1 = 1;
        _damp1     = 0;
        _gain      = MUTED;
    }
    else {
        _roomSize1 = _roomSize;
        _damp1     = _damp;
        _gain      = FIXED_GAIN;
    }

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        auto combData = _combFilters.at( c );
        for ( int i = 0; i < VST::NUM_COMBS; i++ ) {
            combData->filters.at( i )->setFeedback( _roomSize1 );
            combData->filters.at( i )->setDamp( _damp1 );
        }
    }
}

}
