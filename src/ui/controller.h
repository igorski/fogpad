/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2020 Igor Zinken - https://www.igorski.nl
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
#ifndef __CONTROLLER_HEADER__
#define __CONTROLLER_HEADER__

#include "vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"

#include <vector>

namespace Steinberg {
namespace Vst {

using namespace VSTGUI;

template<typename T>
class FogPadUIMessageController;

//------------------------------------------------------------------------
// FogPadController
//------------------------------------------------------------------------
class FogPadController : public EditControllerEx1, public IMidiMapping, public VST3EditorDelegate
{
    public:
        typedef FogPadUIMessageController<FogPadController> UIMessageController;
        //--- ---------------------------------------------------------------------
        // create function required for Plug-in factory,
        // it will be called to create new instances of this controller
        //--- ---------------------------------------------------------------------
        static FUnknown* createInstance( void* /*context*/ )
        {
            return ( IEditController* ) new FogPadController;
        }

        //---from IPluginBase--------
        tresult PLUGIN_API initialize( FUnknown* context ) SMTG_OVERRIDE;
        tresult PLUGIN_API terminate() SMTG_OVERRIDE;

        //---from EditController-----
        tresult PLUGIN_API setComponentState( IBStream* state ) SMTG_OVERRIDE;
        IPlugView* PLUGIN_API createView( const char* name ) SMTG_OVERRIDE;
        tresult PLUGIN_API setState( IBStream* state ) SMTG_OVERRIDE;
        tresult PLUGIN_API getState( IBStream* state ) SMTG_OVERRIDE;
        tresult PLUGIN_API setParamNormalized( ParamID tag, ParamValue value) SMTG_OVERRIDE;
        tresult PLUGIN_API getParamStringByValue( ParamID tag, ParamValue valueNormalized,
                                                  String128 string ) SMTG_OVERRIDE;
        tresult PLUGIN_API getParamValueByString( ParamID tag, TChar* string,
                                                  ParamValue& valueNormalized ) SMTG_OVERRIDE;

        //---from ComponentBase-----
        tresult receiveText( const char* text ) SMTG_OVERRIDE;

        //---from IMidiMapping-----------------
        tresult PLUGIN_API getMidiControllerAssignment (int32 busIndex, int16 channel,
                                                        CtrlNumber midiControllerNumber,
                                                        ParamID& tag) SMTG_OVERRIDE;

        //---from VST3EditorDelegate-----------
        IController* createSubController( UTF8StringPtr name, const IUIDescription* description,
                                          VST3Editor* editor ) SMTG_OVERRIDE;

        DELEGATE_REFCOUNT ( EditController )
        tresult PLUGIN_API queryInterface( const char* iid, void** obj ) SMTG_OVERRIDE;

        //---Internal functions-------
        void addUIMessageController( UIMessageController* controller );
        void removeUIMessageController( UIMessageController* controller );

        void setDefaultMessageText( String128 text );
        TChar* getDefaultMessageText();

    private:
        typedef std::vector<UIMessageController*> UIMessageControllerList;
        UIMessageControllerList uiMessageControllers;

        String128 defaultMessageText;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif
