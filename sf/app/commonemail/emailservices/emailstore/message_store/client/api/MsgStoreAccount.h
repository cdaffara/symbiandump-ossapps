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
* Description:  Message store account.
*
*/



#ifndef __MSG_STORE_ACCOUNT_H__
#define __MSG_STORE_ACCOUNT_H__

#include <e32base.h>

const TInt KMaxAccountNameLen = 256;

/**
  Stores the informationof an account.
*/
class CMsgStoreAccount : public CBase
{
public:
    /** Constructs with the owner id and the account name.
        \param aOwnerUid, the owner, for freestyle, the plug-in implementation UID can be used for this.
        \param aName,  the name of the account to be used, This is needed in case when a plugin needs
                       to create multiple accounts
    */	
    IMPORT_C static CMsgStoreAccount* NewL( const TInt32 aOwnerUid, const TDesC& aName );
    
    IMPORT_C static CMsgStoreAccount* NewLC( const TInt32 aOwnerUid, const TDesC& aName );

    /** Destructor */
    virtual ~CMsgStoreAccount();
    
    /** Gets the owner ID */
    IMPORT_C TInt32 Owner() const;

    /** Gets the name */
    IMPORT_C const TDesC& Name() const;
    
    /** Sets the owner ID */
    IMPORT_C void SetOwner( TInt32 aOwner );

    /** Gets the name */
    IMPORT_C void SetNameL( const TDesC& aName );

private:

	CMsgStoreAccount();
	
    void ConstructL( const TInt32 aOwnerUid, const TDesC& aName );

private:
	TInt32 iOwnerUid;
    RBuf   iName;
};

#endif