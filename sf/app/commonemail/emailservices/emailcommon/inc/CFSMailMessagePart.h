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
* Description:  common email part object
*
*/

#ifndef __CFSMAILMESSAGEPART_H
#define __CFSMAILMESSAGEPART_H

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <badesca.h>

//<cmail>
#include "CFSMailMessageBase.h"
#include "mmrinfoobject.h"
//</cmail>

//<qmail>
#include <QExplicitlySharedDataPointer>

class NmMessagePartPrivate;
class NmMessagePart;
//</qmail>

class CFSMailPlugin;
class CFSMailMessage;
class MFSMailRequestObserver;

const TInt KArrayGranularity = 16;

/**
 *  email parts handling class
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailMessagePart ) : public CFSMailMessageBase
{
 public:
// <qmail>
    /**
     * Enumeration to indicate data source
     */
    enum TFSMailMessagePartDataSource
        {
        EDataSourceMessageStore = 0,    // plugin (data from message store)
        EDataSourceLocal                // internal private shared object
        };
// </qmail>
   
 public:

    /**
     * Two-phased constructor.
     *
     * @param aMessageId message id in plugin containing email
     * @param aMessagePartId message part id in plugin containing email
     */
     IMPORT_C static CFSMailMessagePart* NewL( TFSMailMsgId aMessageId, TFSMailMsgId aMessagePartId );

    /**
     * Two-phased constructor.
     *
     * @param aMessageId message id in plugin containing email
     * @param aMessagePartId message part id in plugin containing email
     */
     IMPORT_C static CFSMailMessagePart* NewLC( TFSMailMsgId aMessageId, TFSMailMsgId aMessagePartId );

// <qmail>
     /**
      * Two-phased constructor.
      *
      * @param aMessageId message id in plugin containing email
      */
      IMPORT_C static CFSMailMessagePart* NewLC( const NmId &aNmMessageId, const NmMessagePart& aNmMessagePart );     
// </qmail>

    /**
     * destructor
     */  
     IMPORT_C virtual ~CFSMailMessagePart();
     
    /**
     * email part id accessor
     *
     * @return email part id
     */
     IMPORT_C TFSMailMsgId GetPartId() const;

// <qmail>
    /**
     * email part content-type accessor (e.g. "text/plain")
     *
     * @return email part content type
     */
     IMPORT_C const TDesC& GetContentType() const;
// </qmail>
    
    /**
     * email part content-type mutator 
     *
     * @param aContentType new content type value
     */
     IMPORT_C void SetContentType(const TDesC& aContentType);

    /**
     * Returns content-type parameters and their associated values.
     * Updating/setting values should be done by first getting the array and
     * then modifying it's contents.
     * 
     * @return content type parameters
     */
     IMPORT_C CDesCArray& ContentTypeParameters();
     
    /**
     * Returns email Content-Description value, includes also parameters,
     * if any.
     *
     * @return email Content-Description
     */
     IMPORT_C const TDesC& ContentDescription();
     
    /**
     * Stores email Content-Description value, parameters can be included,
     * if any.
     *
     * @param aContentDescription new email content description value
     */
     IMPORT_C void SetContentDescription(const TDesC& aContentDesctiption);

    /**
     * Returns email Content-Disposition value, includes also parameters,
     * if any.
     *
     * @return email Content-Disposition
     */
     IMPORT_C const TDesC& ContentDisposition();
     
    /**
     * Stores email Content-Disposition value, parameters can be included,
     * if any.
     *
     * @param aContentDisposition new email content disposition value
     */    
     IMPORT_C void SetContentDisposition(const TDesC& aContentDesctiption);

    /**
     * Returns email Content-Disposition Parameters and their associated values.
     * Updating/setting values should be done by first getting the array and
     * then modifying it's contents.
     *
     * @return content disposition parameters
     */
     IMPORT_C CDesCArray& ContentDispositionParameters();

    /**
     * email Content-Id accessor.
     *
     * @return email Content-Id
     */
     IMPORT_C const TDesC& ContentID();
     
    /**
     * email Content-Id mutator
     *
     * @param aContentID new email Content-Id value
     */
     IMPORT_C void SetContentIDL(const TDesC& aContentID);

    /**
     * email Content-Class accessor
     *
     * @return email Content-Class
     */
     IMPORT_C const TDesC& GetContentClass();

    /**
     * email Content-Class mutator
     *
     * @param aContentClass new email Content-Class value
     */
     IMPORT_C void SetContentClass( const TDesC& aContentClass );

    /**
     * saves this message part 
     */
     IMPORT_C void SaveL();

