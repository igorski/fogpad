/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2023 Igor Zinken - https://www.igorski.nl
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
#include "global.h"
#include "vst.h"
#include "paramids.h"
#include "calc.h"

#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/vstpresetkeys.h"

#include "base/source/fstreamer.h"

#include <stdio.h>

namespace Igorski {

float VST::SAMPLE_RATE = 44100.f; // updated in setupProcessing()

//------------------------------------------------------------------------
// FogPad Implementation
//------------------------------------------------------------------------
FogPad::FogPad()
: fReverbSize( 0.5f )
, fReverbWidth( 1.f )
, fReverbDryMix( 0.5f )
, fReverbWetMix( 0.5f )
, fReverbFreeze( 0.f )
, fReverbPlaybackRate( 0.5f )
, fBitResolution( 0.f )
, fBitResolutionChain( 0.f )
, fLFOBitResolution( 0.f )
, fLFOBitResolutionDepth( 0.5f )
, fDecimator( 0.f )
, fFilterCutoff( 0.5f )
, fFilterResonance( 1.f )
, fLFOFilter( 0.f )
, fLFOFilterDepth( 0.5f )
, reverbProcess( nullptr )
// , outputGainOld( 0.f )
, currentProcessMode( -1 ) // -1 means not initialized
{
    // register its editor class (the same as used in vstentry.cpp)
    setControllerClass( VST::FogPadControllerUID );

    reverbProcess = new ReverbProcess( 2 );
}

//------------------------------------------------------------------------
FogPad::~FogPad()
{
    // free all allocated resources
    delete reverbProcess;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::initialize( FUnknown* context )
{
    //---always initialize the parent-------
    tresult result = AudioEffect::initialize( context );
    // if everything Ok, continue
    if ( result != kResultOk )
        return result;

    //---create Audio In/Out buses------
    addAudioInput ( STR16( "Stereo In" ),  SpeakerArr::kStereo );
    addAudioOutput( STR16( "Stereo Out" ), SpeakerArr::kStereo );

    //---create Event In/Out buses (1 bus with only 1 channel)------
    addEventInput( STR16( "Event In" ), 1 );

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::terminate()
{
    // nothing to do here yet...except calling our parent terminate
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::setActive (TBool state)
{
    if (state)
        sendTextMessage( "FogPad::setActive (true)" );
    else
        sendTextMessage( "FogPad::setActive (false)" );

    // reset output level meter
    // outputGainOld = 0.f;

    // call our parent setActive
    return AudioEffect::setActive( state );
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::process( ProcessData& data )
{
    // In this example there are 4 steps:
    // 1) Read inputs parameters coming from host (in order to adapt our model values)
    // 2) Read inputs events coming from host (note on/off events)
    // 3) Apply the effect using the input buffer into the output buffer

    //---1) Read input parameter changes-----------
    IParameterChanges* paramChanges = data.inputParameterChanges;
    if ( paramChanges )
    {
        int32 numParamsChanged = paramChanges->getParameterCount();
        // for each parameter which are some changes in this audio block:
        for ( int32 i = 0; i < numParamsChanged; i++ )
        {
            IParamValueQueue* paramQueue = paramChanges->getParameterData( i );
            if ( paramQueue )
            {
                ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();
                switch ( paramQueue->getParameterId())
                {
                    // we use in this example only the last point of the queue.
                    // in some wanted case for specific kind of parameter it makes sense to retrieve all points
                    // and process the whole audio block in small blocks.

                    case kReverbSizeId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbSize = ( float ) value;
                        break;

                    case kReverbWidthId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbWidth = ( float ) value;
                        break;

                    case kReverbDryMixId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbDryMix = ( float ) value;
                        break;

                    case kReverbWetMixId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbWetMix = ( float ) value;
                        break;

                    case kReverbFreezeId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbFreeze = ( float ) value;
                        break;

                    case kReverbPlaybackRateId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fReverbPlaybackRate = ( float ) value;
                        break;

                    case kBitResolutionId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fBitResolution = ( float ) value;
                        break;

                    case kBitResolutionChainId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fBitResolutionChain = ( float ) value;
                        break;

                    case kLFOBitResolutionId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fLFOBitResolution = ( float ) value;
                        break;

                    case kLFOBitResolutionDepthId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fLFOBitResolutionDepth = ( float ) value;
                        break;

                    case kDecimatorId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fDecimator = ( float ) value;
                        break;

                    case kFilterCutoffId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fFilterCutoff = ( float ) value;
                        break;

                    case kFilterResonanceId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fFilterResonance = ( float ) value;
                        break;

                    case kLFOFilterId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fLFOFilter = ( float ) value;
                        break;

                    case kLFOFilterDepthId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            fLFOFilterDepth = ( float ) value;
                        break;

                    case kBypassId:
                        if ( paramQueue->getPoint( numPoints - 1, sampleOffset, value ) == kResultTrue )
                            _bypass = ( value > 0.5f );
                        break;

                }
                syncModel();
            }
        }
    }

    //---2) Read input events-------------
//    IEventList* eventList = data.inputEvents;

    //-------------------------------------
    //---3) Process Audio---------------------
    //-------------------------------------

    if ( data.numInputs == 0 || data.numOutputs == 0 )
    {
        // nothing to do
        return kResultOk;
    }

    int32 numInChannels  = data.inputs[ 0 ].numChannels;
    int32 numOutChannels = data.outputs[ 0 ].numChannels;

    // --- get audio buffers----------------
    uint32 sampleFramesSize = getSampleFramesSizeInBytes( processSetup, data.numSamples );
    void** in  = getChannelBuffersPointer( processSetup, data.inputs [ 0 ] );
    void** out = getChannelBuffersPointer( processSetup, data.outputs[ 0 ] );

    // process the incoming sound!

    bool isDoublePrecision = ( data.symbolicSampleSize == kSample64 );
    bool isSilentInput     = data.inputs[ 0 ].silenceFlags != 0;
    bool isSilentOutput    = false;

    if ( _bypass )
    {
        // bypass mode, write the input unchanged into the output
        for ( int32 i = 0, l = std::min( numInChannels, numOutChannels ); i < l; i++ )
        {
            if ( in[ i ] != out[ i ])
            {
                memcpy( out[ i ], in[ i ], sampleFramesSize );
            }
            isSilentOutput = isSilentInput;
        }
    } else {
        if ( isDoublePrecision ) {
            // 64-bit samples, e.g. Reaper64
            reverbProcess->process<double>(
                ( double** ) in, ( double** ) out, numInChannels, numOutChannels,
                data.numSamples, sampleFramesSize
            );
        }
        else {
            // 32-bit samples, e.g. Ableton Live, Bitwig Studio... (oddly enough also when 64-bit?)
            reverbProcess->process<float>(
                ( float** ) in, ( float** ) out, numInChannels, numOutChannels,
                data.numSamples, sampleFramesSize
            );
        }
    }

    // output flags

    data.outputs[ 0 ].silenceFlags = isSilentOutput ? (( uint64 ) 1 << numOutChannels ) - 1 : 0;
    
    //float outputGain = reverbProcess->limiter->getLinearGR();

    return kResultOk;
}

//------------------------------------------------------------------------
tresult FogPad::receiveText( const char* text )
{
    // received from Controller
    fprintf( stderr, "[FogPad] received: " );
    fprintf( stderr, "%s", text );
    fprintf( stderr, "\n" );

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::setState( IBStream* state )
{
    // called when we load a preset, the model has to be reloaded

    IBStreamer streamer( state, kLittleEndian );

    float savedReverbSize = 0.f;
    if ( streamer.readFloat( savedReverbSize ) == false )
        return kResultFalse;

    float savedReverbWidth = 0.f;
    if ( streamer.readFloat( savedReverbWidth ) == false )
        return kResultFalse;

    float savedReverbDryMix = 0.f;
    if ( streamer.readFloat( savedReverbDryMix ) == false )
        return kResultFalse;

    float savedReverbWetMix = 0.f;
    if ( streamer.readFloat( savedReverbWetMix ) == false )
        return kResultFalse;

    float savedReverbFreeze = 0.f;
    if ( streamer.readFloat( savedReverbFreeze ) == false )
        return kResultFalse;

    float savedReverbPlaybackRate = 0.f;
    if ( streamer.readFloat( savedReverbPlaybackRate ) == false )
        return kResultFalse;

    float savedBitResolution = 0.f;
    if ( streamer.readFloat( savedBitResolution ) == false )
        return kResultFalse;

    float savedBitResolutionChain = 0.f;
    if ( streamer.readFloat( savedBitResolutionChain ) == false )
        return kResultFalse;

    float savedLFOBitResolution = 0.f;
    if ( streamer.readFloat( savedLFOBitResolution ) == false )
        return kResultFalse;

    float savedLFOBitResolutionDepth = 0.f;
    if ( streamer.readFloat( savedLFOBitResolutionDepth ) == false )
        return kResultFalse;

    float savedDecimator = 0.f;
    if ( streamer.readFloat( savedDecimator ) == false )
        return kResultFalse;

    float savedFilterCutoff = 0.f;
    if ( streamer.readFloat( savedFilterCutoff ) == false )
        return kResultFalse;

    float savedFilterResonance = 0.f;
    if ( streamer.readFloat( savedFilterResonance ) == false )
        return kResultFalse;

    float savedLFOFilter = 0.f;
    if ( streamer.readFloat( savedLFOFilter ) == false )
        return kResultFalse;

    float savedLFOFilterDepth = 0.f;
    if ( streamer.readFloat( savedLFOFilterDepth ) == false )
        return kResultFalse;

    // may fail as this was only added in version 1.0.3.1
    int32 savedBypass = 0;
    if ( streamer.readInt32( savedBypass ) != false ) {
        _bypass = savedBypass;
    }

    fReverbSize             = savedReverbSize;
    fReverbWidth            = savedReverbWidth;
    fReverbDryMix           = savedReverbDryMix;
    fReverbWetMix           = savedReverbWetMix;
    fReverbFreeze           = savedReverbFreeze,
    fReverbPlaybackRate     = savedReverbPlaybackRate;
    fBitResolution          = savedBitResolution;
    fBitResolutionChain     = savedBitResolutionChain;
    fLFOBitResolution       = savedLFOBitResolution;
    fLFOBitResolutionDepth  = savedLFOBitResolutionDepth;
    fDecimator              = savedDecimator;
    fFilterCutoff           = savedFilterCutoff;
    fFilterResonance        = savedFilterResonance;
    fLFOFilter              = savedLFOFilter;
    fLFOFilterDepth         = savedLFOFilterDepth;

    syncModel();

    // Example of using the IStreamAttributes interface
    FUnknownPtr<IStreamAttributes> stream (state);
    if ( stream )
    {
        IAttributeList* list = stream->getAttributes ();
        if ( list )
        {
            // get the current type (project/Default..) of this state
            String128 string = {0};
            if ( list->getString( PresetAttributes::kStateType, string, 128 * sizeof( TChar )) == kResultTrue )
            {
                UString128 tmp( string );
                char ascii[128];
                tmp.toAscii( ascii, 128 );
                if ( !strncmp( ascii, StateType::kProject, strlen( StateType::kProject )))
                {
                    // we are in project loading context...
                }
            }

            // get the full file path of this state
            TChar fullPath[1024];
            memset( fullPath, 0, 1024 * sizeof( TChar ));
            if ( list->getString( PresetAttributes::kFilePathStringType,
                 fullPath, 1024 * sizeof( TChar )) == kResultTrue )
            {
                // here we have the full path ...
            }
        }
    }
    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::getState( IBStream* state )
{
    // here we save the model values

    IBStreamer streamer( state, kLittleEndian );

    streamer.writeFloat( fReverbSize );
    streamer.writeFloat( fReverbWidth );
    streamer.writeFloat( fReverbDryMix );
    streamer.writeFloat( fReverbWetMix );
    streamer.writeFloat( fReverbFreeze );
    streamer.writeFloat( fReverbPlaybackRate );
    streamer.writeFloat( fBitResolution );
    streamer.writeFloat( fBitResolutionChain );
    streamer.writeFloat( fLFOBitResolution );
    streamer.writeFloat( fLFOBitResolutionDepth );
    streamer.writeFloat( fDecimator );
    streamer.writeFloat( fFilterCutoff );
    streamer.writeFloat( fFilterResonance );
    streamer.writeFloat( fLFOFilter );
    streamer.writeFloat( fLFOFilterDepth );
    streamer.writeInt32( _bypass ? 1 : 0 );

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::setupProcessing( ProcessSetup& newSetup )
{
    // called before the process call, always in a disabled state (not active)

    // here we keep a trace of the processing mode (offline,...) for example.
    currentProcessMode = newSetup.processMode;

    VST::SAMPLE_RATE = newSetup.sampleRate;


    syncModel();

    return AudioEffect::setupProcessing( newSetup );
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::setBusArrangements( SpeakerArrangement* inputs,  int32 numIns,
                                               SpeakerArrangement* outputs, int32 numOuts )
{
    if ( numIns == 1 && numOuts == 1 )
    {
        // the host wants Mono => Mono (or 1 channel -> 1 channel)
        if ( SpeakerArr::getChannelCount( inputs[0])  == 1 &&
             SpeakerArr::getChannelCount( outputs[0]) == 1 )
        {
            AudioBus* bus = FCast<AudioBus>( audioInputs.at( 0 ));
            if ( bus )
            {
                // check if we are Mono => Mono, if not we need to recreate the buses
                if ( bus->getArrangement() != inputs[0])
                {
                    removeAudioBusses();
                    addAudioInput ( STR16( "Mono In" ),  inputs[0] );
                    addAudioOutput( STR16( "Mono Out" ), inputs[0] );
                }
                return kResultOk;
            }
        }
        // the host wants something else than Mono => Mono, in this case we are always Stereo => Stereo
        else
        {
            AudioBus* bus = FCast<AudioBus>( audioInputs.at(0));
            if ( bus )
            {
                tresult result = kResultFalse;

                // the host wants 2->2 (could be LsRs -> LsRs)
                if ( SpeakerArr::getChannelCount(inputs[0]) == 2 && SpeakerArr::getChannelCount( outputs[0]) == 2 )
                {
                    removeAudioBusses();
                    addAudioInput  ( STR16( "Stereo In"),  inputs[0] );
                    addAudioOutput ( STR16( "Stereo Out"), outputs[0]);
                    result = kResultTrue;
                }
                // the host want something different than 1->1 or 2->2 : in this case we want stereo
                else if ( bus->getArrangement() != SpeakerArr::kStereo )
                {
                    removeAudioBusses();
                    addAudioInput ( STR16( "Stereo In"),  SpeakerArr::kStereo );
                    addAudioOutput( STR16( "Stereo Out"), SpeakerArr::kStereo );
                    result = kResultFalse;
                }
                return result;
            }
        }
    }
    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::canProcessSampleSize( int32 symbolicSampleSize )
{
    if ( symbolicSampleSize == kSample32 )
        return kResultTrue;

    // we support double processing
    if ( symbolicSampleSize == kSample64 )
        return kResultTrue;

    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPad::notify( IMessage* message )
{
    if ( !message )
        return kInvalidArgument;

    if ( !strcmp( message->getMessageID(), "BinaryMessage" ))
    {
        const void* data;
        uint32 size;
        if ( message->getAttributes ()->getBinary( "MyData", data, size ) == kResultOk )
        {
            // we are in UI thread
            // size should be 100
            if ( size == 100 && ((char*)data)[1] == 1 ) // yeah...
            {
                fprintf( stderr, "[FogPad] received the binary message!\n" );
            }
            return kResultOk;
        }
    }

    return AudioEffect::notify( message );
}

void FogPad::syncModel()
{
    reverbProcess->setRoomSize( fReverbSize );
    reverbProcess->setWidth( fReverbWidth );
    reverbProcess->setDry( fReverbDryMix );
    reverbProcess->setWet( fReverbWetMix );
    reverbProcess->setMode( fReverbFreeze );
    reverbProcess->setPlaybackRate( fReverbPlaybackRate );

    reverbProcess->bitCrusherPostMix = Calc::toBool( fBitResolutionChain );

    reverbProcess->bitCrusher->setAmount( fBitResolution );
    reverbProcess->bitCrusher->setLFO( fLFOBitResolution, fLFOBitResolutionDepth );

    // invert the decimator range 0 == max bits (no distortion), 1 == min bits (severely distorted)
    float scaledDecimator = abs( fDecimator - 1.0f );
    int decimation = ( int )( scaledDecimator * 32.f );
    reverbProcess->decimator->setBits( decimation );
    reverbProcess->decimator->setRate( scaledDecimator );

    reverbProcess->filter->updateProperties( fFilterCutoff, fFilterResonance, fLFOFilter, fLFOFilterDepth );
}

}
