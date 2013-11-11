/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CGlxMediaList unit tests
*
*/



//  CLASS HEADER
#include "ut_cglxmedialist.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>

#include <mpxcommandgeneraldefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>
#include <mpxcollectionmessage.h>
#include <glxcommandgeneraldefs.h>
#include <glxdrmutility.h>

//  INTERNAL INCLUDES
#include "glxmedialist.h"
#include "glxattributecontext.h"

_LIT(KTitle, "Albums");

const TInt KTestCollectionPluginUid = 0x20007195;
const TInt KTestAlbumCollectionPluginUid = 0x2000718B;
const TInt KFavouritesAlbumId = 3000;
const TInt KFavouritesAlbumFirstItemId = 3009;
const TInt KProgressTotalCount = 10;

const TMPXAttribute KProgressCountAttr(KMPXMessageContentIdProgress, EMPXMessageProgressCurrentCount);
const TMPXAttribute KProgressTotalCountAttr(KMPXMessageContentIdProgress, EMPXMessageProgressTotalCount);

void CGlxDRMUtility::Close()
    {
    delete this;
    }
    
TBool CGlxDRMUtility::ItemRightsValidityCheckL(const TDesC&, TBool)
    {
    return ETrue;
    }
    
CGlxDRMUtility* CGlxDRMUtility::InstanceL()
    {
    CGlxDRMUtility* drm = new(ELeave)CGlxDRMUtility();
    return drm;
    }
    
TSize CGlxDRMUtility::DRMThumbnailSize(TSize& aSize)
    {
    return aSize;
    }
    
CGlxDRMUtility::CGlxDRMUtility()
    {
    
    }
    
CGlxDRMUtility::~CGlxDRMUtility()
    {
    
    }

// CONSTRUCTION
UT_CGlxMediaList* UT_CGlxMediaList::NewL()
    {
    UT_CGlxMediaList* self = UT_CGlxMediaList::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxMediaList* UT_CGlxMediaList::NewLC()
    {
    UT_CGlxMediaList* self = new( ELeave ) UT_CGlxMediaList();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxMediaList::~UT_CGlxMediaList()
    {
    }

// Default constructor
UT_CGlxMediaList::UT_CGlxMediaList()
    {
    }

// Second phase construct
void UT_CGlxMediaList::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CGlxMediaList::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    iSchedulerWait->AsyncStop();
    }

void UT_CGlxMediaList::HandleAttributesAvailableL(TInt /*aItemIndex*/, const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
    iSchedulerWait->AsyncStop();
    }

void UT_CGlxMediaList::SetupL(  )
    {
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);

    path->AppendL(KTestCollectionPluginUid);
    path->AppendL(KFavouritesAlbumId);

    iCGlxMediaList = CGlxMediaList::InstanceL(*path, NULL);
    CleanupStack::PopAndDestroy(); // path

    iCGlxMediaList->AddMediaListObserverL(this);

    iContext = CGlxDefaultAttributeContext::NewL();
    iContext->AddAttributeL(KMPXMediaGeneralTitle);

    iCGlxMediaList->AddContextL(iContext, 0);
    

    iSchedulerWait = new (ELeave) CActiveSchedulerWait();
    iSchedulerWait->Start(); // wait for Media list to be populated (HandleItemAddedL will be called)
    }

void UT_CGlxMediaList::Teardown(  )
    {
    iCGlxMediaList->RemoveContext(iContext);
    delete iContext;
    iContext = NULL;

    iCGlxMediaList->RemoveMediaListObserver(this);
    iCGlxMediaList->Close();

    delete iSchedulerWait;
    iSchedulerWait = NULL;
    }