// <qmail>
    /**
     * lists all direct child parts of this email part
     *
     * @param aParts table containing list of child parts owned by user
     * @param aDataSource data source: email store/local object
     */
     IMPORT_C void ChildPartsL(RPointerArray<CFSMailMessagePart>& aParts, 
         TFSMailMessagePartDataSource aDataSource = EDataSourceMessageStore);
// </qmail>

    /**
     * Returns child part of this part identified by part id.
     * 
     * @param aPartId defines parent part id
     * @return child part object, ownership is transferred to user
     */
     IMPORT_C CFSMailMessagePart* ChildPartL( const TFSMailMsgId aPartId );

    /**
     * Returns ETrue if this email part is actually an email,
     * so it's safe to cast this part to CFSMailMessage
     * 
     * @return ETrue if this email part is email
     */        
     IMPORT_C virtual TBool IsMessageL() const;

    /**
     * Creates and adds a new child part to this email part
     * 
     * @param aInsertBefore defines place where new child part is created,
     *        if aInsertBefore is NULL id then new part is added as last.
     * @param aContentType content type of the new child part
     *
     * @return created child part object, ownership is transferred to user
     */
     IMPORT_C CFSMailMessagePart* NewChildPartL( const TFSMailMsgId aInsertBefore,
                                                 const TDesC& aContentType );

    /**
     * Removes child part (and it's children, if any) from this email part
     * 
     * @param aPartId part id of the email part to be removed
     */
     IMPORT_C void RemoveChildPartL(TFSMailMsgId aPartId);

// <qmail>
     /**
      * Removes child part (and it's children, if any) from this email part
      * 
      * @param aPartId part id of the email part to be removed
      * @param aOperationObserver Observer for the operation 
      * 
      * @return id of the request, KErrNotSupported if the protocol plugin this
      * instance is attached to does not support the async method.
      */
     IMPORT_C TInt RemoveChildPartL( TFSMailMsgId aPartId,
                                     MFSMailRequestObserver& aOperationObserver);
// </qmail>
     
    /**
     * Retrieves a read-only file handle for the content file of this message part.
     * The caller must close the file handle.
     * 
     * @param aForWriting Open the file for writing
     *
     * @return content file handle 
     */
     IMPORT_C RFile GetContentFileL( TBool aForWriting = EFalse );
        
    /**
     * copies contents of this message part to given file
     *
     * @param aContentLocation defines file location as 
     * a) directory, when plugin finds out corresponding file name
     *    related to this part id, in this case last character should
     *    be '/'
     * b) directory & filename, when both directory and file name
     *    are given by user
     * 
     */        
     IMPORT_C void CopyContentFileL(const TDesC& aContentLocation);
    
    /**
     * returns email part full content size (in bytes)
     *
     */
     IMPORT_C TUint ContentSize() const;

    /**
     * Sets email part full content size (in bytes)
     *
     * @param aContentSize content size to be set
     */
     IMPORT_C void SetContentSize(const TUint aContentSize);
    
    /**
     * returns email part currently fetched (local) content size (in bytes).
     * 
     */
     IMPORT_C TUint FetchedContentSize() const;

    /**
     * Sets email part currently fetched (local) content size (in bytes).
     * 
     * @param aContentSize content size to be set
     */
     IMPORT_C void SetFetchedContentSize(TUint aContentSize);

    /**
     * returns email part content current fetch state
     * Intended as utility function for UI.
     * If (0 == FetchedContentSize())
     *    return EFSNone;
     * else if (FetchedContentSize() < ContentSize()) then
     *    return EFSPartial
     * else
     *    return EFSFull
     *
     * @return email fetch state (EFSNone / EFSPartial / EFSFull)
     */
     IMPORT_C TFSPartFetchState FetchLoadState() const;

    /**
     * copies email part contents to buffer given by user
     *
     * @param aBuffer buffer owned by user
     * @param aStartOffset offset from content begin
     */
     IMPORT_C void GetContentToBufferL(TDes16& aBuffer, TUint aStartOffset);
    
    /**
     * copies email part contents from buffer given by user
     *
     * @param aBuffer buffer given by user
     */
     IMPORT_C void SetContent(TDes16& aBuffer); 

    /**
     * meeting request accessor if email part is a meeting request
     * 
     * @return meeting request object
     */
     IMPORT_C MMRInfoObject& GetMRInfo();
    
    /**
     * stores given meeting request object as email part
     * 
     * @param aMeetingRequest meeting request object
     */
     IMPORT_C void SetMRInfo(MMRInfoObject* aMeetingRequest);

    /**
     * method checks if MRInfo exists
     * 
     * @return true / false
     */
     IMPORT_C TBool IsMRInfoSet();

    /**
     * Sets attachment name for email part. Full path can be
     * given as input even though only filename is saved. 
     * 
     * @param aFilePath attachment name
     */        
     IMPORT_C void SetAttachmentNameL(const TDesC& aFilePath);

    /**
     * returns email part attachment name
     * 
     * @return attachment name
     */        
     IMPORT_C TDesC& AttachmentNameL();
    
