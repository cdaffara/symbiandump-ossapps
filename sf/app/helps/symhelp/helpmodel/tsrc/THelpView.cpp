// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#pragma message(__FILE__ "(5) : This test code memory leaks on exit due to a bug in the code...fix when have time")

#include "THelpView.h"

// System includes
#include <coemain.h>
#include <eikenv.h>
#include <techview/eikrted.h>
#include <techview/eikmenup.h>
#include <techview/eikchlst.h>
#include <techview/eikon.hrh>
#include <techview/eiksbfrm.h>
#include <eikstart.h>
#include <txtrich.h>
#include "hlpmodel.h"
#include <thelpview.rsg>

// User includes
#include "thelpviewctrls.h"
#include "THelpView.hrh"

// Constants
const TInt KSizeSearchPromptIndent = 5;
const TInt KSizeSearchPromptHeight = 25;
const TInt KResultsViewWidth = 150;

// Panic enum
enum THlpViewerPanic
	{
	EHlpViewPanicNoSearchSpec
	};


//
// ----> Global functions / Entry points (source)
//

LOCAL_C CApaApplication* NewApplication()
// Ordinal 1 export for App Dll's
	{
	return new CHlpApplication();
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}


void Panic(THlpViewerPanic aPanic)
	{
	_LIT(KHlpViewPanicCategory, "HlpView");
	User::Panic(KHlpViewPanicCategory, aPanic);
	}



//
// ----> CHlpApplication (source)
//
TUid CHlpApplication::AppDllUid() const
	{
	return KUidHelpViewApp;
	}

CApaDocument* CHlpApplication::CreateDocumentL()
	{
	CHlpDocument* document = new(ELeave) CHlpDocument(*this);
	CleanupStack::PushL(document);
	document->NewDocumentL();
	CleanupStack::Pop();
	return document;
	}





//
// ----> CHlpAppUi (source)
//
CHlpDocument::CHlpDocument(CEikApplication& aApp)
:	CEikDocument(aApp)
	{
	}

CHlpDocument::~CHlpDocument()
	{
	delete iModel;
	}

CEikAppUi* CHlpDocument::CreateAppUiL()
	{
    return(new(ELeave) CHlpAppUi());
	}

void CHlpDocument::NewDocumentL()
	{
	CHlpModel* model = CHlpModel::NewL(Process()->FsSession(), NULL);
	CleanupStack::PushL(model);
	model->OpenL();
	delete iModel;
	iModel = model;
	CleanupStack::Pop(); // model
	}





//
// ----> CHlpAppUi (source)
//
CHlpAppUi::~CHlpAppUi()
	{
	CCoeAppUi::RemoveFromStack(iView);
	delete iView;
	}

void CHlpAppUi::ConstructL()
	{
    BaseConstructL();

	iView = new(ELeave) CHlpMainView(*this, Document().Model());
	iView->ConstructL();
	iView->SetRect(ClientRect());

	AddToStackL(iView);
	}

//
// FROM CEikAppUi
//

void CHlpAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit: 
		Exit();
		break;

	case EHlpCmdSearch:
		iView->ShowSearchPrompt(ETrue);
		break;

	case EHlpCmdSearchDone:
		iView->MakeResultsVisible(EFalse);
		break;

	case EEikCmdZoomIn:
	case EEikCmdZoomOut:
		HandleZoomL(aCommand);	
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

void CHlpAppUi::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
	{
	switch (aMenuId)
		{
	case R_HELPVIEW_TOOLS_MENU:
		aMenuPane->SetItemDimmed(EHlpCmdSearchDone, !iView->IsSearchResultVisible());
		break;
		}
	}

//
// NEW
//

void CHlpAppUi::HandleZoomL(TInt aZoomEvent)
	{
	if	(!iZoomHandler)
		return;
	iZoomHandler->HandleZoomEventL(STATIC_CAST(MZoomableControl::TZoomEvent, aZoomEvent));
	}






//
// ----> CHlpView (source)
//
CHlpView::CHlpView(CHlpAppUi& aAppUi, CHlpModel& aModel)
:	iAppUi(aAppUi), iHelpModel(aModel), iCurrentZoom(EZoomStateTwo)
	{
	}

CHlpView::~CHlpView()
	{
	delete iHelpCategories;
	delete iHelpTopicsForCategory;
	delete iRichTextEditor;
	delete iCurrentTopics;
	delete iLastTopic;

	// Remove this when the memory leak is finished...
	iCoeEnv->DisableExitChecks(ETrue);
	}