void UT_CGlxMediaList::UT_CGlxMediaList_InstanceLL(  )
    {
    // Obtain instance of media list to existing path with filter applied
    CMPXCollectionPath* existingPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL(existingPath);
    

    existingPath->AppendL(KTestCollectionPluginUid);
    existingPath->AppendL(KFavouritesAlbumId);
    

    CMPXFilter* filter1 = CMPXFilter::NewL();
    CleanupStack::PushL(filter1);

    EUNIT_PRINT(_L("Obtain instance of media list to existing path with filter applied"));
    CGlxMediaList* filteredCGlxMediaList1 = CGlxMediaList::InstanceL(*existingPath, KGlxIdNone, filter1);

    // filter ownership passed to media list
    CleanupStack::Pop(filter1);

    CleanupClosePushL(*filteredCGlxMediaList1);

    // Check instance is different
    EUNIT_PRINT(_L("Check instance is different"));
    EUNIT_ASSERT(filteredCGlxMediaList1 != iCGlxMediaList);

    // Obtain instance of media list with different path
    CMPXCollectionPath* differentPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL(differentPath);

    differentPath->AppendL(KTestAlbumCollectionPluginUid);
    differentPath->AppendL(KFavouritesAlbumId);

    EUNIT_PRINT(_L("Obtain instance of media list with different path"));
    CGlxMediaList* cGlxMediaList1 = CGlxMediaList::InstanceL(*differentPath, NULL);

    CleanupClosePushL(*cGlxMediaList1);

    // Check instance is different
    EUNIT_PRINT(_L("Check instance is different"));
    EUNIT_ASSERT(cGlxMediaList1 != iCGlxMediaList);

    // Obtain instance of media list to existing path with filter applied
    CMPXFilter* filter2 = CMPXFilter::NewL();
    CleanupStack::PushL(filter2);

    EUNIT_PRINT(_L("Obtain instance of media list to existing path with filter applied"));
    CGlxMediaList* filteredCGlxMediaList2 = CGlxMediaList::InstanceL(*existingPath, KGlxIdNone, filter2);

    // filter ownership passed to media list
    CleanupStack::Pop(filter2);

    CleanupClosePushL(*filteredCGlxMediaList2);

    // Check instance is different
    EUNIT_PRINT(_L("Check instance is different"));
    EUNIT_ASSERT(filteredCGlxMediaList2 != filteredCGlxMediaList1) 

    // Obtain instance of media list with same path
    EUNIT_PRINT(_L("Obtain instance of media list with same path"));
    CGlxMediaList* cGlxMediaList2 = CGlxMediaList::InstanceL(*existingPath, NULL);

    CleanupClosePushL(*cGlxMediaList2);

    // Check instance is the same
    EUNIT_PRINT(_L("Check instance is the same"));
    EUNIT_ASSERT(cGlxMediaList2 == iCGlxMediaList);

    CleanupStack::PopAndDestroy(cGlxMediaList2);
    CleanupStack::PopAndDestroy(filteredCGlxMediaList2);
    CleanupStack::PopAndDestroy(cGlxMediaList1);
    CleanupStack::PopAndDestroy(differentPath);
    CleanupStack::PopAndDestroy(filteredCGlxMediaList1);
    CleanupStack::PopAndDestroy(existingPath);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_MediaListsL(  )
    {
    // Check media lists array
    EUNIT_PRINT(_L("Check media lists array"));
    RPointerArray<CGlxMediaList>& mediaLists = CGlxMediaList::MediaListsL();

    // Check media lists initial count
    TInt mediaListCount = mediaLists.Count();
    EUNIT_PRINT(_L("Check media lists initial count"));
    EUNIT_ASSERT(mediaListCount == 1);

    // Check media lists initial item
    EUNIT_PRINT(_L("Check media lists initial item"));
    EUNIT_ASSERT(mediaLists[0] == iCGlxMediaList);

    // Obtain instance of media list with different path
    CMPXCollectionPath* differentPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL(differentPath);

    differentPath->AppendL(KTestAlbumCollectionPluginUid);
    differentPath->AppendL(KFavouritesAlbumId);

    EUNIT_PRINT(_L("Obtain instance of media list with different path"));
    CGlxMediaList* cGlxMediaList1 = CGlxMediaList::InstanceL(*differentPath, NULL);

    CleanupClosePushL(*cGlxMediaList1);

    // Check media lists count
    mediaListCount = mediaLists.Count();
    EUNIT_PRINT(_L("Check media lists count"));
    EUNIT_ASSERT(mediaListCount == 2);

    // Check media lists items
    EUNIT_PRINT(_L("Check media lists items"));
    EUNIT_ASSERT((mediaLists[0] == cGlxMediaList1) && (mediaLists[1] == iCGlxMediaList));

    // Obtain instance of media list to existing path
    CMPXCollectionPath* existingPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL(existingPath);

    existingPath->AppendL(KTestCollectionPluginUid);
    existingPath->AppendL(KFavouritesAlbumId);

    EUNIT_PRINT(_L("Obtain instance of media list with existing path"));
    CGlxMediaList* cGlxMediaList2 = CGlxMediaList::InstanceL(*existingPath, NULL);

    CleanupClosePushL(*cGlxMediaList2);

    // Check media lists count
    mediaListCount = mediaLists.Count();
    EUNIT_PRINT(_L("Check media lists count"));
    EUNIT_ASSERT(mediaListCount == 2);

    // Check media lists items
    EUNIT_PRINT(_L("Check media lists items"));
    EUNIT_ASSERT((mediaLists[1] == cGlxMediaList1) && (mediaLists[0] == iCGlxMediaList));
    
    CleanupStack::PopAndDestroy(cGlxMediaList2);
    CleanupStack::PopAndDestroy(existingPath);
    CleanupStack::PopAndDestroy(cGlxMediaList1);
    CleanupStack::PopAndDestroy(differentPath);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_NewLCL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_OpenLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_AddReferenceL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_RemoveReferenceL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_ReferenceCountL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_EqualsL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_OfferMediaL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_HandleAttributesAvailableLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_AttributeRequestLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_CloseL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_IdL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_CountL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_FocusIndexL(  )
    {
    // Setup test
    SetupFocusTestL();

    // Check focus is index 3
    EUNIT_PRINT(_L("Check focus is index 3"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 3);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_SetFocusLL(  )
    {
    // Setup test
    SetupFocusTestL();

    MoveFocusAbsoluteGreaterThanCurrentFocusL();
    MoveFocusAbsoluteLessThanCurrentFocusL();
    MoveFocusRelativeForwardL();
    MoveFocusRelativeBackwardL();
    MoveFocusRelativeForwardGreaterThanListCountL();
    MoveFocusRelativeBackwardLessThanZeroL();
    }

void UT_CGlxMediaList::UT_CGlxMediaList_ItemL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_IndexL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_AddMediaListObserverLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_RemoveMediaListObserverL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_AddContextLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_RemoveContextL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_CollectionL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_PathLCL(  )
    {
    // Setup test
    SetupPathTestL();

    CMPXCollectionPath* path = iCGlxMediaList->PathLC();

    // Check path has 3 levels
    EUNIT_PRINT(_L("Check path has 3 levels"));
    EUNIT_ASSERT(path->Levels() == 3);

    // Check level 0 is test collection plugin uid
    EUNIT_PRINT(_L("Check level 0 is test collection plugin uid"));
    EUNIT_ASSERT(path->Id(0) == KTestCollectionPluginUid);

    // Check level 1 is favourites album id
    EUNIT_PRINT(_L("Check level 1 is favourites album id"));
    EUNIT_ASSERT(path->Id(1) == KFavouritesAlbumId);

    // Check level 2 is favourites album first item id
    EUNIT_PRINT(_L("Check level 2 is favourites album first item id"));
    EUNIT_ASSERT(path->Id(2) == KFavouritesAlbumFirstItemId);

    // Check path item count
    TInt pathCount = path->Count();
    TInt listCount = iCGlxMediaList->Count();
    EUNIT_PRINT(_L("Check path item count"));
    EUNIT_ASSERT(pathCount == listCount);

    // Check path items
    EUNIT_PRINT(_L("Check path items"));
    for (TInt count = 0; count < pathCount; ++count)
        {
        EUNIT_ASSERT(path->IdOfIndex(count) == TMPXItemId(iCGlxMediaList->Item(count).Id().Value()));
        }

    // Check path selection count
    TArray<TInt> pathSelection = path->Selection();
    TInt pathSelectionCount = pathSelection.Count();
    TInt listSelectionCount = iCGlxMediaList->SelectionCount();
    EUNIT_PRINT(_L("Check path selection count"));
    EUNIT_ASSERT(pathSelectionCount == listSelectionCount);

    // Check path selected items
    EUNIT_PRINT(_L("Check path selected items"));
    for (TInt count = 0; count < pathSelectionCount; ++count)
        {
        EUNIT_ASSERT(pathSelection[count] == iCGlxMediaList->SelectedItemIndex(count));
        }

    // Check focused item
    EUNIT_PRINT(_L("Check focused item"));
    EUNIT_ASSERT(path->Id() == TMPXItemId(iCGlxMediaList->Item(iCGlxMediaList->FocusIndex()).Id().Value()));

    CleanupStack::PopAndDestroy(path);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_IsSelectedL(  )
    {
    // Setup test
    SetupSelectionTestL();

    TBool isSelected = EFalse;

    // Check 2 is selected
    EUNIT_PRINT(_L("Check 2 is selected"));
    isSelected = iCGlxMediaList->IsSelected(2);
    EUNIT_ASSERT(isSelected == ETrue);

    // Check 3 is not selected
    EUNIT_PRINT(_L("Check 3 is not selected"));
    isSelected = iCGlxMediaList->IsSelected(3);
    EUNIT_ASSERT(isSelected == EFalse);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_SetSelectedLL(  )
    {
    // Setup test
    SetupSelectionTestL();

    // Select 1 (insert at beginning)
    EUNIT_PRINT(_L("Select 1"));
    iCGlxMediaList->SetSelectedL(1, ETrue);

    RArray<TInt> selection;
    CleanupClosePushL(selection);

    TInt selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 1, 2 and 4 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 3 && selection[0] == 1 && selection[1] == 2 && selection[2] == 4);

    // Select 3 (insert in middle)
    EUNIT_PRINT(_L("Select 3"));
    iCGlxMediaList->SetSelectedL(3, ETrue);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 1, 2, 3 and 4 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 4 && selection[0] == 1 && selection[1] == 2 && selection[2] == 3 && selection[3] == 4);

    // Select 3 again (already selected)
    EUNIT_PRINT(_L("Select 3 again"));
    iCGlxMediaList->SetSelectedL(3, ETrue);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 1, 2, 3 and 4 is still selected in order"));
    EUNIT_ASSERT(selection.Count() == 4 && selection[0] == 1 && selection[1] == 2 && selection[2] == 3 && selection[3] == 4);

    // Select 5 (insert at end)
    EUNIT_PRINT(_L("Select 5"));
    iCGlxMediaList->SetSelectedL(5, ETrue);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 1, 2, 3, 4 and 5 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 5 && selection[0] == 1 && selection[1] == 2 && selection[2] == 3 && selection[3] == 4 && selection[4] == 5);

    // Deselect 1 (remove from beginning)
    EUNIT_PRINT(_L("Deselect 1"));
    iCGlxMediaList->SetSelectedL(1, EFalse);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 2, 3, 4 and 5 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 4 && selection[0] == 2 && selection[1] == 3 && selection[2] == 4 && selection[3] == 5);

    // Deselect 3 (remove from middle)
    EUNIT_PRINT(_L("Deselect 3"));
    iCGlxMediaList->SetSelectedL(3, EFalse);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 2, 4 and 5 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 3 && selection[0] == 2 && selection[1] == 4 && selection[2] == 5);

    // Deselect 3 again (already removed)
    EUNIT_PRINT(_L("Deselect 3 again"));
    iCGlxMediaList->SetSelectedL(3, EFalse);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 2, 4 and 5 is still selected in order"));
    EUNIT_ASSERT(selection.Count() == 3 && selection[0] == 2 && selection[1] == 4 && selection[2] == 5);

    // Deselect 5 (remove from end)
    EUNIT_PRINT(_L("Deselect 5"));
    iCGlxMediaList->SetSelectedL(5, EFalse);

    selection.Reset();
    selectionCount = iCGlxMediaList->SelectionCount();
    for (TInt count = 0; count < selectionCount; ++count)
        {
        selection.AppendL(iCGlxMediaList->SelectedItemIndex(count));
        }

    EUNIT_PRINT(_L("Check 2 and 4 is selected in order"));
    EUNIT_ASSERT(selection.Count() == 2 && selection[0] == 2 && selection[1] == 4);

    CleanupStack::PopAndDestroy(&selection);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_SelectionCountL(  )
    {
    // Setup test
    SetupSelectionTestL();

    // Check selection count is 2
    TInt selectionCount = iCGlxMediaList->SelectionCount();

    EUNIT_PRINT(_L("Check selection count is 2"));
    EUNIT_ASSERT(selectionCount == 2);
    }

void UT_CGlxMediaList::UT_CGlxMediaList_SelectedItemIndexL(  )
    {
    // Setup test
    SetupSelectionTestL();

    // Check selected item index for selection index 0 is 2
    TInt selectedItemIndex = iCGlxMediaList->SelectedItemIndex(0);

    EUNIT_PRINT(_L("Check selected item index for selection index 0 is 2"));
    EUNIT_ASSERT(selectedItemIndex == 2);

    // Check selected item index for selection index 1 is 4
    selectedItemIndex = iCGlxMediaList->SelectedItemIndex(1);

    EUNIT_PRINT(_L("Check selected item index for selection index 1 is 4"));
    EUNIT_ASSERT(selectedItemIndex == 4);

    // Check selected item index for out of bounds selection index is KErrNotFound
    // SelectedItemIndex does not return KErrNotFound.
	EUNIT_ASSERT_PANIC_DESC( selectedItemIndex = iCGlxMediaList->SelectedItemIndex(2) ,"USER",130,"Index out of bounds" );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_CommandLL(  )
    {
    CMPXCommand* addContainerCommand = CMPXCommand::NewL();
    CleanupStack::PushL(addContainerCommand);

    addContainerCommand->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdAdd);
    addContainerCommand->SetTObjectValueL<TInt>(KMPXCommandGeneralSessionId, (TInt)this);
    addContainerCommand->SetTextValueL(KMPXMediaGeneralTitle, KTitle());

    EUNIT_PRINT(_L("Send add container command"));
    iCGlxMediaList->CommandL(*addContainerCommand);

    TInt attributeCount = addContainerCommand->Count();
    EUNIT_PRINT(_L("Check correct attributes for sent command"));
    EUNIT_ASSERT(attributeCount == 3);

    TBool commandIdAvailable = addContainerCommand->IsSupported(KMPXCommandGeneralId);
    TUint32 commandId(0);
    if (commandIdAvailable)
        {
        commandId = *(addContainerCommand->Value<TUint32>(KMPXCommandGeneralId));
        }
    EUNIT_PRINT(_L("Check command id sent and is correct value"));
    EUNIT_ASSERT(commandIdAvailable && (commandId == KGlxCommandIdAdd));

    TBool sessionIdAvailable = addContainerCommand->IsSupported(KMPXCommandGeneralSessionId);
    TInt sessionId(0);
    if (sessionIdAvailable)
        {
        sessionId = *(addContainerCommand->Value<TInt>(KMPXCommandGeneralSessionId));
        }
    EUNIT_PRINT(_L("Check session id sent and is correct value"));
    EUNIT_ASSERT(sessionIdAvailable && (sessionId == (TInt)this));

    TBool titleAvailable = addContainerCommand->IsSupported(KMPXMediaGeneralTitle);
    HBufC* title = NULL;
    if (titleAvailable)
        {
        title = HBufC::NewL((addContainerCommand->ValueText(KMPXMediaGeneralTitle)).Length());
        TPtr titlePtr = title->Des();
        titlePtr.Append(addContainerCommand->ValueText(KMPXMediaGeneralTitle));
        }
    EUNIT_PRINT(_L("Check title sent and is correct value"));
    EUNIT_ASSERT(titleAvailable && (*title == KTitle()));

    delete title;
    CleanupStack::PopAndDestroy(); // addContainerCommand
    }

void UT_CGlxMediaList::UT_CGlxMediaList_CancelCommandL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_SetFilterLL(  )
    {
    // Setup test
    SetupFilterTestL();

    // Change filter to videos
    EUNIT_PRINT(_L("Set filter to videos"));
    CMPXFilter* filter = iCGlxMediaList->Filter();
    filter->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXVideo);
    iCGlxMediaList->SetFilterL(filter);

    filter = iCGlxMediaList->Filter();

    TBool mediaCategoryAvailable = filter->IsSupported(KMPXMediaGeneralCategory);
    TMPXGeneralCategory category(EMPXNoCategory);
    if (mediaCategoryAvailable)
        {
        category = *(filter->Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory));
        }
    EUNIT_PRINT(_L("Check filter is videos"));
    EUNIT_ASSERT(mediaCategoryAvailable && (category == EMPXVideo));
    }