// <qmail>
    /**
     * finds email body part based on given content type 
     * (plain text or html body part of email message)
     * 
     * @param aContentType body part content type to be searched
     * @param aDataSource data source: email store/local object
     *
     * @return email body part or NULL if not found, ownership is transferred to user
     */
     IMPORT_C CFSMailMessagePart* FindBodyPartL(const TDesC& aContentType,
         TFSMailMessagePartDataSource aDataSource = EDataSourceMessageStore);
// </qmail>

    /**
     * starts email part fetching from email server
     * 
     * @param aMessagePartId message part id of email part to be fetched
     * @param aOperationObserver observer to forward fetching progress events
     * to user
     * @param aPreferredByteCount indicates how much more content for part(s)
     *        user wants to fetch. Default value zero indicates that all content
     *        should be fetched. Actual amount of data fetched may differ from
     *        requested (possibly all fetched in any case).
     *
     * @return err code
     */        
     IMPORT_C TInt FetchMessagePartL(   const TFSMailMsgId aMessagePartId,
                                        MFSMailRequestObserver& aOperationObserver,
                                        const TUint aPreferredByteCount);

    /**
     * starts email parts fetching from email server
     * 
     * @param aMessagePartIds message part ids of email parts to be fetched
     * @param aOperationObserver observer to forward fetching progress events
     * to user
     * @param aPreferredByteCount indicates how much more content for part(s)
     *        user wants to fetch. Default value zero indicates that all content
     *        should be fetched. Actual amount of data fetched may differ from
     *        requested (possibly all fetched in any case).
     *
     * @return err code
     */        
     IMPORT_C TInt FetchMessagesPartsL( const RArray<TFSMailMsgId>& aMessagePartIds,
                                        MFSMailRequestObserver& aOperationObserver,
                                        const TUint aPreferredByteCount);
    /**
     * creates a list containing all email / email part subparts
     * 
     * @param aParts email part list given/owned by user
     */        
     IMPORT_C void AppendAttachmentsL(RPointerArray<CFSMailMessagePart>& aParts);

    /**
     * sets email part fetch status
     * 
     * @param aMessagePartStatus (EFSMessagePartsKnown)
     */
     IMPORT_C void SetMessagePartsStatus(TFSPartFetchState aMessagePartStatus);

    /**
     * Adds new attachment to this email (part).
     * 
     * @param aFilePath full path of new attachment.
     * @param aInsertBefore defines position of new attachment,
     *        if NullId then new attachment is created as last part.
     * @param aContentType attachment content type

     * @return new email part, ownership is transferred to user
     */ 
     IMPORT_C CFSMailMessagePart* AddNewAttachmentL( const TDesC& aFilePath, 
                                                     const TFSMailMsgId aInsertBefore,
                                                     const TDesC& aContentType );
    /**
     * read only part size accessor (character or byte count)
     *
     */
     IMPORT_C TUint ReadOnlyPartSize() const;

    /**
     * read only part size mutator
     *
     * @param aReadOnlyPartSize read only size to be set
     * (character or byte count)
     */
     IMPORT_C void SetReadOnlyPartSize(const TUint aReadOnlyPartSize);
     