void CHlpView::ConstructL()
	{
	iLastTopic = CHlpTopic::NewL();
	iZoom.SetGraphicsDeviceMap(iCoeEnv->ScreenDevice());

	iHelpModel.SetObserver(this);

	iHelpCategories = new(ELeave) CEikChoiceList();
	iHelpCategories->SetContainerWindowL(*this);
	iHelpCategories->SetFocus(ETrue);
	iHelpCategories->SetObserver(this);

	iHelpTopicsForCategory = new(ELeave) CEikChoiceList();
	iHelpTopicsForCategory->SetContainerWindowL(*this);
	iHelpTopicsForCategory->SetObserver(this);

	iRichTextEditor = new(ELeave) CEikRichTextEditor();
	iRichTextEditor->SetContainerWindowL(*this);
	iRichTextEditor->ConstructL(this, 6, 100000, CEikEdwin::EWidthInPixels | CEikEdwin::EReadOnly | CEikEdwin::ENoAutoSelection | CEikEdwin::EKeepDocument);
	iRichTextEditor->SetZoomFactorL(&iZoom);
	iRichTextEditor->CreateTextViewL();
	iRichTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iRichTextEditor->SetObserver(this);

	HandleZoomEventL(EEventZoomDefault);
	PopulateCategoryListL();

	iHelpModel.SetZoomFactors(EHlpZoomStateSmall, 750);
	iHelpModel.SetZoomFactors(EHlpZoomStateMedium, 1000);
	iHelpModel.SetZoomFactors(EHlpZoomStateLarge, 1250);
	}


//
// NEW
//
void CHlpView::PopulateCategoryListL()
	{
	// First get a category listing, then get the list of topics
	// for each specified category.
	// Get and print the category listing
	CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(array);
	iHelpModel.CategoryListL(array);

	if	(!array->Count())
		User::Leave(KErrArgument); // ??

	iHelpCategories->SetArrayExternalOwnership(EFalse);
	iHelpCategories->SetArrayL(array);
	iHelpCategories->SetArrayExternalOwnership(EFalse);
	iHelpCategories->SetCurrentItem(0);

	PopulateTopicListL(iHelpCategories->Array()->MdcaPoint(iHelpCategories->CurrentItem()));

	CleanupStack::Pop();
	}

void CHlpView::PopulateTopicListL(const TDesC& aTopic)
	{
	iHelpModel.SearchL(ETopicListForCategory, aTopic);

	CHlpList* list = CHlpList::NewLC();
	iHelpModel.LoadListL(list);
	iHelpTopicsForCategory->SetArrayL(list); // Derrives from MDesCArray so we can do this
	delete iCurrentTopics;
	iCurrentTopics = list;
	iHelpTopicsForCategory->SetArrayExternalOwnership(ETrue); // owned by this class
	CleanupStack::Pop(); // list

	if	(list->MdcaCount())
		{
		UpdateEdwinL(*list->Item(0));
		iHelpTopicsForCategory->SetCurrentItem(0);
		iHelpTopicsForCategory->DrawNow();
		}
	}


void CHlpView::UpdateEdwinL(const CHlpItem& aItem)
//
//	Extract the rich text from the help file and display it in the
//	read only edit window.
//
	{
	// Search for a specific topic.
	iHelpModel.TopicSearchL(aItem);
	if	(iLastResponseFromHelpModel == ETopicAvailable)
		{
		CHlpTopic* topic = CHlpTopic::NewLC();
		iHelpModel.LoadTopicL(topic);
		iRichTextEditor->SetDocumentContentL(*topic->TopicText(), CEikEdwin::EUseText);
		iRichTextEditor->NotifyNewFormatL();
		iRichTextEditor->TextView()->SetViewLineAtTopL(1);
		iRichTextEditor->UpdateScrollBarsL();
		delete iLastTopic;
		iLastTopic = topic;
		CleanupStack::Pop(); // topic
		}
	else
		iEikonEnv->InfoMsg(R_HELPVIEW_TEXT_NO_TOPIC_FOR_ID);
	}

//
// FROM MZoomableControl
//

void CHlpView::HandleZoomEventL(TZoomEvent aEvent)
	{
	switch(aEvent)
		{
	case EEventZoomIn:
		if	(++iCurrentZoom>=KNumberOfZoomStates)
			iCurrentZoom = 0;
		break;
	case EEventZoomOut:
		if	(--iCurrentZoom < 0)
			iCurrentZoom = KNumberOfZoomStates-1;
		break;
	case EEventZoomDefault:
		iCurrentZoom = 1;
		break;
	default:
		User::Leave(KErrNotSupported);
		}

	// Select bitmap zoom size in the 'hlpmodel', according to the value of 'iCurrentZoom'
	switch (iCurrentZoom)
		{
	case 0:
		iHelpModel.SetZoomSizeL(EHlpZoomStateSmall);
		break;
	default:
	case 1:
		iHelpModel.SetZoomSizeL(EHlpZoomStateMedium);
		break;
	case 2:
		iHelpModel.SetZoomSizeL(EHlpZoomStateLarge);
		break;
		}


	iZoom.SetZoomFactor(ZoomForIndex(iCurrentZoom));
	iRichTextEditor->NotifyNewFormatL();
	iRichTextEditor->UpdateScrollBarsL();
	iRichTextEditor->DrawNow();
	}

