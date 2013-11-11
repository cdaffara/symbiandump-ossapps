/*
* Copyright (c)2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CEmailFolder.
*
*/

#include "emailfolder.h"
#include "emailmailbox.h"
#include "emailapiutils.h"
#include "messageiterator.h"
#include "emailsorting.h"
#include "CFSMailFolder.h"
#include "CFSMailPlugin.h"
#include "emailclientapi.hrh"
#include "emailclientapiimpldefs.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailFolder* CEmailFolder::NewLC( 
    CPluginData& aPluginData,
    const TFolderId& aFolderId,  
    CFSMailFolder *aFolder )
    {
    CEmailFolder* self = new ( ELeave ) 
        CEmailFolder( aPluginData, aFolderId, aFolder );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailFolder* CEmailFolder::NewL( 
    CPluginData& aPluginData,
    const TFolderId& aFolderId,  
    CFSMailFolder *aFolder )
    {
    CEmailFolder* self = CEmailFolder::NewLC( aPluginData, aFolderId, aFolder);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailFolder::CEmailFolder( 
    CPluginData& aPluginData, 
    const TFolderId& aFolderId,
    CFSMailFolder* aFolder) : 
    iPluginData( aPluginData ), 
    iFolderType( EOther ), 
    iFolderId( aFolderId ),
    iFolder( aFolder )
    {
    }
   

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailFolder::ConstructL()
    {
    iPlugin = iPluginData.ClaimInstance();
    
    User::LeaveIfNull( iFolder );
    
    const TFSFolderType fsType = iFolder->GetFolderType();
    switch ( fsType )
        {
        case EFSInbox:
            iFolderType = EInbox;
            break;
        case EFSOutbox:
            iFolderType = EOutbox;
            break;
        case EFSDraftsFolder:
            iFolderType = EDrafts;
            break;
        case EFSSentFolder:
            iFolderType = ESent;
            break;
        case EFSDeleted:
            iFolderType = EDeleted;
            break;
        case EFSOther:
        default:
            iFolderType = EOther;
            break;
        }
    iParentId = TFolderId( 
        iFolder->GetParentFolderId().Id(), 
        iFolderId.iMailboxId );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailFolder::~CEmailFolder()
    {
    iPluginData.ReleaseInstance();
    delete iFolder;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailFolder::InterfaceId() const
    {
    return KEmailIFUidFolder;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailFolder::Release()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TFolderId CEmailFolder::FolderId() const
    {
    return iFolderId;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TFolderId CEmailFolder::ParentFolderId() const
    {
    return iParentId;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TFolderType CEmailFolder::FolderType() const
    {
    return iFolderType;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailFolder::Name() const
    {
    if ( !iFolder )
        return KNullDesC();
    return TPtrC ( iFolder->GetFolderName() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailFolder::GetSubfoldersL( RFolderArray& aSubfolders ) const
    {
    User::LeaveIfNull( iFolder );

    RPointerArray<CFSMailFolder> folders;
    CleanupResetAndDestroy<RPointerArray<CFSMailFolder> >::PushL( folders );

    iFolder->GetSubFoldersL( folders );
    
    TInt res( folders.Count() );
    
    for ( TInt i = 0; i < res; i++ )
        {
        const CFSMailFolder* fsfolder = folders[i];
        const TEntryId id = fsfolder->GetFolderId().Id();
        const TFolderId folderId( id, iFolderId.iMailboxId.iId );
        MEmailFolder* folder = CEmailFolder::NewL( iPluginData, folderId, folders[i] );
        aSubfolders.AppendL( folder );
        }
    CleanupStack::Pop( &folders );
    folders.Close();
    return res;
    }

// -----------------------------------------------------------------------------
//  CEmailFolder::MessagesL
// -----------------------------------------------------------------------------
MMessageIterator* CEmailFolder::MessagesL(
        const RSortCriteriaArray& aCriteria )
    {
    RArray<TFSMailSortCriteria> sortCriterias;
    CleanupClosePushL( sortCriterias );
    CEmailFolder::ToFsSortCriteriaL( aCriteria, sortCriterias );

    MFSMailIterator* fsIter = iFolder->ListMessagesL( EFSMsgDataEnvelope, sortCriterias );
    TUint count = iFolder->GetMessageCount();

    CleanupStack::PopAndDestroy( &sortCriterias );
    CMessageIterator* iter = CMessageIterator::NewL( 
        fsIter, iPluginData, count );

    return iter;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailFolder::DeleteMessagesL( const REmailMessageIdArray& aMessageIds )
    {
    RArray<TFSMailMsgId> fsArray;
    CleanupClosePushL( fsArray );
    const TInt count( aMessageIds.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        TMessageId msgId = aMessageIds[i];
        if ( iFolderId != msgId.iFolderId )
            {
            // not all messages in the same folder, plugin API doesn't accept this.
            User::Leave( KErrArgument );
            }
        fsArray.AppendL( FsMsgId( iPluginData, msgId ) );
        }
    iPlugin->DeleteMessagesByUidL( 
            FsMsgId( iPluginData, iFolderId.iMailboxId ), 
            FsMsgId( iPluginData, iFolderId ), 
            fsArray );
    CleanupStack::PopAndDestroy( &fsArray );
    }

// -----------------------------------------------------------------------------
// Maps email api sort criteria to sort criteria type that protocol plugin
// accepts.
// -----------------------------------------------------------------------------
void CEmailFolder::ToFsSortCriteriaL( 
    const RSortCriteriaArray& aSortCriteria, 
    RArray<TFSMailSortCriteria>& aFsCriteria )
    {
    const TFSMailSortField fieldValues[] = {
        EFSMailDontCare,        
        EFSMailSortByDate,      
        EFSMailSortBySender,    
        EFSMailSortByRecipient, 
        EFSMailSortBySubject,   
        EFSMailSortByPriority,  
        EFSMailSortByFlagStatus,
        EFSMailSortByUnread,    
        EFSMailSortBySize,      
        EFSMailSortByAttachment };

    for ( TInt i=0; i < aSortCriteria.Count(); i++ )
        {
        const TEmailSortCriteria& criteria  = aSortCriteria[i];
        __ASSERT_ALWAYS( criteria.iField < sizeof( fieldValues ) / sizeof (fieldValues [i] ),
            Panic( EMailPanicSortMapIndexOutOfBounds ) );
        TFSMailSortCriteria fsCriteria;
        fsCriteria.iField = fieldValues[ criteria.iField ];
        if ( criteria.iAscending )
            {
            fsCriteria.iOrder = EFSMailAscending;
            }
        else
            {
            fsCriteria.iOrder = EFSMailDescending;
            }
        aFsCriteria.AppendL( fsCriteria );
        }
    }

// End of file
