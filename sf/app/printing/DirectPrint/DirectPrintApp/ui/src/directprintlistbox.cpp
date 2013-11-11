/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
*
*/

#include <barsread.h>
#include <DirectPrintApp.rsg>

#include "DirectPrintListBox.h"

CDirectPrintListBox::CDirectPrintListBox()
	{
	iListObserver = NULL;
	}

CDirectPrintListBox::~CDirectPrintListBox()
	{
	delete iListBox;
	}

CDirectPrintListBox* CDirectPrintListBox::NewL(const TRect& aRect, CDesCArray* aItemArray)
	{
	CDirectPrintListBox* self = CDirectPrintListBox::NewLC(aRect, aItemArray);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintListBox* CDirectPrintListBox::NewLC(const TRect& aRect, CDesCArray* aItemArray)
	{
	CDirectPrintListBox* self = new(ELeave) CDirectPrintListBox();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aItemArray);
	return self;
	}

void CDirectPrintListBox::ConstructL(const TRect& aRect, CDesCArray* aItemArray)
	{
	CreateWindowL();
	SetRect(aRect);

	InitializeControlsL();
	iListBox->SetRect(Rect());
	iListBox->Model()->SetItemTextArray(aItemArray);
	iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListBox->HandleItemAdditionL();

	ActivateL();
	}

TInt CDirectPrintListBox::CountComponentControls() const
	{
	return (int)ELastControl;
	}

CCoeControl* CDirectPrintListBox::ComponentControl(TInt aIndex) const
	{
	CCoeControl* ctrl = NULL;

	switch ( aIndex )
		{
		case EListBox:
			{
			ctrl = iListBox;
			}
			break;
		default:
			break;
		}

	return ctrl;
	}

void CDirectPrintListBox::InitializeControlsL()
	{
	TBuf<50> buf;
	//_LIT ( KStringHeader, "%S\t%S" );
	iListBox = new(ELeave) CAknDoubleStyleListBox;
	iListBox->SetContainerWindowL(*this);
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC(reader, R_DIRECTPRINT_MAINVIEW_LISTBOX);
		iListBox->ConstructFromResourceL(reader);
		CleanupStack::PopAndDestroy(); // reader internal state
		}

	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn);
	iListBox->ScrollBarFrame()->DrawScrollBarsNow();
	iListBox->SetFocus( ETrue );
	iListBox->SetListBoxObserver(this);
    iListBox->View()->SetListEmptyTextL( _L("No Data") );
	}

void CDirectPrintListBox::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	switch (aEventType)
		{
		case EEventItemClicked:
		case EEventEnterKeyPressed:
			{
			TInt index = iListBox->CurrentItemIndex();
			if (iListObserver)
				{
				iListObserver->HandleListIndexL(index);
				}
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintListBox::SetItemL(CDesCArray* aItemArray)
	{
	iListBox->Model()->SetItemTextArray(aItemArray);
	iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListBox->HandleItemAdditionL();
	}

void CDirectPrintListBox::SetListObserver(MDirectPrintListObserver* aObserver)
	{
	iListObserver = aObserver;
	}

TInt CDirectPrintListBox::DeleteCurrentItemL()
	{
	TInt index = -1;
	if (iListBox->Model()->NumberOfItems() > 0)
		{
		index = iListBox->CurrentItemIndex();
		CDesCArray* itemArray = static_cast<CDesC16ArrayFlat*>(iListBox->Model()->ItemTextArray());
		itemArray->Delete(index);
		}
	iListBox->HandleItemAdditionL();

	return index;
	}

void CDirectPrintListBox::RedrawListL()
	{
	iListBox->HandleItemAdditionL();
	iListBox->DrawNow();
	DrawNow();
	}

TInt CDirectPrintListBox::CurrentIndex()
	{
	return iListBox->CurrentItemIndex();
	}

void CDirectPrintListBox::SetFocusL(TInt aIndex)
	{
	iListBox->SetCurrentItemIndexAndDraw(aIndex);
	}