TInt CHlpView::ZoomForIndex(TInt aIndex)
	{
	switch(aIndex)
		{
	case 0:
		return EZoomStateOne;
	default:
	case 1:
		return EZoomStateTwo;
	case 2:
		return EZoomStateThree;
		}
	}

//
// FROM CCoeControl
//

void CHlpView::SizeChanged()
	{
	const TInt KKludgeFactor = 4;

	TRect rect(Position(), Rect().Size());
	TSize choiceListSize = iHelpCategories->MinimumSize() + TSize(0, KKludgeFactor);

	TPoint topLeft(rect.iTl);

	iHelpCategories->SetExtent(topLeft, TSize(rect.Width()/2, choiceListSize.iHeight));
	iHelpTopicsForCategory->SetExtent(TPoint(rect.Size().iWidth/2,topLeft.iY), TSize(rect.Size().iWidth/2, choiceListSize.iHeight));
	iRichTextEditor->SetRect(TRect(TPoint(topLeft.iX, choiceListSize.iHeight), rect.iBr));
	}

TInt CHlpView::CountComponentControls() const
	{
	return 3;
	}

CCoeControl* CHlpView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
	default:
	case 0:
		return iHelpCategories;
	case 1:
		return iHelpTopicsForCategory;
	case 2:
		return iRichTextEditor;
		}
	}

TKeyResponse CHlpView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	return iRichTextEditor->OfferKeyEventL(aKeyEvent, aType);
	}

//
// FROM MCoeControlObserver
//
void CHlpView::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
	{
	if	(aEventType == EEventRequestFocus)
		{
		if	(iFocusedControl)
			{
			iFocusedControl->PrepareForFocusLossL();
			iFocusedControl->SetFocus(EFalse);
			}
		iFocusedControl = aControl;
		iFocusedControl->SetFocus(ETrue);
		}
	else if	(aControl == iHelpCategories && aEventType == EEventStateChanged)
		{
		// Update the list of topics available for this category
		PopulateTopicListL(iHelpCategories->Array()->MdcaPoint(iHelpCategories->CurrentItem()));
		}
	else if	(aControl == iHelpTopicsForCategory && aEventType == EEventStateChanged)
		{
		// Update the edwin contents
		UpdateEdwinL(*iCurrentTopics->Item(iHelpTopicsForCategory->CurrentItem()));
		}
	}

//
// FROM MHlpModelObserver
//
void CHlpView::HandleModelEventL(TInt aEvent)
	{
	iLastResponseFromHelpModel = aEvent;
	}







//
// ----> CHlpMainView (source)
//

CHlpMainView::CHlpMainView(CHlpAppUi& aAppUi, CHlpModel& aModel)
:	iAppUi(aAppUi), iModel(aModel)
	{
	}

CHlpMainView::~CHlpMainView()
	{
	delete iSearchResults;
	delete iSearchPrompt;
	delete iView;
	}

void CHlpMainView::ConstructL()
	{
	CreateWindowL();

	iView = new(ELeave) CHlpView(iAppUi, iModel);
	iView->SetContainerWindowL(*this);
	iView->ConstructL();
	iAppUi.SetZoomHandler(iView);

	iSearchPrompt = new(ELeave) CHlpSearchPrompt();
	iSearchPrompt->ConstructL(*this);
	iSearchPrompt->SetObserver(this);
	iSearchPrompt->MakeVisible(EFalse);

	iSearchResults = new(ELeave) CHlpListBox();
	iSearchResults->ConstructL(*this);
	iSearchResults->SetObserver(this);
	iSearchResults->MakeVisible(EFalse);

	iView->SetFocus(ETrue);

	ActivateL();
	}

//
// ACCESS
//

TBool CHlpMainView::IsSearchPromptVisible() const
	{
	return iSearchPrompt->IsVisible();
	}

TBool CHlpMainView::IsSearchResultVisible() const
	{
	return iSearchResults->IsVisible();
	}

void CHlpMainView::ShowSearchPrompt(TBool aShow)
	{
	iSearchPrompt->MakeVisible(aShow);
	}

