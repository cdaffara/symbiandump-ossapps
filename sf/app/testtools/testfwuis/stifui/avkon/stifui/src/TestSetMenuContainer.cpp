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
* Description: This file contains CTestSetMenuContainer class 
* definition.
*
*/

// INCLUDE FILES
#include <aknlists.h>  // ListBox
#include <AknIconArray.h> // CAknIconArray
#include <aknnotewrappers.h>

#include <eikclbd.h> // CColumnListBoxData

#include "TestSetMenuContainer.h"
#include "TestSetMenuView.h"
#include "StartCasesContainer.h"
#include <Stifui.rsg>
#include "Stifui.hrh"
#include "AppUIAppUi.h"

#include "MenuListBox.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CTestSetMenuContainer::ConstructL
// 
// Symbian OS two phased constructor.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver)
    {
    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: ConstructL"));

	iParentView = (CTestSetMenuView*)aListBoxObserver;
    CreateWindowL();

    iUIStore = ( (CAppUIAppUi*)iCoeEnv->AppUi() )->UIStoreHandler();

    iTestCasesInView.Reset();
        //((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: iTCInV Reset"));
    
	iListBox = CMenuListBox::NewL(ETestSetMenuViewId);
	iListBox->SetContainerWindowL( *this );
	iListBox->SetListBoxObserver( aListBoxObserver /*this*/ ); // jos peritty MEikListBoxObserver:sta
    iListBox->ConstructL(this, EAknListBoxMarkableList /*EAknListBoxSelectionList*/);

    ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: ListBox constructed"));

    //Create Scroller control for ListBox and set its visibility
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
                     iListBox->HandleItemAdditionL();   

    if ( iUIStore )
        {
        //RRefArray<const CTestInfo> allCases;
        //CTestSetInfo* testSetInfo;
        //IMPORT_C const CTestSetInfo& TestSetL( const TDesC& aSetName  );
        
        TPtrC ptr = iParentView->CurrentTestSet();
        
        const CTestSetInfo* testSetInfo = 
        	&iUIStore->TestSetL( ptr );
        
        //inline const RRefArray<const CTestInfo>& TestCases() const
   		const RRefArray<const CTestInfo>* allCases = &testSetInfo->TestCases();
        
        /*if( ret != KErrNone )
            {
            ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("iUIStore->TestCases() fails"));
            // Leave
            }
		*/

        const TInt KTestCaseCount = allCases->Count();
        for (TInt i=0; i < KTestCaseCount; i++)
            {
            iTestCasesInView.Append( &allCases->operator[](i) );
            }

        iListBoxModel = CTestCaseListBoxModel::NewL(&iTestCasesInView);
             ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: ListBox model"));

        iListBox->Model()->SetItemTextArray(iListBoxModel);
             ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: ListBox model set"));

        }
    else
        {
        //User::Leave( syy?? )
        }

    // Creates graphic.
    SetGraphicIconL( iListBox );
       ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("TestSetMenuContainer: icons created"));

    iListBox->ActivateL();
    ((CMenuListBox*)iListBox)->SetPreviousFocus();

    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SetGraphicIconL
