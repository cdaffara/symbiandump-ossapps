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
* Description: This file defines class CIpsPlgSmtpService.
 *
*/


#ifndef IPSPLGSMTPSERVICE_H
#define IPSPLGSMTPSERVICE_H

class CIpsPlgSosBasePlugin;
class CIpsPlgMsgMapper;

/**
 *  Freestyle message send and creation implementation
 *  
 *  Encapsulates message creation and send operations using
 *  Symbian os smtp services.
 *
 *
 *  @lib ipssosplugin.lib
 *  @since FS 1.0
 */
NONSHARABLE_CLASS( CIpsPlgSmtpService ) : 
    public CBase
    {

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpService* NewL(
        CMsvSession& aSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpService* NewLC( 
        CMsvSession& aSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Class destructor
     *
     * @since FS 1.0
     * @return None
     */
    virtual ~CIpsPlgSmtpService();

    /**
     * Stores message to symbian message store
     *
     * @param aMailboxId id for corresponding mailbox
     * @param aMessage FS Email message object
     * @param aSession MsvSession
     * @since FS 1.0
     */
    void StoreMessageL(
        TFSMailMsgId aMailBoxId,
        CFSMailMessage& aMessage );

    // <qmail> removing unused functions
    // CreateNewSmtpMessageL
    // CreateForwardSmtpMessageL
    // CreateReplySmtpMessageL
    // </qmail>
            
// <qmail> moved for public access
    /**
     * Creates proper fs message object and set flags
     * to correspond orginal message flags
     *
     * @since FS 1.0
     * @param aMessageId id of created message
     * @param aOrginalMsgId id of orginal message
     * @param aMailboxId mailbox id
     * @param aCopyOriginalMsgProperties Copy properties from original message
     * @return TMscId
     */
    CFSMailMessage* CreateFSMessageAndSetFlagsL(
       TMsvId aMessageId, 
       TMsvId aOriginalMsgId, 
       TMsvId aMailboxId,
       TBool aCopyOriginalMsgProperties = EFalse );

    /**
     * Changes messages service id
     *
     * @since FS 1.0
     * @param aEntry Original message entry 
     * @return None
     */
    void ChangeServiceIdL( TMsvEntry& aEntry );
	
	 /**
     * Changes messages service id to the given id
     *
     * @param aEntry message entry
     * @param aServiceId new service id
     * @return None
     */
    void ChangeServiceIdL( TMsvEntry& aEntry, TMsvId aServiceId );    
// </qmail>

protected:

    /**
     * Class constructor
     *
     * @since FS 1.0
     * @return None
     */
    CIpsPlgSmtpService( CMsvSession& aSession,
                    CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Constructor for leaving methods
     *
     * @since FS 1.0
     * @return None
     */
    void ConstructL();

private:

    /**
     * Combines alias and email address to one string
     * format : Alias<email_address>
     *
     * @since FS 1.0
     * @param TDesC& aAliasName alias
     * @param TDesC& aEmailName email address
     * @return result of combination
     */
    HBufC* CreateDisplayStringLC( const TDesC& aDisplayName, 
                              const TDesC& aEmailName );

    /**
     * Gets MsvId from msv operations final progress 
     * descriptor, leaves if msvId is null entry
     *
     * @since FS 1.0
     * @param aProg progress descriptor
     * @return TMscId
     */
    inline static TMsvId GetIdFromProgressL( const TDesC8& aProg );

    /**
     * Gets character set from original message
     * @param aOriginalMsgId id of original message
     * */
    TUid GetOriginalCharsetL( TMsvId aOriginalMsgId );
    
    /**
     * Sets the charset for outgoing messages 
     * @param aEntry message where the charset is to be set
     * @param aCharset the charset to be set
     */
    void SetCharactersetL( CMsvEntry& aEntry, TUid aCharset );
private: // data

    CIpsPlgSosBasePlugin& iPlugin;

    CMsvSession& iSession;
    
    CIpsPlgMsgMapper* iMsgMapper;
    
    /**
     * Own: Feature manager initialization flag
     */
    TBool iFeatureManagerInitialized;

    };

#endif /* IPSPLGSMTPSERVICE_H*/

// End of File
