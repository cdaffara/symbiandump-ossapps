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
* Description: Definition of Email attachment.
*
*/


#ifndef EMAILATTACHMENT_H
#define EMAILATTACHMENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "CFSMailClient.h"
#include <memailcontent.h>
#include "emailapiutils.h"

using namespace EmailInterface;

class CEmailMessageContent;
// CLASS DECLARATION

/**
 *  CEmailAttachment
 * 
 */

NONSHARABLE_CLASS(CEmailAttachment) : public CBase, public MEmailAttachment
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CEmailAttachment();

    /**
     * Two-phased constructor.
     */
    static CEmailAttachment* NewL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner );    

    /**
     * Two-phased constructor.
     */
    static CEmailAttachment* NewLC(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,    
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner );    

public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();

public: // from MEmailAttachment
    /**
    * Returns file handle of this attachment. If the attachment is
    * not associated with a file, null file handle is returned.
    */
    RFile FileL() const;

    /**
    * Sets file name field
    */
    void SetFileNameL( const TDesC& aFileName );

    /**
    * Returns file name or null pointer descriptor if attachment
    * is not associated with any file
    */
    TPtrC FileNameL() const;
    
    
public: // from MEmailMessageContent
    
    TMessageContentId Id() const;

    TPtrC ContentType() const;

    void SetContentType( const TDesC& aContentType );

    TPtrC ContentId() const;

    void SetContentId( const TDesC& aContentId );

    TPtrC ContentDescription() const;

    void SetContentDescription( const TDesC& aContentDescription );

    TPtrC ContentDisposition() const;

    void SetContentDisposition( const TDesC& aContentDisposition );

    TPtrC ContentClass() const;

    void SetContentClass( const TDesC& aContentClass );

    TInt AvailableSize() const;

    TInt TotalSize() const;

    TPtrC ContentL() const;

    void SetContentL( const TDesC& aContent );

    void FetchL( MEmailFetchObserver& aObserver );

    void CancelFetch();

    void SaveToFileL( const TDesC& aPath );

    MEmailMultipart* AsMultipartOrNull() const;
    
    MEmailTextContent* AsTextContentOrNull() const;

    MEmailAttachment* AsAttachmentOrNull() const;
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CEmailAttachment( const TDataOwner aOwner );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId );    

    void ConstructL(
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId,
            CFSMailMessagePart *aPart);    
    
private:        
    CEmailMessageContent*   iEmailMsgContent;
    TDataOwner iOwner;
    };

#endif // EMAILATTACHMENT_H

// End of file
