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
* Description: Definition of Email text content.
*
*/


#ifndef EMAILMULTIPART_H_
#define EMAILMULTIPART_H_
// INCLUDES
#include "CFSMailClient.h"
#include <memailcontent.h>
#include "emailapiutils.h"

using namespace EmailInterface;

class CEmailMessageContent;


// CLASS DECLARATION

NONSHARABLE_CLASS( CEmailMultipart ) : public CBase,
                                       public MEmailMultipart
{
public:
    static CEmailMultipart* NewL( CPluginData& aPluginData, 
            const TMessageContentId& aMsgContentId, 
            CFSMailMessagePart* aPart,
            const TDataOwner aOwner );
    
    ~CEmailMultipart();
    
public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();

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
    
public: // from MEmailMultipart
    TInt PartCountL();

    MEmailMessageContent* PartByIndexL( const TUint aIndex ) const;

    void DeletePartL( const TUint aIndex );

    void AddPartL(
        const MEmailMessageContent& aPart,
        const TUint aPos );
    
public: // for internal usage
    void SetOwner( const TDataOwner aOwner );
    
    
private:
    CEmailMultipart( const TDataOwner aOwner );
    
    void ConstructL( CPluginData& aPluginData, const TMessageContentId& aMsgContentId, CFSMailMessagePart* aPart );
    
private: // Private data        
    CEmailMessageContent* iEmailMsgContent;
    RArray<TFSMailMsgId> iChildParts;
    TUint iChildPartCount;
    TDataOwner iOwner;
};

#endif // EMAILMULTIPART_H_

// End of file

