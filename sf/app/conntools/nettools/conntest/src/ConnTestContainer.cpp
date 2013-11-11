/*
 * Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: The container class for ConnTest
 * Shows status line and notification text editor
 *
 */

// INCLUDE FILES
#include <eikenv.h>
#include <eikrted.h>
#include <txtrich.h>
#include <eiklabel.h>
#include "ConnTestContainer.h"
#include "ConnTest.pan"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnTestContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CConnTestContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    Window().SetShadowDisabled(EFalse);

    SetBlank();

    // Get an alternative font for output window.
    // Note! Only NormalFont works in output window with current
    // build version, alternative font doesn't.
    _LIT(KMyFontName,"Arial");
    CFont* myFont;
    TFontSpec myFontSpec(KMyFontName,1); // to get smallest Swiss font
    CGraphicsDevice* screenDevice=iCoeEnv->ScreenDevice();
    screenDevice->GetNearestFontInTwips(myFont,myFontSpec);
    //TFontSpec testFont = myFont->FontSpecInTwips();
    screenDevice->ReleaseFont(myFont);

    // construct main text display window
    const CFont* font = CEikonEnv::Static()->NormalFont();

    TFontSpec fontspec = font->FontSpecInTwips();

    TCharFormat defaultCharFormat( fontspec.iTypeface.iName, fontspec.iHeight );
    //    TCharFormat defaultCharFormat( testFont.iTypeface.iName, testFont.iHeight );
    TCharFormatMask defaultCharFormatMask;

    defaultCharFormatMask.SetAttrib(EAttFontTypeface);
    defaultCharFormatMask.SetAttrib(EAttFontHeight);

    iParaFormat = CParaFormatLayer::NewL();
    iCharFormat = CCharFormatLayer::NewL(defaultCharFormat,defaultCharFormatMask);

    iRichText = CRichText::NewL(iParaFormat, iCharFormat);

    //Create bordered edit windows.
    iOutputWindow = new (ELeave) CEikRichTextEditor(TGulBorder(TGulBorder::EShallowRaised));
    iOutputWindow->ConstructL(this,0,0,EEikEdwinOwnsWindow|EEikEdwinInclusiveSizeFixed|
            EEikEdwinKeepDocument|EEikEdwinUserSuppliedText|
            EEikEdwinLineCursor|EEikEdwinAlwaysShowSelection|
            EEikEdwinReadOnly);

    iOutputWindow->SetContainerWindowL(*this);

    // Create the single line 'status' display
    iStatusWindow = new (ELeave) CEikLabel;
    iStatusWindow->SetContainerWindowL(*this);
    iStatusWindow->SetNonFocusing();
    iStatusWindow->SetTextL(_L("Uninitialised"));

    TRect outputRect(Rect());
    TRect statusRect(Rect());

    // Set status display to be a single line at bottom of screen
    statusRect.iTl.iY = statusRect.iBr.iY - iStatusWindow->MinimumSize().iHeight;

    // Set main output window to occupy rest of screen
    outputRect.iBr.iY = statusRect.iTl.iY - 1;

    // Create scrollbar show tabs
    iOutputWindow->SetAknEditorFlags(EAknEditorFlagEnableScrollBars|EAknEditorFlagNoEditIndicators);
    
    iOutputWindow->CreateScrollBarFrameL();
    iOutputWindow->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    iOutputWindow->SetDocumentContentL(*iRichText,CEikEdwin::EUseText);
    iOutputWindow->SetRect(outputRect);
    iOutputWindow->ActivateL();
    iOutputWindow->SetFocus(ETrue);
    iStatusWindow->SetRect(statusRect);

    SetRect(aRect);
    ActivateL();
    iOutputWindow->UpdateScrollBarsL();
    iOutputWindow->DrawNow();
    SizeChanged();
    }

// ----------------------------------------------------
// CConnTestContainer::~CConnTestContainer()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CConnTestContainer::~CConnTestContainer()
    {
    delete iParaFormat;
    iParaFormat = NULL;

    delete iCharFormat;
    iCharFormat = NULL;

    delete iOutputWindow;
    iOutputWindow = NULL;

    delete iRichText;
    iRichText = NULL;

    delete iStatusWindow;
    iStatusWindow = NULL;
    }

