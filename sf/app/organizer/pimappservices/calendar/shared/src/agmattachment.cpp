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
#include "agmpanic.h"
#include "agmcontent.h"
#include <s32stor.h>

// AttachmentFactory //

/** Create a new attachment from a stream.
*/
EXPORT_C CAgnAttachment* AttachmentFactory::NewAttachmentL(RReadStream& aStream)
	{
	CCalAttachment::TType type = static_cast<CCalAttachment::TType>(aStream.ReadUint8L());
	CAgnAttachment* attachment = NULL;
	if (type == CCalAttachment::EUri)
		{
		attachment = new (ELeave) CAgnAttachmentUri();
		}
	else if (type == CCalAttachment::EFile)
		{
		attachment = new (ELeave) CAgnAttachmentFile();
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PushL(attachment);
	attachment->InternalizeL(aStream);
	CleanupStack::Pop(attachment);		
	return attachment;
	}

/** Create a new attachment by copying an existing attachment.
*/
EXPORT_C CAgnAttachment* AttachmentFactory::CloneL(const CAgnAttachment& aOriginal)
	{
	CCalContent::TDisposition type = aOriginal.Type();
	CAgnAttachment* newAttachment = NULL;
	if (type == CCalContent::EDispositionUrl)
		{
		newAttachment = new (ELeave) CAgnAttachmentUri();
		}
	else if (type == CCalContent::EDispositionInline)
		{
		newAttachment = new (ELeave) CAgnAttachmentFile();
		}
	else
		{
		Panic(EAgmErrBadAttachmentType);
		}
		
	CleanupStack::PushL(newAttachment);
	newAttachment->CopyL(aOriginal);
	CleanupStack::Pop(newAttachment);
	return newAttachment;
	}

/** Create a new attachment by specifying its type only.
*/
EXPORT_C CAgnAttachment* AttachmentFactory::NewAttachmentL(CCalContent::TDisposition aType)
	{
	CAgnAttachment* newAttachment = NULL;
	if (aType == CCalContent::EDispositionUrl)
		{
		newAttachment = new (ELeave) CAgnAttachmentUri();
		}
	else if (aType == CCalContent::EDispositionInline)
		{
		newAttachment = new (ELeave) CAgnAttachmentFile();
		}
	else
		{
		Panic(EAgmErrBadAttachmentType);
		}
	return newAttachment;
	}

// CAgnAttachment //

//Constructors and Destructors
CAgnAttachment::CAgnAttachment(CCalContent::TDisposition aType) :
	CAgnContent(aType)
	{
	}

EXPORT_C CAgnAttachment::~CAgnAttachment()
	{
	delete iLabel;
	}

/** Set the value of the attachment.
For a CCalContent::EDispositionUrl attachment, this is the URI.
For a CCalContent::EDispositionInline attachment, this is the binary data.
*/
EXPORT_C void CAgnAttachment::SetValue(TDesC8* aContent)
	{
	SetContent(aContent);
	if (aContent)
		{
		iSize = aContent->Length();
		}
	else
		{
		iSize = 0;	
		}
	}

EXPORT_C void CAgnAttachment::SetMimeTypeL(const TDesC8& aMimeType)
	{
	SetMimeType(aMimeType.AllocL());
	}
	
/** Attributes are defined by CCalAttachment::TAttributes
*/
EXPORT_C void CAgnAttachment::SetAttribute(TUint16 aAttribute)
	{
	iAttributes |= aAttribute;
	}

EXPORT_C void CAgnAttachment::ClearAttribute(TUint16 aAttribute)
	{
	iAttributes &= ~aAttribute;
	}

EXPORT_C TBool CAgnAttachment::IsAttributeSet(TUint16 aAttribute) const
	{
	return (iAttributes & aAttribute);
	}

/** Set / get the attachment label.
*/
EXPORT_C void CAgnAttachment::SetLabelL(const TDesC& aLabel)
	{
	__ASSERT_ALWAYS(aLabel.Length() <= KCalAttachmentMaxLabelLength, User::Leave(KErrArgument));
	delete iLabel;
	iLabel = NULL;
	iLabel = aLabel.AllocL();
	}
 
EXPORT_C const TDesC& CAgnAttachment::Label() const
	{
	if (iLabel)
		{
		return *iLabel;
		}
	return KNullDesC();
	}

/** Set / get the attachment UID. This UID is internal and used to uniquely identify an attachment in a file (using the attachment index).
*/
EXPORT_C TCalAttachmentUid CAgnAttachment::Uid() const
	{
	return iUid;
	}

EXPORT_C void CAgnAttachment::SetUid(TCalAttachmentUid aUid)
	{
	iUid = aUid;
	}
	
void CAgnAttachment::InternalizeL(RReadStream& aStream)
	{
	// don't read in type as assume already read
	HBufC8* mimeType = HBufC8::NewL(aStream, KCalAttachmentMaxMimeTypeLength);
	SetMimeType(mimeType);
	iUid = aStream.ReadUint32L();
	iAttributes = static_cast<CCalAttachment::TAttributes>(aStream.ReadUint16L());
	iFlags = aStream.ReadUint16L();
	iSize = aStream.ReadInt32L();
	delete iLabel;
	iLabel = NULL;
	iLabel = HBufC::NewL(aStream, KCalAttachmentMaxLabelLength);
	}

void CAgnAttachment::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(Type());
	
	aStream << MimeType();
	aStream.WriteUint32L(iUid);
	aStream.WriteUint16L(iAttributes);
	aStream.WriteUint16L(iFlags);
	aStream.WriteInt32L(iSize);
	aStream << Label();
	}