void UT_CGlxMediaList::UT_CGlxMediaList_FilterL(  )
    {
    // Setup test
    SetupFilterTestL();

    CMPXFilter* filter = iCGlxMediaList->Filter();

    TBool mediaCategoryAvailable = filter->IsSupported(KMPXMediaGeneralCategory);
    TMPXGeneralCategory category(EMPXNoCategory);
    if (mediaCategoryAvailable)
        {
        category = *(filter->Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory));
        }
    EUNIT_PRINT(_L("Check filter is images"));
    EUNIT_ASSERT(mediaCategoryAvailable && (category == EMPXImage));
    }

void UT_CGlxMediaList::UT_CGlxMediaList_HandleOpenLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_HandleOpenL_1L(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_HandleCollectionMediaLL(  )
    {
    EUNIT_ASSERT( EFalse );
    }

void UT_CGlxMediaList::UT_CGlxMediaList_HandleCollectionMessageL_1L(  )
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL(message);

    message->SetTObjectValueL<TUint32>(KMPXMessageGeneralId, KMPXMessageContentIdProgress);
    message->SetTObjectValueL<TInt>(KMPXCommandGeneralSessionId, (TInt)this);
    message->SetTObjectValueL<TInt>(KProgressTotalCountAttr, KProgressTotalCount);

    // Send progress messages
    EUNIT_PRINT(_L("Send progress messages"));
    for (TInt count = 0; count < KProgressTotalCount; ++count)
        {
        message->SetTObjectValueL<TInt>(KProgressCountAttr, count + 1);
        iCGlxMediaList->HandleCollectionMessageL(*message);
        }

    CleanupStack::PopAndDestroy(); // message

    // Check correct notification was observed
    const TInt messageCount = mediaListObserver->MessageCount();

    EUNIT_PRINT(_L("Check correct number of messages was received by observer"));
    EUNIT_ASSERT(messageCount == KProgressTotalCount);

    const MGlxMediaList* observedMediaList = mediaListObserver->List();

    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    const CMPXMessage* observedMessage = mediaListObserver->Message();    
    TInt attributeCount = observedMessage->Count();

    EUNIT_PRINT(_L("Check correct number of attributes for received progress message"));
    EUNIT_ASSERT(attributeCount == 4);

    TBool messageIdAvailable = observedMessage->IsSupported(KMPXMessageGeneralId);
    TUint32 messageId(0);
    if (messageIdAvailable)
        {
        messageId = *(observedMessage->Value<TUint32>(KMPXMessageGeneralId));
        }
    EUNIT_PRINT(_L("Check message id received and is correct value"));
    EUNIT_ASSERT(messageIdAvailable && (messageId == KMPXMessageContentIdProgress));

    TBool sessionIdAvailable = observedMessage->IsSupported(KMPXCommandGeneralSessionId);
    TInt sessionId(0);
    if (sessionIdAvailable)
        {
        sessionId = *(observedMessage->Value<TInt>(KMPXCommandGeneralSessionId));
        }
    EUNIT_PRINT(_L("Check session id received and is correct value"));
    EUNIT_ASSERT(sessionIdAvailable && (sessionId == (TInt)this));

    TBool progressCountAvailable = observedMessage->IsSupported(KProgressCountAttr);
    TInt progressCount(0);
    if (progressCountAvailable)
        {
        progressCount = *(observedMessage->Value<TInt>(KProgressCountAttr));
        }
    EUNIT_PRINT(_L("Check progress current count received and is correct value"));
    EUNIT_ASSERT(progressCountAvailable && (progressCount == KProgressTotalCount));

    TBool progressTotalCountAvailable = observedMessage->IsSupported(KProgressTotalCountAttr);
    TInt progressTotalCount(0);
    if (progressTotalCountAvailable)
        {
        progressTotalCount = *(observedMessage->Value<TInt>(KProgressTotalCountAttr));
        }
    EUNIT_PRINT(_L("Check progress total count received and is correct value"));
    EUNIT_ASSERT(progressTotalCountAvailable && (progressTotalCount == KProgressTotalCount));

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::SetupFocusTestL()
    {
    // Initial focus 3
    EUNIT_PRINT(_L("Initial focus 3"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::EAbsolute, 3);
    }

void UT_CGlxMediaList::SetupPathTestL()
    {
    SetupSelectionTestL();
    }

void UT_CGlxMediaList::SetupSelectionTestL()
    {
    // Initial selection 2 and 4
    EUNIT_PRINT(_L("Initial selection 2 and 4"));
    iCGlxMediaList->SetSelectedL(2, ETrue);
    iCGlxMediaList->SetSelectedL(4, ETrue);
    }

void UT_CGlxMediaList::SetupFilterTestL()
    {
    // Initial filter images
    EUNIT_PRINT(_L("Initial filter images"));
    CMPXFilter* filter = CMPXFilter::NewL();
    CleanupStack::PushL(filter);

    filter->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);

    iCGlxMediaList->SetFilterL(filter);

    CleanupStack::Pop(filter); // filter ownership passed to media list
    }

void UT_CGlxMediaList::MoveFocusAbsoluteGreaterThanCurrentFocusL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to absolute value 4
    EUNIT_PRINT(_L("Set focus to absolute value 4"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::EAbsolute, 4);

    // Check focus is index 4
    EUNIT_PRINT(_L("Check focus is index 4"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 4);

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EForward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == 4);

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 3);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::MoveFocusAbsoluteLessThanCurrentFocusL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to absolute value 2
    EUNIT_PRINT(_L("Set focus to absolute value 2"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::EAbsolute, 2);

    // Check focus is index 2
    EUNIT_PRINT(_L("Check focus is index 2"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 2);

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EBackward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == 2);

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 4);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::MoveFocusRelativeForwardL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to relative value forward 1
    EUNIT_PRINT(_L("Set focus to relative value forward 1"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::ERelative, 1);

    // Check focus is index 3
    EUNIT_PRINT(_L("Check focus is index 3"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 3);

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EForward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == 3);

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 2);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::MoveFocusRelativeBackwardL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to relative value backwards 1
    EUNIT_PRINT(_L("Set focus to relative value backwards 1"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::ERelative, -1);

    // Check focus is index 2
    EUNIT_PRINT(_L("Check focus is index 2"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 2);

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EBackward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == 2);

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 3);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::MoveFocusRelativeForwardGreaterThanListCountL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to relative value forward list count - 1
    EUNIT_PRINT(_L("Set focus to relative value forward list count - 1"));
    TInt listCount = iCGlxMediaList->Count();
    iCGlxMediaList->SetFocusL(NGlxListDefs::ERelative, listCount - 1);

    // Check focus is index 1
    EUNIT_PRINT(_L("Check focus is index 1"));
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == 1);

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EForward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == 1);

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 2);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::MoveFocusRelativeBackwardLessThanZeroL()
    {
    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Set focus to relative value backwards 2
    EUNIT_PRINT(_L("Set focus to relative value backwards 2"));
    iCGlxMediaList->SetFocusL(NGlxListDefs::ERelative, -2);

    // Check focus is index (list count - 1)
    EUNIT_PRINT(_L("Check focus is index (list count - 1)"));
    TInt listCount = iCGlxMediaList->Count();
    TInt focusIndex = iCGlxMediaList->FocusIndex();
    EUNIT_ASSERT(focusIndex == (listCount - 1));

    // Check correct notification was observed
    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    const MGlxMediaList* observedMediaList = mediaListObserver->List();
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    EUNIT_PRINT(_L("Check correct focus change type was received by observer"));
    NGlxListDefs::TFocusChangeType observedFocusChangeType = mediaListObserver->FocusChangeType();
    EUNIT_ASSERT(observedFocusChangeType == NGlxListDefs::EBackward);

    EUNIT_PRINT(_L("Check correct new focus index was received by observer"));
    TInt observedFocusNewIndex = mediaListObserver->FocusNewIndex();
    EUNIT_ASSERT(observedFocusNewIndex == (listCount - 1));

    EUNIT_PRINT(_L("Check correct old focus index was received by observer"));
    TInt observedFocusOldIndex = mediaListObserver->FocusOldIndex();
    EUNIT_ASSERT(observedFocusOldIndex == 1);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(); // mediaListObserver
    }

