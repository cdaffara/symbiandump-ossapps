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
* Description: This file contains CStartedTestsListBoxModel class
* definition.
*
*/

// INCLUDE FILES

#include <aknlists.h>  // ListBox
#include <AknIconArray.h>  // CAknIconArray
#include <badesca.h> // CDesCArray
#include <eikclbd.h> // CColumnListBoxData
#include <aknnotewrappers.h>

#include <Stifui.rsg>

//#include "UIStoreIf.h"         
//#include "UIStore.h"            
//#include "UIEngine.h"           
//#include "UIEngineContainer.h"  

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>

#include "ShowStartedCasesContainer.h"

#include "Stifui.hrh"
#include "AppUIAppUi.h"
#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CStartedTestsListBoxModel::NewL
// 
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
CStartedTestsListBoxModel* CStartedTestsListBoxModel::NewL 
            ( RRefArray<CStartedTestCase>* aTestCasesInView )
    {
    CStartedTestsListBoxModel* self = new ( ELeave ) CStartedTestsListBoxModel();
    CleanupStack::PushL( self );
    self->iBuffer = HBufC::NewL( 130 );
    self->iTestCasesInView = aTestCasesInView;
    CleanupStack::Pop();
    return self;

    }

// ---------------------------------------------------------
// CStartedTestsListBoxModel::ConstructL
// 
// Symbian OS two phased constructor
// Completes the construction of the object.
// ---------------------------------------------------------
//

void CStartedTestsListBoxModel::ConstructL()
    {
    }

// ---------------------------------------------------------
// CStartedTestsListBoxModel::~CStartedTestsListBoxModel
// 
// Destructor
// ---------------------------------------------------------
//

CStartedTestsListBoxModel::~CStartedTestsListBoxModel()
    {
    }

// ---------------------------------------------------------
// CStartedTestsListBoxModel::MdcaCount
// 
// Returns the number of descriptor elements in the array.
// ---------------------------------------------------------
//

TInt CStartedTestsListBoxModel::MdcaCount() const
    {
    return iTestCasesInView->Count();

    }

// ---------------------------------------------------------
// CStartedTestsListBoxModel::MdcaPoint
// 
// Indexes into a descriptor array.
// ---------------------------------------------------------
//
TPtrC CStartedTestsListBoxModel::MdcaPoint(TInt aIndex) const
    {
    TPtr buffer( iBuffer->Des() );
    buffer.Zero();
    switch ( (*iTestCasesInView)[aIndex].Status() )
        {
        case CUIStoreIf::EStatusRunning:
            {
            if( (*iTestCasesInView)[aIndex].UIEngineContainer().State() ==
                CUIEngineContainer::ERunning )
                {
                buffer.Append(_L("Running\t"));
                }
            else
                {
                buffer.Append(_L("Paused\t"));
                }
            break;
            }
        case CUIStoreIf::EStatusPassed:
            {
            buffer.Append(_L("Passed\t"));
            break;
            }
        case CUIStoreIf::EStatusFailed:
            {
            buffer.Append(_L("Failed\t"));
            break;
            }
        /*case ECrashed:
            buffer.Append(_L("Crashed\t"));
            break;*/
        case CUIStoreIf::EStatusAborted:
        	{
            buffer.Append(_L("Aborted\t"));
            break;
        	}
        default:
        	{
            if((*iTestCasesInView)[aIndex].Status() & CUIStoreIf::EStatusPassed)
            	{
            	buffer.Append(_L("Passed\t"));
            	}
        	else if((*iTestCasesInView)[aIndex].Status() & CUIStoreIf::EStatusFailed)
            	{
            	buffer.Append(_L("Failed\t"));
            	}
        	else if((*iTestCasesInView)[aIndex].Status() & CUIStoreIf::EStatusAborted)
            	{
            	buffer.Append(_L("Aborted\t"));
            	}
        	else if((*iTestCasesInView)[aIndex].Status() & CUIStoreIf::EStatusCrashed)
	        	{
	            buffer.Append(_L("Crashed\t"));
	        	}
        	else
        		{
        		buffer.Append(_L("\t"));
        		}
        	break;
        	}
        }
        
    buffer.Append((*iTestCasesInView)[aIndex].TestInfo().TestCaseTitle() );
    
    return *iBuffer;
    
    }



