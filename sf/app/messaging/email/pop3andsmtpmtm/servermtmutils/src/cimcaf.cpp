// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the CImCaf class.
// 
//
/**
 @file
 @internalTechnology 
*/

#include <cimcaf.h>
#include <tmsvpackednotifierrequest.h>

_LIT8(KXHyphen,"X-");
_LIT8(KContentHyphen,"Content-");

/**
Construct with callers file session handle.
c'tor
*/
EXPORT_C CImCaf::CImCaf(RFs& aFs) : iFsSession(aFs)
	{}

/**
d'tor
*/
EXPORT_C  CImCaf::~CImCaf()
	{
	delete iSupplier;
	delete iContentType;
	delete iMetaData;
	delete iImportFile;
	}
	

/**
Prior to StartProcessing() the CAF agent could provide an initial filename for the CAF session.
@param aAttachmentFileName Descriptor for writing CAF agent suggested filename.
@return KErrNone success or system error code.
@pre PrepareProcessingL() must be called prior to this call.
@panic imut 36
*/
EXPORT_C TInt CImCaf::GetSuggestedAttachmentFileName(TDes& aAttachmentFileName) const
	{
	__ASSERT_DEBUG(iImportFile != NULL, gPanic(EPanicInvalidCafState));
	return iImportFile->GetSuggestedOutputFileName(aAttachmentFileName);
	}


/**
Write a buffer of data to the CAF Agent.
@param aData Data to write to the CAF agent.
@return KErrNone success or system error code.
@pre StartProcessingL() must be called prior to this call
@post Generates 0-N receipt files under the current message attachment entry.
@panic imut 36
*/
EXPORT_C TInt CImCaf::WriteData(const TDesC8& aData)
	{
	TInt err(KErrNone);
	__ASSERT_DEBUG(iImportFile != NULL, gPanic(EPanicInvalidCafState));
	err = iImportFile->WriteData(aData);
	__ASSERT_DEBUG(err == KErrNone || err == KErrNotReady || err == KErrCANewFileHandleRequired, gPanic(EPanicInvalidCafState));
	if(err == KErrNone || err == KErrNotReady)
		{		
		// Write succeeded
		return err;
		}
	else if(err == KErrCANewFileHandleRequired)
		{
		// We need a new file handle see if the class member is set up ready
		if(iAttachmentFile.SubSessionHandle())
			{
			// Caller provided open file handle
			err = iImportFile->ContinueWithNewOutputFile(iAttachmentFile,iAttachmentFilePath);
			// Close the caller's file handle as CAF has made a copy
			iAttachmentFile.Close();
			if(err == KErrCANewFileHandleRequired)
				TRAP(err,NewFileL());
			}
		else
			{
			TRAP(err,NewFileL());
			}
		// New file handle is created, write data
		err = iImportFile->WriteData(aData);
		}
	__ASSERT_DEBUG(err == KErrNone, gPanic(EPanicInvalidCafState));
	return err;
	}

/**
Add Mime header metadata to CAF class in preparation for PrepareProcessingL()
Metadata comprises a field and data pair. For example:
aField - "X-Oma-Drm-Separate-Delivery" aData - "12"
If field does not contain "X-" or "Content-" then the method returns without
adding them to the class.
@param aField - Mime header field
@param aData - Mime field's corresponding data
@panic imut 36
*/
EXPORT_C void CImCaf::AddToMetaDataL(const TDesC8& aField, const TDesC8& aData)
	{
	if(aField.FindF(KContentHyphen) == KErrNone && aField.FindF(KXHyphen) == KErrNone)
		{		
		return;
		}
	if(!iMetaData)
		{		
		iMetaData = CMetaDataArray::NewL();
		}
	// Check we're not overwriting an existing one. Could show up a logic error.
	#if (defined _DEBUG)
	TPtrC8 des(iMetaData->SearchL(aField));
	__ASSERT_DEBUG(des.Length() == 0, gPanic(EPanicInvalidCafState));	
	#endif
	iMetaData->AddL(aField,aData);
	}