void UT_CGlxMediaList::ReceiveMessageItemChangedItemModifiedL()
    {
    iSchedulerWait->Start(); // wait for attributes to be retrieved (HandleAttributesAvailableL will be called)

    // Check media attributes initial count
    TInt listIndex = iCGlxMediaList->Index(NULL, TGlxMediaId(KFavouritesAlbumFirstItemId));

    EUNIT_PRINT(_L("Check initial media has 1 attribute"));
    const TGlxMedia& tGlxMediaBefore = iCGlxMediaList->Item(listIndex);
    const CGlxMedia* cGlxMediaBefore = tGlxMediaBefore.Properties();
    EUNIT_ASSERT(cGlxMediaBefore->Count() == 1);

    CGlxMediaListObserverTest* mediaListObserver = CGlxMediaListObserverTest::NewL();
    CleanupStack::PushL(mediaListObserver);

    iCGlxMediaList->AddMediaListObserverL(mediaListObserver);

    // Send item modified notification for item not in list
    EUNIT_PRINT(_L("Send item modified notification for item not in list"));
    TMPXCollectionMessage message1(TMPXCollectionMessage::EItemChanged, EMcItemModified, KFavouritesAlbumId);
//    iCGlxMediaList->HandleCollectionMessageL(message1);

    // Check no notification was observed
    TInt messageCount = mediaListObserver->MessageCount();

    EUNIT_PRINT(_L("Check no messages was received by observer"));
    EUNIT_ASSERT(messageCount == 0);

    // Send item modified notification for item in list
    EUNIT_PRINT(_L("Send item modified notification for item in list"));
    TMPXCollectionMessage message2(TMPXCollectionMessage::EItemChanged, EMcItemModified, KFavouritesAlbumFirstItemId);
//    iCGlxMediaList->HandleCollectionMessageL(message2);

    // Check correct notification was observed
    messageCount = mediaListObserver->MessageCount();

    EUNIT_PRINT(_L("Check correct number of messages was received by observer"));
    EUNIT_ASSERT(messageCount == 1);

    const MGlxMediaList* observedMediaList = mediaListObserver->List();

    EUNIT_PRINT(_L("Check correct media list was received by observer"));
    EUNIT_ASSERT(observedMediaList == (MGlxMediaList*)iCGlxMediaList);

    const RArray<TInt>& modifiedItemIndices = mediaListObserver->ModifiedItemIndices();

    EUNIT_PRINT(_L("Check correct number of modified items was received by observer"));
    TInt modifiedItemIndicesCount = modifiedItemIndices.Count();
    EUNIT_ASSERT(modifiedItemIndicesCount == 1);

    EUNIT_PRINT(_L("Check correct modified items was received by observer"));
    TInt modifedItemIndex = modifiedItemIndices[0];
    EUNIT_ASSERT(modifedItemIndex == listIndex);

    // Check media attributes removed
    EUNIT_PRINT(_L("Check media has no attributes"));
    const TGlxMedia& tGlxMediaAfter = iCGlxMediaList->Item(listIndex);
    const CGlxMedia* cGlxMediaAfter = tGlxMediaAfter.Properties();
    EUNIT_ASSERT(cGlxMediaAfter->Count() == 0);

    iCGlxMediaList->RemoveMediaListObserver(mediaListObserver);

    CleanupStack::PopAndDestroy(mediaListObserver);
    }