// ---------------------------------------------------------
// CShowStartedCasesContainer::ConstructL
// 
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    const TInt KSelectedMode = ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;
    
    if(KSelectedMode == EShowOngoingCases || KSelectedMode == EShowPausedCases
    		|| KSelectedMode == EShowCrashedAbortedCases  || KSelectedMode == EShowAllStartedCases)
    	{
    	iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
    	}
    else
    	{
    	iListBox = CMenuListBox::NewL(EShowStartedCasesViewId); //CAknSingleStyleListBox();
    	}
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver );
    iListBox->ConstructL(this, EAknListBoxSelectionList);

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    if ( iUIStore )
        {
        ConstructListBoxModelL();
        iListBoxModel = CStartedTestsListBoxModel::NewL( &iStartedTestsPtrs );
        iListBox->Model()->SetItemTextArray(iListBoxModel);
        }
    else
        {
        //User::Leave( syy?? )
        }
	iListBox->ActivateL();
	if(KSelectedMode != EShowOngoingCases && KSelectedMode != EShowPausedCases
	    	&& KSelectedMode != EShowCrashedAbortedCases  && KSelectedMode != EShowAllStartedCases)
		{
		((CMenuListBox*)iListBox)->SetPreviousFocus();
		}

    SetRect(aRect);
    ActivateL();

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::ConstructListBoxModelL
// 
// Constructs list box model without any filtering.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::ConstructListBoxModelL()
    {
    TInt ret( 0 );
    RRefArray<CStartedTestCase> startedTestCases;
    ret = iUIStore->StartedTestCases( startedTestCases );
    if( KErrNone != ret )
        {
        startedTestCases.Reset();
        startedTestCases.Close();
        User::Leave( ret );
        }
    CleanupClosePushL( startedTestCases ); // Closes the handle

    const TInt KStartedCaseCount = startedTestCases.Count();

    iStartedTestsPtrs.Reset(); // Remove all pointers (does not delete objects)

    const TInt KSelectedMode = ((CAppUIAppUi*)iCoeEnv->AppUi())->iShowStartedCasesMode;

    // Loop through all started cases
    for( TInt i=0; i < KStartedCaseCount; i++ )
        {
        switch ( KSelectedMode )
            {
            case EShowAllStartedCases:
                {
                iStartedTestsPtrs.Append( startedTestCases[i] );
                break;
                }
            case EShowOngoingCases:
                {
                // Note: PAUSE IS ALSO RUNNIN STATUS
                if ( startedTestCases[i].Status() & CUIStoreIf::EStatusRunning )
                    {
                    iStartedTestsPtrs.Append(startedTestCases[i]);
                    }
                break;
                }
            case EShowPassedCases:
                {
                if ( startedTestCases[i].Status() & CUIStoreIf::EStatusPassed )
                    {
                    iStartedTestsPtrs.Append( startedTestCases[i] );
                    }
                break;
                }
            case EShowPausedCases:
                {
                if ( startedTestCases[i].Status() & CUIStoreIf::EStatusRunning )
                    {
                    if( startedTestCases[i].UIEngineContainer().State() == CUIEngineContainer::EPaused )
                        {
                        iStartedTestsPtrs.Append( startedTestCases[i] );
                        }
                    }
                break;
                }
            case EShowFailedCases:
                {
                if ( startedTestCases[i].Status() & CUIStoreIf::EStatusFailed )
                    {
                    iStartedTestsPtrs.Append( startedTestCases[i] );
                    }
                break;
                }
            case EShowCrashedAbortedCases:
                {
                if ( startedTestCases[i].Status() & CUIStoreIf::EStatusAborted 
                		|| startedTestCases[i].Status() & CUIStoreIf::EStatusCrashed)
                    {
                    iStartedTestsPtrs.Append( startedTestCases[i] );
                    }
                break;
                }
            }
        }
    startedTestCases.Reset();
    startedTestCases.Close();

    CleanupStack::PopAndDestroy(); // startedTestCases

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::~CShowStartedCasesContainer
// 
// Destructor
// ---------------------------------------------------------
//
CShowStartedCasesContainer::~CShowStartedCasesContainer()
    {
    iStartedTestsPtrs.Close(); // does not delete objects whose pointers are contained in the array
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::SizeChanged
// 
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::CountComponentControls
//
// Gets a count of the component controls of this list box control.
// ---------------------------------------------------------
//
TInt CShowStartedCasesContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::ComponentControl
//
// Gets a pointer to the specified component control.
// ---------------------------------------------------------
//
CCoeControl* CShowStartedCasesContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
		case 0: 
			return iListBox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::Draw
// 
// Draw a control, called by window server.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::OfferKeyEventL
// 
// Handles key events.
// ---------------------------------------------------------
//
TKeyResponse CShowStartedCasesContainer::OfferKeyEventL(
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
		
	}
	
// ---------------------------------------------------------
// CShowStartedCasesContainer::SelectedTestCase
// 
// Returns reference to currently selected test case in view (listbox).
// ---------------------------------------------------------
//
CStartedTestCase* CShowStartedCasesContainer::SelectedTestCase()
    {
    if ( iStartedTestsPtrs.Count() > 0 )
        {
        return &(iStartedTestsPtrs[ iListBox->CurrentItemIndex() ]);
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::DrawListBox
// 
// Refresh ListBox, if aSelectedTestCase is still found from ListBox
// it is set as selected test case.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::DrawListBox()
    {
    if ( iListBox )
        {
        iListBox->DrawNow();
        }

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::HandleItemAdditionL
// 
// Refresh ListBox after new item was added to listbox model.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::HandleItemAdditionL()
    {
    iListBox->HandleItemAdditionL();

    }


// ---------------------------------------------------------
// CShowStartedCasesContainer::HandleItemRemovalL
// 
// Refresh ListBox after item is removed from listbox model.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::HandleItemRemovalL()
    {
    iListBox->HandleItemRemovalL();

    // HandleItemRemovalL "loses selection" if current item is removed
    // -> we have to check it and set one item as current item to make it possible for
    // user to select one item from items left after remove
    if ( iListBox->CurrentItemIndex() == -1 ) // No item selected
        if ( ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount() > 0 ) // there are items
            SetCurrentItemIndex(0);

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::TestsInViewCount
// 
// Returns count of test cases in view.
// ---------------------------------------------------------
//
TInt CShowStartedCasesContainer::TestsInViewCount()
    {
    return ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount();

    }
    
// ---------------------------------------------------------
// CShowStartedCasesContainer::CurrentTestCase
// 
// Returns pointer to currently selected test case.
// ---------------------------------------------------------
//
CStartedTestCase* CShowStartedCasesContainer::CurrentTestCase()
    {
    return &iStartedTestsPtrs[ iListBox->CurrentItemIndex() ];
    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::SetCurrentItemIndex
// 
// Sets the current item.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::SetCurrentItemIndex(TInt aIndex)
    {
    TInt itemCount = ((CStartedTestsListBoxModel*)iListBox->Model())->MdcaCount();
    if ( aIndex < itemCount )
        {
        iListBox->SetCurrentItemIndex(aIndex);
        }
    else
        {
        iListBox->SetCurrentItemIndex(itemCount);
        }

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::ResetListBox
// 
// Resets the selection indices, top and current item indices,
// the selection, and the horizontal scroll offset of this list box.
// This function does not redraw the list box.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::ResetListBox()
    {
    iListBox->Reset();

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::RemoveSelectedExecutionsL
// 
// Removes items from list box.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::RemoveSelectedExecutionsL()
    {
    TInt indexOfRemovedExecution = 0;
    iStartedTestsPtrs.Remove(indexOfRemovedExecution);
    HandleItemRemovalL();

    }

// ---------------------------------------------------------
// CShowStartedCasesContainer::RemoveAllExecutionsInViewL
// 
// Removes all started test cases from list box.
// ---------------------------------------------------------
//
void CShowStartedCasesContainer::RemoveAllExecutionsInViewL()
    {
    iStartedTestsPtrs.Reset(); // Empties the array, does not delete the objects whose pointers are contained in the array
    HandleItemRemovalL();
    }

// ----------------------------------------------------------------------------
// CContainer::FilterCasesByModuleL
// 
// Show only testcases which are defined is specified module.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesContainer::FilterCasesByModuleL(TName aModuleName)
    {
    //First the list box model have to be reconstructed to make sure that all
    //possible earlier filtering does not affect
    ConstructListBoxModelL();
    
    TInt i;
    const TInt KCurrentShowedCaseCount = iStartedTestsPtrs.Count();

    for ( i = KCurrentShowedCaseCount-1; i >= 0; i-- )
        {
        RRefArray<CStartedTestCase> startedTestCases;
        TInt ret = iUIStore->StartedTestCases( startedTestCases );
        if( ret != KErrNone )
            {
            startedTestCases.Reset();
            startedTestCases.Close();
            }
        if ( startedTestCases[i].TestInfo().ModuleName() != aModuleName )
            {
            iStartedTestsPtrs.Remove(i); // delete pointer to CStartedTestCase
            }
        startedTestCases.Reset();
        startedTestCases.Close();
        }
    }

// ----------------------------------------------------------------------------
// CContainer::FilterCasesByTCFileNameL
// 
// Show only testcases which are defined is specified test case file.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesContainer::FilterCasesByTCFileNameL(TFileName aTestCaseFileName)
    {

    //First the list box model have to be reconstructed to make sure that all
    //possible earlier filtering does not affect
    ConstructListBoxModelL();

    const TInt KCurrentShowedCaseCount = iStartedTestsPtrs.Count();

    for ( TInt i = KCurrentShowedCaseCount-1; i >= 0; i-- )
        {
        RRefArray<CStartedTestCase> startedTestCases;
        TInt ret = iUIStore->StartedTestCases( startedTestCases );
        if( ret != KErrNone )
            {
            startedTestCases.Reset();
            startedTestCases.Close();
            }
        if ( startedTestCases[i].TestInfo().TestCaseFile() != aTestCaseFileName )
            {
            iStartedTestsPtrs.Remove(i); // delete pointer to CStartedTestCase
            }
        startedTestCases.Reset();
        startedTestCases.Close();
        }
    }

// ----------------------------------------------------------------------------
// CContainer::NoFilteringL
// 
// Remove possible filtering of test cases -> show all test cases.
// ----------------------------------------------------------------------------
//
void CShowStartedCasesContainer::NoFilteringL()
    {
    ConstructListBoxModelL();
    }

// ----------------------------------------------------------------------------
// CShowStartedCasesContainer:::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//
void CShowStartedCasesContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
