/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains CTestCaseOutputContainer class
* definition.
*
*/

// INCLUDE FILES
#include "TestCaseOutputContainer.h"
#include <Stifui.rsg>
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include <aknlists.h>  // ListBox
#include <AknIconArray.h>  // CAknIconArray
#include <badesca.h> // CDesCArray
#include <eikclbd.h> // CColumnListBoxData
#include <aknnotewrappers.h>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include "MenuListBox.h"

//#include <gdi.h>


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputContainer::ConstructL(const TRect& aRect, CStartedTestCase* aStartedTestCase ) //TInt aExecutedTestCaseCount )
    {
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();
    
	iListBox = CMenuListBox::NewL(ETestCaseOutputViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( this );
    iListBox->ConstructL(this, EAknListBoxSelectionList);

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    if( iUIStore )
        {
        iListBoxModel = CTestOutputListBoxModel::NewL( aStartedTestCase );
        iListBox->Model()->SetItemTextArray( iListBoxModel );
        }
    else
        {
        //User::Leave( syy?? )
        }
	iListBox->ActivateL();
	((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();

    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::~CTestCaseOutputContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestCaseOutputContainer::~CTestCaseOutputContainer()
    {
    // Discard and destroy the font
    //CWindowGc* listBoxGc = iListBox->View()->ItemDrawer()->Gc();
    //listBoxGc->DiscardFont();

    //CWindowGc& gc = SystemGc();
    //gc.DiscardFont();
    
    //iCoeEnv->ScreenDevice()->ReleaseFont(iListBoxFont);

    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestCaseOutputContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestCaseOutputContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
		case 0: 
			return iListBox;
        default:
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::HandleListBoxEventL
// 
// Handles list box events.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
        //TBuf<200> outputLine;
        //CAknNoteDialog* outputLineNote = new (ELeave) CAknNoteDialog;
        //outputLineNote->SetTextL( iListBoxModel->MdcaPoint(iListBox->CurrentItemIndex()) );
        //outputLineNote->RunDlgLD();



        //CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
        //informationNote->SetTextL( iListBoxModel->MdcaPoint(iListBox->CurrentItemIndex()) );
		//informationNote->SetTextPluralityL( ETrue );
        //informationNote->SetTextL( _L("Number of cases: %d") );
        //informationNote->SetTextNumberL( count );
        //informationNote->ExecuteLD();
        }
    }

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CTestCaseOutputContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType != EEventKey)
	{
		return EKeyWasNotConsumed;
	}

	if (iListBox)
		{
		return iListBox->OfferKeyEventL( aKeyEvent, aType );
		}
	else
		{
		return EKeyWasNotConsumed;
		}

    /*
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
			{ */  /* Left ja right key eventit valitetaan AppUille joka hoitaa siirtymisen toisiin nakymiin */
    /*			return EKeyWasNotConsumed;
            break;
            }
        default:
			{
			//Muut Key eventit valitetaan listboxille, joka hoitaa ne
			if (iListBox)
				{
				return iListBox->OfferKeyEventL( aKeyEvent, aType );
				}
			else
				{
				return EKeyWasNotConsumed;
				}
            break;
			}
        }*/
}

// ----------------------------------------------------------------------------
// CTestCaseOutputContainer::OutputUpdateL
// 
// Handles addition of item to list box.
// ----------------------------------------------------------------------------
//
void CTestCaseOutputContainer::OutputUpdateL()
    {
    if ( iListBox )
        {
        iListBox->HandleItemAdditionL();
        iListBox->DrawNow();
        }
    }


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CTestOutputListBoxModel::NewL
// 
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CTestOutputListBoxModel* CTestOutputListBoxModel::NewL(CStartedTestCase* aStartedTestCase /*, HBufC* aBuffer*/)
    {

    CTestOutputListBoxModel* self = new ( ELeave ) CTestOutputListBoxModel();
    CleanupStack::PushL( self );
    //self->ConstructL();
    //self->iBuffer = aBuffer;
    self->iBuffer = HBufC::NewL( 150 );
    self->iStartedTestCase = aStartedTestCase;
    CleanupStack::Pop();
    return self;

    }

// ----------------------------------------------------------------------------
// CTestOutputListBoxModel::ConstructL
// 
// Symbian OS two phased constructor.
// Completes the construction of the object.
// ----------------------------------------------------------------------------
//
void CTestOutputListBoxModel::ConstructL()
    {
    //iBuffer = HBufC::NewLC( KMaxInfoName + KMaxName + 2 );
    }

// ----------------------------------------------------------------------------
// CTestOutputListBoxModel::~CTestCaseOutputContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestOutputListBoxModel::~CTestOutputListBoxModel()
    {
    delete iBuffer;
    }

// ----------------------------------------------------------------------------
// CTestOutputListBoxModel::MdcaCount
// 
// Returns the number of descriptor elements in the array.
// ----------------------------------------------------------------------------
//
TInt CTestOutputListBoxModel::MdcaCount() const
    {

    TInt rows = iStartedTestCase->PrintArray().Count();
    return rows;

    }

// ----------------------------------------------------------------------------
// CTestOutputListBoxModel::MdcaPoint
// 
// Indexes into a descriptor array.
// ----------------------------------------------------------------------------
//
TPtrC CTestOutputListBoxModel::MdcaPoint(TInt aIndex) const
    {
    const RPointerArray<CTestProgress> printArray = iStartedTestCase->PrintArray();

    TPtr buffer( iBuffer->Des() );
    buffer.Zero();
    buffer.Append(_L("\t"));
    buffer.Append(printArray[aIndex]->iDescription);
    buffer.Append(_L(" "));
    buffer.Append(printArray[aIndex]->iText);
    
    return *iBuffer;

    }

// End of File
