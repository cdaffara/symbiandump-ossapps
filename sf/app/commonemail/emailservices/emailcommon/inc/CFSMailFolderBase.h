/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email folder base object
*
*/


#ifndef __FSMAILFOLDERBASE_H
#define __FSMAILFOLDERBASE_H

#include "CFSMailMessage.h"
#include "cemailextensionbase.h"

// <qmail>
class NmFolder;
class NmFolderPrivate;
// </qmail>

/**
 *  class for handling mailbox folder data
 *
 *  @lib FSFWCommonLib
 *  @since S60 S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailFolderBase ) : public CExtendableEmail
{
 public:
    
    /**
     * Two-phased constructor.
     *
     * @param aFolderId folder id in plugin containing folder
     */
     IMPORT_C static CFSMailFolderBase* NewL(const TFSMailMsgId aFolderId);
    
    /**
     * Two-phased constructor.
     *
     * @param aFolderId folder id in plugin containing folder
     */
     IMPORT_C static CFSMailFolderBase* NewLC(const TFSMailMsgId aFolderId);

    /**
     * Destructor.
     *
     */  
     IMPORT_C virtual ~CFSMailFolderBase();

    /**
     * folder id accessor
     *
     * @return folder id
     */
     IMPORT_C TFSMailMsgId GetFolderId() const;

    /**
     * parent folder id accessor
     *
     * @return parent folder id
     */
     IMPORT_C TFSMailMsgId GetParentFolderId() const;

    /**
     * parent folder id mutator
     *
     * @param aFolderId parent folder id to be set
     */
     IMPORT_C void SetParentFolderId( const TFSMailMsgId aFolderId );

    /**
     * folder name accessor
     *
     * @return folder name
     */
     IMPORT_C TDesC& GetFolderName() const;

    /**
     * Folder name mutator. Note ! Some clients may ignore names set for
     * standard folders (TFSFolderType being EFSInbox, EFSOutbox, EFSDraftsFolder,
     * EFSSentFolder, or EFSDeleted) and display own localized names for them.
     * E.g. Email UI behaves this way.
     *
     * @param aFolderName folder name to be set
     */
     IMPORT_C void SetFolderName(const TDesC& aFolderName);

    /**
     * folder type accessor
     *
     * @return folder type
     */
     IMPORT_C TFSFolderType GetFolderType() const;

    /**
     * folder type mutator
     *
     * @param aFolderType folder type to be set
     */
     IMPORT_C void SetFolderType( const TFSFolderType aFolderType );

    /**
     * returns id of mailbox containing this folder
     *
     * @return mailbox id
     */
     IMPORT_C TFSMailMsgId GetMailBoxId() const;

    /**
     * folder mailbox id mutator
     *
     * @param aMailBoxId mailbox id to be set
     */
     IMPORT_C void SetMailBoxId( const TFSMailMsgId aMailBoxId );

    /**
     * returns message count contained by this folder
     *
     * @return message count
     */
     IMPORT_C TUint GetMessageCount() const;

    /**
     * returns unread message count contained by this folder
     *
     * @return unread message count
     */
     IMPORT_C TUint GetUnreadCount() const;

    /**
     * returns unseen message count contained by this folder
     *
     * @return unseen message count
     */
     IMPORT_C TUint GetUnseenCount() const;

    /**
     * returns subfolder count contained by this folder
     *
     * @return subfolder count
     */
     IMPORT_C TUint GetSubFolderCount() const;

    /**
     * set message count contained by this folder
     *
     * @param aMessageCount message count
     */
     IMPORT_C void SetMessageCount( const TUint aMessageCount );

    /**
     * set unread message count contained by this folder
     *
     * @param aMessageCount unread message count
     */
     IMPORT_C void SetUnreadCount( const TUint aMessageCount );

    /**
     * set unseen message count contained by this folder
     *
     * @param aMessageCount unseen message count
     */
     IMPORT_C void SetUnseenCount( const TUint aMessageCount );

    /**
     * set subfolder count contained by this folder
     *
     * @param subfolder count
     */
     IMPORT_C void SetSubFolderCount( const TUint aMessageCount );

    /**
     * blocks copying to this folder from given folder types
     *
     * @param aFolderTypes blocked folder types
     * @param aMailBoxStatus mailbox status (online / offline) when
     * blocking is done
     */
     IMPORT_C void BlockCopyFromL( RArray<TFSFolderType> aFolderTypes, 
                                    TFSMailBoxStatus aMailBoxStatus );
    /**
     * blocks moving to this folder from given folder types
     *
     * @param aFolderTypes blocked folder types
     * @param aMailBoxStatus mailbox status (online / offline) when
     * blocking is done
     */
     IMPORT_C void BlockMoveFromL( RArray<TFSFolderType> aFolderTypes, 
                                    TFSMailBoxStatus aMailBoxStatus );

    // <qmail>
    /**
     * returns nmfolder constructed with shared data,
     * ownership is transferred to caller
     */
    IMPORT_C NmFolder* GetNmFolder();
    // </qmail>
     
protected:

    /**
     * C++ default constructor.
     */
     CFSMailFolderBase();

// <qmail>
     /**
      * Two-phased constructor
      */
    IMPORT_C void ConstructL( const TFSMailMsgId aFolderId );
// </qmail>  

// <qmail> unnecessary iFolderId member removed </qmail>

    /**
     * blocked folders in copying
     */
     RArray<TFSFolderType>    iCopyOfflineBlocked;
     RArray<TFSFolderType>    iCopyOnlineBlocked;

    /**
     * blocked folders in moving
     */
     RArray<TFSFolderType>    iMoveOfflineBlocked;
     RArray<TFSFolderType>    iMoveOnlineBlocked;


private: // data

    // <qmail>
    // unused data variables removed
    // </qmail>
    
    // <qmail>
    mutable TPtrC16 iTextPtr;
    QExplicitlySharedDataPointer<NmFolderPrivate> iNmPrivateFolder;
    // </qmail>
};

#endif // __FSMAILFOLDERBASE_H
