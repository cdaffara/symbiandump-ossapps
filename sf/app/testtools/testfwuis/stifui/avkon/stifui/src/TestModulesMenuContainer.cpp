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
* Description: This file contains CTestModulesListBoxModel class
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>  // ListBox
#include <barsread.h> // for TResourceReader
#include <aknnotewrappers.h>

#include "Stifui.hrh"
#include <Stifui.rsg>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include "TestModulesMenuContainer.h"
#include "AppUIAppUi.h"

#include "MenuListBox.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestModulesListBoxModel::NewL
// 
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CTestModulesListBoxModel* CTestModulesListBoxModel::NewL(RRefArray<TDesC> aTestModules)
    {

    CTestModulesListBoxModel* self = new ( ELeave ) CTestModulesListBoxModel();
    CleanupStack::PushL( self );
    self->iBuffer = HBufC::NewL( 130 );
    self->iTestModules = aTestModules;
    CleanupStack::Pop();
    return self;

    }

// ----------------------------------------------------------------------------
// CTestModulesListBoxModel::ConstructL
// 
// Symbian OS two phased constructor.
// Completes the construction of the object.
// ----------------------------------------------------------------------------
//
void CTestModulesListBoxModel::ConstructL()
    {
    //iBuffer = HBufC::NewLC( KMaxInfoName + KMaxName + 2 );

    }

// ----------------------------------------------------------------------------
// CTestModulesListBoxModel::~CTestCaseOutputView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestModulesListBoxModel::~CTestModulesListBoxModel()
    {
    delete iBuffer;
    
    }

// ----------------------------------------------------------------------------
// CTestModulesListBoxModel::MdcaCount
// 
// Returns the number of descriptor elements in the array.
// ----------------------------------------------------------------------------
//
TInt CTestModulesListBoxModel::MdcaCount() const
    {
    return iTestModules.Count();
    }

// ----------------------------------------------------------------------------
// CTestModulesListBoxModel::MdcaPoint
// 
// Indexes into a descriptor array.
// ----------------------------------------------------------------------------
//
TPtrC CTestModulesListBoxModel::MdcaPoint(TInt aIndex) const
    {
    TPtrC testModule = (iTestModules)[aIndex];
    TPtr buffer( iBuffer->Des() );
    buffer.Zero();

    buffer.Append(_L("\t"));
    buffer.Append( testModule );
    
    return *iBuffer;
    
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    _LIT( KErrMessage, "Error loading modules! Check Test engine log.");
    CreateWindowL();

  	iListBox = CMenuListBox::NewL(ETestModulesMenuViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver /*this*/ ); // jos peritty MEikListBoxObserver:sta
    iListBox->ConstructL(this, EAknListBoxSelectionList /*EAknListBoxMarkableList*/ );
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox constructed"));

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    if( iUIStore )
        {
        /*const CFixedFlatArray<TTestInfo>& allCases = iData->AllCases();
        const TInt KTestCaseCount = allCases.Count();
        for (TInt i=0; i < KTestCaseCount; i++)
            {
            iTestCasesInView.Append(&(allCases[i]));
            } */

        //RRefArray<TDesC> modules;
        TInt ret = iUIStore->Modules( iModules );
        if( ret != KErrNone )
            {
             //iModules.Reset();
             //iModules.Close();
			TMessageBoxUtil::ShowErrorNoteL( KErrMessage );
            }
       
        iListBoxModel = CTestModulesListBoxModel::NewL( iModules );
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox model"));


         iListBox->Model()->SetItemTextArray(iListBoxModel);
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("StartCasesContainer: ListBox model set"));
        
        }
    else
        {
        // General error becouse UIStore should be opened in AppUI when
        // program starts.
        User::Leave( KErrGeneral );
        }

    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();
    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::~CTestCaseOutputView
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestModulesMenuContainer::~CTestModulesMenuContainer()
    {
    
		iModules.Reset();
        iModules.Close();

	if( iListBox )
		{
		delete iListBox;
		iListBox = NULL;
		}
		
	}

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}

    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestModulesMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container

    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestModulesMenuContainer::ComponentControl(TInt aIndex) const
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
// CTestModulesMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestModulesMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CTestModulesMenuContainer::OfferKeyEventL(
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
// CTestModulesMenuContainer::CurrentItemIndex
// 
// Returns current item index in list box.
// ----------------------------------------------------------------------------
//
TInt CTestModulesMenuContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex();

    }

// ----------------------------------------------------------------------------
// CTestModulesMenuContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CTestModulesMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