UT_CGlxMediaList::CGlxMediaListObserverTest* UT_CGlxMediaList::CGlxMediaListObserverTest::NewL()
    {
    CGlxMediaListObserverTest* self = new (ELeave) CGlxMediaListObserverTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

UT_CGlxMediaList::CGlxMediaListObserverTest::~CGlxMediaListObserverTest()
    {
    delete iMessage;
    iMessage = NULL;

    iModifiedItemIndices.Close();
    }

UT_CGlxMediaList::CGlxMediaListObserverTest::CGlxMediaListObserverTest() :
	iList(NULL),
	iFocusChangeType(NGlxListDefs::EUnknown),
	iFocusNewIndex(KErrNotFound),
	iFocusOldIndex(KErrNotFound),
	iModifiedItemIndices(),
	iMessage(NULL),
	iMessageCount(0)
    {
    }

void UT_CGlxMediaList::CGlxMediaListObserverTest::ConstructL()
    {
    iMessage = CMPXMessage::NewL();
    }

void UT_CGlxMediaList::CGlxMediaListObserverTest::HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList)
    {
    iList = aList;

    iFocusChangeType = aType;
    iFocusNewIndex = aNewIndex;
    iFocusOldIndex = aOldIndex;

    ++iMessageCount;
    }

void UT_CGlxMediaList::CGlxMediaListObserverTest::HandleItemModifiedL(const RArray<TInt>& aItemIndices, MGlxMediaList* aList)
    {
    iList = aList;

    TInt itemIndexCount = aItemIndices.Count();
    for (TInt count = 0; count < itemIndexCount; ++count)
        {
        iModifiedItemIndices.AppendL(aItemIndices[count]);
        }

    ++iMessageCount;
    }