void CHlpMainView::MakeResultsVisible(TBool aVisible)
	{
	TRect rect(Rect());
	TSize resultsSize = iSearchResults->Size();

	if	(!aVisible)
		{
		// Make the main view rect 'full size' again
		iView->SetRect(rect);
		iSearchResults->ListBox()->SetFocus(EFalse);
		iView->SetFocus(ETrue);
		}
	else
		{
		rect.iBr.iX -= resultsSize.iWidth;
		iView->SetRect(rect);
		iView->SetFocus(EFalse);
		iSearchResults->ListBox()->SetFocus(ETrue);
		}
	iSearchResults->MakeVisible(aVisible);
	DrawNow();
	}

//
// FROM CCoeControl
//

void CHlpMainView::SizeChanged()
	{
	iView->SetRect(Rect());

	TRect rect(Rect());
	TSize KScreenSize(iCoeEnv->ScreenDevice()->SizeInPixels());

	// Work out the size of the search prompt minus the inset for the left and right
	// hand size. Kludge this a bit to make sure we allow for the size of any scrollbar
	// (hence the *6 rather than the expected *2)
	TSize searchPromptSize(KScreenSize.iWidth - (4*KSizeSearchPromptIndent), KSizeSearchPromptHeight);
	iSearchPrompt->SetExtent(TPoint(KSizeSearchPromptIndent, rect.iBr.iY - KSizeSearchPromptHeight - KSizeSearchPromptIndent), searchPromptSize);

	TRect resultsRect(rect);
	resultsRect.iTl.iX = resultsRect.iBr.iX - KResultsViewWidth;
	iSearchResults->SetRect(resultsRect);
	}

TInt CHlpMainView::CountComponentControls() const
	{
	return 3;
	}

CCoeControl* CHlpMainView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
	default:
	case 0:
		return iView;
	case 1:
		return iSearchPrompt;
	case 2:
		return iSearchResults;
		}
	}

TKeyResponse CHlpMainView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	// Send cursors to the rich text edwin (kludgy)
	if	(aKeyEvent.iCode >= EKeyPageUp && aKeyEvent.iCode <= EKeyDownArrow)
		{
		if	(iView->IsFocused())
			return iView->OfferKeyEventL(aKeyEvent, aType);
		else
			return iSearchResults->OfferKeyEventL(aKeyEvent, aType);
		}
	else
		{
		if	(!iSearchPrompt->IsVisible() && aType == EEventKey)
			{
			iSearchPrompt->MakeVisible(ETrue);
			iSearchPrompt->SetFocus(ETrue);
			}
		return iSearchPrompt->OfferKeyEventL(aKeyEvent, aType);
		}
	}

//
// FROM MCoeControlObserver
//

void CHlpMainView::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
	{
	if	(aControl == iSearchPrompt && aEventType == EEventStateChanged)
		{
		// Action the search
		DoSearchL();

		// Update the list of topics available for this category
		iSearchPrompt->MakeVisible(EFalse);

		// Reset the search prompt
		iSearchPrompt->ResetSearchL();
		}
	else if (aControl == iSearchResults && aEventType == EEventRequestExit)
		{
		// Finished with results
		MakeResultsVisible(EFalse);
		}
	else if (aControl == iSearchResults && aEventType == EEventStateChanged)
		{
		// Display the selected topic that the user searched for
		iView->UpdateEdwinL(iSearchResults->HelpItemL());
		}
	}

//
// FROM MHlpModelObserver
//

void CHlpMainView::HandleModelEventL(TInt aEvent)
	{
	iLastHelpModelResponse = aEvent;

	if (aEvent == ESearchListAvailable || aEvent == ESearchListNoneFound)
		{
		iEikonEnv->BusyMsgCancel();

		// This is a special case which is initiated by full text searching...
		iModel.SetObserver(iView);

		// Now populate the search results listbox.
		if	(iLastHelpModelResponse == ESearchListNoneFound)
			{
			iEikonEnv->InfoMsg(R_HELPVIEW_TEXT_NO_SEARCH_RESULTS);
			return;
			}

		// Get a topic list (because some were found)
		CHlpList* list = CHlpList::NewL();
		iModel.LoadListL(list);
		iSearchResults->PopulateListBoxL(list); // takes ownership
		MakeResultsVisible(ETrue); // show results
		}
	}

//
// INTERNAL
//

void CHlpMainView::DoSearchL()
	{
	HBufC* searchSpec = iSearchPrompt->SearchTextLC();
	__ASSERT_DEBUG(searchSpec, Panic(EHlpViewPanicNoSearchSpec));

	// Set this class as the observer (kludge)
	iModel.SetObserver(this);

	// Request a search
	iModel.SearchL(EFullTextSearch, searchSpec); // Asynch...
	iEikonEnv->BusyMsgL(R_HELPVIEW_TEXT_SEARCHING, 0);

	CleanupStack::PopAndDestroy(); // searchSpec
	}