// Used when an entry is copied
void CAgnAttachment::CopyL(const CAgnAttachment& aSource)
	{
	__ASSERT_ALWAYS(Type() == aSource.Type(), Panic(EAgmErrBadAttachmentType));
	
	HBufC8* content = NULL;
	if (aSource.Content().Length())
		{
		content = aSource.Content().AllocL();
		}
	SetContent(content);

	HBufC8* mimeType = NULL;
	if (aSource.MimeType().Length())
		{
		mimeType = aSource.MimeType().AllocL();
		}
	SetMimeType(mimeType);

	SetUid(aSource.Uid());
	iAttributes = aSource.iAttributes;
	iFlags = aSource.iFlags;
	SetSize(aSource.Size());
	SetLabelL(aSource.Label());
	}

// return ETrue if the attachment passed in is the same
TBool CAgnAttachment::CompareL(const CAgnAttachment& aSource) const
	{
	if (Type() != aSource.Type())
		{
		return EFalse;
		}
	
	if (MimeType() != aSource.MimeType())
		{
		return EFalse;
		}
	
	if (Content() != aSource.Content())
		{
		return EFalse;
		}
	
	if (iAttributes != aSource.iAttributes)
		{
		return EFalse;
		}
	
	if (Label() != aSource.Label())
		{
		return EFalse;
		}
		
	if (Size() != aSource.Size())
		{
		return EFalse;
		}
	// Only properties exposed to clients are compared, i.e. not UIDs.
	
	return ETrue;
	}

/** Set / get the attachment flags. Unlike the attributes, these flags are internal and not exposed to the client.
*/
EXPORT_C void CAgnAttachment::SetFlag(TUint16 aFlag)
	{
	iFlags |= aFlag;
	}

EXPORT_C void CAgnAttachment::ClearFlag(TUint16 aFlag)
	{
	iFlags &= ~aFlag;
	}

EXPORT_C TBool CAgnAttachment::FlagsSet(TUint16 aFlag) const
	{
	return (iFlags & aFlag);
	}

/** Set / get the attachment size. 
For file attachments, this effectively caches it for the client.
*/
EXPORT_C TInt CAgnAttachment::Size() const
	{
	return iSize;
	}

EXPORT_C void CAgnAttachment::SetSize(TInt aSize)
	{
	iSize = aSize;
	}

// CAgnAttachmentUri //
	
CAgnAttachmentUri::CAgnAttachmentUri() : 
	CAgnAttachment(CCalContent::EDispositionUrl)
	{
	}

void CAgnAttachmentUri::ExternalizeL(RWriteStream& aStream) const
	{
	CAgnAttachment::ExternalizeL(aStream);
	aStream << Content();
	}

void CAgnAttachmentUri::InternalizeL(RReadStream& aStream)
	{
	CAgnAttachment::InternalizeL(aStream);
	HBufC8* content = HBufC8::NewL(aStream, KMaxTInt);
	SetContent(content);
	}

// CAgnAttachmentFile //

//Constructors and Destructors 
CAgnAttachmentFile::CAgnAttachmentFile() : 
	CAgnAttachment(CCalContent::EDispositionInline),
	iLastModifiedTimeUtc(Time::NullTTime())
	{
	}

EXPORT_C CAgnAttachmentFile::~CAgnAttachmentFile()
	{
	delete iContentId;
	delete iFileName;
	
	iFileHandle.Close();
	}
	
EXPORT_C TBool CAgnAttachmentFile::IsFileHandleSet() const
	{
	return iIsFileHandleSet;	
	}
	
/** Set the file handle (should be called on client side only).
*/
EXPORT_C void CAgnAttachmentFile::CopyFileHandle(const RFile& aFile)
	{
	iFileHandle.Close();
	User::LeaveIfError(iFileHandle.Duplicate(aFile));
	iIsFileHandleSet = ETrue;
	// any attachment with a file handle will not be exported inline by default
	ClearAttribute(CCalAttachment::EExportInline);
	SetFlag(EDataHasBeenSet);
	TInt size;
	aFile.Size(size);
	SetSize(size);
	}
