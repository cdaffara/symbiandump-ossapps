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
* Description: This file contains CMenuListBox class definition.
*
*/

// INCLUDE FILES
#include "MenuListBox.h"
#include <aknview.h>

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CMainMenuView::CMenuListBox
// 
// c++ constructor
// ---------------------------------------------------------
//
CMenuListBox::CMenuListBox(TAppUIViewNumber aViewId) 
	: CAknSingleStyleListBox(), iViewId(aViewId)
	{
	}

// ---------------------------------------------------------
// CMenuListBox::ConstructL
// 
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMenuListBox::ConstructL()
	{
	
	}

// ---------------------------------------------------------
// CMenuListBox::~CMenuListBox
// 
// Destructor
// ---------------------------------------------------------
//
CMenuListBox::~CMenuListBox()
	{

	}

// ---------------------------------------------------------
// CMenuListBox::NewL
// 
// Symbian OS public constructor
// @param aViewId - id of view to identify position in array where focus position will be saved to
// or retrieved from
//
// @return - pointer to an instance of CMenuListBOx
// ---------------------------------------------------------
//
CMenuListBox* CMenuListBox::NewL(TAppUIViewNumber aViewId)
	{
	CMenuListBox* self = new(ELeave) CMenuListBox(aViewId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------
// CMenuListBox::SaveFocusPosition
// 
// Saves current focus position to CAppUiAppUi object
// ---------------------------------------------------------
//
void CMenuListBox::SaveFocusPosition()
	{
	((CAppUIAppUi*)iCoeEnv->AppUi())->SaveFocusPosition(iViewId, this->CurrentItemIndex());
	}

// ---------------------------------------------------------
// CMenuListBox::SetPreviousFocus
// 
// Retrieves previous focus position from CAppUiAppUi object and sets focus
// to that position
// ---------------------------------------------------------
//
void CMenuListBox::SetPreviousFocus()
	{
	TInt previousPosition = ((CAppUIAppUi*)iCoeEnv->AppUi())->GetFocusPosition(iViewId);
	if(previousPosition != KErrNotFound)
		{
		if(previousPosition < this->Model()->NumberOfItems() && previousPosition != 0)
			{
			this->SetCurrentItemIndex(previousPosition);
			}
		if(previousPosition >= this->Model()->NumberOfItems()) 
			{
			this->SetCurrentItemIndex(0); // setting current item to first element if pervious position is grater then current. 
			}
		this->SetTopItemIndex(0);
		}
	}

// End of File