/**
Add Mime header metadata to CAF class in preparation for PrepareProcessingL()
Parses a complete unfolded mime header line to extract fields and values
Format expected:
Fieldxxx: Valuexxx;ParamField=ParamValue
If line does not contain "X-" or "Content-" then the method returns without
adding data to the class.
@param aMimeLine - Mime header data line minus CRLF
@panic imut 36
*/
EXPORT_C void CImCaf::AddToMetaDataL(const TDesC8& aMimeLine)
	{
	// Don't bother adding if it's neither of these
	// This means we won't add envelope stuff
	if(aMimeLine.FindF(KContentHyphen) == KErrNone && aMimeLine.FindF(KXHyphen) == KErrNone)
		{
		return;
		}
	if(!iMetaData)
		{		
		iMetaData = CMetaDataArray::NewL();
		}
	TInt offset;
	TPtrC8 field;
	TPtrC8 data;
	// Isolate the field and data pair separated by ':'	
	if((offset = aMimeLine.Locate(KImcvColon)) != KErrNotFound)
		{
		field.Set(aMimeLine.Left(++offset));
		// Remove leading whitespace
		while(offset < aMimeLine.Length() && aMimeLine[offset] == ' ')
			{
			offset++;
			}
		if(offset < aMimeLine.Length())
			{
			data.Set(aMimeLine.Mid(offset));
			}
		else
			return;
		}
	else
		{
		// No colon should never happen if mail is not corrupt
		__ASSERT_DEBUG(offset != KErrNotFound, gPanic(EPanicInvalidCafState));
		return;
		}
	// Check we're not overwriting an existing one. Could show up a logic error.
	#if (defined _DEBUG)
	TPtrC8 des(iMetaData->SearchL(field));
	__ASSERT_DEBUG(des.Length() == 0, gPanic(EPanicInvalidCafState));				
	#endif
	iMetaData->AddL(field,data);
	}

/**
Prepare the CImCAF class for a CAF session.
@pre RegisterL() must be called prior to this call and success checked with a Registered() call.
@panic imut 36
*/
EXPORT_C void CImCaf::PrepareProcessingL()
	{
	__ASSERT_DEBUG(iSupplier != NULL && iContentType != NULL && iMetaData != NULL && iImportFile == NULL, gPanic(EPanicInvalidCafState));
	iImportFile = iSupplier->ImportFileL(iContentType->Des(),*iMetaData);// Prepare agent for writing data.
	Deregister();
	}
	
/**
Set the CAF class variables required to Start the CAF write session.
@param aDefaultAttachmentFileName Localised default attachment name.
@param aAttachmentFilePath If CAF agent requires extra files, this is the folder/attachment entry path.
@param aServerEntry Pointer to the message store attachment entry for the CAF session.
@param aStartFile An open File handle for attachment write. Caller can close following this call.
@pre PrepareProcessingL() must be called prior to this call
@pre aServerEntry aServerEntry is set to the attachment entry id and remains set there for the duration of the CAF session
@panic imut 36
*/
EXPORT_C void CImCaf::StartProcessing(const TDesC& aDefaultAttachmentFileName,const TDesC& aAttachmentFilePath,CMsvServerEntry& aServerEntry,RFile& aStartFile)
	{
	__ASSERT_DEBUG(iSupplier != NULL && iImportFile != NULL && iContentType == NULL && iMetaData == NULL && aStartFile.SubSessionHandle() != 0, gPanic(EPanicInvalidCafState));
	iDefaultAttachmentFileName.Set(aDefaultAttachmentFileName);
	iAttachmentFilePath = aAttachmentFilePath;
	iServerEntry = &aServerEntry;
	iAttachmentFile.Duplicate(aStartFile); // Caller can close their copy
	}
	
/**
Terminate CAF session.
Check the attachments under the attachment entry and see if there are any rights receipts.
Set the mime type to a unique CAF one if there are.
@panic imut 36
@pre iServerEntry set to the attachment entry which owns the attachment
*/
EXPORT_C void CImCaf::EndProcessingL()
	{
	__ASSERT_DEBUG(iImportFile != NULL, gPanic(EPanicInvalidCafState));
	WriteDataCompleteL();
	CMsvStore* store = iServerEntry->EditStoreL(); 
	CleanupStack::PushL(store);
	// Loop through the attachments under the current attachment entry
	for(TInt i=0;i<store->AttachmentManagerL().AttachmentCount();i++)
		{
		CMsvAttachment* attachment = store->AttachmentManagerL().GetAttachmentInfoL(i);
		// Loop through the output files and see if there's a receipt that matches the attachment
		CleanupStack::PushL(attachment);
		TInt j;
		for(j=0;j<iImportFile->OutputFileCountL();j++)
			{
			CSupplierOutputFile& outputFile = iImportFile->OutputFileL(j);
			if(outputFile.OutputType() == EReceipt)
				{
				if(attachment->FilePath().MatchF(outputFile.FileName()) != KErrNotFound)
					{
					// Matched so write the Receipt indicator to the attachment and save it
					// ModifyAttachmentInfoL() takes ownership of the CMsvAttachment 
					attachment->SetMimeTypeL(KEpocMimeTypeDrm);
					store->AttachmentManagerExtensionsL().ModifyAttachmentInfoL(attachment);
					CleanupStack::Pop(attachment);
					break;
					}
				}
			}
		// If loop did not break then there was no match so destroy.
		if(j==iImportFile->OutputFileCountL())
			{
			CleanupStack::PopAndDestroy(attachment);
			}
		}
	store->CommitL();
	CleanupStack::PopAndDestroy(store);

	delete iImportFile;
	iImportFile = NULL;
	}

