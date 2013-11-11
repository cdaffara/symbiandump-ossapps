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
* Description: This file defines class CIpsPlgPop3Plugin.
*
*/


#ifndef IPSPLGPOP3PLUGIN_H
#define IPSPLGPOP3PLUGIN_H

#include "ipsplgsosbaseplugin.h"

class CPop3ClientMtm;
NONSHARABLE_CLASS( CIpsPlgPop3Plugin ) : public CIpsPlgSosBasePlugin
{
public:

	IMPORT_C static CIpsPlgPop3Plugin* NewL();
    IMPORT_C static CIpsPlgPop3Plugin* NewLC();

    /**
    * Destructor.
    */
    virtual ~CIpsPlgPop3Plugin();
    
public: // Methods

    void PopulateNewMailL( 
        const TFSMailMsgId& aMailboxId , 
        TMsvId aNewId , 
        TMsvId aParentId );

public: // From CFSMailPlugin

    TBool MailboxHasCapabilityL(
        TFSMailBoxCapabilities aCapability,
        TFSMailMsgId aMailBoxId );

    TInt RefreshNowL( );

    void RefreshNowL(
        const TFSMailMsgId& aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId,
        const TBool aSilentConnection=EFalse );

    void ListFoldersL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        RPointerArray<CFSMailFolder>& aFolderList );

    void ListFoldersL(
        const TFSMailMsgId& aMailBoxId,
        RPointerArray<CFSMailFolder>& aFolderList );

    TFSMailMsgId GetStandardFolderIdL(
        const TFSMailMsgId& aMailBoxId,
        const TFSFolderType aFolderType );

    void FetchMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const RArray<TFSMailMsgId>& aMessageIds,
        TFSMailDetails aDetails,
        MFSMailRequestObserver& aObserver,
        TInt aRequestId );

    void FetchMessagesPartsL(
        const RArray<TFSMailMsgId>& aMessagePartIds,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );

    void FetchMessagePartsL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TFSMailMsgId& aMessageId,
        const RArray<TFSMailMsgId>& aMessagePartIds,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        const TUint aPreferredByteCount);
 
    
protected:

    /**
     *
     */
    void HandleOpCompletedL(
        CIpsPlgSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode );
    
private:

    /**
     *
     */
    CIpsPlgPop3Plugin( );

    /**
     *
     */
    void ConstructL( );

private:
    CPop3ClientMtm* iClientMtm;
  };

#endif /* IPSPLGPOP3PLUGIN_H */

// End of File
