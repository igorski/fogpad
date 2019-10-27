/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Igor Zinken - https://www.igorski.nl
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
#include "vst.h"
#include "ui/controller.h"
#include "global.h"
#include "version.h"

#include "public.sdk/source/main/pluginfactoryvst3.h"

using namespace Steinberg::Vst;
using namespace Igorski;

//------------------------------------------------------------------------
//  Module init/exit
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// called after library was loaded
bool InitModule ()
{
    return true;
}

//------------------------------------------------------------------------
// called after library is unloaded
bool DeinitModule ()
{
    return true;
}

//------------------------------------------------------------------------
//  VST Plug-in Entry
//------------------------------------------------------------------------
// Windows: do not forget to include a .def file in your project to export
// GetPluginFactory function!
//------------------------------------------------------------------------

BEGIN_FACTORY_DEF( "igorski.nl",
                   "https://www.igorski.nl",
                   "mailto:info@igorski.nl")

    //---First Plug-in included in this factory-------
    // its kVstAudioEffectClass component
    DEF_CLASS2( INLINE_UID_FROM_FUID( Igorski::VST::FogPadProcessorUID ),
                PClassInfo::kManyInstances,      // cardinality
                kVstAudioEffectClass,            // the component category (do not change this)
                Igorski::VST::NAME,              // plug-in name
                Vst::kDistributable,             // means that component and controller could be distributed on different computers
                "Fx",                            // Subcategory for this Plug-in
                FULL_VERSION_STR,                // Plug-in version
                kVstVersionString,               // the VST 3 SDK version (do not change this)
                FogPad::createInstance )         // function pointer called when this component should be instantiated

    // its kVstComponentControllerClass component
    DEF_CLASS2( INLINE_UID_FROM_FUID( Igorski::VST::FogPadControllerUID ),
                PClassInfo::kManyInstances,   // cardinality
                kVstComponentControllerClass, // the Controller category (do not change this)
                Igorski::VST::NAME,           // controller name (could be the same as component name)
                0, "",                        // neither of these are used here
                FULL_VERSION_STR,             // Plug-in version
                kVstVersionString,            // the VST 3 SDK version (do not change this)
                FogPad::createInstance )      // function pointer called when this component should be instantiated

END_FACTORY