// 
// Sets graphic icon using listbox as CEikColumnListBox.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::SetGraphicIconL( CEikColumnListBox* aListBox )
    {
	if ( aListBox )
        {
        // Creates gul icon.
        CAknIconArray* iconArray = new(ELeave) CAknIconArray(1);
        CleanupStack::PushL( iconArray );
        
        GraphicIconL( iconArray ); // Appends graphic data.
        
        // Sets graphics as ListBox icon.
        aListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
        
        CleanupStack::Pop();
        }
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::CurrentItemIndex
// 
// Returns current item index in list box.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SetCurrentItemIndex
// 
// Sets current item index in list box.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::SetCurrentItemIndex(TInt aCurrentTestCase)
    {
    iListBox->SetCurrentItemIndexAndDraw(aCurrentTestCase);
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::GraphicIconL
// 
// Appends graphics data.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::GraphicIconL( CArrayPtr<CGulIcon>* aIcons )
    {
    if ( aIcons )
        {
        CFbsBitmap* markBitmap = NULL;
        CFbsBitmap* markBitmapMask = NULL;
	
        TRgb defaultColor;
        defaultColor = CEikonEnv::Static()->Color(EColorControlText);
	
        AknsUtils::CreateColorIconL(AknsUtils::SkinInstance(),
        		KAknsIIDQgnIndiMarkedAdd,
        		KAknsIIDQsnIconColors,
        		EAknsCIQsnIconColorsCG13,
        		markBitmap,
        		markBitmapMask,
        		AknIconUtils::AvkonIconFileName(),
        		EMbmAvkonQgn_indi_marked_add,
        		EMbmAvkonQgn_indi_marked_add_mask,
        		defaultColor );
        
        CGulIcon* markIcon = CGulIcon::NewL(markBitmap,markBitmapMask);
        aIcons->AppendL(markIcon); 
        }
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::~CTestSetMenuContainer
// 
// Destructor.
// ----------------------------------------------------------------------------
//
CTestSetMenuContainer::~CTestSetMenuContainer()
    {
    iTestCasesInView.Close(); // does not delete objects whose pointers are contained in the array
    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SizeChanged
// 
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::SizeChanged()
    {
    if ( iListBox )
		{
		iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
		}
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::CountComponentControls
// 
// Gets a count of the component controls of this list box control.
// ----------------------------------------------------------------------------
//
TInt CTestSetMenuContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::ComponentControl
// 
// Gets a pointer to the specified component control.
// ----------------------------------------------------------------------------
//
CCoeControl* CTestSetMenuContainer::ComponentControl(TInt aIndex) const
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
// CTestSetMenuContainer::Draw
// 
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::Draw(const TRect& /*aRect*/) const
    {
    // CWindowGc& gc = SystemGc();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SelectedTestCases
// 
// Returns pointers to selected test cases.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::SelectedTestCases
                            (RPointerArray<CTestInfo>& aSelectedTestCases)
    {
    aSelectedTestCases.Append( iTestCasesInView[iListBox->CurrentItemIndex()] );
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::OfferKeyEventL
// 
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CTestSetMenuContainer::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType )
    {
	if (aType != EEventKey)
	    {
		return EKeyWasNotConsumed;
	    }

	if (iListBox)
		{
		//if multiple items selected
		if ( iListBox->SelectionIndexes()->Count() > 0 )
			{
			TUint mask = 0x40488;
			
			//if event is enter key,
			//don´t send it to listbox
			if ( aKeyEvent.iScanCode == 0xa7 
				&& ( aKeyEvent.iModifiers & mask ) == 0 )
				{
				iParentView->HandleListBoxEventL( iListBox, MEikListBoxObserver::EEventEnterKeyPressed );
				return EKeyWasConsumed;	
				}
			}
			
		return iListBox->OfferKeyEventL( aKeyEvent, aType );
		}
	else
		{
		return EKeyWasNotConsumed;
		}
    }
    
// ----------------------------------------------------------------------------
// CTestSetMenuContainer::HandleMarkCommandL
// 
// Handles mark commands.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::HandleMarkCommandL( TInt aCommand )
	{
	if (iListBox)
		{
		AknSelectionService::HandleMarkableListProcessCommandL( aCommand, iListBox );		
		}
	}

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::ProcessCommandL
// 
// Processes user commands.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::ProcessCommandL( TInt  aCommand )
	{
	AknSelectionService::HandleMarkableListProcessCommandL( aCommand, iListBox );
    }
    
// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SelectionListProcessCommandL
// 
// Processes user commands.
// ----------------------------------------------------------------------------
//    
void CTestSetMenuContainer::SelectionListProcessCommandL( TInt  aCommand )
	{
	AknSelectionService::HandleSelectionListProcessCommandL( aCommand, iListBox );
    }
    
// ----------------------------------------------------------------------------
// CTestSetMenuContainer::FilterCasesByModuleL
// 
// Show only testcases which are defined is specified module.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::FilterCasesByModuleL( TName aModuleName )
    {
    iTestCasesInView.Reset(); // Clear testCasesInView pointer array
   
    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        if ( allCases[i].ModuleName() == aModuleName )
            {
            iTestCasesInView.Append( &( allCases[i] ) );
            }
        }
    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::FilterCasesByTCFileNameL
// 
// Show only testcases which are defined is specified test case file.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::FilterCasesByTCFileNameL( 
                                                TFileName aTestCaseFileName )
    {

    iTestCasesInView.Reset(); // Clear testCasesInView pointer array
  
    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        if ( allCases[i].TestCaseFile() == aTestCaseFileName )
            {
            iTestCasesInView.Append( &( allCases[i] ) );
            }
        }

    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::NoFilteringL
// 
// Remove possible filtering of test cases -> show all test cases.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::NoFilteringL()
    {
    iTestCasesInView.Reset(); // Clear testCasesInView pointer array

    RRefArray<CTestInfo> allCases;
    TInt ret = iUIStore->TestCases( allCases );
    if( KErrNone != ret )
        {
        ((CAppUIAppUi*)iCoeEnv->AppUi())->iLogger->Log(_L("FilterCasesByModuleL(): iUIStore->TestCases() fails"));
        allCases.Reset();
        allCases.Close();
        User::Leave( ret );
        }
    const TInt KTestCaseCount = allCases.Count();

    // Add all cases to iTestCasesInView pointer array
    for( TInt i=0; i < KTestCaseCount; i++ )
        {
        iTestCasesInView.Append( &( allCases[i] ) );
        }

    allCases.Reset();
    allCases.Close();

    iListBox->Reset();
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::RemoveListBoxItemsL
// 
// Removes items from list box.
// ----------------------------------------------------------------------------
//
void CTestSetMenuContainer::RemoveListBoxItemL( TInt aSelected )
	{
	TInt currentItem(0);
    TBool remCurr(EFalse);
	
	currentItem = iListBox->CurrentItemIndex();
	if( aSelected == currentItem )
	    {
	    remCurr = ETrue;
	    }
	    
	iTestCasesInView.Remove( aSelected );
	AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(iListBox, aSelected, remCurr);
	 iListBox->HandleItemAdditionL();
	}

// ----------------------------------------------------------------------------
// CTestSetMenuContainer::SaveActiveLine
//
// Makes the iListBox member save its focus position
// ----------------------------------------------------------------------------
//	
void CTestSetMenuContainer::SaveActiveLine()
	{
	((CMenuListBox*)iListBox)->SaveFocusPosition();
	}

// End of File
