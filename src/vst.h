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
#ifndef __VST_HEADER__
#define __VST_HEADER__

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "reverbprocess.h"
#include "global.h"

using namespace Steinberg::Vst;

namespace Igorski {

class FogPad : public AudioEffect
{
    public:
        FogPad ();
        virtual ~FogPad(); // do not forget virtual here

        //--- ---------------------------------------------------------------------
        // create function required for Plug-in factory,
        // it will be called to create new instances of this Plug-in
        //--- ---------------------------------------------------------------------
        static FUnknown* createInstance( void* /*context*/ ) { return ( IAudioProcessor* ) new FogPad; }

        //--- ---------------------------------------------------------------------
        // AudioEffect overrides:
        //--- ---------------------------------------------------------------------
        /** Called at first after constructor */
        tresult PLUGIN_API initialize( FUnknown* context ) SMTG_OVERRIDE;

        /** Called at the end before destructor */
        tresult PLUGIN_API terminate() SMTG_OVERRIDE;

        /** Switch the Plug-in on/off */
        tresult PLUGIN_API setActive( TBool state ) SMTG_OVERRIDE;

        /** Here we go...the process call */
        tresult PLUGIN_API process( ProcessData& data ) SMTG_OVERRIDE;

        /** Test of a communication channel between controller and component */
        tresult receiveText( const char* text ) SMTG_OVERRIDE;

        /** For persistence */
        tresult PLUGIN_API setState( IBStream* state ) SMTG_OVERRIDE;
        tresult PLUGIN_API getState( IBStream* state ) SMTG_OVERRIDE;

        /** Will be called before any process call */
        tresult PLUGIN_API setupProcessing( ProcessSetup& newSetup ) SMTG_OVERRIDE;

        /** Bus arrangement managing */
        tresult PLUGIN_API setBusArrangements( SpeakerArrangement* inputs, int32 numIns,
                                               SpeakerArrangement* outputs,
                                               int32 numOuts ) SMTG_OVERRIDE;

        /** Asks if a given sample size is supported see \ref SymbolicSampleSizes. */
        tresult PLUGIN_API canProcessSampleSize( int32 symbolicSampleSize ) SMTG_OVERRIDE;

        /** We want to receive message. */
        tresult PLUGIN_API notify( IMessage* message ) SMTG_OVERRIDE;

    //------------------------------------------------------------------------
    protected:
        //==============================================================================

        // our model values, these are all 0 - 1 range
        // (normalized) RangeParameter values

        float fReverbSize;
        float fReverbWidth;
        float fReverbDryMix;
        float fReverbWetMix;
        float fReverbFreeze;
        float fReverbPlaybackRate;

        float fBitResolution;
        float fBitResolutionChain;
        float fLFOBitResolution;
        float fLFOBitResolutionDepth;

        float fDecimator;

        float fFilterCutoff;
        float fFilterResonance;
        float fLFOFilter;
        float fLFOFilterDepth;

        float outputGainOld; // for visualizing output gain in DAW

        int32 currentProcessMode;

        Igorski::ReverbProcess* reverbProcess;

        // synchronize the processors model with UI led changes

        void syncModel();
};

}

#endif
