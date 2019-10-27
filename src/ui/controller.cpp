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
#include "../global.h"
#include "controller.h"
#include "uimessagecontroller.h"
#include "../paramids.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"

#include "base/source/fstring.h"

#include "vstgui/uidescription/delegationcontroller.h"

#include <stdio.h>
#include <math.h>

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// FogPadController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::initialize( FUnknown* context )
{
    tresult result = EditControllerEx1::initialize( context );

    if ( result != kResultOk )
        return result;

    //--- Create Units-------------
    UnitInfo unitInfo;
    Unit* unit;

    // create root only if you want to use the programListId
    /*	unitInfo.id = kRootUnitId;	// always for Root Unit
    unitInfo.parentUnitId = kNoParentUnitId;	// always for Root Unit
    Steinberg::UString (unitInfo.name, USTRINGSIZE (unitInfo.name)).assign (USTRING ("Root"));
    unitInfo.programListId = kNoProgramListId;

    unit = new Unit (unitInfo);
    addUnitInfo (unit);*/

    // create a unit1
    unitInfo.id = 1;
    unitInfo.parentUnitId = kRootUnitId;    // attached to the root unit

    Steinberg::UString( unitInfo.name, USTRINGSIZE( unitInfo.name )).assign( USTRING( "FogPad" ));

    unitInfo.programListId = kNoProgramListId;

    unit = new Unit( unitInfo );
    addUnit( unit );
    int32 unitId = 1;

    // Reverb controls

    parameters.addParameter( new RangeParameter(
        USTRING( "Reverb size" ), kReverbSizeId, USTRING( "0 - 1" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Reverb width" ), kReverbWidthId, USTRING( "0 - 1" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Reverb dry mix" ), kReverbDryMixId, USTRING( "0 - 1" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Reverb wet mix" ), kReverbWetMixId, USTRING( "0 - 1" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter(
        USTRING( "Reverb feeze mode" ), 0, 1, 0, ParameterInfo::kCanAutomate, kReverbFreezeId, unitId
    );

    parameters.addParameter( new RangeParameter(
        USTRING( "Reverb playback rate" ), kReverbPlaybackRateId, USTRING( "0 - 1" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    // BitCrusher controls

    parameters.addParameter( new RangeParameter(
        USTRING( "Bit resolution" ), kBitResolutionId, USTRING( "0 - 16" ),
        16, 0, 16,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter(
        USTRING( "BitCrusher chain" ), 0, 1, 0, ParameterInfo::kCanAutomate, kBitResolutionChainId, unitId
    );

    parameters.addParameter( new RangeParameter(
        USTRING( "Bit LFO rate" ), kLFOBitResolutionId, USTRING( "Hz" ),
        Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MAX_LFO_RATE(), Igorski::VST::MIN_LFO_RATE(),
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Bit LFO depth" ), kLFOBitResolutionDepthId, USTRING( "%" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    // Decimator controls

    parameters.addParameter( new RangeParameter(
        USTRING( "Decimator resolution" ), kDecimatorId, USTRING( "1 - 32" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    // Filter controls

    parameters.addParameter( new RangeParameter(
        USTRING( "Filter cutoff" ), kFilterCutoffId, USTRING( "Hz" ),
        Igorski::VST::FILTER_MIN_FREQ, Igorski::VST::FILTER_MAX_FREQ, Igorski::VST::FILTER_MIN_FREQ,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Filter resonance" ), kFilterResonanceId, USTRING( "dB" ),
         Igorski::VST::FILTER_MIN_RESONANCE, Igorski::VST::FILTER_MAX_RESONANCE, Igorski::VST::FILTER_MIN_RESONANCE,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Filter LFO rate" ), kLFOFilterId, USTRING( "Hz" ),
        Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MAX_LFO_RATE(), Igorski::VST::MIN_LFO_RATE(),
        0, ParameterInfo::kCanAutomate, unitId
    ));

    parameters.addParameter( new RangeParameter(
        USTRING( "Filter LFO depth" ), kLFOFilterDepthId, USTRING( "%" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    ));

    // initialization

    String str( "FOGPAD" );
    str.copyTo16( defaultMessageText, 0, 127 );

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::terminate()
{
    return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::setComponentState( IBStream* state )
{
    // we receive the current state of the component (processor part)
    if ( state )
    {
        float savedReverbSize = 1.f;
        if ( state->read( &savedReverbSize, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedReverbWidth = 1.f;
        if ( state->read( &savedReverbWidth, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedReverbDryMix = 1.f;
        if ( state->read( &savedReverbDryMix, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedReverbWetMix = 1.f;
        if ( state->read( &savedReverbWetMix, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedReverbFreeze = 1.f;
        if ( state->read( &savedReverbFreeze, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedReverbPlaybackRate = 1.f;
        if ( state->read( &savedReverbPlaybackRate, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedBitResolution = 1.f;
        if ( state->read( &savedBitResolution, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedBitResolutionChain = 0.f;
        if ( state->read( &savedBitResolutionChain, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedLFOBitResolution = Igorski::VST::MIN_LFO_RATE();
        if ( state->read( &savedLFOBitResolution, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedLFOBitResolutionDepth = 1.f;
        if ( state->read( &savedLFOBitResolutionDepth, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedDecimator = 1.f;
        if ( state->read( &savedDecimator, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedFilterCutoff = Igorski::VST::FILTER_MAX_FREQ;
        if ( state->read( &savedFilterCutoff, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedFilterResonance = Igorski::VST::FILTER_MAX_RESONANCE;
        if ( state->read( &savedFilterResonance, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedLFOFilter = Igorski::VST::MIN_LFO_RATE();
        if ( state->read( &savedLFOFilter, sizeof( float )) != kResultOk )
            return kResultFalse;

        float savedLFOFilterDepth = 1.f;
        if ( state->read( &savedLFOFilterDepth, sizeof( float )) != kResultOk )
            return kResultFalse;

#if BYTEORDER == kBigEndian
    SWAP32( savedReverbSize )
    SWAP32( savedReverbWidth )
    SWAP32( savedReverbDryMix )
    SWAP32( savedReverbWetMix )
    SWAP32( savedReverbFreeze )
    SWAP32( savedReverbPlaybackRate )
    SWAP32( savedBitResolution )
    SWAP32( savedBitResolutionChain )
    SWAP32( savedLFOBitResolution )
    SWAP32( savedLFOBitResolutionDepth )
    SWAP32( savedDecimator )
    SWAP32( savedFilterCutoff )
    SWAP32( savedFilterResonance )
    SWAP32( savedLFOFilter )
    SWAP32( savedLFOFilterDepth )
#endif

        setParamNormalized( kReverbSizeId,              savedReverbSize );
        setParamNormalized( kReverbWidthId,             savedReverbWidth );
        setParamNormalized( kReverbDryMixId,            savedReverbDryMix );
        setParamNormalized( kReverbWetMixId,            savedReverbWetMix );
        setParamNormalized( kReverbFreezeId,            savedReverbFreeze );
        setParamNormalized( kReverbPlaybackRateId,      savedReverbPlaybackRate );
        setParamNormalized( kBitResolutionId,           savedBitResolution );
        setParamNormalized( kBitResolutionChainId,      savedBitResolutionChain );
        setParamNormalized( kLFOBitResolutionId,        savedLFOBitResolution );
        setParamNormalized( kLFOBitResolutionDepthId,   savedLFOBitResolutionDepth );
        setParamNormalized( kDecimatorId,               savedDecimator );
        setParamNormalized( kFilterCutoffId,            savedFilterCutoff );
        setParamNormalized( kFilterResonanceId,         savedFilterResonance );
        setParamNormalized( kLFOFilterId,               savedLFOFilter );
        setParamNormalized( kLFOFilterDepthId,          savedLFOFilterDepth );

        state->seek( sizeof ( float ), IBStream::kIBSeekCur );
    }
    return kResultOk;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API FogPadController::createView( const char* name )
{
    // create the visual editor
    if ( name && strcmp( name, "editor" ) == 0 )
    {
        VST3Editor* view = new VST3Editor( this, "view", "fogpad.uidesc" );
        return view;
    }
    return 0;
}

//------------------------------------------------------------------------
IController* FogPadController::createSubController( UTF8StringPtr name,
                                                    const IUIDescription* /*description*/,
                                                    VST3Editor* /*editor*/ )
{
    if ( UTF8StringView( name ) == "MessageController" )
    {
        UIMessageController* controller = new UIMessageController( this );
        addUIMessageController( controller );
        return controller;
    }
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::setState( IBStream* state )
{
    tresult result = kResultFalse;

    int8 byteOrder;
    if (( result = state->read( &byteOrder, sizeof( int8 ))) != kResultTrue )
        return result;

    if (( result = state->read( defaultMessageText, 128 * sizeof( TChar ))) != kResultTrue )
        return result;

    // if the byteorder doesn't match, byte swap the text array ...
    if ( byteOrder != BYTEORDER )
    {
        for ( int32 i = 0; i < 128; i++ )
            SWAP_16( defaultMessageText[ i ])
    }

    // update our editors
    for ( UIMessageControllerList::iterator it = uiMessageControllers.begin (), end = uiMessageControllers.end (); it != end; ++it )
        ( *it )->setMessageText( defaultMessageText );

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::getState( IBStream* state )
{
    // here we can save UI settings for example

    // as we save a Unicode string, we must know the byteorder when setState is called
    int8 byteOrder = BYTEORDER;
    if ( state->write( &byteOrder, sizeof( int8 )) == kResultTrue )
    {
        return state->write( defaultMessageText, 128 * sizeof( TChar ));
    }
    return kResultFalse;
}

//------------------------------------------------------------------------
tresult FogPadController::receiveText( const char* text )
{
    // received from Component
    if ( text )
    {
        fprintf( stderr, "[FogPadController] received: " );
        fprintf( stderr, "%s", text );
        fprintf( stderr, "\n" );
    }
    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::setParamNormalized( ParamID tag, ParamValue value )
{
    // called from host to update our parameters state
    tresult result = EditControllerEx1::setParamNormalized( tag, value );
    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::getParamStringByValue( ParamID tag, ParamValue valueNormalized, String128 string )
{
    switch ( tag )
    {
        // these controls are floating point values in 0 - 1 range, we can
        // simply read the normalized value which is in the same range

        case kReverbSizeId:
        case kReverbWidthId:
        case kReverbDryMixId:
        case kReverbWetMixId:
        case kReverbFreezeId:
        case kReverbPlaybackRateId:
        case kBitResolutionId:
        case kBitResolutionChainId:
        case kLFOBitResolutionDepthId:
        case kDecimatorId:
        case kLFOFilterDepthId:
        {
            char text[32];

            if (( tag == kReverbFreezeId )) {
                sprintf( text, "%s", ( valueNormalized == 0 ) ? "Off" : "On" );
            }
            else if ( tag == kBitResolutionChainId ) {
                sprintf( text, "%s", ( valueNormalized == 0 ) ? "Pre-reverb mix" : "Post-reverb mix" );
            }
            else {
                sprintf( text, "%.2f", ( float ) valueNormalized );
            }
            Steinberg::UString( string, 128 ).fromAscii( text );

            return kResultTrue;
        }

        // bitcrusher LFO setting is also floating point but in a custom range
        // request the plain value from the normalized value

        case kLFOBitResolutionId:
        case kFilterCutoffId:
        case kFilterResonanceId:
        case kLFOFilterId:
        {
            char text[32];
            if (( tag == kLFOBitResolutionId ) && valueNormalized == 0 )
                sprintf( text, "%s", "Off" );
            else
                sprintf( text, "%.2f", normalizedParamToPlain( tag, valueNormalized ));
            Steinberg::UString( string, 128 ).fromAscii( text );

            return kResultTrue;
        }

        // everything else
        default:
            return EditControllerEx1::getParamStringByValue( tag, valueNormalized, string );
    }
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::getParamValueByString( ParamID tag, TChar* string, ParamValue& valueNormalized )
{
    /* example, but better to use a custom Parameter as seen in RangeParameter
    switch (tag)
    {
        case kAttackId:
        {
            Steinberg::UString wrapper ((TChar*)string, -1); // don't know buffer size here!
            double tmp = 0.0;
            if (wrapper.scanFloat (tmp))
            {
                valueNormalized = expf (logf (10.f) * (float)tmp / 20.f);
                return kResultTrue;
            }
            return kResultFalse;
        }
    }*/
    return EditControllerEx1::getParamValueByString( tag, string, valueNormalized );
}

//------------------------------------------------------------------------
void FogPadController::addUIMessageController( UIMessageController* controller )
{
    uiMessageControllers.push_back( controller );
}

//------------------------------------------------------------------------
void FogPadController::removeUIMessageController( UIMessageController* controller )
{
    UIMessageControllerList::const_iterator it = std::find(
        uiMessageControllers.begin(), uiMessageControllers.end (), controller
    );
    if ( it != uiMessageControllers.end())
        uiMessageControllers.erase( it );
}

//------------------------------------------------------------------------
void FogPadController::setDefaultMessageText( String128 text )
{
    String tmp( text );
    tmp.copyTo16( defaultMessageText, 0, 127 );
}

//------------------------------------------------------------------------
TChar* FogPadController::getDefaultMessageText()
{
    return defaultMessageText;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::queryInterface( const char* iid, void** obj )
{
    QUERY_INTERFACE( iid, obj, IMidiMapping::iid, IMidiMapping );
    return EditControllerEx1::queryInterface( iid, obj );
}

//------------------------------------------------------------------------
tresult PLUGIN_API FogPadController::getMidiControllerAssignment( int32 busIndex, int16 /*midiChannel*/,
    CtrlNumber midiControllerNumber, ParamID& tag )
{
    // we support for the Gain parameter all MIDI Channel but only first bus (there is only one!)
/*
    if ( busIndex == 0 && midiControllerNumber == kCtrlVolume )
    {
        tag = kDelayTimeId;
        return kResultTrue;
    }
*/
    return kResultFalse;
}

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
