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
* Description: This file contains CStartedCasesMenuContainer
* class definition.
*
*/

// INCLUDE FILES
#include "StartedCasesMenuContainer.h"
#include "Stifui.hrh"
#include <Stifui.rsg>

#include <aknlists.h>  //ListBox
#include <barsread.h> // for TResourceReader

#include <aknnotewrappers.h>

#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::ConstructL
// 
// Symbian OS default constructor.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::ConstructL(
                    const TRect& aRect )
    {
    CreateWindowL();

	iListBox = CMenuListBox::NewL(EStartedCasesMenuViewId);
	iListBox->SetContainerWindowL( *this );
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC( reader, R_STARTED_CASES_MENU_LISTBOX );
	iListBox->SetListBoxObserver( this ); // jos peritty MEikListBoxObserver:sta
	//iListBox->SetObserver( this /*iMainMenuObserver*/ ); //jos peritty MCoeControlObserver:sta
	iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); // resource stuffs. 

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    ((CMenuListBox*)iListBox)->SetPreviousFocus();
	iListBox->ActivateL();

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CStartedCasesMenuContainer::~CStartedCasesMenuContainer()
    {
    if ( iListBox )
		{    
    	iListBox->Reset();
    	delete iListBox;
        }	
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CStartedCasesMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CStartedCasesMenuContainer::ComponentControl(TInt aIndex) const
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
// CStartedCasesMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::HandleListBoxEventL
// 
// Handles list box events.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::HandleListBoxEventL(
                                CEikListBox* aListBox,
                                TListBoxEvent aEventType )
	{

	if ( ( aEventType == EEventEnterKeyPressed ) || ( aEventType == EEventItemDoubleClicked ) )
		{
		    HandleSelectedListItemL( aListBox->CurrentItemIndex() );
		}
	}



// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::HandleSelectedListItemL
// 
// Method HandleSelectedListItemL handles valid index.
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::HandleSelectedListItemL( TInt aIndex )
	{	 
		TInt selection = aIndex;

	    switch ( selection )
			{
            case 0: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowAllStartedCases);
				break;
            case 1:	( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowOngoingCases);
				break;
            case 2: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowPausedCases);
				break;
            case 3: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowPassedCases);
				break;
            case 4: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowFailedCases);
				break;
            case 5: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowCrashedAbortedCases);
				break;
            case 6: ( (CEikAppUi*)iCoeEnv->AppUi() )->HandleCommandL(ECmdShowStatistics);
				break;
	        default:
				break;
			}
	}	

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CStartedCasesMenuContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	if (aType != EEventKey)
	    {
		return EKeyWasNotConsumed;
	    }

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
			{   /* Left ja right key eventit valitetaan AppUille joka hoitaa siirtymisen toisiin nakymiin */
			return EKeyWasNotConsumed;
            //break;
            }
        default:
			{
			//Muut Key eventit valitetaan listboxille, joka hoitaa ne
			if (iListBox)
				{
				return iListBox->OfferKeyEventL( aKeyEvent, aType );
				}
			}
        }
    return EKeyWasNotConsumed;        
    }

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer::GetActiveLine
// 
// Get currently selected items index.
// ----------------------------------------------------------------------------
//
TInt CStartedCasesMenuContainer::GetActiveLine()
	{
	return iListBox->CurrentItemIndex();
	}

// ----------------------------------------------------------------------------
// CStartedCasesMenuContainer:::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CStartedCasesMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
