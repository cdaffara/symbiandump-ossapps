// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THelpViewCtrls module
// 
//

#include "thelpviewctrls.h"

// System includes
#include <techview/eikcmbut.h>
#include <techview/eikedwin.h>
//
#include "hlpmodel.h"

// Constants
const TInt KTimeOutPeriod = 10000000; // Ten seconds

// Literal constants
_LIT(KDoneCaption, "Done");


//
// ----> CHlpSearchPrompt (header)
//

CHlpSearchPrompt::CHlpSearchPrompt()
	{
	}

CHlpSearchPrompt::~CHlpSearchPrompt()
	{
	iHideTimer->Cancel();
	delete iHideTimer;
	delete iEdwin;
	}

void CHlpSearchPrompt::ConstructL(CCoeControl& aParent)
	{
	CreateWindowL(&aParent);

	iHideTimer = CPeriodic::NewL(CActive::EPriorityIdle);

	iEdwin = new(ELeave) CEikEdwin();
	iEdwin->SetContainerWindowL(*this);
	iEdwin->ConstructL(0, 25, 140, 1);
	iEdwin->CreateTextViewL();
	iEdwin->SetObserver(this);
	}

//
//
//

void CHlpSearchPrompt::ResetSearchL()
	{
	iEdwin->Text()->Reset();
	iEdwin->NotifyNewDocumentL();
	iEdwin->DrawNow();
	}

HBufC* CHlpSearchPrompt::SearchTextLC()
	{
	HBufC* buf = iEdwin->GetTextInHBufL();
	CleanupStack::PushL(buf);
	return buf;
	}

//
//
//

void CHlpSearchPrompt::MakeVisible(TBool aVisible)
	{
	CCoeControl::MakeVisible(aVisible);
	if	(!aVisible)
		{
		iHideTimer->Cancel();
		ResetSearchL();       // Leaves if not visible
		}
	}

void CHlpSearchPrompt::SizeChanged()
	{
	iEdwin->SetRect(Rect());
	}

TInt CHlpSearchPrompt::CountComponentControls() const
	{
	return 1;
	}

CCoeControl* CHlpSearchPrompt::ComponentControl(TInt/* aIndex*/) const
	{
	return iEdwin;
	}

TKeyResponse CHlpSearchPrompt::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if	(aKeyEvent.iCode == EKeyEnter && aType == EEventKey)
		{
		ReportEventL(EEventStateChanged);
		return EKeyWasConsumed;
		}
	else if	(aKeyEvent.iCode == EKeyEscape && aType == EEventKey)
		{
		MakeVisible(EFalse);
		return EKeyWasConsumed;
		}
	else
		{
		iHideTimer->Cancel();
		iHideTimer->Start(KTimeOutPeriod, KTimeOutPeriod, TCallBack(HandleTimerExpiredL, this));
		return iEdwin->OfferKeyEventL(aKeyEvent, aType);
		}
	}

//
//
//

void CHlpSearchPrompt::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
	{
	}

//
//
//

TInt CHlpSearchPrompt::HandleTimerExpiredL(TAny* aSelf)
	{
	CHlpSearchPrompt* self = STATIC_CAST(CHlpSearchPrompt*, aSelf);
	self->MakeVisible(EFalse);
	return EFalse; // one shot
	}









//
// ----> CHlpListBox (source)
//

CHlpListBox::CHlpListBox()
	{
	}

CHlpListBox::~CHlpListBox()
	{
	delete iContent;
	delete iButton;
	delete iListBox;
	}

void CHlpListBox::ConstructL(CCoeControl& aParent)
	{
	SetContainerWindowL(aParent);

	iButton = new(ELeave) CEikCommandButton();
	iButton->SetContainerWindowL(*this);
	iButton->SetTextL(KDoneCaption);
	iButton->SetObserver(this);

	iListBox = new(ELeave) CEikTextListBox();
	iListBox->ConstructL(this, CEikListBox::ENoExtendedSelection);
	iListBox->CreateScrollBarFrameL()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iListBox->SetObserver(this);
	}

//
// FROM CCoeControl
//

void CHlpListBox::SizeChanged()
	{
	TRect rect(Rect());
	TSize size(rect.Size());
	const TInt KButtonHeight = iButton->MinimumSize().iHeight;
	iButton->SetExtent(rect.iTl, TSize(size.iWidth, KButtonHeight));
	rect.iTl.iY += KButtonHeight;
	iListBox->SetRect(rect);
	}

TInt CHlpListBox::CountComponentControls() const
	{
	return 2;
	}

CCoeControl* CHlpListBox::ComponentControl(TInt aIndex) const
	{
	if	(!aIndex)
		return iListBox;
	return iButton;
	}

TKeyResponse CHlpListBox::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	return iListBox->OfferKeyEventL(aKeyEvent, aType);
	}

TSize CHlpListBox::MinimumSize()
	{
	return iButton->MinimumSize();
	}

//
// NEW
//

const CHlpItem& CHlpListBox::HelpItemL() const
	{
	TInt index = iListBox->CurrentItemIndex();
	if	(!iContent || index >= iContent->MdcaCount())
		User::Leave(KErrNotFound);

	// Return the topic id of the topic that the user is interested in
	return *iContent->Item(index);
	}

void CHlpListBox::PopulateListBoxL(CHlpList* aContent)
	{
	SetContent(aContent);

	CDesCArray* itemArray = STATIC_CAST(CDesCArray*, iListBox->Model()->ItemTextArray());
	itemArray->Reset();

	const TInt count = iContent->MdcaCount();
	for(TInt i=0; i<count; i++)
		{
		itemArray->AppendL(iContent->MdcaPoint(i));
		}

	iListBox->HandleItemAdditionL();
	iListBox->SetCurrentItemIndex(0); // first item
	}

//
// FROM MCoeControlObserver
//

void CHlpListBox::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
	{
	if	(aControl == iButton)
		ReportEventL(EEventRequestExit);
	else if (aControl == iListBox)
		ReportEventL(aEventType);
	}


//
// INTERNAL
//

void CHlpListBox::SetContent(CHlpList* aContent)
	{
	delete iContent;
	iContent = aContent;
	}
