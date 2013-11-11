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
* Description:  common email base object
*
*/


#ifndef __CFSMAILMESSAGEBASE_H
#define __CFSMAILMESSAGEBASE_H

#include <e32std.h>
#include <e32base.h>
#include "CFSMailAddress.h"
#include "cemailextensionbase.h"

//<qmail>
#include <QExplicitlySharedDataPointer>
class NmMessageEnvelopePrivate;
class NmMessageEnvelope;
//</qmail>

class CFSMailRequestHandler;

/**
 *  email handling base class
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailMessageBase ) : public CExtendableEmail
{
 public:

    /**
     * Two-phased constructor.
     *
     * @param aMessageId id of the email to be created
     */
     IMPORT_C static CFSMailMessageBase* NewL(const TFSMailMsgId aMessageId);
    
    /**
     * Two-phased constructor.
     *
     * @param aMessageId id of the email to be created
     */
     IMPORT_C static CFSMailMessageBase* NewLC(const TFSMailMsgId aMessageId);
 
    /**
     * Destructor.
     */  
     IMPORT_C virtual ~CFSMailMessageBase();
 
    /**
     * message id accessor
     *
     * @return message id
     */
     IMPORT_C TFSMailMsgId GetMessageId() const;
     

    /**
     * email parent folder id accessor
     *
     * @returns parent folder id
     */
     IMPORT_C TFSMailMsgId GetFolderId() const ;

    /**
     * email parent folder id mutator
     *
     * @param aFolderId parent folder id
     */
     IMPORT_C void SetFolderId( const TFSMailMsgId aFolderId );

    /**
     * email mailbox id accessor
     *
     * @returns mailbox id
     */
     IMPORT_C TFSMailMsgId GetMailBoxId() const ;

    /**
     * email mailbox id mutator
     *
     * @param aMailBoxId mailbox id
     */
     IMPORT_C void SetMailBoxId( const TFSMailMsgId aMailBoxId );

    /**
     * email sender address accessor
     *
     * @return email sender address, ownership not transferred to user
     */
     IMPORT_C CFSMailAddress* GetSender() const;

    /**
     * email sender address mutator
     *
     * @param aSender email sender address, ownership transferred from user
     */
     IMPORT_C void SetSender(CFSMailAddress* aSender);

// <qmail>
    /**
     * returns TO-recipients list of email
     *
     * @return to-recipients list, ownership not transferred user
     */
     IMPORT_C RPointerArray<CFSMailAddress> GetToRecipients();

    /**
     * returns CC-recipients list of email
     *
     * @return cc-recipients list, ownership not transferred user
     */
     IMPORT_C RPointerArray<CFSMailAddress> GetCCRecipients(  );

    /**
     * returns BCC-recipients list of email
     *
     * @return bcc-recipients list, ownership not transferred user
     */
     IMPORT_C RPointerArray<CFSMailAddress> GetBCCRecipients( );
