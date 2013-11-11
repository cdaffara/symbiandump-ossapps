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
* Description:  Message store account client implementation.
*
*/



//<cmail>
#include "MsgStoreAccount.h"
//</cmail>

EXPORT_C CMsgStoreAccount* CMsgStoreAccount::NewL( const TInt32 aOwnerUid, const TDesC& aName )
{
	CMsgStoreAccount* self = new(ELeave) CMsgStoreAccount();
	CleanupStack::PushL(self);
	self->ConstructL(aOwnerUid, aName);
	CleanupStack::Pop( self );
	return self;
}

EXPORT_C CMsgStoreAccount* CMsgStoreAccount::NewLC( const TInt32 aOwnerUid, const TDesC& aName )
{
	CMsgStoreAccount* self = new(ELeave) CMsgStoreAccount();
	CleanupStack::PushL(self);
	self->ConstructL(aOwnerUid, aName);
	return self;
}

/** Constructor */
CMsgStoreAccount::CMsgStoreAccount()
{
}

/** Gets the owner ID */
EXPORT_C TInt32 CMsgStoreAccount::Owner() const 
{ 
	return iOwnerUid; 
};

/** Gets the name */
EXPORT_C const TDesC& CMsgStoreAccount::Name() const 
{ 
	return iName; 
};

/** Gets the owner ID */
EXPORT_C void CMsgStoreAccount::SetOwner( TInt32 aOwner )
{ 
	iOwnerUid = aOwner; 
};

/** Gets the name */
EXPORT_C void CMsgStoreAccount::SetNameL( const TDesC& aName )
{ 
	if ( iName.MaxLength() < aName.Length() )
	{
		iName.ReAllocL( aName.Length() );
	}
	iName.Copy( aName );
};

/** Destructor */
CMsgStoreAccount::~CMsgStoreAccount()
{
	iName.Close();
}

void CMsgStoreAccount::ConstructL(const TInt32 aOwnerUid, const TDesC& aName)
{
	iOwnerUid = aOwnerUid;
	iName.CreateL( aName );
}
