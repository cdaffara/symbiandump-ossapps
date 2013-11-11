/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Email Client API definitions
*
*/

#ifndef __EMAILAPIDEFS
#define __EMAILAPIDEFS

#include <e32base.h>
#include <f32file.h>

namespace EmailInterface {

/**
* Id for message, message part, folder and mailbox entries
*/
typedef TUint TEntryId;

/**
* Interface id. Each implementation of MEmailInterface has unique id value.
*/
typedef TInt TEmailTypeId;

const TEntryId KUndefinedEntryId = 0;

/**
 * base interface for all email interfaces available to clients
 * @since S60 v5.0
 */
class MEmailInterface
{
public:
    /** returns interface id (kind of RTTI) */
    virtual TEmailTypeId InterfaceId() const = 0;

    /** frees memory allocated by interface impl. */    
    virtual void Release() = 0;
};

/**
 * Defines abstraction for entry ID. Used by message, folder, mailbox etc types.
 * @since S60 v5.0
 */
class TBaseId
{
public:
    inline TBaseId() : iId( KUndefinedEntryId ){}
    inline TBaseId( TEntryId aId ) : iId( aId ){}
    
    TEntryId   iId;
};

/**
 * Defines mailbox ID
 * @since S60 v5.0
 */
class TMailboxId : public TBaseId  
{
public:
    inline TMailboxId() : 
        TBaseId(){}
    inline TMailboxId( TEntryId aId ) : TBaseId( aId ){}
    inline TBool operator==( const TMailboxId& aMailboxId ) const {
        return ( aMailboxId.iId == iId ); }
};

/**
 * Defines folder ID which is associated with a mailbox
 * @since S60 v5.0
 */
class TFolderId : public TBaseId
{
public:
    // parent mailbox
    TMailboxId iMailboxId;
    inline TFolderId( TEntryId aId, const TMailboxId& aMailboxId ) : 
        TBaseId( aId ),iMailboxId( aMailboxId.iId ){}
        
    inline TFolderId() : TBaseId(), iMailboxId() {}
    
    inline TBool operator==( const TFolderId& aFolderId ) const {
        return ( iMailboxId.iId == aFolderId.iMailboxId.iId &&
        iId == aFolderId.iId ); }
        
    inline TBool operator!=( const TFolderId& aFolderId ) const {
        return !( aFolderId == *this ); }
};

/**
 * Defines email message ID which is associated with a mailbox and folder
 * @since S60 v5.0
 */
class TMessageId : public TBaseId
{
public:
    inline TMessageId() : TBaseId(), iFolderId(){}
    
    inline TMessageId( TEntryId aMsgId, TEntryId aFolderId, TMailboxId aMailboxId ) : 
    TBaseId( aMsgId ), iFolderId( aFolderId, aMailboxId ){}

    inline TBool operator==( const TMessageId& aMessageId ) const {
        return ( iFolderId == aMessageId.iFolderId &&
        iId == aMessageId.iId ); }
        
    inline TBool operator!=( const TMessageId& aMessageId ) const {
        return !( aMessageId == *this ); }
    
    /**
    * parent folder id.
    */
    TFolderId iFolderId;
};


/**
 * Message content (part) id
 * @since S60 v5.0
 */
class TMessageContentId : public TBaseId
{
public:
    inline TMessageContentId(): TBaseId(), iMessageId() {}
    
    inline TMessageContentId( TEntryId aContentId, TEntryId aMsgId, TEntryId aFolderId, TMailboxId aMailboxId ) : 
    TBaseId( aContentId ), iMessageId( aMsgId, aFolderId, aMailboxId ){}
    
    inline TBool operator==( const TMessageContentId& aContentId ) const {
        return ( iMessageId == aContentId.iMessageId &&
        iId == aContentId.iId ); }
        
    inline TBool operator!=( const TMessageContentId& aContentId ) const {
        return !( aContentId == *this ); }
    
    // parent message
    TMessageId iMessageId;
};

typedef RArray<TMessageId> REmailMessageIdArray;

typedef RArray<TFolderId>  REmailFolderIdArray;

typedef RArray<TMailboxId>     REmailMailboxIdArray;

} // EmailInterface

#endif // __EMAILAPIDEFS