// </qmail>

    /**
     * appends one recipient to email TO-field, ownership transferred from user
     *
     * @param aRecipient new recipient email address
     */
     IMPORT_C void AppendToRecipient( CFSMailAddress* aRecipient);

    /**
     * appends one recipient to email CC-field, ownership transferred from user
     *
     * @param aRecipient new recipient email address 
     */
     IMPORT_C void AppendCCRecipient( CFSMailAddress* aRecipient );

    /**
     * appends one recipient to email BCC field, ownership transferred from user
     *
     * @param aRecipient new recipient email address
     */
     IMPORT_C void AppendBCCRecipient( CFSMailAddress* aRecipient );

    /**
     * clears email TO-recipients list
     *
     */
     IMPORT_C void ClearToRecipients( );

    /**
     * clears email CC-recipients list
     *
     */
     IMPORT_C void ClearCcRecipients( );

    /**
     * clears email BCC-recipients list
     *
     */
     IMPORT_C void ClearBccRecipients( );

    /**
     * email subject accessor
     *
     * @return email subject, ownership not transferred user
     */
     IMPORT_C TDesC& GetSubject() const;

    /**
     * email subject mutator
     *
     * @param aSubject new email subject
     */
     IMPORT_C void SetSubject(const TDesC& aSubject);

    /**
     * email date accessor
     *
     * @return email date
     */
     IMPORT_C TTime GetDate() const;

    /**
     * email date mutator
     *
     * @param aDate new email date
     */
     IMPORT_C void SetDate(const TTime aDate);

    /**
     * email flags accessor
     *
     * @return email flags
     */
     IMPORT_C TInt GetFlags( ) const;

    /**
     * email flag set
     *
     * @aFlag email flag to be set
     */
     IMPORT_C void SetFlag(const TInt aFlag);

    /**
     * email flag reset
     *
     * @aFlag email flag to be reset
     */
     IMPORT_C void ResetFlag(const TInt aFlag);

    /**
     * test email flag is set
     *
     * @aFlag email flag to be tested
     * @return email flag status (set/reset)
     */
     IMPORT_C TBool IsFlagSet(const TInt aFlag) const;

    /**
     * Email reply-to address accessor ; defines optional email address
     * that clients should use to send replies. Returns NULL pointer
     * if address is not set, ownership not transferred to user.
     *
     * @return email reply-to address
     */
     IMPORT_C const CFSMailAddress& GetReplyToAddress();

    /**
     * email reply-to address mutator, ownership transferred from user
     *
     * @param aReplyToAddress email reply-to address
     */
     IMPORT_C void SetReplyToAddress(CFSMailAddress* aReplyToAddress);

     /**
      * plugin request handler accessor
      * 
      * @return request handler
      */
     IMPORT_C CFSMailRequestHandler& RequestHandler( );
	 
// <qmail>
    /**
     * gets the new NmMessageEnvelope object
     *
     * @return NmMessageEnvelope object
     */
     IMPORT_C NmMessageEnvelope* GetNmMessageEnvelope();
// </qmail>
          
public: // from  CExtendableEmail

     /**
     * @see CExtendableEmail::ReleaseExtension
     */                                        
     IMPORT_C void ReleaseExtension( CEmailExtension* aExtension );
     
     /**
      * Returns extension by uid, leaves KErrNotSupported if extension is
      * not available.
      * @param aInterfaceUid extension interface uid
      * @return extension pointer. Ownership depends on extension.
      */
     IMPORT_C CEmailExtension* ExtensionL( const TUid& aInterfaceUid );

 protected:

    /**
     * C++ default constructor.
     */
     CFSMailMessageBase();

// <qmail>
     // iMessageId removed
    /**
     * ConstructL
     */
    IMPORT_C void ConstructL( const TFSMailMsgId aMessageId );      

    IMPORT_C void ConstructL( const NmMessageEnvelope &aMessageEnvelope );
    
    /**
     * email message id mutator
     *
     * @param aMessageId message id to be set
     */
    IMPORT_C void SetMessageId( const TFSMailMsgId aMessageId );
// </qmail>
 
protected:	// data
    // Request handler from tls 
    CFSMailRequestHandler*          iRequestHandler;
	
//<qmail>
    /**
     * Reference to QT side of the message meta data object.
     */
     QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> iNmPrivateMessageEnvelope;
//</qmail>

 private: // data

    /**
     * email mailbox id
     */
     TFSMailMsgId    iMailBoxId;

     /**
     * email parent folder id
     */
     TFSMailMsgId    iFolderId;

// <qmail>
     /**
     * email sender address
     * do not use it directly!
     */
     CFSMailAddress*    iSender;
// </qmail>

     /**
     * email reply-to address
     */
     CFSMailAddress*    iReplyTo;

// <qmail>
// Unnecessary members removed: iToRecipients, iCcRecipients, iBccRecipients, iFlags, iDate
// </qmail>

// <qmail>
    /**
     * email subject
     */
     mutable TPtrC    iSubjectPtr;
// </qmail>
     
};


#endif // __CFSMAILMESSAGEBASE_H


