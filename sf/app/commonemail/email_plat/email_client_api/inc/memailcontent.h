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
* Description: ECom interface for Email Client API
*
*/

#ifndef MEMAILCONTENT_H
#define MEMAILCONTENT_H

#include <emailapidefs.h>

namespace EmailInterface {

_LIT( KContentTypeTextPlain, "text/plain" );
_LIT( KContentTypeTextHtml, "text/html" );

class MEmailOperationObserver;
class MEmailAttachment;
class MEmailMultipart;
class MEmailTextContent;
class MEmailMessageContent;

/**
* Callback interface used for content fetching asynchronously.
* Object deriving this interface is passed to MEmailMessageContent::FetchL
* @since S60 v5.0
*/
class MEmailFetchObserver
{
public:
    /** Content fetched
    * @param operation result 
    */
    virtual void DataFetchedL( const TInt aResult ) = 0;
};

typedef RPointerArray<MEmailMessageContent> REmailContentArray;

/**
* Base interface for mime message parts.
* @since S60 v5.0
*/
class MEmailMessageContent : public MEmailInterface
{
public:
    /** numeric identifier of message content */
    virtual TMessageContentId Id() const = 0;

// fields as per RFC 2045
    /**
     * return sContent-Type field value
     */
    virtual TPtrC ContentType() const = 0;

    /**
     * sets value of Content-Type field
     * @param content type
     */
    virtual void SetContentType( const TDesC& aContentType ) = 0;

    /**
    /* Returns Content-ID field value.
     * @return content id
     */
    virtual TPtrC ContentId() const = 0;

    /**
     * Sets value of Content-ID field
     * @param content id
     */
    virtual void SetContentId( const TDesC& aContentId ) = 0;

    /**
     * Returns value of content-description field
     * @return content description 
     */
    virtual TPtrC ContentDescription() const = 0;

    /**
     * Sets value of content description field
     * @param content description 
     */
    virtual void SetContentDescription( const TDesC& aContentDescription ) = 0;

    /**
     * Returns value of content disposition field
     * @return content disposition
     */
    virtual TPtrC ContentDisposition() const = 0;

    /**
     * Sets value of content-disposition field
     * @param content disposition
     */
    virtual void SetContentDisposition( const TDesC& aContentDisposition ) = 0;

// end of standard  RFC 2045 fields

    /**
     * returns Content-class field value (non-standard)
     */
    virtual TPtrC ContentClass() const = 0;

    /**
     * Sets value of Content-class field (non-standard)
     * @param content class
     */
    virtual void SetContentClass( const TDesC& aContentClass ) = 0;

    /**
     * Available (=fetched) size accessor. If this is less than value from
     * TotalSize(), Fetch() should be used to retrieve more data from
     * remote mail server.
     * @return fetched size of the data.
     */
    virtual TInt AvailableSize() const = 0;

    /**
     * Total size accessor
     * @return total size of message text.
     */
    virtual TInt TotalSize( ) const = 0;

    /**
     * Returns pointer descriptor to content data
     */
    virtual TPtrC ContentL() const = 0;

    /**
     * Sets content data.
     */
    virtual void SetContentL( const TDesC& aContent ) = 0;

    /**
     * Starts fetching rest of content asynchronously.
     *
     * If available size after fetch is smaller than total size, next chunk can
     * be fetched with new invocatin of Fetch method.
     *
     * Calling Release() implicitly cancels fetching.
     * @param aObserver called when when fetch completes.
     * @exception KErrInUse if fetch is ongoing
     */
    virtual void FetchL( MEmailFetchObserver& aObserver ) = 0;

    /**
     * Cancels fetch operation, observer is not called
     */
    virtual void CancelFetch() = 0;

    /**
     * Stores message content to a file
     */
    virtual void SaveToFileL( const TDesC& aPath ) = 0;

    /**
     * Typesafe multipart accessor for obtaining MEmailMultipart pointer
     * to this object.
     * @param content as multipart or NULL if content is not multipart
     */
    virtual MEmailMultipart* AsMultipartOrNull() const = 0;

    /**
     * Typesafe text content accessor for obtaining MEmailTextContent pointer
     * to this object.
     * @param content as text content or NULL if content is not text
     */
    virtual MEmailTextContent* AsTextContentOrNull() const = 0;

    /**
     * Typesafe attachment content accessor for obtaining MEmailAttachment pointer
     * to this object.
     * @param content as attachment content or NULL if content is not an attachment
     */
    virtual MEmailAttachment* AsAttachmentOrNull() const = 0;
};

/**
* Convenience abstraction for manipulating plain or html text content. Use
* CEmailInterfaceFactory::InterfaceL( KEmailIFUidTextContent ) to make new
* instance of this interface.
* @since S60 v5.0
*/
class MEmailTextContent : public MEmailMessageContent
{
public:
    /**
     * Text (content) type 
     */
    enum TTextType {
        EPlainText, // text/plain
        EHtmlText   // text/html
    };

    /**
    * Tells if content has specified type of text.
    */
    virtual TTextType TextType() const = 0;

    /**
    * Sets (replaces) text to message content. Possible old content is
    * deleted. Content type is set to "text/plain" or "text/html" based on
    * specified text type.
    * @param aPlainOrHtml sub-type of the text
    * @param aText content of the message part
    */
    virtual void SetTextL(
        const TTextType aPlainOrHtml,
        const TDesC& aText ) = 0;
};

/**
* Message part with multiple sub parts. If MEmailMessage::ContentL returns
* content with type "multipart/*" (where '*' is a character sequence) it can
* safely be casted to MEmailMultipart
* @since S60 v5.0
*/
class MEmailMultipart : public MEmailMessageContent
{
public:
    /**
    * Returns number of child parts
    */
    virtual TInt PartCountL() = 0;

    /**
    * Returns a child part, ownership is transferred.
    * @param aIndex part to return, leaves KErrArgument if out of bounds
    */
    virtual MEmailMessageContent* PartByIndexL( const TUint aIndex ) const = 0;

    /**
    * Deletes part by index.
    * @param aIndex part to delete, leaves KErrArgument if out of bounds
    */
    virtual void DeletePartL( const TUint aIndex ) = 0;

    /**
    * Adds message part, ownership is transferred.
    * @param aPart any object derived from MEmailMessageContent
    * @param aPos position among child parts
    */
    virtual void AddPartL(
        const MEmailMessageContent& aPart,
        const TUint aPos ) = 0;
};

typedef RPointerArray<MEmailAttachment> REmailAttachmentArray;

/**
* Email attachment interface
* Attachment is created with MEmailMessage::AddAttachmentL()
* @since S60 v5.0
*/
class MEmailAttachment : public MEmailMessageContent
{
public:
      /**
      * Returns file handle of this attachment. If the attachment is
      * not associated with a file, null file handle is returned.
      */
      virtual RFile FileL() const = 0;

      /**
      * Sets file name field
      */
      virtual void SetFileNameL( const TDesC& aFileName ) = 0;

      /**
      * Returns file name or null pointer descriptor if attachment
      * is not associated with any file
      */
      virtual TPtrC FileNameL() const = 0;
};

} // namespace EmailInterface

#endif // MEMAILCONTENT_H