/** Get the file handle (should be called on client side only).
*/
EXPORT_C  const RFile& CAgnAttachmentFile::FileHandle() const 
	{
	return iFileHandle;
	}

/** Set the drive on which to store the attachment
*/
EXPORT_C void CAgnAttachmentFile::SetDriveL(const TDesC& aDrive)
	{
	if ( ! iFileName || iFileName->Length() <= KMaxDriveName)
		{
		SetFileNameL(aDrive.Left(KMaxDriveName));
		}
	else
		{
		if (iFileName->Left(1) != aDrive.Left(1))
			{
			TPtr ptr(iFileName->Des());
			ptr.Replace(0, aDrive.Length(), aDrive);
			}
		}
	}

/** Set the name of the file where the attachment is stored internally. 
This is not exposed to clients.
*/
EXPORT_C void CAgnAttachmentFile::SetFileNameL(const TDesC& aFileName)
	{
	delete iFileName;
	iFileName = NULL;
	iFileName = aFileName.AllocL();
	}

/** returns the attachment's file name 
  */
EXPORT_C const TDesC& CAgnAttachmentFile::FileName() const
	{
	if (iFileName)
		{
		return *iFileName;
		}
	return KNullDesC();
	}

EXPORT_C TDriveName CAgnAttachmentFile::Drive() const
	{
	if (iFileName && iFileName->Length() >= KMaxDriveName)
		{
		return iFileName->Left(KMaxDriveName);
		}
	return KDefaultAttachmentDrive();
	}

/** Last modified date of an attachment is separate data from the entry's last modified time.
It is set by the client and may not match the actual file attachment's modified time.
*/
EXPORT_C const TTime& CAgnAttachmentFile::LastModifiedTimeUtc() const
	{
	return iLastModifiedTimeUtc;
	}

EXPORT_C void CAgnAttachmentFile::SetLastModifiedTimeUtc(const TTime& aTime)
	{
	iLastModifiedTimeUtc = aTime;
	}

/** The content ID should only be set by a client on import or export.
*/
EXPORT_C void CAgnAttachmentFile::SetContentIdL(const TDesC8& aContentId)
	{
	delete iContentId;
	iContentId = NULL;
	iContentId = aContentId.AllocL();
	}

EXPORT_C const TDesC8& CAgnAttachmentFile::ContentId() const
	{
	if (iContentId)
		{
		return *iContentId;
		}
	return KNullDesC8();
	}

/** Used when store\restore the object to\from the file
*/
EXPORT_C void CAgnAttachmentFile::InternalizeL(RReadStream& aStream)
	{
	CAgnAttachment::InternalizeL(aStream);
	TInt64 lastModifiedUtc;
	aStream >> lastModifiedUtc;
	iLastModifiedTimeUtc = lastModifiedUtc;
	iFileName = HBufC::NewL(aStream, KMaxFileName);
	}

EXPORT_C void CAgnAttachmentFile::ExternalizeL(RWriteStream& aStream) const
	{
	CAgnAttachment::ExternalizeL(aStream);
	aStream << iLastModifiedTimeUtc.Int64();
	aStream << FileName();
	}
	
EXPORT_C void CAgnAttachmentFile::SetHasFileHandle(TBool aHasFileHandel)
	{
	iIsFileHandleSet = aHasFileHandel;
	}
	
//Used when an entry is copied
void CAgnAttachmentFile::CopyL(const CAgnAttachment& aSource)
	{
	CAgnAttachment::CopyL(aSource);

	const CAgnAttachmentFile& KFileSource = static_cast<const CAgnAttachmentFile&>(aSource);
		
	delete iContentId;
	iContentId = NULL;
	if (KFileSource.ContentId().Length())
		{
		iContentId = KFileSource.ContentId().AllocL();
		}
	SetLastModifiedTimeUtc(KFileSource.LastModifiedTimeUtc());
	SetFileNameL(KFileSource.FileName());
	
	if (KFileSource.IsFileHandleSet())
		{
		CopyFileHandle(KFileSource.FileHandle());	
		}
	}

TBool CAgnAttachmentFile::CompareL(const CAgnAttachment& aSource) const
	{
	if (!CAgnAttachment::CompareL(aSource))
		{
		return EFalse;
		}
	
	// CAgnAttachment::CompareL already compared disposition, so both are file attachments
	const CAgnAttachmentFile& KFileSource = static_cast<const CAgnAttachmentFile&>(aSource);
	
	if (ContentId() != KFileSource.ContentId())
		{
		return EFalse;
		}

	if (LastModifiedTimeUtc() != KFileSource.LastModifiedTimeUtc())
		{
		return EFalse;
		}
	
	TDriveName drv1 = Drive();
	TDriveName drv2 = KFileSource.Drive();
	drv1.UpperCase();
	drv2.UpperCase();
	if (drv1 != drv2)
		{
		return EFalse;
		}

	return ETrue;
	}
