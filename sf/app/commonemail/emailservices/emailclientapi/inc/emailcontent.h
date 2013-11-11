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
* Description: mail content implementation definition
*
*/

#ifndef EMAILCONTENT_H
#define EMAILCONTENT_H

#include <memailcontent.h>

#include "CFSMailClient.h"
#include "emailapiutils.h"

using namespace EmailInterface;

class CFSMailPlugin;
class CFSMailRequestObserver;

NONSHARABLE_CLASS( CEmailMessageContent ) : public CBase, 
                                            public MEmailMessageContent
{
public:
    static CEmailMessageContent* NewL( 
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId,
            CFSMailMessagePart* aPart);
        
    ~CEmailMessageContent();
    
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
    
public:     
    CFSMailMessagePart& Part();
    void SetPart( CFSMailMessagePart *aPart );
    
    CPluginData& PluginData();
        
protected:
    CEmailMessageContent( CPluginData& aPluginData, const TMessageContentId& aMsgContentId, CFSMailMessagePart* aPart );
    
private:
    class CContentRequestObserver: public CBase, 
                                   public MFSMailRequestObserver
        {
    public:
        CContentRequestObserver( CEmailMessageContent &aParent);
        
        virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        void SetObserverL( MEmailFetchObserver* aObserver );
    private:
        MEmailFetchObserver* iObserver;
        CEmailMessageContent& iParent;
        };

private:
    void ConstructL();

private:
    CPluginData&            iPluginData;
    CFSMailPlugin*          iPlugin;
    CFSMailMessagePart*     iPart;
    TMessageContentId       iMsgContentId;
    mutable HBufC*          iBuf;
    mutable TUint           iUsed;
    CContentRequestObserver *iFetchObserver;
    TInt                    iRequestId;
};

#endif // EMAILCONTENT_H

// End of file
