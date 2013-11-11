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
* Description: This file defines class CIpsPlgImap4Plugin.
*
*/


#ifndef IPSPLGIMAP4PLUGIN_H
#define IPSPLGIMAP4PLUGIN_H

#include "ipsplgsosbaseplugin.h"
    
NONSHARABLE_CLASS( CIpsPlgImap4Plugin ) : public CIpsPlgSosBasePlugin
{
public:

	IMPORT_C static CIpsPlgImap4Plugin* NewL();
    IMPORT_C static CIpsPlgImap4Plugin* NewLC();

    /**
    * Destructor.
    */
    virtual ~CIpsPlgImap4Plugin();
    
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

    void FetchMessagePartsL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TFSMailMsgId& aMessageId,
        const RArray<TFSMailMsgId>& aMessagePartIds,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        const TUint aPreferredByteCount);
    
    
    void MoveMessagesL(
        const TFSMailMsgId& aMailBoxId, 
        const RArray<TFSMailMsgId>& aMessageIds, 
        const TFSMailMsgId& aSourceFolderId, 
        const TFSMailMsgId& aDestinationFolderId );
    
    TInt MoveMessagesL( 
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aMessageIds, 
        const TFSMailMsgId& aSourceFolderId,
        const TFSMailMsgId& aDestinationFolderId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
    
    void PopulateNewMailL( 
        const TFSMailMsgId& aMailboxId, 
        TMsvId aNewId, 
        TMsvId aParentId );
    
protected:

    /**
     *
     */
    void HandleOpCompletedL(
        CIpsPlgSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode );    
    
private:


    CIpsPlgImap4Plugin( );

    void ConstructL( );

    /**
     * List subfolders of given id. Only direct subfolders of given folder are returned.
     *
     * @param aMailBoxId defines mailbox where parent folder is.
     * @param aFolderId defines parent folder id, otherwise KFolderIdNull.
     * @param aFolderList plugin writes results in this array given by user.
     */
    void DoListFoldersL(
        TFSMailMsgId aMailBoxId,
        TFSMailMsgId aFolderId,
        RPointerArray<CFSMailFolder>& aFolderList,
        TBool aOnlyDirect );

    /**
     * Adds folders to list.
     *
     * @param aId defines parent folder id.
     * @param aFolders array of folder entry IDs.
     */
    void HandleFoldersL(
        TFSMailMsgId aId,
        CMsvEntrySelection& aFolders,
        TBool aOnlyDirect );

    /**
     * Checks children recursively.
     *
     * @param aId defines parent folder id.
     * @param aEntry defines parent folder entry.
     * return List of child folders.
     */
    CMsvEntrySelection* GetChildrenL(
        TMsvId aId,
        CMsvEntry& aEntry,
        TBool aOnlyDirect ) const;

    /**
     * Checks if folder has subfolder entries.
     *
     * @param aId defines parent folder id.
     * return ETrue if folder has subfolder.
     */
    TBool HasChildFoldersL( TMsvId aId ) const; 
    
    
private: // data

  };

#endif /* IPSPLGIMAP4PLUGIN_H */

// End of File
