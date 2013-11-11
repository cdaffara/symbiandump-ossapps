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

#include "clog.h"
#include "directprintapp.hrh"
#include "directprintsettinglistbox.h"

CDirectPrintSettingListBox::CDirectPrintSettingListBox()
	{
	}

CDirectPrintSettingListBox::~CDirectPrintSettingListBox()
	{
	}

CDirectPrintSettingListBox* CDirectPrintSettingListBox::NewL(const TRect& aRect)
	{
	CDirectPrintSettingListBox* self = CDirectPrintSettingListBox::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintSettingListBox* CDirectPrintSettingListBox::NewLC(const TRect& aRect)
	{
	CDirectPrintSettingListBox* self = new(ELeave) CDirectPrintSettingListBox();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

void CDirectPrintSettingListBox::ConstructL(const TRect& aRect)
	{
	CreateWindowL();
	SetRect(aRect);
	}

void CDirectPrintSettingListBox::EditItemL(TInt aIndex, TBool aCalledFromMenu)
	{
	TBuf<25> beforeText, afterText;
	beforeText.Copy((*SettingItemArray())[aIndex]->SettingTextL());

	CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);

	if (iObserver)
		{
		afterText.Copy((*SettingItemArray())[aIndex]->SettingTextL());
		TBool changed = (beforeText.Compare(afterText) != 0);
		iObserver->HandleEditItemL(aIndex, changed);
		}
	}

void CDirectPrintSettingListBox::SetSettingListObserver(MDirectPrintSettingListObserver* aObserver)
	{
	iObserver = aObserver;
	}