/**
Attempt to register a mime content-type with a CAF agent.
Success can be checked by a subsequent call to Registered().
@param aMimeLine MIME Content-Type for possible interest by a CAF agent. For example - application/vnd.oma.drm.rights+xml
@panic imut 36
*/
EXPORT_C void CImCaf::RegisterL(const TDesC8& aMimeLine)
	{
	// There's no nested CAF registration so iContentType instance should always be NULL
	__ASSERT_DEBUG(iContentType == NULL, gPanic(EPanicInvalidCafState));
	if(!iSupplier)
		{		
		iSupplier = CSupplier::NewL();
		}
	if(iSupplier->IsImportSupported(aMimeLine))
		{
		// CAF agent interested in the content-type
		// Use this buffer for storing the content-type.
		// We could extract it from the caf metadata later but this will be quicker
		iContentType = aMimeLine.AllocL();
		// REMOVE when CAF fixed
		iContentType->Des().LowerCase();
		// Construct the metadata array here but add to it later
		// Could contain stuff like "X-Oma-Drm-Separate-Delivery: 12"
		if(!iMetaData)
			{		
			iMetaData = CMetaDataArray::NewL();
			}
		}
	}

/**
Free resources allocated during CAF session
@pre RegisterL() must be called prior to this call.
@panic imut 36
*/
EXPORT_C void CImCaf::Deregister()
	{
	__ASSERT_DEBUG(Registered(), gPanic(EPanicInvalidCafState));
	// Don't delete iSupplier. This is kept open to prevent reloading of DLLs
	delete iContentType;
	iContentType = NULL;
	delete iMetaData;
	iMetaData = NULL;
	}
		
/**
Check whether RegisterL() succeeded.
@return Registered status of the CAF instance.
*/
EXPORT_C TBool CImCaf::Registered() const
	{
	return iContentType != NULL;
	}
	
/**
Retrieve processing status for the CAF session.
@return Processing status of the CAF instance
*/
EXPORT_C TBool CImCaf::Processing() const
	{
	return iImportFile != NULL;
	}

//Tell the CAF agent this that the last data for the CAF session has been written.
void CImCaf::WriteDataCompleteL()
	{
	TInt err(KErrNone);
	// Expect KErrNone or KErrCANewFileHandleRequired
	err = iImportFile->WriteDataComplete();
	__ASSERT_DEBUG(err == KErrNone || err == KErrCANewFileHandleRequired,gPanic(EPanicInvalidCafState));	
	if(err == KErrCANewFileHandleRequired)
		{
		// CAF framework requires another file handle to finish the session
		NewFileL();
		}
	else
		{
		User::LeaveIfError(err);
		}
	}

// Gets an output file for continued write by the framework.
// To be called when CAF framework has returned KErrCANewFileHandleRequired
// from a WriteData() or WriteDataComplete() call.
// Creates 1-N new CAF files under the session attachment entry
void CImCaf::NewFileL()
	{
	// Our iServerEntry will be referencing the current session attachment entry
	CMsvStore* store = iServerEntry->EditStoreL(); 
	CleanupStack::PushL(store);

	TInt err(KErrNone);
	TFileName filename;
	// Loop creating files until success or error
	do
		{
		// See if the framework can suggest a filename
		// Failing that see if it can suggest an extension
		// Use the default filename if we have to
		if((err = iImportFile->GetSuggestedOutputFileName(filename)) == KErrUnknown)
			{
			TFileName ext;
			filename = iDefaultAttachmentFileName;
			if(iImportFile->GetSuggestedOutputFileExtension(ext) == KErrNone)
				{				
				filename.Append(ext);				
				}
			}
		__ASSERT_ALWAYS(err == KErrUnknown || err == KErrNone,gPanic(EPanicInvalidCafState));
		CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
		// Ok to use stack file handle because agent clones it
		RFile file;
		store->CreateShareProtectedAttachmentL(filename,file,attachment);
		err = iImportFile->ContinueWithNewOutputFile(file,attachment->FilePath());
		__ASSERT_ALWAYS(err == KErrNone || err == KErrCANewFileHandleRequired,gPanic(EPanicInvalidCafState));
		file.Close(); // Framework clones the file handle so close it
		}while (err == KErrCANewFileHandleRequired);
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	}