void UT_CGlxMediaList::CGlxMediaListObserverTest::HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList)
    {
    iList = aList;

    if (aMessage.IsSupported(KMPXMessageGeneralId))
        {
        TUint32 messageId = *(aMessage.Value<TUint32>(KMPXMessageGeneralId));
        iMessage->SetTObjectValueL<TUint32>(KMPXMessageGeneralId, messageId);
        }

    if (aMessage.IsSupported(KMPXCommandGeneralSessionId))
        {
        TInt sessionId = *(aMessage.Value<TInt>(KMPXCommandGeneralSessionId));
        iMessage->SetTObjectValueL<TInt>(KMPXCommandGeneralSessionId, sessionId);
        }

    if (aMessage.IsSupported(KProgressCountAttr))
        {
        TInt progressCount = *(aMessage.Value<TInt>(KProgressCountAttr));
        iMessage->SetTObjectValueL<TInt>(KProgressCountAttr, progressCount);
        }

    if (aMessage.IsSupported(KProgressTotalCountAttr))
        {
        TInt progressTotalCount = *(aMessage.Value<TInt>(KProgressTotalCountAttr));
        iMessage->SetTObjectValueL<TInt>(KProgressTotalCountAttr, progressTotalCount);
        }

    ++iMessageCount;
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxMediaList,
    "Unit tests for CGlxMediaList",
    "UNIT" )

