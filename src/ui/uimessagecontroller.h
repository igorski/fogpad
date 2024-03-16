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
#ifndef __MESSAGE_CONTROLLER_HEADER__
#define __MESSAGE_CONTROLLER_HEADER__

#include "vstgui/lib/iviewlistener.h"
#include "vstgui/uidescription/icontroller.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// FogPadUIMessageController
//------------------------------------------------------------------------
template <typename ControllerType>
class FogPadUIMessageController : public VSTGUI::IController, public VSTGUI::ViewListenerAdapter
{
    public:
        enum Tags
        {
            kSendMessageTag = 1000
        };

        FogPadUIMessageController( ControllerType* fogPadController ) : fogPadController( fogPadController ), textEdit( nullptr )
        {
        }

        ~FogPadUIMessageController()
        {
            viewWillDelete( textEdit );
            fogPadController->removeUIMessageController( this );
        }

        void setMessageText( String128 msgText )
        {
            if ( !textEdit )
                return;

            textEdit->setText( VST3::StringConvert::convert( msgText ));
        }

    private:
        typedef VSTGUI::CControl CControl;
        typedef VSTGUI::CView CView;
        typedef VSTGUI::CTextEdit CTextEdit;
        typedef VSTGUI::UTF8String UTF8String;

        //--- from IControlListener ----------------------
        void valueChanged( CControl* /*pControl*/ ) override {}
        void controlBeginEdit( CControl* /*pControl*/ ) override {}
        void controlEndEdit( CControl* pControl ) override
        {
            if ( pControl->getTag () == kSendMessageTag )
            {
                if ( pControl->getValueNormalized () > 0.5f )
                {
                    fogPadController->sendTextMessage( textEdit->getText ().data() );
                    pControl->setValue( 0.f );
                    pControl->invalid();

                    //---send a binary message
                    if ( IPtr<IMessage> message = owned( fogPadController->allocateMessage()))
                    {
                        message->setMessageID ("BinaryMessage");
                        uint32 size = 100;
                        char8 data[100];
                        memset( data, 0, size * sizeof( char ));
                        // fill my data with dummy stuff
                        for ( uint32 i = 0; i < size; i++ )
                            data[ i ] = i;
                        message->getAttributes ()->setBinary( "MyData", data, size );
                        fogPadController->sendMessage( message );
                    }
                }
            }
        }
        //--- from IControlListener ----------------------
        //--- is called when a view is created -----
        CView* verifyView ( CView* view, const UIAttributes& /*attributes*/,
                            const IUIDescription* /*description*/ ) override
        {
            if ( CTextEdit* te = dynamic_cast<CTextEdit*>( view ))
            {
                // this allows us to keep a pointer of the text edit view
                textEdit = te;

                // add this as listener in order to get viewWillDelete and viewLostFocus calls
                textEdit->registerViewListener( this );

                // set its contents
                textEdit->setText( VST3::StringConvert::convert( fogPadController->getDefaultMessageText()));
            }
            return view;
        }
        //--- from IViewListenerAdapter ----------------------
        //--- is called when a view will be deleted: the editor is closed -----
        void viewWillDelete (CView* view) override
        {
            if (dynamic_cast<CTextEdit*> (view) == textEdit)
            {
                textEdit->unregisterViewListener (this);
                textEdit = nullptr;
            }
        }
        //--- is called when the view is unfocused -----------------
        void viewLostFocus (CView* view) override
        {
            if (dynamic_cast<CTextEdit*> (view) == textEdit)
            {
                // save the last content of the text edit view
                const auto& text = textEdit->getText();
                auto utf16Text = VST3::StringConvert::convert( text.getString());
                fogPadController->setDefaultMessageText( utf16Text.data());
            }
        }
        ControllerType* fogPadController;
        CTextEdit* textEdit;
};

//------------------------------------------------------------------------
} // Vst
} // Igorski

#endif
