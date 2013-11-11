/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email object
*
*/

#ifndef __CFSMAILMESSAGE_H
#define __CFSMAILMESSAGE_H

//<cmail>
#include "CFSMailMessagePart.h"
//</cmail>


//<qmail>
class NmMessage;
//</qmail>

/**
 *  email data handling
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailMessage ) : public CFSMailMessagePart
{
 public:

    /**
     * Two-phased constructor.
     *
     * @param aMessageId message id in plugin containing email
     */
     IMPORT_C static CFSMailMessage* NewL( TFSMailMsgId aMessageId );
    
    /**
     * Two-phased constructor.
     *
     * @param aMessageId message id in plugin containing email
     */
     IMPORT_C static CFSMailMessage* NewLC( TFSMailMsgId aMessageId );

// <qmail>
     /**
      * Two-phased constructor.
      *
      * @param aMessageId message id in plugin containing email
      */
      IMPORT_C static CFSMailMessage* NewL( const NmMessage &aMessage );
// </qmail>

    /**
     * saves this message
     */
     IMPORT_C void SaveMessageL();

// <qmail>
    /**
     * saves this message asynchronously
     */
    IMPORT_C TInt SaveMessageL(MFSMailRequestObserver& aOperationObserver);

    /**
     * saves message parts asynchronously
     */
    IMPORT_C TInt SaveMessagePartsL(
            MFSMailRequestObserver& aOperationObserver);
// </qmail>

    /**
     * destructor
     */  
     IMPORT_C ~CFSMailMessage();

    /**
     * Adds new attachment to this email.
     * 
     * @param aFilePath contains full path to new attachment file.
     * @param aInsertBefore specifies position where new attachment is created.
     *        If NullId then new attachment is created as last.
     *
     * @return new email part object, ownership is transferred to user
     */ 
     IMPORT_C CFSMailMessagePart* AddNewAttachmentL( const TDesC& aFilePath,
                                                     const TFSMailMsgId aInsertBefore);

// <qmail>
     /**
      * Adds new attachment to this email asynchronously
      */
     IMPORT_C TInt AddNewAttachmentL( const TDesC& aFilePath,
                                      MFSMailRequestObserver& aOperationObserver);
// </qmail>
     
     /**
      * Adds new attachment to this email.
      * 
      * @param aFile contains access to new attachment file.
      * @param aMimeType user can give content type of the new part,
      *        otherwise content type is tried to recognized by method
      *
      * @return new email part object, ownership is transferred to user
      */ 
     IMPORT_C CFSMailMessagePart* AddNewAttachmentL( RFile& aFile, 
                                                     const TDesC8& aMimeType );

     /**
     * Returns a flat list of message parts that can be handled as attachments.
     * Excludes message parts that are multipart and parts that are considered
     * plain text or html body.
     * 
     * @param aParts contains flat list of attachments
     */        
     IMPORT_C void AttachmentListL(RPointerArray<CFSMailMessagePart>& aParts);

// <qmail>
    /**
     * Returns plain text body part for this email or NULL if not found.
     * 
     * @return plain text email body part, ownership is transferred to user
     */        
    IMPORT_C CFSMailMessagePart* PlainTextBodyPartL(
            TFSMailMessagePartDataSource aDataSource =
                    EDataSourceMessageStore);

    /**
     * Returns html body part for this message or NULL if not found.
     * 
     * @return html email body part, ownership is transferred to user
     */        
    IMPORT_C CFSMailMessagePart* HtmlBodyPartL(
            TFSMailMessagePartDataSource aDataSource =
                    EDataSourceMessageStore);
// </qmail>

     /**
     * Returns a flat list of message parts that can be handled as attachments.
     * Excludes message parts that are multipart and parts that are considered
     * plain text or html body.
     * 
     * @param aParts contains flat list of attachments
     */        
     void DoAttachmentListL(RPointerArray<CFSMailMessagePart>& aParts);

// <qmail>
    /**
     * gets the new NmMessage object
     *
     * @return NmMessage object based on iNmPrivateMessagePart and iNmPrivateMessageEnvelope
     */
    IMPORT_C NmMessage* GetNmMessage();
// </qmail>

 protected:

 private:

    /**
     * C++ default constructor.
     */
    CFSMailMessage();

    /**
     * Two-phased constructor
     */
     void ConstructL( TFSMailMsgId aMessageId );

// <qmail>
     /**
      * Two-phased constructor
      */
      void ConstructL( const NmMessage &aMessage );
// </qmail>

    /**
     * Returns content-type parameters and their associated values.
     * This function is made private to prevent usage from CFSMailMessage
     * 
     * @return content type parameters
     */
    CDesCArray& ContentTypeParameters();

 private: // data
    //<qmail>
    // Array is used to pass self pointer as RPointerArray reference to plugin funstion
    // Do not destroy its content (just Reset)!
    RPointerArray<CFSMailMessage> iMessageBufferForAsyncOp;
    //</qmail>
};


#endif // __CFSMAILMESSAGE_H