EUNIT_TEST(
    "InstanceL - test ",
    "CGlxMediaList",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_InstanceLL, Teardown)

EUNIT_TEST(
    "MediaLists - test ",
    "CGlxMediaList",
    "MediaLists",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_MediaListsL, Teardown)
/*
EUNIT_TEST(
    "NewLC - test ",
    "CGlxMediaList",
    "NewLC",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_NewLCL, Teardown)

EUNIT_TEST(
    "OpenL - test ",
    "CGlxMediaList",
    "OpenL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_OpenLL, Teardown)

EUNIT_TEST(
    "AddReference - test ",
    "CGlxMediaList",
    "AddReference",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_AddReferenceL, Teardown)

EUNIT_TEST(
    "RemoveReference - test ",
    "CGlxMediaList",
    "RemoveReference",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_RemoveReferenceL, Teardown)

EUNIT_TEST(
    "ReferenceCount - test ",
    "CGlxMediaList",
    "ReferenceCount",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_ReferenceCountL, Teardown)

EUNIT_TEST(
    "Equals - test ",
    "CGlxMediaList",
    "Equals",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_EqualsL, Teardown)

EUNIT_TEST(
    "OfferMedia - test ",
    "CGlxMediaList",
    "OfferMedia",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_OfferMediaL, Teardown)

EUNIT_TEST(
    "HandleAttributesAvailableL - test ",
    "CGlxMediaList",
    "HandleAttributesAvailableL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleAttributesAvailableLL, Teardown)

EUNIT_TEST(
    "AttributeRequestL - test ",
    "CGlxMediaList",
    "AttributeRequestL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_AttributeRequestLL, Teardown)

EUNIT_TEST(
    "Close - test ",
    "CGlxMediaList",
    "Close",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_CloseL, Teardown)

EUNIT_TEST(
    "Id - test ",
    "CGlxMediaList",
    "Id",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_IdL, Teardown)

EUNIT_TEST(
    "Count - test ",
    "CGlxMediaList",
    "Count",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_CountL, Teardown)
*/
EUNIT_TEST(
    "FocusIndex - test ",
    "CGlxMediaList",
    "FocusIndex",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_FocusIndexL, Teardown)

