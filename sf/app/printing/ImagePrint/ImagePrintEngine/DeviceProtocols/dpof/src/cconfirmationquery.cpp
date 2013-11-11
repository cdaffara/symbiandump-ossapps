/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/


#include <avkon.hrh>

#include "clog.h"
#include "cconfirmationquery.h"

CConfirmationQuery::CConfirmationQuery( MConfirmationNotify& aNotify ) : CActive( CActive::EPriorityStandard),
iNotify (aNotify)
		{
		CActiveScheduler::Add( this );
		}

CConfirmationQuery::~CConfirmationQuery()
	{
	Cancel();
	}

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CConfirmationQuery::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CConfirmationQuery* CConfirmationQuery::NewL( MConfirmationNotify& aNotify )  
	{
	CConfirmationQuery* self = CConfirmationQuery::NewLC( aNotify );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CConfirmationQuery::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CConfirmationQuery* CConfirmationQuery::NewLC( MConfirmationNotify& aNotify )
	{
	CConfirmationQuery* self;
	self = new ( ELeave ) CConfirmationQuery( aNotify );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CConfirmationQuery::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CConfirmationQuery::ConstructL()
	{

	}

void CConfirmationQuery::StartWaitingAnswer( )
	{	
	// Start active object and set iStatus = KRequestPending
	SetActive();
	}

void CConfirmationQuery::RunL()
	{
	LOG1("[CConfirmationQuery::RunL]\t iStatus: %d", iStatus.Int());	
	if (iStatus == KErrNone || iStatus == KErrCancel || iStatus == EAknSoftkeyYes || iStatus == EAknSoftkeyNo )
		{		
		iNotify.ConfirmationQueryCompleteL(iStatus.Int());		
		}	
	LOG("[CConfirmationQueryCConfirmationQuery::RunL]\t out");
	}

void CConfirmationQuery::RunError()
	{

	}

void CConfirmationQuery::DoCancel()
	{

	}

// End of file
