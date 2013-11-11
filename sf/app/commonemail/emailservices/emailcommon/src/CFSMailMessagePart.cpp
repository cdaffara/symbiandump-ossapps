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
* Description:  common email part object
*
*/

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>

#include <apgcli.h>
#include <apmrec.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy

//<cmail>
#include "CFSMailMessagePart.h"
#include "CFSMailPlugin.h"
//</cmail>

#include "CFSMailRequestObserver.h"

//<qmail>
#include "nmmessagepart.h"
#include "nmconverter.h"
//</qmail>


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailMessagePart::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::NewLC( TFSMailMsgId aMessageId,
														TFSMailMsgId aMessagePartId )
{
    NM_FUNCTION;
    
    CFSMailMessagePart* adr = new (ELeave) CFSMailMessagePart();
    CleanupStack::PushL(adr);
    adr->ConstructL( aMessageId, aMessagePartId );
    return adr;
} 

// -----------------------------------------------------------------------------
// CFSMailMessagePart::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::NewL(	TFSMailMsgId aMessageId,
														TFSMailMsgId aMessagePartId )
{
    NM_FUNCTION;
    
    CFSMailMessagePart* adr =  CFSMailMessagePart::NewLC( aMessageId, aMessagePartId );
    CleanupStack::Pop(adr);
    return adr;
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::NewLC(const NmId &aNmMessageId,
    const NmMessagePart& aNmMessage )
{
    NM_FUNCTION;
    
	CFSMailMessagePart* self = new (ELeave) CFSMailMessagePart();
	CleanupStack::PushL(self);
	self->ConstructL( aNmMessageId,aNmMessage );
	return self;
} 
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessagePart::ConstructL( TFSMailMsgId aMessageId,
                                     TFSMailMsgId aMessagePartId )
{
    NM_FUNCTION;
    
//<qmail>
	// Base class initialization
    CFSMailMessageBase::ConstructL(aMessageId);
	
	iNmPrivateMessagePart = new NmMessagePartPrivate();
	
	iNmPrivateMessagePart->mPartId.setId32(aMessagePartId.Id());
	iNmPrivateMessagePart->mPartId.setPluginId32((quint32)aMessagePartId.PluginId().iUid);
		
	iMessagePartsStatus = EFSDefault;

	iContentTypeParams  = new (ELeave)CDesCArrayFlat( KArrayGranularity );
	iContentDispositionParams = new (ELeave)CDesCArrayFlat( KArrayGranularity );
//</qmail>
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessagePart::ConstructL( const NmId &aNmMessageId,const NmMessagePart& aNmMessage )
{
    NM_FUNCTION;
    
    // Base class initialization
    CFSMailMessageBase::ConstructL(TFSMailMsgId(aNmMessageId));
    
    iNmPrivateMessagePart = aNmMessage.d;
           
    iMessagePartsStatus = EFSDefault;

    iContentTypeParams  = new (ELeave)CDesCArrayFlat( KArrayGranularity );
    iContentDispositionParams = new (ELeave)CDesCArrayFlat( KArrayGranularity );

}
//</qmail>

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessagePart::ConstructL(
    const NmMessagePart& aNmMessage,
    const NmMessageEnvelope& aNmMessageEnvelope)
{
    NM_FUNCTION;
    
    // Base class initialization
    CFSMailMessageBase::ConstructL(aNmMessageEnvelope);
    
    iNmPrivateMessagePart = aNmMessage.d;
           
    iMessagePartsStatus = EFSDefault;

    iContentTypeParams  = new (ELeave)CDesCArrayFlat( KArrayGranularity );
    iContentDispositionParams = new (ELeave)CDesCArrayFlat( KArrayGranularity );

}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessagePart::~CFSMailMessagePart
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart::~CFSMailMessagePart()
{
    NM_FUNCTION;
    
//<qmail> iContentType, iContentDescription, iContentDisposition, iContentID removed </qmail>
	if(iAttachmentName)
	{
		delete iAttachmentName;
	}
 	iAttachmentName = NULL;

	if(iContentClass)
	{
		delete iContentClass;
	}
 	iContentClass = NULL;

	if(iMeetingRequest)
	{
		delete iMeetingRequest;
	}
	iMeetingRequest = NULL;

	if(iContentDispositionParams)
		{
	    iContentDispositionParams->Reset();
	    delete iContentDispositionParams;
		}
	if(iContentTypeParams)
		{
	    iContentTypeParams->Reset();
	    delete iContentTypeParams;		
		}
	
	iMessageParts.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::CFSMailMessagePart
// -----------------------------------------------------------------------------
CFSMailMessagePart::CFSMailMessagePart() : CFSMailMessageBase()
{
    NM_FUNCTION;
	
//<qmail> iContentType, iContentDescription, iContentDisposition, iContentID removed </qmail>
	iContentClass = HBufC::New(1);
	iContentClass->Des().Copy(KNullDesC());

	iAttachmentName = HBufC::New(1);
	iAttachmentName->Des().Copy(KNullDesC());

	iReadMessageParts = ETrue;
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetContentType
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CFSMailMessagePart::GetContentType() const
{
    NM_FUNCTION;
    
//<qmail>
    iContentTypePtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateMessagePart->mContentType.utf16()),
    		iNmPrivateMessagePart->mContentType.length());
    return iContentTypePtr;
//</qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentType
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentType(const TDesC& aContentType)
{
    NM_FUNCTION;
    
//<qmail>
    QString qtContentType = QString::fromUtf16(aContentType.Ptr(), aContentType.Length());
    iNmPrivateMessagePart->mContentType = qtContentType;
//</qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetPartId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessagePart::GetPartId() const
{
    NM_FUNCTION;
    
//<qmail>
    return TFSMailMsgId(iNmPrivateMessagePart->mPartId);
//</qmail>	
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::ChildPartsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::ChildPartsL( RPointerArray<CFSMailMessagePart>& aParts,
    TFSMailMessagePartDataSource aDataSource)
	{
    NM_FUNCTION;

    if (aDataSource == EDataSourceMessageStore)
        {
        if (CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
            {
            plugin->ChildPartsL(GetMailBoxId(), GetFolderId(),
                    GetMessageId(), GetPartId(), aParts);
            }
        }
    else
        {
        int count = iNmPrivateMessagePart->mChildParts.count();
        for (int i = 0; i < count; i++)
            {
            CFSMailMessagePart* part = CFSMailMessagePart::NewLC(
                    NmConverter::mailMsgIdToNmId(GetMessageId()),
                    *iNmPrivateMessagePart->mChildParts[i]);
            aParts.AppendL(part);
            CleanupStack::Pop(part);
            }
		}
	}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ChildPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::ChildPartL(TFSMailMsgId aPartId)
	{
    NM_FUNCTION;
	
	CFSMailMessagePart* part = NULL;
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
		{
		part = plugin->MessagePartL(GetMailBoxId(),
									GetFolderId(),
									GetMessageId(),
									aPartId);
		}
	return part;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::IsMessageL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessagePart::IsMessageL() const
	{
    NM_FUNCTION;

	TBuf<KMaxDataTypeLength> ptr;
//<qmail>
    if ( !iNmPrivateMessagePart->mContentType.isNull() ) 
    {
		ptr.Copy(GetContentType());
//</qmail>
		TInt length = ptr.Locate(';');
		if(length >= 0)
			{
			ptr.SetLength(length);
			}
		
		if( !ptr.CompareF(KFSMailContentTypeMessage) )
		{
			return ETrue;
		}
    }
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::NewChildPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::NewChildPartL( const TFSMailMsgId aInsertBefore,
																const TDesC& aContentType )
	{
    NM_FUNCTION;

	CFSMailMessagePart* part = NULL;
	
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
		{
		part = plugin->NewChildPartL( 	GetMailBoxId(), GetFolderId(), GetMessageId(),
								 		GetPartId(), aInsertBefore, aContentType );
		}
	return part;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::RemoveChildPartL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::RemoveChildPartL(TFSMailMsgId aPartId)
{
    NM_FUNCTION;
    
	// get plugin pointer
	TFSMailMsgId id = GetPartId();
	if(id.IsNullId())
		{
		id = GetMessageId();
		}
	CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(id);
	
	if(plugin)
		{
		plugin->RemoveChildPartL( GetMailBoxId(), GetFolderId(), GetMessageId(), GetPartId(), aPartId );
		}
	}
	
// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::RemoveChildPartL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessagePart::RemoveChildPartL( TFSMailMsgId aPartId,
                                                    MFSMailRequestObserver& aOperationObserver)
{
    NM_FUNCTION;
    
    TFSPendingRequest request;
    // get plugin pointer
    TFSMailMsgId id = GetPartId();
    if(id.IsNullId())
        {
        id = GetMessageId();
        }
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(id);

    if(plugin)
        {
        request = iRequestHandler->InitAsyncRequestL( id.PluginId(),
                                                      aOperationObserver );
        
        MFSMailRequestObserver* observer = request.iObserver;
        
        TRAPD(err, plugin->RemoveChildPartL( GetMailBoxId(),
            GetFolderId(),
            GetMessageId(),
            GetPartId(),
            aPartId,
            *observer,
            request.iRequestId));
        
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
// CFSMailMessagePart::GetContentClass
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CFSMailMessagePart::GetContentClass()
	{
    NM_FUNCTION;
    
		return *iContentClass;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentClass
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentClass( const TDesC& aContentClass )
	{
    NM_FUNCTION;

	// 
	HBufC* contentClass = HBufC::New(aContentClass.Length());
	
	// store new mailbox name
	delete iContentClass;
	iContentClass = contentClass;
	iContentClass->Des().Copy(aContentClass);
	iContentClass->ReAlloc(aContentClass.Length());
	iContentClass->Des().Copy(aContentClass);		
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentSize
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailMessagePart::ContentSize() const
	{
    NM_FUNCTION;
    
//<qmail>
    return (TUint)iNmPrivateMessagePart->mSize;
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentSize
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentSize( TUint aContentSize )
	{
    NM_FUNCTION;
    
//<qmail>
    iNmPrivateMessagePart->mSize = quint32(aContentSize);
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentID
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CFSMailMessagePart::ContentID()
	{
    NM_FUNCTION;
    
//<qmail>	
    iContentIDPtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateMessagePart->mContentId.utf16()),
    		iNmPrivateMessagePart->mContentId.length());
    return iContentIDPtr;	
//</qmail>
	}
	
// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentIDL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentIDL(const TDesC& aContentID)
	{
    NM_FUNCTION;
    
//<qmail>
    QString qtContentID = QString::fromUtf16(aContentID.Ptr(), aContentID.Length());
    iNmPrivateMessagePart->mContentId = qtContentID;    
//</qmail>    
	}
	

// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetContentFileL
// -----------------------------------------------------------------------------
EXPORT_C RFile CFSMailMessagePart::GetContentFileL( TBool aForWriting )
	{
    NM_FUNCTION;
	
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
		TInt rcode = plugin->GetMessagePartFileL( GetMailBoxId(), GetFolderId(),
					                            GetMessageId(), GetPartId(),
					                            iFile, aForWriting);
		// if content is encrypted, use temp dir files
		if(rcode == KErrNotSupported)
		    {
		    // temp directory C:\Private\<uid>
		    TFileName fileName;
		    iFile = iRequestHandler->GetTempFileL(GetPartId(),fileName);
		    plugin->CopyMessagePartFileL(GetMailBoxId(), GetFolderId(),
		                                 GetMessageId(), GetPartId(),
		                                 fileName);
		    }
		}
	
	return iFile;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::CopyContentFileL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::CopyContentFileL( const TDesC& aFilePath )
	{
    NM_FUNCTION;
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
		plugin->CopyMessagePartFileL( GetMailBoxId(), GetFolderId(),
			GetMessageId(), GetPartId(), aFilePath);		
		}
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetContentToBufferL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::GetContentToBufferL(TDes16& aBuffer, TUint aStartOffset)
	{
    NM_FUNCTION;
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
		plugin->GetContentToBufferL( GetMailBoxId(), GetFolderId(), GetMessageId(),
									 GetPartId(), aBuffer, aStartOffset );
		}
	}
	
// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContent
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContent( TDes16& aBuffer )
	{
    NM_FUNCTION;
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
// <qmail>
        TRAP_IGNORE(plugin->SetContentL( aBuffer, GetMailBoxId(), GetFolderId(),
            GetMessageId(), GetPartId() ));
// </qmail>
        }
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentDescription
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CFSMailMessagePart::ContentDescription()
	{
    NM_FUNCTION;
    
//<qmail>
    iContentDescriptionPtr.Set(reinterpret_cast<const TUint16*> (
            iNmPrivateMessagePart->mContentDescription.utf16()),
    		iNmPrivateMessagePart->mContentDescription.length());
    
    return iContentDescriptionPtr;
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentDescription
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentDescription( const TDesC& aContentDescription )
	{
    NM_FUNCTION;
    
//<qmail>
    QString qtContentDescription = QString::fromUtf16(
            aContentDescription.Ptr(), aContentDescription.Length());
    iNmPrivateMessagePart->mContentDescription = qtContentDescription;
//</qmail>
	}
// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentDisposition
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CFSMailMessagePart::ContentDisposition()
	{
    NM_FUNCTION;
    
//<qmail>
	iContentDispositionPtr.Set(reinterpret_cast<const TUint16*> (
	        iNmPrivateMessagePart->mContentDisposition.utf16()),
			iNmPrivateMessagePart->mContentDisposition.length());
	return iContentDispositionPtr;
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetContentDisposition
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetContentDisposition( const TDesC& aContentDisposition )
	{
    NM_FUNCTION;
    
//<qmail>
    QString qtContentDisposition = QString::fromUtf16(
            aContentDisposition.Ptr(), aContentDisposition.Length());
    iNmPrivateMessagePart->mContentDisposition = qtContentDisposition;
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentTypeParameters
// -----------------------------------------------------------------------------
EXPORT_C CDesCArray& CFSMailMessagePart::ContentTypeParameters()
	{
    NM_FUNCTION;
    
//<qmail>	
    if (iNmPrivateMessagePart->mContentType.isNull())
		{
		return *iContentTypeParams;
		}
	
	// clear parameter table
	iContentTypeParams->Reset();
	TInt ret = KErrNone;
	
	// check content type parameters existence
	TInt index = GetContentType().Locate(';');
//</qmail>	
	if(index >= 0)
		{
		TInt attLength(0);
		TInt valueLength(0);
		while(index >= 0)
			{
			TPtrC attribute;
			TPtrC value;
			HBufC* buf;
			
			// set pointer to first parameter
//<qmail>
			TPtrC parameter = GetContentType().Mid(index+1);
//</qmail>
			attLength = parameter.Locate('=');
			if(attLength >= 0)
				{
				attribute.Set(parameter.Left(attLength));
		  		valueLength = parameter.Locate(';');
			  	if(valueLength < 0)
			  		{
				  	value.Set( parameter.Right(parameter.Length()-attLength-1) );
				  	index = -1;
			  		}
			  	else
			  		{
				  	value.Set(parameter.Mid(attLength+1,(valueLength-attLength-1)));
					index += valueLength + 1;			  		
			  		}
				}
			else
				{
		  		attLength = parameter.Locate(';');
			  	if(attLength < 0)
			  		{
				  	attribute.Set(parameter);
				  	index = -1;
			  		}
			  	else
			  		{
					attribute.Set(parameter.Left(attLength));
				  	index += attLength + 1;
			  		}
			  	}
			
			if(attribute.Length())
				{
				buf = HBufC::New(attribute.Length());
				buf->Des().Copy(attribute);
				TRAPD(err,iContentTypeParams->AppendL(buf->Des()));
				if(value.Length() && err == KErrNone)
					{
					buf = HBufC::New(value.Length());
					buf->Des().Copy(value);
					TRAP(err,iContentTypeParams->AppendL(buf->Des()));
					ret = err;
					}
				else
					{
					TBufC<1> empty(KNullDesC);
					HBufC* buf = HBufC::New(empty.Des().Length());
					buf->Des().Copy(empty);
					TRAP(err,iContentTypeParams->AppendL(buf->Des()));
					ret = err;
					}
    			}
    		}
    	}
	
	if(ret != KErrNone)
		{
		// clear parameter table
		iContentTypeParams->Reset();
		}
		
	return *iContentTypeParams;
    }

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ContentDispositionParameters
// -----------------------------------------------------------------------------
EXPORT_C CDesCArray& CFSMailMessagePart::ContentDispositionParameters()
	{
    NM_FUNCTION;
	
//<qmail>
    const TDesC& contentDisposition = ContentDisposition();
    
	if(!contentDisposition.Ptr())
		{
		return *iContentDispositionParams;
		}
//</qmail>
	
	// clear parameter array
	iContentDispositionParams->Reset();
	TInt ret = KErrNone;
    
	// check content disposition parameters existence
//<qmail>
	TInt index = contentDisposition.Locate(';');
//</qmail>
	if( index >= 0 )
		{
		TInt attLength(0);
		TInt valueLength(0);
		while(index >= 0)
			{
			TPtrC attribute;
			TPtrC value;
			
			// set pointer to first parameter
//<qmail>
			TPtrC parameter = contentDisposition.Mid(index+1);
//</qmail>
			attLength = parameter.Locate('=');
			if( attLength >= 0 )
				{
				attribute.Set(parameter.Left(attLength));
		  		valueLength = parameter.Locate(';');
			  	if( valueLength < 0 )
			  		{
				  	value.Set( parameter.Right(parameter.Length()-attLength-1) );
				  	index = -1;
			  		}
			  	else
			  		{
				  	value.Set( parameter.Mid(attLength+1,(valueLength-attLength-1)) );
					index += valueLength + 1;			  		
			  		}
				}
			else
				{
		  		attLength = parameter.Locate(';');
			  	if(attLength < 0)
			  		{
				  	attribute.Set(parameter);
				  	index = -1;
			  		}
			  	else
			  		{
					attribute.Set(parameter.Left(attLength));
				  	index += attLength + 1;
			  		}
			  	}
		    
			if(attribute.Length())
				{
				HBufC* attBuf = HBufC::New(attribute.Length());
				attBuf->Des().Copy(attribute);
				// let's trim the attribute before adding to array
				TPtr attTrim( attBuf->Des() );
				attTrim.Trim();
				
				if ( attTrim.Length() )
				    {
				    // add trimmed attribute to the array
				    TRAPD(err,iContentDispositionParams->AppendL( attTrim ));
				    
				    if(value.Length() && err == KErrNone)
    					{
    					HBufC* valueBuf = HBufC::New(value.Length());
    					valueBuf->Des().Copy(value);
    					// let's trim also the value before adding to array
    					TPtr valueTrim( valueBuf->Des() );
    				    valueTrim.Trim();
    				    
    				    if ( valueTrim.Length() )
        				    {
        				    // add trimmed value to the array
        				    TRAPD(err,iContentDispositionParams->AppendL( valueTrim ));
							ret = err;
        				    }
        				else
        				    {
        				    delete valueBuf;
        				    valueBuf = NULL;
        				    
        				    TBufC<1> empty(KNullDesC);
        					HBufC* emptyBuf = HBufC::New(empty.Des().Length());
        					emptyBuf->Des().Copy(empty);
        					TRAPD(err,iContentDispositionParams->AppendL(emptyBuf->Des()));
							ret = err;
        				    }
    					}
    				else
    			        {
    			        TBufC<1> empty(KNullDesC);
    					HBufC* emptyBuf = HBufC::New(empty.Des().Length());
    					emptyBuf->Des().Copy(empty);
    					TRAPD(err,iContentDispositionParams->AppendL(emptyBuf->Des()));
    					ret = err;
    			        }
				    }
				else
				    {
				    delete attBuf;
				    attBuf = NULL;
				    }
				}
    		} // while
    	}

	if(ret != KErrNone)
		{
		// clear parameter table
		iContentDispositionParams->Reset();
		}
	
	return *iContentDispositionParams;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetMRInfo
// -----------------------------------------------------------------------------
EXPORT_C MMRInfoObject& CFSMailMessagePart::GetMRInfo()
{
    NM_FUNCTION;
    
	return *iMeetingRequest;
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::IsMRInfoSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessagePart::IsMRInfoSet()
{
    NM_FUNCTION;
    
	if(iMeetingRequest)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetMRInfo
// -----------------------------------------------------------------------------	
EXPORT_C void CFSMailMessagePart::SetMRInfo(MMRInfoObject* aMeetingRequest)
{
    NM_FUNCTION;
    
	if(iMeetingRequest)
	{
		delete iMeetingRequest;
	}
	iMeetingRequest = aMeetingRequest;
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SaveL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SaveL()
	{
    NM_FUNCTION;
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
		plugin->StoreMessagePartL( GetMailBoxId(), GetFolderId(), GetMessageId(), *this );		
		}
	}


// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetAttachmentNameL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetAttachmentNameL(const TDesC& aFilePath)
	{
    NM_FUNCTION;
    
	    // Parse file name in case full path is given
		TPtrC name;
		for(TInt i=0;i<aFilePath.Length();)
			{
			name.Set(aFilePath.Mid(i,(aFilePath.Length()-i)));
			TInt skip = name.Locate(KPathDelimiter);
			if(skip < 0) break;
			else i += skip + 1;
			}
			
	    // Set Content-Type param "name" = filename
	    TUint length =  KFSMailContentTypeParamName.iTypeLength + name.Length() + 5;
//<qmail>
	    if (!iNmPrivateMessagePart->mContentType.isNull()) 
			{
			length += GetContentType().Length();
	    	}

	    HBufC* buffer = HBufC::NewL(length);
	    if (!iNmPrivateMessagePart->mContentType.isNull()) 
			{
			buffer->Des().Append(GetContentType());
	    	}
//</qmail>
		
	    buffer->Des().Append(_L("; "));
	    buffer->Des().Append(KFSMailContentTypeParamName);
	    buffer->Des().Append('"');
	    buffer->Des().Append(name);
	    buffer->Des().Append('"');
	    
//<qmail>
	    SetContentType(*buffer);
	    delete buffer;
//</qmail>
	    
	    // Set Content-Disposition as "attachment" and
	    // Content-Disposition param "filename" = filename and
	    length = KFSMailContentDispAttachment.iTypeLength +
	    KFSMailContentDispParamFilename.iTypeLength + name.Length() + 4;
		
//<qmail>
	    buffer = HBufC::NewL(length);
	    if (ContentDisposition().FindF(KFSMailContentDispInline) != KErrNotFound)
	        {
            buffer->Des().Append(KFSMailContentDispInline);
	        }
	    else if (ContentDisposition().FindF(KFSMailContentDispAttachment) != KErrNotFound)
	        {
            buffer->Des().Append(KFSMailContentDispAttachment);
	        }
	    
	    buffer->Des().Append(_L("; "));
	    buffer->Des().Append(KFSMailContentDispParamFilename);
	    buffer->Des().Append('"');
	    buffer->Des().Append(name);
	    buffer->Des().Append('"');
	    SetContentDisposition(*buffer);    
	    delete buffer;	    
	    
		// set content description		
		SetContentDescription(name);
//</qmail>	
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::AttachmentNameL
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailMessagePart::AttachmentNameL()
	{
    NM_FUNCTION;
    
//<qmail>
    // Look first from Content-Type param "name"
	TInt ptr = GetContentType().Find(KFSMailContentTypeParamName);
    if(ptr >= 0)
    	{
    	ptr = GetContentType().Locate('=');
    	TInt length = GetContentType().Length()-ptr-3;
    	TPtrC name = GetContentType().Mid((ptr+2),length);
    	if(iAttachmentName)
    		{
    		delete iAttachmentName;
    		iAttachmentName = NULL;
    		}
    	iAttachmentName = HBufC::New(name.Length());
    	iAttachmentName->Des().Copy(name);
    	return *iAttachmentName;
    	}
    // then if Content-Disposition is "attachment" look from Content-Disposition param "filename" and
    TDesC contentDisposition = ContentDisposition(); 
    ptr = contentDisposition.Find(KFSMailContentDispAttachment);
	if(ptr >= 0)
		{
		ptr = contentDisposition.Find(KFSMailContentDispParamFilename);
		if(ptr > 0)
			{
			ptr = contentDisposition.Locate('=');
			TInt length = contentDisposition.Length()-ptr-3;
	    	TPtrC name = contentDisposition.Mid(ptr+2,length);
    		if(iAttachmentName)
    		{
    			delete iAttachmentName;
    			iAttachmentName = NULL;
    		}
	    	iAttachmentName = HBufC::New(name.Length());
	    	iAttachmentName->Des().Copy(name);
    		return *iAttachmentName;
			}
		}
    // finally look if there is Content-Description.
	return iContentDescriptionPtr;
//</qmail>
	}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::FindBodyPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::FindBodyPartL(
        const TDesC& aContentType, 
        TFSMailMessagePartDataSource aDataSource)
    {
    NM_FUNCTION;

    TBuf<KMaxDataTypeLength> ptr;
    if (!iNmPrivateMessagePart->mContentType.isNull())
        {
        ptr.Copy(GetContentType());
        TInt length = ptr.Locate(';');
        if (length >= 0)
            {
            ptr.SetLength(length);
            }

        if (!ptr.CompareF(aContentType))
            {
            return this;
            }
        }
    CFSMailMessagePart* messagePart(NULL);
    if (aDataSource == EDataSourceMessageStore)
        {
        if (CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
            {
            if (iReadMessageParts)
                {
                plugin->ChildPartsL(GetMailBoxId(), GetFolderId(),
                        GetMessageId(), GetPartId(), iMessageParts);
                iReadMessageParts = EFalse;
                }
            if (iMessageParts.Count())
                {
                if (!ptr.CompareF(KFSMailContentTypeMultipartAlternative))
                    {
                    // multipart / alternative
                    for (TInt i = 0; i < iMessageParts.Count(); i++)
                        {
                        ptr.Copy(iMessageParts[i]->GetContentType());
                        TInt length = ptr.Locate(';');
                        if (length >= 0)
                            {
                            ptr.SetLength(length);
                            }
                        if (!ptr.CompareF(aContentType))
                            {
                            messagePart = iMessageParts[i];
                            // remove part from table
                            iMessageParts.Remove(i);
                            break;
                            }
						else if(!ptr.CompareF(KFSMailContentTypeMultipartRelated) ||
						   	 	!ptr.CompareF(KFSMailContentTypeMultipartMixed) ||
								!ptr.CompareF(KFSMailContentTypeMultipartAlternative) ||
								!ptr.CompareF(KFSMailContentTypeMultipartDigest) ||
								!ptr.CompareF(KFSMailContentTypeMultipartParallel))
                            {
                            // multipart, check child parts
                            messagePart = iMessageParts[i];
                            // remove part from table
                            messagePart = messagePart->FindBodyPartL(aContentType);
                            if (messagePart && messagePart->GetPartId() == iMessageParts[0]->GetPartId())
                                {
                                iMessageParts.Remove(i);
                                }
                            break;
                            }
                        }
                    }
                else
                    {
                    // all other cases
                    messagePart = iMessageParts[0];
                    // remove part from table
                    messagePart = messagePart->FindBodyPartL(aContentType);
                    if (messagePart && messagePart->GetPartId() == iMessageParts[0]->GetPartId())
                        {
                        iMessageParts.Remove(0);
                        }
                    }
                }
            }
        }
    else
        {
        const QString contentType = NmConverter::toQString(aContentType);
        int count = iNmPrivateMessagePart->mChildParts.count();
        for (int i = 0; i < count; i++)
            {
            NmMessagePart* part = iNmPrivateMessagePart->mChildParts[i]->findContentPart(contentType);
            if (part) 
                {
                messagePart = CFSMailMessagePart::NewLC(
                        NmConverter::mailMsgIdToNmId(GetMessageId()),*part);
                messagePart->SetMailBoxId(GetMailBoxId());
                CleanupStack::Pop(messagePart);
                break;
                }
            }
        }
    return messagePart;
    }
//</qmail>
	
// -----------------------------------------------------------------------------
// CFSMailMessagePart::AppendAttachmentsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::AppendAttachmentsL(
    RPointerArray<CFSMailMessagePart>& aParts)
    {
    NM_FUNCTION;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetMessageId() );
    if ( plugin )
        {
        RPointerArray<CFSMailMessagePart> messageParts;
        CleanupResetAndDestroyPushL( messageParts );
        plugin->ChildPartsL( GetMailBoxId(), GetFolderId(),
                GetMessageId(), GetPartId(), messageParts );	

        const TInt messagePartCount = messageParts.Count();
        for ( TInt i = 0; i < messagePartCount; i++ )
            {
            const TDesC& contentType = messageParts[i]->GetContentType();
            if (!contentType.CompareF(KFSMailContentTypeMultipartMixed) ||
                !contentType.CompareF(KFSMailContentTypeMultipartAlternative) ||
                !contentType.CompareF(KFSMailContentTypeMultipartDigest) ||
                !contentType.CompareF(KFSMailContentTypeMultipartRelated) ||
                !contentType.CompareF(KFSMailContentTypeMultipartParallel))
                {
                // get multipart message subparts
                messageParts[i]->AppendAttachmentsL( aParts );
                }
            else
                {
                aParts.AppendL( messageParts[i] );
                messageParts[i] = NULL;
                }
            }
        CleanupStack::PopAndDestroy( &messageParts );
        }
    }
// -----------------------------------------------------------------------------
// CFSMailMessagePart::FetchedContentSize
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailMessagePart::FetchedContentSize() const
	{
    NM_FUNCTION;
    
//<qmail>
    return (TUint)iNmPrivateMessagePart->mFetchedSize;
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetFetchedContentSize
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetFetchedContentSize(TUint aContentSize)
	{
    NM_FUNCTION;
    
//<qmail>
    iNmPrivateMessagePart->mFetchedSize = quint32(aContentSize);
//</qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::FetchLoadState
// -----------------------------------------------------------------------------
EXPORT_C TFSPartFetchState CFSMailMessagePart::FetchLoadState() const
	{
    NM_FUNCTION;
    
//<qmail>
	 if(iMessagePartsStatus != EFSDefault)
		{
		return iMessagePartsStatus;
		}
     else if (iNmPrivateMessagePart->mSize == 0)
     	{
     	return EFSFull;     	
     	}
     else if(iNmPrivateMessagePart->mFetchedSize == 0)
     	{
     	return EFSNone;
     	}
     else if (iNmPrivateMessagePart->mFetchedSize < iNmPrivateMessagePart->mSize)
     	{
     	return EFSPartial;
     	}
     else
     	{
     	return EFSFull;
     	}
//</qmail>
	}
	
// -----------------------------------------------------------------------------
// CFSMailMessagePart::FetchMessagePartL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessagePart::FetchMessagePartL( 	const TFSMailMsgId /*aMessagePartId*/,
        										  		MFSMailRequestObserver& aOperationObserver,
        												const TUint aPreferredByteCount)
    {
    NM_FUNCTION;
    
	TInt requestId(0);
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
	    RArray<TFSMailMsgId> messageList;
    	messageList.AppendL(GetPartId());
		
	    // init async request
	    TFSPendingRequest request = iRequestHandler->InitAsyncRequestL(	GetPartId().PluginId(),
	    															  	aOperationObserver);

	    requestId = request.iRequestId;
    	MFSMailRequestObserver* observer = request.iObserver;

		// fetch message parts
	    TRAPD(err,plugin->FetchMessagePartsL(	GetMailBoxId(),
	    										GetFolderId(),
	    										GetMessageId(),
	        									messageList,
	        									*observer,
	        									requestId,
	        									aPreferredByteCount ));
	    messageList.Reset();
	    if(err != KErrNone)
			{
			iRequestHandler->CompleteRequest(requestId);
			User::Leave(err);
			}
		}
	return requestId;
    }
    
// -----------------------------------------------------------------------------
// CFSMailMessagePart::FetchMessagesPartsL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessagePart::FetchMessagesPartsL( 
									const RArray<TFSMailMsgId>& aMessagePartIds,
        							MFSMailRequestObserver& aOperationObserver,
        							const TUint aPreferredByteCount)
    {
    NM_FUNCTION;

	TInt requestId(0);
    
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetPartId()))
		{
		
	    // get async request id
	    TFSPendingRequest request = 
	    iRequestHandler->InitAsyncRequestL(	GetPartId().PluginId(),
	    								 	aOperationObserver);
	    								 	
	    requestId = request.iRequestId;
    	MFSMailRequestObserver* observer = request.iObserver;
	    
	    // fetch message parts
	    TRAPD(err,plugin->FetchMessagePartsL( 	GetMailBoxId(),
	    										GetFolderId(),
	    										GetMessageId(),
	        									aMessagePartIds,
												*observer,
	        									requestId,
	        									aPreferredByteCount ));
	            
	    if(err != KErrNone)
			{
			iRequestHandler->CompleteRequest(requestId);
			User::Leave(err);
			}
		}
	else
	    {
        User::Leave(KErrArgument);
	    }
    return requestId;
    }
// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetMessagePartsStatus
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetMessagePartsStatus(TFSPartFetchState aMessagePartStatus)
	{
    NM_FUNCTION;
    
	iMessagePartsStatus = aMessagePartStatus;
	}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessagePart::AddNewAttachmentL(	const TDesC& aFilePath,
																const TFSMailMsgId /*aInsertBefore*/,
																const TDesC& aContentType )
    {
    NM_FUNCTION;
    
    CFSMailMessagePart* newPart = NULL;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetMessageId() );
    if ( plugin )
        {
        newPart = plugin->NewChildPartFromFileL( GetMailBoxId(),
            GetFolderId(),
            GetMessageId(),
            GetPartId(),
            aContentType, //contentType,
            aFilePath );
        if (newPart) 
            {
            CleanupStack::PushL( newPart );
        
            // Set attachment name
            newPart->SetAttachmentNameL( aFilePath );

            // save message part
            newPart->SaveL();
        
            CleanupStack::Pop( newPart );
            }
        }

    return newPart;
    }

// -----------------------------------------------------------------------------
// CFSMailMessagePart::ReadOnlyPartSize
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailMessagePart::ReadOnlyPartSize() const
{
    NM_FUNCTION;
    
    return iReadOnlyPartSize;
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::SetReadOnlyPartSize
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessagePart::SetReadOnlyPartSize(const TUint aReadOnlyPartSize)
{
    NM_FUNCTION;
    
    iReadOnlyPartSize = aReadOnlyPartSize;
}

// -----------------------------------------------------------------------------
// CFSMailMessagePart::DoAttachmentListL
// -----------------------------------------------------------------------------
void CFSMailMessagePart::DoAttachmentListL( RPointerArray<CFSMailMessagePart>& /*aParts*/ )
{
    NM_FUNCTION;
}
//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetNmMessagePart
// -----------------------------------------------------------------------------
EXPORT_C NmMessagePart* CFSMailMessagePart::GetNmMessagePart()
{
    NM_FUNCTION;
    
    return new NmMessagePart(iNmPrivateMessagePart);
}
//</qmail>

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessagePart::GetLocalTextContentLC
// -----------------------------------------------------------------------------
EXPORT_C HBufC* CFSMailMessagePart::GetLocalTextContentLC()
{
    NM_FUNCTION;
    
    return NmConverter::qstringToHBufCLC(iNmPrivateMessagePart->mTextContent);
}
//</qmail>
