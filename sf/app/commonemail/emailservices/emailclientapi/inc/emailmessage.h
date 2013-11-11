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
* Description: mail message implementation definition
*
*/

#ifndef EMAILMESSAGE_H
#define EMAILMESSAGE_H

#include <memailmessage.h>
#include "CFSMailCommon.h"
#include "emailapiutils.h"
#include "MFSMailRequestObserver.h"
#include <QEventLoop>

using namespace EmailInterface;

class CFSMailMessagePart;
class CFSMailMessage;
class CFSMailAddress;
class CFSMailPlugin;
class CPluginData;
class CEmailAddress;
class CEmailAttachment;
class CEmailTextContent;
class CEmailMultipart;

NONSHARABLE_CLASS( CEmailMessage ) : public CBase, public MEmailMessage, public MFSMailRequestObserver
    {
public:
    /**
    * Creates email message from plugin message
    */
    static CEmailMessage* NewL( CPluginData& aPluginData,
                                CFSMailMessage* aFsMessage,
                                const TDataOwner aOwner );

    ~CEmailMessage();

public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();
    
public: // from MEmailMessage
    /**@see MEmailMessage */
    const TMessageId& MessageId() const;
    
    /**@see MEmailMessage */
    MEmailAddress* SenderAddressL() const;
    
    /**@see MEmailMessage */
    MEmailAddress* ReplyToAddressL() const;
    
    /**@see MEmailMessage */
    void SetReplyToAddressL( const MEmailAddress& aSender );
    
    /**@see MEmailMessage */
    TInt GetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients ) const;            
    
    /**@see MEmailMessage */
    void SetRecipientsL( const MEmailAddress::TRole aRole, REmailAddressArray& aRecipients );
    
    /**@see MEmailMessage */
    void RemoveRecipientL( const MEmailAddress& aRecipient );
    
    /**@see MEmailMessage */
    TPtrC Subject() const;

    /**@see MEmailMessage */
    void  SetSubjectL( const TPtrC& aSubject );    
    
    /**@see MEmailMessage */
    TTime Date() const;

    /**@see MEmailMessage */
    TInt Flags() const;
    
    /**@see MEmailMessage */
    void SetFlag( const TUint aFlag );
    
    /**@see MEmailMessage */
    void ResetFlag( const TUint aFlag );

    /**@see MEmailMessage */
    MEmailMessageContent* ContentL() const;

    /**@see MEmailMessage */
    void SetContentL( const MEmailMessageContent* aContent );
    
    /**@see MEmailMessage */
    void SetPlainTextBodyL( const TDesC& aPlainText );

    /**@see MEmailMessage */
    MEmailAttachment* AddAttachmentL( const TDesC& aFullPath );
    
    /**@see MEmailMessage */
    MEmailAttachment* AddAttachmentL( RFile& aFile );
    
    /**@see MEmailMessage */
    TInt GetAttachmentsL( REmailAttachmentArray& aAttachments );
    
    /**@see MEmailMessage */
    void RemoveAttachmentL( const MEmailAttachment& aAttachment );

    /**@see MEmailMessage */
    const TFolderId& ParentFolderId() const;

    /**@see MEmailMessage */
    void SaveChangesL();

    /**@see MEmailMessage */
    void SendL();

    /**@see MEmailMessage */
    void ShowMessageViewerL();
    
    /**@see MEmailMessage */
    void ReplyToMessageL( const TBool aReplyToAll = ETrue );
    
    /**@see MEmailMessage */
    void ForwardMessageL();
    
protected: // From MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

private:        

    // Copies/moves flag values from iPluginMessage's flags to local flag member variable
    void InitializeFlagValues();
    
    CEmailMessage( CPluginData& aPluginData, CFSMailMessage* aMessage, const TDataOwner aOwner );
    
    void ConstructL();
							       
    void ConvertAddressArrayL( const MEmailAddress::TRole aRole, 
                               const RPointerArray<CFSMailAddress>& aSrc, 
                               REmailAddressArray& aDst ) const; 
    CEmailAddress* CreateAddressLC( const MEmailAddress::TRole aRole, CFSMailAddress& aFsAddress ) const;
    TUint MapFlags( const TUint& aFlag );
    
    TMessageContentId MessageContentId( TEntryId aContentId ) const;
        
private:
    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMessageId      iMessageId;
    
    TMessageContentId   iMsgContentId;
                                                                                  
    mutable CEmailAddress*  iSender;
    
    mutable CEmailAddress*  iReplyTo;
    
    REmailAddressArray iRecipients;
    
    CFSMailMessage* iPluginMessage;
    
    TUint        iFlags;
    
    mutable CEmailTextContent *iTextContent;
    mutable CEmailMultipart* iContent;
    RPointerArray<CEmailAttachment> iAttachments;
    TDataOwner iOwner;
    QEventLoop iEventLoop;
    TInt iError;

    };
//Class for compare email content type 
class TContentType
{
public:
    TContentType( const TDesC& aContentType );
    // Returns ETrue iff the content type represented by this object 
    // (after parameters have been removed) is equal to the given content type.
    TBool Equals( const TDesC& aContentType );

private:
    // Content type (without parameters)
    TPtrC iContentType;
};


#endif // EMAILMESSAGE_H

// End of file
