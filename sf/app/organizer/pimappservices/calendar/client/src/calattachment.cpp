// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "agmattachment.h"
#include "calclient.h"
#include "calsessionimpl.h"
#include "calcommonimpl.h"


/** Allocates and constructs a new file attachment from binary data.
@param aBinaryData The attachment binary data. CCalAttachment takes ownership.
@return A pointer to the new attachment.
@leave KErrArgument If the aBinaryData parameter is NULL or if it points to an empty descriptor.
Otherwise one of the system-wide error codes.
@pre None
@post A EFile attachment is created with the given binary data.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalAttachment::NewFileL(TDesC8* aBinaryData)
	{
	__ASSERT_ALWAYS(aBinaryData && aBinaryData->Length() > 0, User::Leave(KErrArgument));

	CCalAttachment* self = new (ELeave) CCalAttachment();
	CleanupStack::PushL(self);
	self->ConstructL(EFile, aBinaryData);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a new file attachment from a file handle.
The file will be moved to the Calendar store when the entry containing this attachment is stored. 
To avoid the file being moved to the Calendar store, a copy should be made and a file handle to 
the copy passed into this function.
@param aFileHandle The handle to the attachment file. CCalAttachment will make a copy of the handel.
The file handle must be opened using the mode EFileWrite and it should belong to a shared 
file server session. 
@see ShareProtected
@return A pointer to the new attachment.
@leave KErrAccessDenied If the file pointed to by this file handle is read only. 
Otherwise one of the system-wide error codes.
@panic If the file handle has not been initialised by calling RFs::Open.
@pre None
@post A EFile attachment is created with the given file handle.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalAttachment::NewFileL(RFile& aFileHandle)
	{
	CCalAttachment* self = new (ELeave) CCalAttachment();
	CleanupStack::PushL(self);
	self->ConstructL(EFile, NULL);
	self->FileAttachment()->SetResourceL(aFileHandle);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a new file attachment with a content ID.
Content IDs are only used during import and export. The actual attachment data is specified 
by calling SetResourceL. 
If an entry is stored which references an attachment that only has a content ID and no data, 
then the StoreL function will leave with KErrArgument.
@param aContentId A descriptor containing the content ID. CCalAttachment doesn't take ownership.
@return A pointer to the new attachment.
@leave KErrArgument if the parameter is an empty descriptor.
Otherwise any of the system-wide error codes.
@pre None
@post A EFile attachment is created with the given content ID.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalAttachment::NewFileByContentIdL(const TDesC8& aContentId)
	{
	__ASSERT_ALWAYS(aContentId.Length() > 0, User::Leave(KErrArgument));
	CCalAttachment* self = new (ELeave) CCalAttachment();
	CleanupStack::PushL(self);
	self->ConstructL(EFile, NULL);
	self->FileAttachment()->SetContentIdL(aContentId);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a new URI attachment. URIs are defined in RFC 3986.
@return A pointer to the new attachment.
@param aUri A descriptor containing the attachment URI. CCalAttachment doesn't take ownership.
@return A pointer to the new attachment.
@leave KErrArgument if the parameter is an empty descriptor.
Otherwise any of the system-wide error codes.
@pre None
@post A EUri attachment is created with the given data.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalAttachment::NewUriL(const TDesC8& aUri)
	{
	__ASSERT_ALWAYS(aUri.Length() > 0, User::Leave(KErrArgument));
	CCalAttachment* self = new (ELeave) CCalAttachment();
	CleanupStack::PushL(self);
	HBufC8* uriCopy = aUri.AllocLC();
	self->ConstructL(EUri, uriCopy);
	CleanupStack::Pop(uriCopy);
	CleanupStack::Pop(self);
	return self;
	}

CCalAttachment* CCalAttachment::NewL(CAgnAttachment& aAttachment, CCalSessionImpl& aSessionImpl)
	{
	CCalAttachment* self = new (ELeave) CCalAttachment(aAttachment);
	CleanupStack::PushL(self);
	self->CreateFileAttachmentImplIfRequiredL(&aSessionImpl);
	CleanupStack::Pop(self);
	return self;
	}

CCalAttachment::CCalAttachment()
	{
	}

CCalAttachment::CCalAttachment(CAgnAttachment& aAttachment) :
	iAttachmentImpl(&aAttachment)
	{
	}
	
void CCalAttachment::ConstructL(TType aType, TDesC8* aData)
	{
	switch (aType)
		{
		case EFile:
			iAttachmentImpl = AttachmentFactory::NewAttachmentL(CCalContent::EDispositionInline);
			iAttachmentImpl->SetAttribute(EExportInline);
			break;
		case EUri:
			iAttachmentImpl = AttachmentFactory::NewAttachmentL(CCalContent::EDispositionUrl);
			break;
		default:
			CalUtils::Panic(EInvalidAttachmentType);
		}
	
	iAttachmentImpl->SetFlag(CAgnAttachment::EOwnedByCalAttachment);
	CreateFileAttachmentImplIfRequiredL(NULL);
	
	// must set data last because ownership is taken
	if (aData)
		{
		iAttachmentImpl->SetValue(aData);
		iAttachmentImpl->SetFlag(CAgnAttachment::EDataHasBeenSet);
		}
	}
	
void CCalAttachment::CreateFileAttachmentImplIfRequiredL(CCalSessionImpl* aSessionImpl)
	{
	if (Type() == EFile && !iFileAttachment)
		{
		iFileAttachment = new (ELeave) CCalAttachmentFile(*this, aSessionImpl);
		}
	}

/** The destructor frees all resources owned by the attachment, prior to its destruction.
@pre An attachment object has been constructed.
@post The attachment object is destroyed.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment::~CCalAttachment()
	{
	if (iAttachmentImpl && iAttachmentImpl->FlagsSet(CAgnAttachment::EOwnedByCalAttachment))
		{
		delete iAttachmentImpl;
		}
	delete iFileAttachment;
	}

CAgnAttachment& CCalAttachment::Impl() const
	{
	ASSERT(iAttachmentImpl);
	return *iAttachmentImpl;
	}

/**Get the type of the attachment, CCalAttachment::TType.
@return The attachment type.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment::TType CCalAttachment::Type() const
	{
	CCalAttachment::TType calType = CCalAttachment::EUri;
	switch (iAttachmentImpl->Type())
		{
		case CCalContent::EDispositionUrl:
			calType = CCalAttachment::EUri;
			break;
		case CCalContent::EDispositionInline:
			calType = CCalAttachment::EFile;
			break;
		default:
			CalUtils::Panic(EInvalidAttachmentType);
			break;
		}
	return calType;
	}

/** Get the value of an attachment, as defined by the VALUE property in vCalendar / iCalendar (RFC 2445).
Note that content IDs are exported as part the VALUE property, but these are not accessible through this function.
@return For EUri types, this returns the URI.
For EFile types, this returns the binary data of the attachment file if present. If the binary data is not present, 
it may be loaded by calling CCalAttachmentFile::LoadBinaryDataL().
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C const TDesC8& CCalAttachment::Value() const
	{
	return iAttachmentImpl->Content();
	}

/** Set the value of the MIME type (RFC 2046).
@param aMimeType The MIME type of the attachment.
@leave KErrArgument If the MIME type is longer than KCalAttachmentMaxMimeTypeLength.
@pre None
@post The MIME type of this attachment has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachment::SetMimeTypeL(const TDesC8& aMimeType)
	{
	__ASSERT_ALWAYS(aMimeType.Length() <= KCalAttachmentMaxMimeTypeLength, User::Leave(KErrArgument));
	iAttachmentImpl->SetMimeTypeL(aMimeType);
	}

/** Get the MIME type (RFC 2046).
@return The MIME type of the attachment, or an empty descriptor if none has been set.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C const TDesC8& CCalAttachment::MimeType() const
	{
	return iAttachmentImpl->MimeType();
	}

/** Set the label for the attachment.
The label is the user-visible name for the attachment. 
It could be the filename, but does not have to be. Setting the label has no effect on the actual filename.
@param aName The attachment label.
@leave KErrArgument If the label is longer than KCalAttachmentMaxLabelLength.
@pre None
@post The attachment's label has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachment::SetLabelL(const TDesC& aName)
	{
	iAttachmentImpl->SetLabelL(aName);
	}

/** Get the label for the attachment.
@return The label for the attachment, or an empty descriptor if none has been set.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C const TDesC& CCalAttachment::Label() const
	{
	return iAttachmentImpl->Label();
	}

/** Allow access to the file-specific functions of a file attachment.
@return If this is a EFile attachment, return a pointer to CCalAttachmentFile. 
The caller does not take ownership.
Return NULL if this is a EUri attachment. 

This function can be used to test whether an attachment has file-specific data, e.g.
@code
	CCalAttachment* attachment = entry->Attachment(aIndex);
	
	CCalAttachmentFile* attachmentFile = attachment->FileAttachment();
	if (attachmentFile)
		{
		TInt attachSize = attachmentFile->Size();
		...
		}
@endcode

@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachmentFile* CCalAttachment::FileAttachment() const
	{
	return iFileAttachment;
	}

/** Sets attributes of an attachment.
@param aAttribute The attribute or attributes to be set, as defined in CCalAttachment::TAttributes.
@pre None
@post The attachment's attribute data has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachment::SetAttribute(TUint16 aAttribute)
	{
	iAttachmentImpl->SetAttribute(aAttribute);
	}

/** Clears attributes of an attachment.
@param aAttribute The attribute or attributes to be cleared, as defined in CCalAttachment::TAttributes.
@pre None
@post The attachment's attribute data has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachment::ClearAttribute(TUint16 aAttribute)
	{
	iAttachmentImpl->ClearAttribute(aAttribute);
	}

/** Tests attributes of an attachment, as defined in CCalAttachment::TAttributes.
@param aAttribute The attribute or attributes to be tested.
@return ETrue if the attachment has all these attributes. Otherwise EFalse.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TBool CCalAttachment::IsAttributeSet(TUint16 aAttribute) const
	{
	return iAttachmentImpl->IsAttributeSet(aAttribute);
	}

// CCalAttachmentFile //

CCalAttachmentFile::CCalAttachmentFile(CCalAttachment& aAttachment, CCalSessionImpl* aCalSessionImpl) 
	{
	__ASSERT_ALWAYS(aAttachment.Type() == CCalAttachment::EFile, CalUtils::Panic(EInvalidAttachmentType));
	iAttachmentImpl = static_cast<CAgnAttachmentFile*>(&aAttachment.Impl());
	
	iCalSessionImpl = aCalSessionImpl;
	if (iCalSessionImpl)
		{
		iCalSessionImpl->IncrementReferenceCount();
		}
	}

CCalAttachmentFile::~CCalAttachmentFile()
	{
	if (iCalSessionImpl)
		{
		iCalSessionImpl->DecrementReferenceCount();
		}
	}

/** Set the content ID of a file attachment. 
The content ID is only used during import / export of attachments.
@param aContentId The content ID.
@pre None
@post The attachment's content ID has been set.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachmentFile::SetContentIdL(const TDesC8& aContentId)
	{
	iAttachmentImpl->SetContentIdL(aContentId);
	}
	
/** Fetch the content ID of a file attachment.
Note that the content ID of an entry is not stored in the calendar store. Content IDs are intended to 
be used on import / export only.
@return The content ID. This will be an empty descriptor if no content ID has been set.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C const TDesC8& CCalAttachmentFile::ContentId() const
	{
	return iAttachmentImpl->ContentId();
	}

/** Set the drive on which to store a file attachment.
The default value is the same as the current calendar file. 
@param aDrive The drive on which to store a file attachment.
The first character of the string specifies the drive letter. The second character, if present, must be a semi-colon.
The following strings are all valid and will set the drive to D:
"D"
"D:"
"D:\someFolder\someFile.ext"
The drive letter can be given in either upper or lower case.
@leave KErrArgument if the parameter is an empty descriptor, or if the first character is not a drive letter (A to Z), or if the 
second character exists but is not the drive separator ':'.
Otherwise any of the system-wide error codes.
@pre None
@post The attachment's drive has been set.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachmentFile::SetDriveL(const TDesC& aDrive)
	{
	__ASSERT_ALWAYS(aDrive.Length() > 0, User::Leave(KErrArgument));
	TChar driveLetter = aDrive[0];
	__ASSERT_ALWAYS(driveLetter.IsAlpha(), User::Leave(KErrArgument));

	if (aDrive.Length() == 1)
		{
		TDriveName drive(aDrive);
		drive.Append(':');
		iAttachmentImpl->SetDriveL(drive);
		}
	else
		{
		__ASSERT_ALWAYS(aDrive[1] == ':', User::Leave(KErrArgument));
		iAttachmentImpl->SetDriveL(aDrive);	
		}
	}

/** Get the drive on which a file attachment is stored.
The default value is the same as the current calendar file.
@return The drive containing the file attachment.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TDriveName CCalAttachmentFile::Drive() const
	{
	return iAttachmentImpl->Drive();
	}

/** Get the size of the attachment in bytes. 
This is the same as the size of the attachment file.
@return The size in bytes.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TInt CCalAttachmentFile::Size() const
	{
	return iAttachmentImpl->Size();
	}

/** Set the last modified time of an attachment file.
The user is responsible for setting the last modified time.
@param aUtcTime The last modified time in UTC. 
@pre None
@post The attachment's last modified time has been set.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachmentFile::SetLastModifiedTimeUtc(const TTime& aUtcTime)
	{
	return iAttachmentImpl->SetLastModifiedTimeUtc(aUtcTime);
	}

/** Get the last modified time of a file attachment.
The user is responsible for setting the last modified time.
@return The last modified time of the attachment file in UTC.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C const TTime& CCalAttachmentFile::LastModifiedTimeUtc() const
	{
	return iAttachmentImpl->LastModifiedTimeUtc();
	}

/** Sets an attachment's data from a file handle.
@param aFileHandle The handle to the attachment file. CCalAttachmentFile will make copy of "aFileHandle"
The file handle must be opened using the mode EFileWrite and it should belong to a shared file server session. 
@see ShareProtected
@leave KErrAccessDenied If the file pointed to by this file handle is read only. 
@leave KErrArgument If this attachment's data has already been set or if the attachment was not created from a content ID.
Otherwise any of the system-wide error codes.
@panic If the file handle has not been initialised by calling RFs::Open.
@pre This attachment has been created from a content ID, and SetResourceL has not been called previously.
@post The attachment takes ownership of the given file handle.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalAttachmentFile::SetResourceL(RFile& aFileHandle)
	{
	TUint attValue = 0;
	
	// Panics if the file handle is invalid:
	User::LeaveIfError(aFileHandle.Att(attValue));
	if (attValue & KEntryAttReadOnly)
		{
		User::Leave(KErrAccessDenied);
		}
	
	if (iAttachmentImpl->FlagsSet(CAgnAttachment::EDataHasBeenSet))
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iAttachmentImpl->CopyFileHandle(aFileHandle);
		}
	}

/** Fetch the file handle for this attachment.
If a new file handle has been set but the attachment has not been stored, nothing happens.
@param aFileHandle On return, this file handle will point to the attachment file in the calendar store.
@leave KErrArgument If this attachment has not been fetched from the calendar store.
@leave KErrNotFound If the attachment has been deleted from the calendar store.
@leave KErrNotReady If the attachment file is on a drive where the media has been removed.
Otherwise any of the system-wide error codes.
@pre The calendar attachment has been fetched from the calendar store.
@post None
@publishedPartner
@released
@capability ReadUserData
*/
EXPORT_C void CCalAttachmentFile::FetchFileHandleL(RFile& aFileHandle) const
	{
    if ( iAttachmentImpl->IsFileHandleSet() )
        {
        aFileHandle.Close();
        User::LeaveIfError(
                aFileHandle.Duplicate( iAttachmentImpl->FileHandle() ) );
        }
    else if ( iCalSessionImpl )
        {
        iCalSessionImpl->Server().FetchFileHandleL(
                aFileHandle, 
                iAttachmentImpl->Uid(), 
                iCalSessionImpl->FileId() );            
        }
    else
        {
        User::Leave( KErrArgument );
        }
	}

