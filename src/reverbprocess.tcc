/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Igor Zinken - https://www.igorski.nl
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
namespace Igorski
{
template <typename SampleType>
void ReverbProcess::process( SampleType** inBuffer, SampleType** outBuffer, int numInChannels, int numOutChannels,
                             int bufferSize, uint32 sampleFramesSize ) {

    // input and output buffers can be float or double as defined
    // by the templates SampleType value. Internally we process
    // audio as floats

    SampleType inSample;
    float frac, s1, s2;
    int i, t, t2, readIndex, recordIndex, channelDelayBufferChannel;
    bool hasDrift = ( _playbackRate != 1.0f );
    float orgPlaybackReadIndex = _playbackReadIndex;

    int maxRecordIndex = _recordBuffer->bufferSize;

    // prepare the mix buffers and clone the incoming buffer contents into the pre-mix buffer

    prepareMixBuffers( inBuffer, numInChannels, bufferSize );

    for ( int32 c = 0; c < numInChannels; ++c )
    {
        SampleType* channelInBuffer  = inBuffer[ c ];
        SampleType* channelOutBuffer = outBuffer[ c ];
        float* channelRecordBuffer   = _recordBuffer->getBufferForChannel( c );
        float* channelPreMixBuffer   = _preMixBuffer->getBufferForChannel( c );
        float* channelPostMixBuffer  = _postMixBuffer->getBufferForChannel( c );

        orgPlaybackReadIndex = _playbackReadIndex;
        recordIndex          = _recordIndices[ c ];

        // when processing the first channel, store the current effects properties
        // so each subsequent channel is processed using the same processor variables

        if ( c == 0 ) {
            decimator->store();
            filter->store();
        }

        // PRE MIX processing

        if ( !bitCrusherPostMix )
            bitCrusher->process( channelPreMixBuffer, bufferSize );

        if ( !decimatorPostMix )
            decimator->process( channelPreMixBuffer, bufferSize );

        if ( !filterPostMix )
            filter->process( channelPreMixBuffer, bufferSize, c );

        // REVERB processing applied onto the temp buffer

        SampleType inputSample, processedSample;
        combFilters* combs        = _combFilters.at( c );
        allpassFilters* allpasses = _allpassFilters.at( c );

        for ( i = 0; i < bufferSize; ++i )
        {
            // in case the process is running in drift mode, read sample
            // from the pre-recorded buffer so we can vary playback speeds
            if ( hasDrift ) {
                t    = ( int ) _playbackReadIndex;
                t2   = t + 1;
                frac = _playbackReadIndex - t;

                s1 = channelRecordBuffer[ t ];
                s2 = channelRecordBuffer[ t2 < maxRecordIndex ? t2 : t ];

                inputSample = s1 + ( s2 - s1 ) * frac;

                if (( _playbackReadIndex += _playbackRate ) >= maxRecordIndex ) {
                    _playbackReadIndex = 0.f;
                }
            }
            else {
                // np drift enabled, take sample directly from the input buffer
                inputSample = channelInBuffer[ i ];
            }
            // record the sample into the buffer so we can read it later when in drift mode
            channelRecordBuffer[ recordIndex ] = channelPreMixBuffer[ i ] + inputSample;

            if ( ++recordIndex >= maxRecordIndex ) {
                recordIndex = 0;
            }

            // ---- REVERB process

            processedSample = 0;
            inputSample *= _gain;

            // Accumulate comb filters in parallel
            for ( int i = 0; i < NUM_COMBS; i++ ) {
                processedSample += combs->filters.at( i )->process( inputSample );
            }

            // Feed through allPasses in series
            for ( int i = 0; i < NUM_ALLPASSES; i++ ) {
                processedSample = allpasses->filters.at( i )->process( processedSample );
            }

            // write the reverberated sample into the post mix buffer
            channelPostMixBuffer[ i ] = processedSample/* * _wet1 + ( input * _dry ) */;
        }

        // update last recording index for this channel

        _recordIndices[ c ] = recordIndex;

        // POST MIX processing
        // apply the post mix effect processing

        if ( decimatorPostMix )
            decimator->process( channelPostMixBuffer, bufferSize );

        if ( bitCrusherPostMix )
            bitCrusher->process( channelPostMixBuffer, bufferSize );

        if ( filterPostMix )
            filter->process( channelPostMixBuffer, bufferSize, c );

        // mix the input and processed post mix buffers into the output buffer

        for ( i = 0; i < bufferSize; ++i ) {

            // before writing to the out buffer we take a snapshot of the current in sample
            // value as VST2 in Ableton Live supplies the same buffer for in and out!
            inSample = channelInBuffer[ i ];

            // wet mix (e.g. the effected signal)
            channelOutBuffer[ i ] = ( SampleType ) channelPostMixBuffer[ i ] * _wet1;

            // dry mix (e.g. mix in the input signal)
            channelOutBuffer[ i ] += ( inSample * _dry );
        }

        // prepare effects for the next channel

        if ( c < ( numInChannels - 1 )) {
            decimator->restore();
            filter->restore();
        }
    }

    // limit the output signal as it can get quite hot
    limiter->process<SampleType>( outBuffer, bufferSize, numOutChannels );
}

template <typename SampleType>
void ReverbProcess::prepareMixBuffers( SampleType** inBuffer, int numInChannels, int bufferSize )
{
    // if the pre mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _preMixBuffer == 0 || _preMixBuffer->bufferSize != bufferSize ) {
        delete _preMixBuffer;
        _preMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }

    // clone the in buffer contents
    // note the clone is always cast to float as it is
    // used for internal processing (see ReverbProcess::process)

    for ( int c = 0; c < numInChannels; ++c ) {
        SampleType* inChannelBuffer = ( SampleType* ) inBuffer[ c ];
        float* outChannelBuffer     = ( float* ) _preMixBuffer->getBufferForChannel( c );

        for ( int i = 0; i < bufferSize; ++i ) {
            outChannelBuffer[ i ] = ( float ) inChannelBuffer[ i ];
        }
    }

    // if the post mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _postMixBuffer == 0 || _postMixBuffer->bufferSize != bufferSize ) {
        delete _postMixBuffer;
        _postMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }
}

}
