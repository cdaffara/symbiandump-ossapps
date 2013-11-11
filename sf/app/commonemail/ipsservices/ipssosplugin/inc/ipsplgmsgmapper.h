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
* Description: This file defines class CIpsPlgMsgMapper.
*
*/


#ifndef IPSPLGMSGMAPPER_H
#define IPSPLGMSGMAPPER_H


class CMsvSession;
class CMsvEntry;
class CFSMailMessage;
class CFSMailMessagePart;
class CFSMailAddress;
class CIpsPlgSosBasePlugin;

/**
 *  
 *
 *  @lib
 *  @since FS 1.0
 */
NONSHARABLE_CLASS( CIpsPlgMsgMapper ) : public CBase
    {
public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return Created object
     */
    static CIpsPlgMsgMapper* NewL( 
        CMsvSession& aSession, 
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return Created object
     */
    static CIpsPlgMsgMapper* NewLC( 
        CMsvSession& aSession, 
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Class destructor
     *
     * @since FS 1.0
     */
    virtual ~CIpsPlgMsgMapper();
    
    
    /**
     * Returns CFSMailMessge constructed from Symbian's message types
     * Ownership is transferred
     *
     * @since FS 1.0
     */
    CFSMailMessage* GetMailMessageL( 
        const TFSMailMsgId& aMailboxId,
        const TMsvEmailEntry& aEntry, 
        const TFSMailDetails& aDetails );

    /**
     * Returns CFSMailMessge constructed from Symbian's message types
     * Ownership is transferred
     *
     * @since FS 1.0
     */
    CFSMailMessage* GetMailMessageL( CMsvEntry& aEntry );
            
    /**
     * Constructs the FS message part objects for the child parts
     * of the entry/part referred by the message ID parameters.
     * 
     * @param aMailBoxId The mailbox where message is located
     * @param aParentFolderId The parent folder where message is located
     * @param aMessageId The id of the message that message part 
     *                   belongs to
     * @param aParentId The id of the parent message part
     * @param aParts List of contructed child parts
     */
    void GetChildPartsL( 
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentId,
        RPointerArray<CFSMailMessagePart>& aParts );
    
    /**
     * Returns a message part object filled with data from a entry
     * in Symbian message store.
     * 
     * @param aEntryId    Identifier of the message part
     * @param aMailBoxId
     * @param aMessageId
     * @return Message part object. Ownership is transferred.
     */
    CFSMailMessagePart* GetMessagePartL(
        TMsvId aEntryId,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId );
    
    /**
     * Updatest the message flags to the message store
     *
     * @param aEntryId     Identifier of the message
     * @param aMessagePart The message part containing the new flag values
     * @since FS 1.0
     */
    void UpdateMessageFlagsL(
        TMsvId aEntryId,
        const CFSMailMessage& aMessage );
    
    /**
     * Change message flags and return msvoperation if flags is modified
     *
     * @param aEntryId     Identifier of the message
     * @param aMessagePart The message part containing the new flag values
     * @param aStatus      status of msv operation observer
     * @return CMsvOperation* operation pointer or NULL
     *                        if there is no need to update flags
     * @since FS 2.0
     */
    CMsvOperation* UpdateMessageFlagsAsyncL( 
        TMsvId aEntryId,
        const CFSMailMessage& aMessage,
        TRequestStatus& aStatus );
    
    /**
     * Updatest the message flags to FS message
     *
     * @param aEntry     email entry of FS message
     * @param aMsg       updated FS message
     * @since FS 1.0
     */
    void SetFSMessageFlagsL( 
        const TMsvEmailEntry& aEntry, CFSMailMessage& aMsg );

    /**
     * Creates a new empty child part. The method only multipart types
     * currently.
     * 
     * @param aMailBoxId Mailbox identifier
     * @param aMessageId Message identifier
     * @param aParentPartId Parent message part identifier
     * @param aContentType Content type
     * @return The new message part object
     */
    CFSMailMessagePart* NewChildPartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TDesC& aContentType );
    
protected:

    /**
     * Class constructor
     *
     * @since FS 1.0
     */
    CIpsPlgMsgMapper( 
        CMsvSession& aSession, 
        CIpsPlgSosBasePlugin& aPlugin );

private:

    /**
     * Symbian 2nd phase construct
     *
     * @since FS 1.0
     */
    void ConstructL();

    /**
     * 
     *
     * @since FS 1.0
     */
    void SetDateL( const TMsvEntry& aEntry, CFSMailMessage& aMsg );
    
    /**
     * 
     *
     * @since FS 1.0
     */
    void SetSubjectL( const TMsvEntry& aEntry, CFSMailMessage& aMsg );
    
    /**
     * 
     *
     * @since FS 1.0
     */
    void SetSenderL( TPtrC aSender, CFSMailMessage& aMsg );
    
    /**
     * 
     *
     * @since FS 1.0
     */
    void SetEnvelopeL( 
        const CMsvEntry* aEntry, 
        CMsvStore* aStore,
        CFSMailMessage& aMsg );
    
    /**
     * 
     *
     * @since FS 1.0
     */
    void SetStructureL( const CMsvEntry* aEntry, CFSMailMessage& aMsg );
    
    /*
     * Sets the flags of the message object based on the contents of
     * TMsvEmailEntry instance
     */
    void SetFlags( const TMsvEmailEntry& aEntry, CFSMailMessage& aMsg );
    
    /**
     * Sets the fetch state of the FS message objects based on the state
     * of TMsvEmailEntry instance
     */
    void SetFetchStateL( 
        const TMsvEmailEntry& aEntry, 
        TMsvId aMsgMainId,
        TBool aIsAtta, 
        CFSMailMessagePart& aMessage );
    
    /**
     * Sets the fetch state of imap message entry
     */
    void DoSetFetchStateL( 
            const TMsvEmailEntry& aEntry, 
            TMsvId aMsgMainId,
            TBool aIsAtta, 
            CFSMailMessagePart& aMessage );
    
    /*
     * Converts Symbian's enumerated folder type as a MIME type
     */
    TPtrC ConvertMultipartMimeType( TImEmailFolderType aFolderType ) const;
   
    // <cmail>
    /*
     * Converts entry type field UID as a MIME type
     */
    TInt ConvertBodyPartMimeType( 
        const TMsvEmailEntry& aEntry, 
        TDes& aMimeType );

    /**
     * Digs out charset of content, and constructs a Content-Type field parameter appending it
     * to given content-type
     * (e.g. "text/html" -> "text/html; charset=windows-1252")
     * @param attachment entry
     * @param descriptor where charset will be appended. 
     *        No validation is done for the given content.
     */
    void GetCharsetParameterL(
        const TMsvEmailEntry& aEntry,
        TDes& aContentType );
    // </cmail>

    /**
     * Separates the actual email address and the alias (display name)
     * and stores them to FS address object
     */
    void ConvertAddressL( 
        TPtrC aSourceAddress, 
        CFSMailAddress& aTargetAddress );
    
    /**
     * Simple utility function that digs the MIME type out from the
     * MIME header object and copies it to FS message part object
     */
    void SetContentTypeL(
        CImMimeHeader& aMimeHeader,
        CFSMailMessagePart& aMessagePart );

    /**
     * XOR function that regards all non-zero values as 'true' and
     * zero as 'false'.
     */
    inline TBool LogicalXor( TInt aLeft, TInt aRight );

	// <cmail> implemented to get rid of cs warning
    /**
     * Gets attachment count of entry
     */
    TInt GetAttachmentCountL( const TMsvEmailEntry& aEntry );
    
    /**
     * Checks email if it has attachments or not.
     * 
     */
    void AttaCheckForIncompleteMsgL( 
        const TMsvEmailEntry& aEntry, 
        CFSMailMessage& aMsg );
    
    // </cmail>

private:

    /**
     * Constructs the FS message part objects for the direct descendants of
     * the message. In the case of a multipart message, these are the top
     * level parts. In the case of simple (one-part) message, one part is
     * constructed.
     */
    void GetChildPartsOfMessageEntryL( 
        const TFSMailMsgId& aMailBoxId, 
        const TFSMailMsgId& aMessageId, 
        RPointerArray<CFSMailMessagePart>& aParts );

    /**
     * Constructs the FS message part objects for the children of
     * the folder entry that represents a MIME multipart structure.
     */
    void GetChildPartsOfFolderEntryL( 
        const TFSMailMsgId& aMailBoxId, 
        const TFSMailMsgId& aMessageId, 
        TMsvId aParentId, 
        RPointerArray<CFSMailMessagePart>& aParts );

    /**
     * Converts an entry representing a message body part as a 
     * FS message part object. Ownership of the returned object
     * is moved.
     */
    CFSMailMessagePart* ConvertBodyEntry2MessagePartL(
        const TMsvEmailEntry& aEntry,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId );

    /**
     * Converts an entry representing an attachment as a
     * FS message part object. Ownership of the returned object
     * is moved.
     */
    CFSMailMessagePart* ConvertAttachmentEntry2MessagePartL(
        const TMsvEmailEntry& aEntry,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId );

    /**
     * Converts a folder entry representing a multipart structure as
     * a FS message part object. Ownership of the returned object
     * is moved.
     */
    CFSMailMessagePart* ConvertFolderEntry2MessagePartL(
        const TMsvEmailEntry& aEntry,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId );
    
    /**
     * Returns False if TMsvEntry is not changed
     */
    TBool ChangeTEntryFlagsL( 
        TMsvEmailEntry& aEmlEntry,
        const CFSMailMessage& aMessage );
    
    /**
     * Changes CMsvEntry Attachment flag to correspond real situation
     */
    void SetAttachmentFlagL( const TMsvEmailEntry& aEntry, 
						     TBool aHasAttachment );
    
private:

    CMsvSession& iSession;
    
    CIpsPlgSosBasePlugin& iPlugin;
    
    };
    
#include "ipsplgmsgmapper.inl"    

#endif /* IPSPLGMSGMAPPER_H */

// End of File
