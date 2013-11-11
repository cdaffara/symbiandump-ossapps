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

#ifndef __CALATTACHMENT_H__
#define __CALATTACHMENT_H__

#include <f32file.h>

class CAgnAttachment;
class CAgnAttachmentFile;
class CCalAttachmentFile;
class CCalSessionImpl;

/** The maximum length of a CCalAttachment label. 
This sets a limit on the descriptor passed into CCalAttachment::SetLabelL().
@publishedPartner
@released
*/
const TInt KCalAttachmentMaxLabelLength = 256;

/** The maximum length of a CCalAttachment MIME type. 
This sets a limit on the descriptor passed into CCalAttachment::SetMimeTypeL().
@publishedPartner
@released
*/
const TInt KCalAttachmentMaxMimeTypeLength = 256;

/**
Class representing a calendar attachment. This allows access to the attachment content itself and associated
metadata.

This can be either a URI (RFC 3986) or a file attachment, which must be specified on creation.
A URI attachment requires a descriptor containing the URI.
A file attachment can be specified as either a descriptor containing the binary data or as a file handle.

Attachment data (i.e. URI, binary data) cannot be changed once an attachment has been created. 
This does not include metadata properties that are modifiable through the CCalAttachment APIs.

File attachments may also have only a content ID on creation. This applies in cases where a vCalendar or 
iCalendar (RFC 2445) is imported as part of a message. The content ID refers to the attachment file 
located elsewhere in the message. The attachment data must be set using CCalAttachment::SetResourceL 
before the attachment can be stored. 
The content ID can be changed once it has been set.
There is more information about content IDs in the MIME specification (RFC2045).

@publishedPartner
@released
*/
NONSHARABLE_CLASS(CCalAttachment) : public CBase
	{
public:
	/** Calendar attachment type.
	@publishedPartner
	@released
	*/
	enum TType
		{
		/** A URI or link to an attachment, e.g. a file or HTTP server address. */
		EUri,
		/** A file attachment to be stored in the calendar, either binary data or a file handle. */
		EFile,
		};
	
	/** Calendar attachment attributes.
	Note that the first 8 bits of this enumeration (up to 0x0080) are reserved for future use.
	@publishedPartner
	@released
	*/
	enum TAttributes
		{
		/** If the EExportInline flag is set on a file attachment then it will be exported 
			inline as binary data.
			
			Note that, by default:
			
			- Any file attachments created using binary data will have the EExportInline flag set. 
			
			- Any file attachments created using a file handle will NOT have the EExportInline flag set,
			i.e the content of the file will not be exported inline as binaries unless the client call
			SetAttribute(EExportInline) after an attachment object has been instantiated.

			- If this flag is set but the attachment has a content ID, then the content ID will be exported instead.
			
			- This flag has no effect for URI attachments.
			
			@see EFile
			@see EUri
			*/
		EExportInline = 0x0001,
		};
		
	IMPORT_C static CCalAttachment* NewFileL(TDesC8* aBinaryData); // takes ownership
	IMPORT_C static CCalAttachment* NewFileL(RFile& aFileHandle); // takes ownership
	IMPORT_C static CCalAttachment* NewFileByContentIdL(const TDesC8& aContentId);
	IMPORT_C static CCalAttachment* NewUriL(const TDesC8& aUri);
	
	IMPORT_C ~CCalAttachment();

	IMPORT_C TType Type() const;
	IMPORT_C const TDesC8& Value() const; 

	IMPORT_C void SetMimeTypeL(const TDesC8& aContentMimeType);
	IMPORT_C const TDesC8& MimeType() const; 

	IMPORT_C void SetLabelL(const TDesC& aLabel); 
	IMPORT_C const TDesC& Label() const; 

	IMPORT_C void SetAttribute(TUint16 aAttribute); // uses values from TAttributes
	IMPORT_C void ClearAttribute(TUint16 aAttribute); // uses values from TAttributes
	IMPORT_C TBool IsAttributeSet(TUint16 aAttribute) const; // uses values from TAttributes

	IMPORT_C CCalAttachmentFile* FileAttachment() const;
	
	static CCalAttachment* NewL(CAgnAttachment& aAttachment, CCalSessionImpl& aSessionImpl);
	CAgnAttachment& Impl() const;

private:
	CCalAttachment();
	CCalAttachment(CAgnAttachment& aAttachment);
	void ConstructL(TType aType, TDesC8* aData);
	void CreateFileAttachmentImplIfRequiredL(CCalSessionImpl* aSessionImpl);

private:
	CAgnAttachment*		iAttachmentImpl;
	CCalAttachmentFile*	iFileAttachment;
	};

/**
Class providing an interface to access the file attachment functionality of a CCalAttachment object. 

This can be accessed using the CCalAttachment::FileAttachment function on any attachment of 
type CCalAttachment::EFile.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(CCalAttachmentFile) : public CBase
	{
public:
	IMPORT_C void SetContentIdL(const TDesC8& aContentId);
	IMPORT_C const TDesC8& ContentId() const;
	
	IMPORT_C void SetDriveL(const TDesC& aDrive); 
	IMPORT_C TDriveName Drive() const; 

	IMPORT_C TInt Size() const;
	
	IMPORT_C void SetLastModifiedTimeUtc(const TTime& aUtcTime); 
	IMPORT_C const TTime& LastModifiedTimeUtc() const; 

	IMPORT_C void SetResourceL(RFile& aFileHandle);
	IMPORT_C void FetchFileHandleL(RFile& aFileHandle) const;
	IMPORT_C void LoadBinaryDataL();

	CCalAttachmentFile(CCalAttachment& aFile, CCalSessionImpl* aCalSessionImpl);
	~CCalAttachmentFile();
	
private:
	CAgnAttachmentFile* iAttachmentImpl; // not owned
	CCalSessionImpl* iCalSessionImpl; // not owned
	};
	
#endif // __CALATTACHMENT_H__
