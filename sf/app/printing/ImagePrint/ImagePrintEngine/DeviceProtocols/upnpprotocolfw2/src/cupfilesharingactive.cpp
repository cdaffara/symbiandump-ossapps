/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPFileSharingActive class
*
*/


#include <upnpfilesharing.h>
#include <upnpitem.h>
#include <e32debug.h>

#include "cupfilesharingactive.h"

_LIT8( KRoot, "0" );


// -----------------------------------------------------------------------------
// CUPFileSharingActive::NewL
// -----------------------------------------------------------------------------
//
CUPFileSharingActive* CUPFileSharingActive::NewL()
	{
	CUPFileSharingActive* self = new (ELeave) CUPFileSharingActive;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::RunL
// -----------------------------------------------------------------------------
//
void CUPFileSharingActive::RunL()
	{
	CActiveScheduler::Stop();
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::DoCancel
// -----------------------------------------------------------------------------
//	
void CUPFileSharingActive::DoCancel()
	{
	; 
	}
	
// -----------------------------------------------------------------------------
// CUPFileSharingActive::RunError
// -----------------------------------------------------------------------------
//
TInt CUPFileSharingActive::RunError( TInt aError )
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::ShareItemL
// -----------------------------------------------------------------------------
//	
void CUPFileSharingActive::ShareItemL( CUpnpItem& aItem )
	{
	iStatus = KRequestPending;
	iFileSharing->ShareItemL( KRoot, aItem, iStatus );
	SetActive();
	CActiveScheduler::Start();
	User::LeaveIfError( iStatus.Int() );
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::UnShareItem
// -----------------------------------------------------------------------------
//	
void CUPFileSharingActive::UnshareItemL( TInt aId )
	{
	iFileSharing->UnshareItemL( aId, iStatus );
	SetActive();
	CActiveScheduler::Start();
	User::LeaveIfError( iStatus.Int() );  
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::GetSharedItemL
// -----------------------------------------------------------------------------
//
void CUPFileSharingActive::GetSharedItemL( TInt aId, CUpnpItem& aItem )
	{
	iFileSharing->GetSharedItemL( aId, aItem , iStatus );
	SetActive();
	CActiveScheduler::Start();
	User::LeaveIfError(iStatus.Int());
	}

// Constructors and destructor
// -----------------------------------------------------------------------------
// CUPFileSharingActive::CUPFileSharingActive
// -----------------------------------------------------------------------------
//
CUPFileSharingActive::CUPFileSharingActive():CActive( EPriorityNormal )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::ConstructL
// -----------------------------------------------------------------------------
//
void CUPFileSharingActive::ConstructL()
	{
	iFileSharing = CUpnpFileSharing::NewL();
	}

// -----------------------------------------------------------------------------
// CUPFileSharingActive::~CUPFileSharingActive
// -----------------------------------------------------------------------------
//
CUPFileSharingActive::~CUPFileSharingActive()
	{
	Cancel();
	delete iFileSharing;
	}

//  End of File