// ---------------------------------------------------------
// CConnTestContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CConnTestContainer::SizeChanged()
    {
    TRect outputRect(Rect());
    TRect statusRect(Rect());

    // Set status display to be a single line at bottom of screen
    statusRect.iTl.iY = statusRect.iBr.iY - iStatusWindow->MinimumSize().iHeight;

    // Find the scrollbar rectangle.
    TRect ScrollBarRect = iOutputWindow->ScrollBarFrame()->VerticalScrollBar()->Rect();


    // Set main output window to occupy rest of screen
    outputRect.iBr.iY = statusRect.iTl.iY - 1;
    outputRect.iBr.iX -= ScrollBarRect.Width();

    iOutputWindow->SetRect(outputRect);
    iOutputWindow->SetFocus(ETrue);
    iStatusWindow->SetRect(statusRect);
    }

// ---------------------------------------------------------
// CConnTestContainer::CountComponentControls() const
// Note! Output window is ignored.
// ---------------------------------------------------------
//
TInt CConnTestContainer::CountComponentControls() const
    {
    // Return nbr of controls inside this container
    // return 2; // Returning control for rich text editor makes it override navipanel.
    return 2;
    }


// ---------------------------------------------------------
// CConnTestContainer::ComponentControl(TInt aIndex) const
// Note! Output window is ignored.
// ---------------------------------------------------------
//
CCoeControl* CConnTestContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iStatusWindow;
        case 1:
            return iOutputWindow;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CConnTestContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CConnTestContainer::Draw(const TRect& aRect) const
    {
    //CWindowGc& gc = SystemGc();
    //gc.Clear(Rect());
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbWhite);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }


// ---------------------------------------------------------
// CConnTestContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CConnTestContainer::HandleControlEventL( CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/ )
    {
    // no implementation required.
    }


// ---------------------------------------------------------
// CConnTestContainer::SetFontStyle(TUint aFontStyle)
// Set font style for output window.
// ---------------------------------------------------------
//
void CConnTestContainer::SetFontStyle(TUint aFontStyle)
    {
    // Set current main window style to match that requested
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EBold);
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EItalic);
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EUnderline);
    }


// ---------------------------------------------------------
// CConnTestContainer::UpdateFontAttribute(TUint aFontStyle, 
//                                         TInt aAttribute)
// Do the real work of setting the font style.
// ---------------------------------------------------------
//
void CConnTestContainer::UpdateFontAttribute(TUint aFontStyle, 
        TInt aAttribute)
    {
    TInt error;

    // Ensure an individual attribute is on or off as requested
    if ((aFontStyle & aAttribute) != (iFontStyle & aAttribute))
        {
        TRAP(error, iOutputWindow->BoldItalicUnderlineEventL(aAttribute));
        if(error != KErrNone)
            {
            User::Panic (KPanicConnTest, EConnTestContainer);
            }
        iFontStyle ^= aAttribute;
        }
    }


// ---------------------------------------------------------
// CConnTestContainer::PrintNotify(const TDesC& aDes, TUint aFontStyle)
// Print text into output window.
// ---------------------------------------------------------
//
void CConnTestContainer::PrintNotify(const TDesC& aDes, TUint aFontStyle)
    {
    SetFontStyle(aFontStyle);
    TChar ch;
    TInt error;

    RDebug::Print(_L("ConnTest: %S"),&aDes);

    for (TInt i = 0; i < aDes.Length(); i++)
        {
        // Add linebreak if character is '\n', otherwise 
        // add character as it is.
        ch = aDes[i] != 10 ? (TChar)aDes[i] : (TChar)CEditableText::ELineBreak;
        TRAP(error, iRichText->InsertL(iOutputWindow->TextLength(), ch));
        if(error != KErrNone)
            {
            User::Panic (KPanicConnTest, EConnTestContainer);
            }
        }

    TRAP(error, iOutputWindow->NotifyNewDocumentL()); 
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }

    TRAP(error, iOutputWindow->SetCursorPosL(iOutputWindow->TextLength(), EFalse));
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }
    }


// ---------------------------------------------------------
// CConnTestContainer::PrintNotify(const TDesC8& aDes, TUint aFontStyle)
// Print text into output window.
// ---------------------------------------------------------
//
void CConnTestContainer::PrintNotify(const TDesC8& aDes, TUint aFontStyle)
    {
    SetFontStyle(aFontStyle);
    TChar ch;
    TInt error;
    const TInt KBufLen( 128 );
    
    TBuf<KBufLen> des;
    des.Copy( aDes.Left(KBufLen) );
    
    RDebug::Print(_L("ConnTest: %S"),&des);
    
    TRAP(error, iRichText->InsertL(iOutputWindow->TextLength(), des));
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }

    TRAP(error, iOutputWindow->NotifyNewDocumentL());
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }

    TRAP(error, iOutputWindow->SetCursorPosL(iOutputWindow->TextLength(), EFalse));
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }
    }