/** Load the binary data into the attachment object to be accessed through the CCalAttachment::Value function.
Note that binary data can be very large so this may use up a lot of RAM.
If binary data has already been loaded, this function will do nothing. In that case, CCalAttachment::Value will 
return a non-empty descriptor.
@leave KErrArgument If this attachment has not been fetched from the calendar store.
@leave KErrNotFound If the attachment has been deleted from the calendar store.
@leave KErrNotReady If the attachment file is on a drive where the media has been removed.
Otherwise any of the system-wide error codes.
@pre A file attachment object has been fetched from the calendar store.
@post The attachment object owns a copy of the binary data from the calendar store.
@publishedPartner
@released
@capability ReadUserData
*/
EXPORT_C void CCalAttachmentFile::LoadBinaryDataL()
	{
	if (iAttachmentImpl->Content().Length() == 0)
		{
		// if no binary data present, try to fetch it from the file directly
		RFile fileHandle;
		FetchFileHandleL(fileHandle);
		CleanupClosePushL(fileHandle);

		TInt size;
		User::LeaveIfError(fileHandle.Size(size));
		HBufC8* data = HBufC8::NewLC(size);
		TPtr8 ptr = data->Des();
		User::LeaveIfError(fileHandle.Read(ptr));

		CleanupStack::Pop(data);
		CleanupStack::PopAndDestroy(&fileHandle); // fileHandle.Close

		// store binary data in attachment
		iAttachmentImpl->SetValue(data);
		}
	}