EUNIT_TEST(
    "SetFocusL - test ",
    "CGlxMediaList",
    "SetFocusL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_SetFocusLL, Teardown)
/*
EUNIT_TEST(
    "Item - test ",
    "CGlxMediaList",
    "Item",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_ItemL, Teardown)

EUNIT_TEST(
    "Index - test ",
    "CGlxMediaList",
    "Index",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_IndexL, Teardown)

EUNIT_TEST(
    "AddMediaListObserverL - test ",
    "CGlxMediaList",
    "AddMediaListObserverL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_AddMediaListObserverLL, Teardown)

EUNIT_TEST(
    "RemoveMediaListObserver - test ",
    "CGlxMediaList",
    "RemoveMediaListObserver",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_RemoveMediaListObserverL, Teardown)

EUNIT_TEST(
    "AddContextL - test ",
    "CGlxMediaList",
    "AddContextL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_AddContextLL, Teardown)

EUNIT_TEST(
    "RemoveContext - test ",
    "CGlxMediaList",
    "RemoveContext",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_RemoveContextL, Teardown)

EUNIT_TEST(
    "Collection - test ",
    "CGlxMediaList",
    "Collection",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_CollectionL, Teardown)
*/
EUNIT_TEST(
    "PathLC - test ",
    "CGlxMediaList",
    "PathLC",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_PathLCL, Teardown)

EUNIT_TEST(
    "IsSelected - test ",
    "CGlxMediaList",
    "IsSelected",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_IsSelectedL, Teardown)

EUNIT_TEST(
    "SetSelectedL - test ",
    "CGlxMediaList",
    "SetSelectedL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_SetSelectedLL, Teardown)

EUNIT_TEST(
    "SelectionCount - test ",
    "CGlxMediaList",
    "SelectionCount",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_SelectionCountL, Teardown)

EUNIT_TEST(
    "SelectedItemIndex - test ",
    "CGlxMediaList",
    "SelectedItemIndex",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_SelectedItemIndexL, Teardown)

EUNIT_TEST(
    "CommandL - test ",
    "CGlxMediaList",
    "CommandL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_CommandLL, Teardown)
/*
EUNIT_TEST(
    "CancelCommand - test ",
    "CGlxMediaList",
    "CancelCommand",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_CancelCommandL, Teardown)
*/
EUNIT_TEST(
    "SetFilterL - test ",
    "CGlxMediaList",
    "SetFilterL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_SetFilterLL, Teardown)

EUNIT_TEST(
    "Filter - test ",
    "CGlxMediaList",
    "Filter",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_FilterL, Teardown)

/**
EUNIT_TEST(
    "HandleCollectionMessageL - test ",
    "CGlxMediaList",
    "HandleCollectionMessageL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleCollectionMessageLL, Teardown)
/
EUNIT_TEST(
    "HandleOpenL - test ",
    "CGlxMediaList",
    "HandleOpenL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleOpenLL, Teardown)

EUNIT_TEST(
    "HandleOpenL - test ",
    "CGlxMediaList",
    "HandleOpenL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleOpenL_1L, Teardown)

EUNIT_TEST(
    "HandleCollectionMediaL - test ",
    "CGlxMediaList",
    "HandleCollectionMediaL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleCollectionMediaLL, Teardown)

EUNIT_TEST(
    "HandleCollectionMessageL - test ",
    "CGlxMediaList",
    "HandleCollectionMessageL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxMediaList_HandleCollectionMessageL_1L, Teardown)
*/
EUNIT_END_TEST_TABLE

//  END OF FILE