// ---------------------------------------------------------
// CConnTestContainer::PrintNotify(TInt aIn)
// Print integer into output window.
// ---------------------------------------------------------
//
void CConnTestContainer::PrintNotify(TInt aInt)
    {
    // Display number in ascii decimal
    TBuf<50> buf;
    buf.Num(aInt);
    PrintNotify(buf);
    }


// ---------------------------------------------------------
// CConnTestContainer::ErrorNotify(const TDesC& aErrMessage, TInt aErrCode)
// Display error message in an information window.
// ---------------------------------------------------------
//
void CConnTestContainer::ErrorNotify(const TDesC& aErrMessage, TInt aErrCode)
    {
    TInt error;

    // Error up call: inform user and quit
    _LIT(KErrorTitle,"Error code ");
    TBuf<25> errorTitleCode(KErrorTitle);
    errorTitleCode.AppendNum(aErrCode);
    RDebug::Print(_L("ConnTest: ErrorNotify: %S %d"), &aErrMessage, aErrCode);
    TRAP(error, CEikonEnv::Static()->InfoWinL(errorTitleCode,aErrMessage));
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }
    }

// ---------------------------------------------------------
// CConnTestContainer::PopupNotify(const TDesC& msg)
// Display message in an information window.
// ---------------------------------------------------------
//
void CConnTestContainer::PopupNotify(const TDesC& aMessage)
    {
    // Error up call: inform user and quit
    /*    _LIT(KMsgTitle,"NOTE");
    TBuf<25> titleCode(KMsgTitle);
    TRAPD(error, CEikonEnv::Static()->InfoWinL(titleCode,aMessage));
    if(error != KErrNone)
    {
        User::Panic (KPanicConnTest, EConnTestContainer);
    }
     */
    RDebug::Print(_L("ConnTest: PopupNotify: %S"), &aMessage);
    CEikonEnv::Static()->InfoMsgWithDuration(aMessage, 1500000);
    }

// ---------------------------------------------------------
// CConnTestContainer::SetStatus(const TDesC& aStatus)
// Update the status line text.
// ---------------------------------------------------------
//
void CConnTestContainer::SetStatus(const TDesC& aStatus)
    {
    TInt error;

    // Update contents of status window
    TRAP(error, iStatusWindow->SetTextL(aStatus));
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }

    iStatusWindow->DrawDeferred();
    }


// ---------------------------------------------------------
// CConnTestContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,
//                                    TEventCode aType)
// Handle key event. Only up and down key arrow events are
// consumed in order to enable scrolling in output window.
// ---------------------------------------------------------
//
TKeyResponse CConnTestContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if(aType != EEventKey)
        return EKeyWasNotConsumed;

    if(aKeyEvent.iCode == EKeyUpArrow)
        {
        iOutputWindow->MoveCursorL(TCursorPosition::EFLineUp,EFalse);
        return EKeyWasConsumed;
        }

    if(aKeyEvent.iCode == EKeyDownArrow)
        {
        iOutputWindow->MoveCursorL(TCursorPosition::EFLineDown,EFalse);
        return EKeyWasConsumed;
        }

    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CConnTestContainer::ClearOutputWindow()
// Clear the output window.
// ---------------------------------------------------------
//
void CConnTestContainer::ClearOutputWindow()
    {
    TInt error;

    iRichText->Reset();
    TRAP(error, iOutputWindow->NotifyNewDocumentL());
    if(error != KErrNone)
        {
        User::Panic (KPanicConnTest, EConnTestContainer);
        }
    }

// ----------------------------------------------------------------------------
// CConnTestContainer::SaveOutputWindowToFile
// Store console output log to a file.
// ----------------------------------------------------------------------------
//
void CConnTestContainer::SaveOutputWindowToFile()
    {
    TInt error;
    _LIT( KOutputInternalMemory, "c:\\data\\conntest_output.txt" );
    
    TBuf<100> filename( KOutputInternalMemory );
    
    TRAP( error, iRichText->ExportAsTextL( filename, CPlainText::EOrganiseByParagraph, 0));
    
    if(error != KErrNone)
         {
         User::Panic (KPanicConnTest, EConnTestContainer);
         }
    }

// End of File  
