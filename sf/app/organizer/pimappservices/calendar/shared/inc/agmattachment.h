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

#ifndef __AGMATTACHMENT_H__
#define __AGMATTACHMENT_H__

#include "calattachment.h"
#include "calcommon.h"
#include "agmcontent.h"

class RReadStream;
class RWriteStream;

typedef TUint32 TCalAttachmentUid;

_LIT(KDefaultAttachmentDrive, "c:");

// used to pass data from client to server when storing a binary data attachment.
struct TAttachmentData
	{
	TCalLocalUid iLocalUid;
	TInt iAttachmentIndex;
	TDriveName iDrive;
	};

NONSHARABLE_CLASS(AttachmentFactory)
	{
public:
	IMPORT_C static CAgnAttachment* NewAttachmentL(RReadStream& aStream);
	IMPORT_C static CAgnAttachment* CloneL(const CAgnAttachment& aAttachment);
	IMPORT_C static CAgnAttachment* NewAttachmentL(CCalContent::TDisposition aType);
	};

/** 
Base class for internal representation of an attachment object.

The following data is inherited from CAgnContent:
	TDesC8* iContent;
	TDesC8* iMimeType;
	CCalContent::TDisposition iType;

@internalComponent
*/
NONSHARABLE_CLASS(CAgnAttachment) : public CAgnContent
	{
public:
	// internal flags
	enum 
		{
		/** If this is true then the CAgnAttachment is owned by a CCalAttachment */
		EOwnedByCalAttachment	= 0x01,
		/** EDataHasBeenSet is used to flag attachments created by content ID. This is set 
		to true once the data has been set (binary data or file handle). If the data is not 
		set then the attachment cannot be stored. */
		EDataHasBeenSet			= 0x02, 
		};
		
	// Destructor
	virtual ~CAgnAttachment(); 

	// Get / set data
	IMPORT_C void SetMimeTypeL(const TDesC8& aMimeType);
	
	IMPORT_C void SetAttribute(TUint16 aAttribute);
	IMPORT_C void ClearAttribute(TUint16 aAttribute);
	IMPORT_C TBool IsAttributeSet(TUint16 aAttribute) const;
	
	IMPORT_C TCalAttachmentUid Uid() const; 
	IMPORT_C void SetUid(TCalAttachmentUid aUid);
	
	IMPORT_C void SetLabelL(const TDesC& aName);
	IMPORT_C const TDesC& Label() const;
	
	IMPORT_C TInt Size() const;
	IMPORT_C void SetSize(TInt aSize);
	
	IMPORT_C void SetFlag(TUint16 aFlag);
	IMPORT_C void ClearFlag(TUint16 aFlag);
	IMPORT_C TBool FlagsSet(TUint16 aFlag) const;
	
	IMPORT_C void SetValue(TDesC8* aValue); // sets binary data or URI (NOT content ID)
	// Note value is fetched by calling CAgnContent::Content()
	
	// Used when store\restore the object to\from the file
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);
	
	// Used when an entry is copied / compared
	virtual void CopyL(const CAgnAttachment& aSource);
	virtual TBool CompareL(const CAgnAttachment& aSource) const;
	
protected:
	CAgnAttachment(CCalContent::TDisposition aType);

private:
	HBufC* iLabel;
	TCalAttachmentUid iUid;
	TUint16 iAttributes; 
	TUint16 iFlags; // internal flags
	TInt iSize;
	};

/** 
Class for internal representation of a URI attachment.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnAttachmentUri) : public CAgnAttachment
	{
public: 
	CAgnAttachmentUri();

	// from CAgnAttachment
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	};

/*
Class for internal representation of a file attachment.
Its data can be either passed in as binary data or by referencing a local file.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnAttachmentFile) : public CAgnAttachment
	{
public: 
	//Constructors and Destructors
	CAgnAttachmentFile();
	~CAgnAttachmentFile();
	
	// Get / set data
	IMPORT_C void  CopyFileHandle(const RFile& aFile);
	IMPORT_C  const RFile& FileHandle() const;
	
	IMPORT_C void SetDriveL(const TDesC& aDrive);
	IMPORT_C TDriveName Drive() const;
	
	IMPORT_C void SetFileNameL(const TDesC& aFileName);
	IMPORT_C const TDesC& FileName() const;
	
	IMPORT_C const TTime& LastModifiedTimeUtc() const;
	IMPORT_C void SetLastModifiedTimeUtc(const TTime& aTime);
	
	IMPORT_C void SetContentIdL(const TDesC8& aContentId);
	IMPORT_C const TDesC8& ContentId() const;
	IMPORT_C TBool IsFileHandleSet() const;
	IMPORT_C void SetHasFileHandle(TBool aHasFileHandel);

	//Used when an entry is copied
	void CopyL(const CAgnAttachment& aSource);
	TBool CompareL(const CAgnAttachment& aSource) const;

	// from CAgnAttachment
	void ExternalizeL(RWriteStream& aStream) const;
private:
	// from CAgnAttachment
	void InternalizeL(RReadStream& aStream);

private:
	TDesC8* iContentId; // stores the content ID. This is not persisted.
	TTime iLastModifiedTimeUtc;
	HBufC* iFileName; // including drive name
	RFile iFileHandle; // used when fetching and storing file handles. Not externalized or internalized as part of CAgnAttachmentFile.
	TBool iIsFileHandleSet;
	};

#endif // __AGMATTACHMENT_H__
