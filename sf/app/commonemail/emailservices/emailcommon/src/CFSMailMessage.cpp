/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "emailtrace.h"

#include <apgcli.h>
#include <apmrec.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy

//<cmail>
#include "CFSMailMessage.h"
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailRequestHandler.h"
#include "CFSMailRequestObserver.h"

//<qmail>
#include <nmcommonheaders.h>
#include "nmmessage.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFSMailMessage::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage * CFSMailMessage::NewLC(TFSMailMsgId aMessageId)
{
    NM_FUNCTION;
    
    CFSMailMessage* message = new (ELeave) CFSMailMessage();
    CleanupStack:: PushL(message);
    message->ConstructL( aMessageId );
    return message;
} 

// -----------------------------------------------------------------------------
// CFSMailMessage::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage * CFSMailMessage::NewL(TFSMailMsgId aMessageId)
{
    NM_FUNCTION;
    
    CFSMailMessage* message =  CFSMailMessage::NewLC(aMessageId);
    CleanupStack:: Pop(message);
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailMessage::NewL( const NmMessage &aMessage )
{
    NM_FUNCTION;
    
    CFSMailMessage* self = new (ELeave) CFSMailMessage();
    CleanupStack::PushL(self);
    self->ConstructL(aMessage);
    CleanupStack::Pop(self);
    return self;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessage::ConstructL( TFSMailMsgId aMessageId )
{
    NM_FUNCTION;
    
// <qmail>
    // Base class initialization
    CFSMailMessagePart::ConstructL( aMessageId, TFSMailMsgId() );
// </qmail>
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessage::ConstructL( const NmMessage &aMessage )
{
    NM_FUNCTION;
    
    // Base class initialization
    
    CFSMailMessagePart::ConstructL( aMessage, aMessage.envelope() );
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::CFSMailMessage
// -----------------------------------------------------------------------------
CFSMailMessage::CFSMailMessage() : CFSMailMessagePart()
{
    NM_FUNCTION;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::~CFSMailMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage::~CFSMailMessage()
{
    NM_FUNCTION;
    
// <qmail>
    iMessageBufferForAsyncOp.Reset();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessage::SaveMessageL()
	{
    NM_FUNCTION;
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
		{
		plugin->StoreMessageL(GetMailBoxId(),*this);
		}
	}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::SaveMessageL( MFSMailRequestObserver& aOperationObserver )
{
    NM_FUNCTION;

    


    TFSPendingRequest request;
    request.iRequestId = 0;
    if (CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        request = iRequestHandler->InitAsyncRequestL(GetMessageId().PluginId(), 
                                                        aOperationObserver);
        MFSMailRequestObserver* observer = request.iObserver;
        iMessageBufferForAsyncOp.Reset();
        iMessageBufferForAsyncOp.AppendL(this);
        // It saves only message header - notice that iMessageBufferForAsyncOp ownership is not transferred
        // to plugin - array contains 'self' pointer so it should not be destroy by plugin
        TRAPD(err, plugin->StoreMessagesL(GetMailBoxId(), iMessageBufferForAsyncOp,
                *observer, request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    return request.iRequestId;
}
//</qmail>

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessagePartsL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::SaveMessagePartsL( MFSMailRequestObserver& aOperationObserver )
{
    NM_FUNCTION;

    RPointerArray<CFSMailMessagePart> messageParts;
    CleanupResetAndDestroyPushL(messageParts);
    
    CFSMailMessagePart *plainTextPart = PlainTextBodyPartL(EDataSourceLocal);
    if (plainTextPart)
        {
        messageParts.AppendL(plainTextPart);
        }
    CFSMailMessagePart *htmlPart = HtmlBodyPartL(EDataSourceLocal);
    if (htmlPart)
        {
        messageParts.AppendL(htmlPart);
        }
        
    TFSPendingRequest request;
    request.iRequestId = 0;
    if (CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        request = iRequestHandler->InitAsyncRequestL(
                         GetMessageId().PluginId(), aOperationObserver);
        MFSMailRequestObserver* observer = request.iObserver;
        // Ownership of parts array is transferred to plugin - it will take care about reseting them
        TRAPD(err, plugin->StoreMessagePartsL(messageParts, *observer,
                request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        // message parts array removed from CleanupStack (not destroyed)
        CleanupStack::Pop(1, &messageParts);
        }
    else
        {
        // message parts array removed from CleanupStack and destroyed
        CleanupStack::PopAndDestroy(1, &messageParts);
        User::Leave(KErrNotFound);
        }

    return request.iRequestId;
}
//<//qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::AddNewAttachmentL( RFile& aFile,
                                                                const TDesC8& aMimeType )
    {
    NM_FUNCTION;
    CFSMailMessagePart* newPart(NULL);
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        TBuf<KMaxDataTypeLength> contentType;
        if(aMimeType.Length() > 0)
            {
            // use user given content type
            contentType.Copy(aMimeType);
            }
        else
            {
            // try to find out content type using recognizers
            RApaLsSession apaSession;
            TDataRecognitionResult dataType;
            User::LeaveIfError(apaSession.Connect());
            User::LeaveIfError(apaSession.RecognizeData(aFile, dataType));
            apaSession.Close();
            contentType.Copy(dataType.iDataType.Des());
            }
        // get file name
        TFileName fileName;
        aFile.FullName( fileName );
        // get new part from plugin
        newPart = plugin->NewChildPartFromFileL(    GetMailBoxId(),
                                                    GetFolderId(),
                                                    GetMessageId(),
                                                    GetPartId(),
                                                    contentType,
                                                    aFile );
        if (newPart)
            {
            CleanupStack::PushL(newPart);
            // set attachment name
            newPart->SetAttachmentNameL(fileName);
            // store new message part
            newPart->SaveL();
            // set flag
            SetFlag(EFSMsgFlag_Attachments);
            CleanupStack::Pop(newPart);
            }
        }
    return newPart;
    }

// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::AddNewAttachmentL(	const TDesC& aFilePath,
																const TFSMailMsgId /*aInsertBefore*/ )
    {
    NM_FUNCTION;

    CFSMailMessagePart* newPart(NULL);
  
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        // use recognizer to find out ContentType 
        RApaLsSession apaSession;
        TDataRecognitionResult dataType;
        TBufC8<KMaxDataTypeLength> buf;
        User::LeaveIfError(apaSession.Connect());
        User::LeaveIfError(apaSession.RecognizeData(aFilePath, buf, dataType));
        apaSession.Close();

        // Create new message part with correct Content-Type
        TBuf<KMaxDataTypeLength> contentType;
        contentType.Copy(dataType.iDataType.Des());
        newPart = plugin->NewChildPartFromFileL(  GetMailBoxId(),
                            GetFolderId(),
                            GetMessageId(),
                            GetPartId(),
                            contentType,
                            aFilePath );
        if (newPart)
            {
            CleanupStack::PushL(newPart);
            // set attachment name
            newPart->SetAttachmentNameL(aFilePath);
            // store new message part
            newPart->SaveL();
            // set flag
            SetFlag(EFSMsgFlag_Attachments);
            CleanupStack::Pop(newPart);
            }
        }
    return newPart;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::AddNewAttachmentL( const TDesC& aFilePath,
                                                 MFSMailRequestObserver& aOperationObserver)
{
    NM_FUNCTION;
    
    TFSPendingRequest request;
  
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetMessageId().PluginId(),
                                                      aOperationObserver );
        
        MFSMailRequestObserver* observer = request.iObserver;
        
        // use recognizer to find out ContentType 
        RApaLsSession apaSession;
        TDataRecognitionResult dataType;
        TBufC8<KMaxDataTypeLength> buf;
        User::LeaveIfError(apaSession.Connect());
        User::LeaveIfError(apaSession.RecognizeData(aFilePath, buf, dataType));
        apaSession.Close();

        // Create new message part with correct Content-Type
        TBuf<KMaxDataTypeLength> contentType;
        contentType.Copy(dataType.iDataType.Des());

        TRAPD( err, plugin->NewChildPartFromFileL( 
            GetMailBoxId(),
            GetFolderId(),
            GetMessageId(),
            GetPartId(),
            contentType,
            aFilePath,
            *observer,
            request.iRequestId) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }    
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::AttachmentListL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessage::AttachmentListL(
    RPointerArray<CFSMailMessagePart>& aParts )
    {
    // First list all message parts
    AppendAttachmentsL( aParts );
    
    NM_FUNCTION;
    
    // special case, if single part content type is not given,
    // default type is text/plain
    if ( aParts.Count() == 1 && aParts[0]->GetContentType().Length() == 0)
        {
        aParts.ResetAndDestroy();
        }
    else
        {
        // find plain text body part from the list
        CFSMailMessagePart* txtPart = FindBodyPartL(KFSMailContentTypeTextPlain);
        CleanupStack::PushL( txtPart );
        if ( txtPart
          && txtPart->AttachmentNameL().Length() == 0 ) // real attachments have names
            {
            // remove plain text body part from attachment list
            for ( TInt ii = aParts.Count() - 1; ii >= 0; --ii )
                {
                if ( aParts[ii]->GetPartId() == txtPart->GetPartId() )
                    {
                    delete aParts[ii];
                    aParts.Remove(ii);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( txtPart );

        // find  html body part from the list
        CFSMailMessagePart* htmlPart = FindBodyPartL(KFSMailContentTypeTextHtml);
        CleanupStack::PushL( htmlPart );
        if ( htmlPart
          && htmlPart->AttachmentNameL().Length() == 0 ) // real attachments have names
            {
            // remove html body part from attachment list
            for ( TInt ii = aParts.Count() - 1; ii >= 0; --ii )
                {
                if ( aParts[ii]->GetPartId() == htmlPart->GetPartId() )
                    {
                    delete aParts[ii];
                    aParts.Remove(ii);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( htmlPart );
        }

    iMessageParts.ResetAndDestroy();
    iReadMessageParts = ETrue;
    }

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::PlainTextBodyPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::PlainTextBodyPartL(
        TFSMailMessagePartDataSource aDataSource)
{
    NM_FUNCTION;
    
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextPlain, aDataSource);
    
    // special case, single part content type is not given
    if (!part && iMessageParts.Count() == 1 && iMessageParts[0]->GetContentType().Length() == 0)
      {
      part = iMessageParts[0];
      iMessageParts.Remove(0);
      }
    iMessageParts.ResetAndDestroy();
    iReadMessageParts = ETrue;
    return part;
}
// </qmail>

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::HtmlBodyPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::HtmlBodyPartL(
        TFSMailMessagePartDataSource aDataSource)
{
    NM_FUNCTION;
    
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextHtml, aDataSource);
    iMessageParts.ResetAndDestroy();
	iReadMessageParts = ETrue;
    return part;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::ContentTypeParameters()
// -----------------------------------------------------------------------------
CDesCArray& CFSMailMessage::ContentTypeParameters()
{
    NM_FUNCTION;
    
   CDesCArray* faked = NULL;
   return *faked;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::DoAttachmentListL
// -----------------------------------------------------------------------------
void CFSMailMessage::DoAttachmentListL( RPointerArray<CFSMailMessagePart>& aParts )
    {
    NM_FUNCTION;
    
    AttachmentListL(aParts);
    }

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::GetNmMessage
// It does not fill the list of child parts
// -----------------------------------------------------------------------------
EXPORT_C NmMessage* CFSMailMessage::GetNmMessage()
{
    NM_FUNCTION;
    
	NmMessage* message = new NmMessage(iNmPrivateMessageEnvelope, iNmPrivateMessagePart);
	return message;
}
//</qmail>
