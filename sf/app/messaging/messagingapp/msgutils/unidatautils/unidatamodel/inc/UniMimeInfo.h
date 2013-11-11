/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       CUniMimeInfo, Storage for objects mime headers.
*
*
*/


#ifndef __UNIMIMEINFO_H
#define __UNIMIMEINFO_H

// INCLUDES

#include <cmsvattachment.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION
class MMsvAttachmentManager;
class CBaseMtm;
class CUniDataUtils;

/**
* CUniMimeInfo - Container for the MIME headers of an attachment.
* NOTE no support for X-Type-Params
*      only support for filename ContentDisposition parameter
*      only support for charset ContentTypeL parameter
*
* If some header is not set zero length descriptor is returned.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniMimeInfo : public CBase
    {
    public:

    /**
    * Constructor
    *
    * @since    3.1
    */
    CUniMimeInfo();

    /**
    * Destructor
    *
    * @since    3.1
    */
    ~CUniMimeInfo();

    //
    // Accessors/Mutators
    //

    /**
    * SetContentTypeL
    * NOTE: 8-bit
    *
    * @since    3.1
    * @param    aMimeType   IN content type
    */
    void SetContentTypeL( const TDesC8& aMimeType );

    /**
    * ContentType
    * NOTE: 8-bit
    *
    * @since    3.1
    * @return   pointer to content type buffer
    */
    inline const TPtrC8 ContentType() const;

    /**
    * SetCharset
    * As content type charset parameter
    * No other content type specific parameters supported
    * charset = 0 means no charset is set
    * (0 is IANA reserved number, not allocated currently to any charset.)
    *
    * @since    3.1
    * @param    aCharset    IN character set as MIB enumeration
    */
    inline void SetCharset( TUint aCharset );

    /**
    * Charset
    * As content type charset parameter
    * No other content type specific parameters supported
    * charset = 0 means no charset is set
    * (0 is IANA reserved number, not allocated currently to any charset.)
    *
    * @since    3.1
    * @return   character set MIB enumeration
    */
    inline TUint Charset() const;

    /**
    * SetContentTypeL
    * NOTE: 16-bit
    *
    * @since    3.1
    * @param    aContLoc    IN content location
    */
    void SetContentLocationL( const TDesC& aContLoc );

    /**
    * ContentLocation
    * NOTE: 16-bit
    *
    * @since    3.1
    * @return   pointer to content location buffer
    */
    inline const TPtrC ContentLocation() const;

    /**
    * SetContentIdL
    * NOTE: 8-bit
    *
    * @since    3.1
    * @param    aCid    IN content id
    */
    void SetContentIdL( const TDesC8& aCid );

    /**
    * ContentId
    * NOTE: 8-bit
    *
    * @since    3.1
    * @return   pointer to content id buffer
    */
    inline const TPtrC8 ContentId() const;

    /**
    * SetContentBaseL
    * NOTE: 8-bit
    *
    * @since    3.1
    * @param    aContBase   IN content base
    */
    void SetContentBaseL( const TDesC8& aContBase );

    /**
    * ContentBase
    * NOTE: 8-bit
    *
    * @since    3.1
    * @return   pointer to content base buffer
    */
    inline const TPtrC8 ContentBase() const;

    /**
    * SetContentDescriptionL
    * NOTE: 8-bit
    *
    * @since    3.1
    * @param    aContDesc   IN content description
    */
    void SetContentDescriptionL( const TDesC8& aContDesc );

    /**
    * ContentDescription
    * NOTE: 8-bit
    *
    * @since    3.1
    * @return   pointer to content description buffer
    */
    inline const TPtrC8 ContentDescription() const;

    /**
    * SetContentDispositionL
    * NOTE: 8-bit
    *
    * @since    3.1
    * @param    aContDesc   IN content disposition
    */
    void SetContentDispositionL( const TDesC8& aContDisp );

    /**
    * ContentDisposition
    * NOTE: 8-bit
    *
    * @since    3.1
    * @return   pointer to content disposition buffer
    */
    inline const TPtrC8 ContentDisposition() const;

    /**
    * Size
    *
    * @since    3.1
    * @return   size of the mime headers in bytes
    */
    TInt Size() const;

    /**
    * SaveMimeInfoL
    * Save objects mime info into message store
    *
    * @since    3.1
    * @param    aManager    IN reference to attachment manager
    * @param    aAttachment IN reference to attachment
    */
    void SaveMimeInfoL( MMsvAttachmentManager& aManager, CMsvAttachment& aAttachment );

    /**
    * ReadMimeInfoL
    * Reads attachments mime info from message store
    *
    * @since    3.1
    * @param    aAttachment IN reference to attachment
    */
    void ReadMimeInfoL( CMsvAttachment& aAttachment );

	/**
    * EnsureContentLocationL
	* Make sure that ContentLocation exists. In case it
    * doesn't one is created from the file name.
    *
    * @since    3.1
    * @param    aManager    IN reference to attachment manager
    * @param    aAttachment IN reference to attachment
    * @param    aPlainFileName   IN attachment file name
	*/
    void EnsureContentLocationL(
        MMsvAttachmentManager& aManager,
        CMsvAttachment& aAttachment,
        TDesC& aPlainFileName );

    private: // data

    HBufC8* iContentDescription;

    HBufC8* iContentBase;
    HBufC16* iContentLocation;
    HBufC8* iContentId;

    HBufC8* iContentType;
    TUint iContentTypeCharset; // As MIB enum.

    HBufC8* iContentDisposition;
    };

#include <UniMimeInfo.inl>

#endif //__UNIMIMEINFO_H