// <qmail>
     /**
 	 * gets the new NmMessagePart object
 	 *
 	 * @return NmMessagePart object
 	 */
 	 IMPORT_C NmMessagePart* GetNmMessagePart();
// </qmail>
 	 
// <qmail>
     /**
      * Gets the text content from private shared object.
      * Returns the ownership of text buffer to caller.
      * Returned object should be removed from CleanupStack after usage.
      * 
      * @return HBufC*
      */
     IMPORT_C HBufC* GetLocalTextContentLC();
// </qmail>
     
     
public: // data

protected:

    /**
     * constructor
     */
     CFSMailMessagePart();

    /**
     * lists subparts
     */
     void ListMessagePartsL( RPointerArray<CFSMailMessagePart>& aParts );
         
    /**
     * clears internal part array
     */
     void ClearPartsArray(TBool aClearAll);
     
    /**
     * finds id of body part
     */
     TFSMailMsgId FindBodyPartIdL(const TDesC& aContentType);

     /**
     * Returns a flat list of message parts that can be handled as attachments.
     * Excludes message parts that are multipart and parts that are considered
     * plain text or html body.
     * 
     * @param aParts contains flat list of attachments
     */        
     virtual void DoAttachmentListL(RPointerArray<CFSMailMessagePart>& aParts);

protected: // data

    /**
     * email fetch from email server status
     */
     TFSPartFetchState                  iMessagePartsStatus;

// <qmail>
 	/**
 	 * message part MIME type - pointer descriptor to access QString object
 	 */
 	 mutable TPtrC						iContentTypePtr;
// </qmail>
	 
  	/** message parts array */
	 RPointerArray<CFSMailMessagePart>	iMessageParts;
	 TBool                              iReadMessageParts;
	 
// <qmail>
	/**
	 * Reference to QT side of the message meta data object.
	 */
	 QExplicitlySharedDataPointer<NmMessagePartPrivate> iNmPrivateMessagePart;
// </qmail>

protected:

    /**
     * Two-phased constructor
     */
     void ConstructL( TFSMailMsgId aMessageId, TFSMailMsgId aMessagePartId );

// <qmail>
    /**
     * Two-phased constructor
     */
     void ConstructL(const NmId &aNmMessageId, const NmMessagePart& aNmMessagePart);

     void ConstructL(const NmMessagePart& aNmMessagePart,
                     const NmMessageEnvelope& aNmMessageEnvelope);
// </qmail>

private:    // data

// <qmail>
// Unnecessary members removed: iMessagePartId, iContentSize, iFetchedContentSize
// </qmail>
     /** read only part size */
     TUint           iReadOnlyPartSize;

     /** fetched from email server indication */
     TBool           iIsFetched;

// <qmail>
   	/**
   	 * email content decription  - pointer descriptor to access QString object
   	 */
   	 mutable TPtrC	iContentDescriptionPtr;      

   	/**
   	 * email content disposition  - pointer descriptor to access QString object
   	 */
   	 mutable TPtrC	iContentDispositionPtr;
// </qmail>
 	 
     /** attachment name */
     HBufC           *iAttachmentName;

     /** email content class */
     HBufC           *iContentClass;

// <qmail>
    /**
     * email content id  - pointer descriptor to access QString object
     */
     mutable TPtrC	iContentIDPtr;
// </qmail>

     /** content disposition parameters */
     CDesCArray      *iContentDispositionParams;

     /** content type parameters */
     CDesCArray      *iContentTypeParams;

     /** meeting request object */
     MMRInfoObject*  iMeetingRequest;                       

     /* temp file handle */
     RFile           iFile;
 };

#endif // __CFSMAILMESSAGEPART_H
