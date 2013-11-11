// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__E32STD_H__)
#include <e32std.h>
#endif

#if !defined (__MIUTATCH_H__)
#include "MIUTATCH.H"
#endif

void TImAttachmentFile::Reset()
	{
	iWritingToFile = EFalse;
	iSpaceOnCache = 0;
	iPositionInCache = 0;
	iDataLine.SetLength(0);
	}

EXPORT_C TImAttachmentFile::TImAttachmentFile(RFs& aFileSession)
				: iFileSession(aFileSession)
	{
	Reset();
	}
EXPORT_C TImAttachmentFile::~TImAttachmentFile()
	{
	iFile.Close();
	}

EXPORT_C TInt TImAttachmentFile::MakeDir (const TDesC& aDirPath)
	{
	// Create that directory
	// Nb directory name must be terminated with '\\' character
	return (iFileSession.MkDir(aDirPath));
	}

EXPORT_C TInt TImAttachmentFile::OpenFile (const TDesC& aDirPath, const TDesC& aFileName)

	{
	TInt err(AppendValidFile(aDirPath, aFileName, iFullFilePath));
	if (err!=KErrNone)
		return err;
	Reset();
	iWritingToFile = EFalse;
	return (iFile.Open(iFileSession, iFullFilePath, EFileShareExclusive|EFileStream));
	}

EXPORT_C TInt TImAttachmentFile::CreateFile (const TDesC& aDirPath, const TDesC& aFileName)
	{
	// Initialise buffer used to store attachment data, prior to writing to file
	iDataLine.FillZ();
	Reset();
	
	// Create a file for the attachment data
	// If the file already exists in this path, generate a new file name
	// with the path: "\aDirPath\aFileName(i)" where "i">0 

	TInt error=MakeDir(aDirPath);	// make directory for the attachment
	if (error!=KErrAlreadyExists && error!=KErrNone)
		return error;
	
	error=AppendValidFile(aDirPath, aFileName, iFullFilePath);
	if (error!=KErrNone)
		return error;

	error=iFile.Create(iFileSession, iFullFilePath, EFileWrite|EFileShareExclusive|EFileStream);

	if (error==KErrNone)
		{
		iWritingToFile = ETrue;
		return error;
		}
	if (error!=KErrAlreadyExists)
		return error;
			
	TParsePtr filenamePPtr(iFullFilePath);
	TFileName newFileName;

	TInt index=0;
	do 
		{
		TPtrC filename=filenamePPtr.Name();
		TPtrC extension=filenamePPtr.Ext();
		iFullFilePath.Format(KImcvAttachFormatStr,&filename,++index,&extension);
		error=AppendValidFile(aDirPath, iFullFilePath, newFileName);
		if (error!=KErrNone)
			error=iFile.Create(iFileSession, newFileName, EFileWrite|EFileShareExclusive|EFileStream);
		}
	while (error==KErrAlreadyExists);
	return error;
	}

EXPORT_C TInt TImAttachmentFile::ReadFile (TDes8& rBuffer,TInt aLength)
	{
	// Only read from file once in a while, put into an intermediate buffer.
	// Pass out data from buffer. Copy aLength of data from iDataLine to rBuffer

	TInt err = 0;			// Error returned by file read.
	TInt bufLength = aLength;	// Length yet to be copied.  

	// Is there anything in the buffer?
	TInt dataLineLength = iDataLine.Length();
	if (dataLineLength==0)
		{
		// Is length<buffer size?
		if (bufLength<KDataLineBufferLength)
			{
			// Fill buffer
			err = iFile.Read(iDataLine, KDataLineBufferLength);
			dataLineLength = iDataLine.Length();

			// Inititialise
			iPositionInCache = 0;
			iSpaceOnCache = dataLineLength;

			// Read 'length' bytes from the data
			rBuffer = SelectFileData(bufLength, iSpaceOnCache);
			bufLength = 0;
			}
		else
			{
			// Don't use intermediate buffer
			// Read 'length' bytes from file to rBuffer
			err = iFile.Read(rBuffer,bufLength);
			bufLength = 0;
			}
		}
	else
		// Is 'length'<data available in buffer
		if (bufLength<iSpaceOnCache)
			{
			rBuffer = SelectFileData(bufLength, dataLineLength);
			bufLength = 0;
			}
		else
			{
			// Read length bytes from the buffer to rBuffer
			rBuffer = SelectFileData(iSpaceOnCache, dataLineLength);
			bufLength -= rBuffer.Length();

			if (bufLength>0)
				{
				if (bufLength<KDataLineBufferLength)
					{
					// Fill buffer
					err = iFile.Read(iDataLine, KDataLineBufferLength);
					dataLineLength = iDataLine.Length();

					// Inititialise
					iPositionInCache = 0;
					iSpaceOnCache = dataLineLength;

					// Read 'length' bytes from the data
					rBuffer.Append(SelectFileData(bufLength, dataLineLength));
					}
				else
					{
					// read in remaining data to the rBuffer
					TInt bufiPositionInCache = aLength-bufLength;
					rBuffer.SetLength(bufiPositionInCache+1); // need this because otherwise get a panic!
					TPtr8 ptr(&rBuffer[bufiPositionInCache], bufLength);
					err = iFile.Read(ptr);
					TInt ptrLength = ptr.Length();
					rBuffer.SetLength(bufiPositionInCache+ptrLength);	// set the original length
					bufLength = 0;
					}
				}
			}
	return err;
	}


TPtrC8 TImAttachmentFile::SelectFileData(TInt& aBufLen, TInt& aDataLen)
	{
	// Read length bytes from the buffer to rBuffer
	TInt size = aBufLen<aDataLen ? aBufLen : aDataLen;

	TPtrC8 rBuf = iDataLine.Mid(iPositionInCache,size);
	iPositionInCache += size;
	iSpaceOnCache -= size;
	return rBuf;
	}


TInt TImAttachmentFile::ReadFile ()
	{
	return iFile.Read(iDataLine);
	}
	
EXPORT_C void TImAttachmentFile::SetFileHandle(RFile& aFile,TImFileOpenMode aFileMode)	
	{
	if(aFileMode == EImFileRead)
		iWritingToFile = EFalse;
	else if(aFileMode == EImFileWrite)
		iWritingToFile = ETrue;
	
	iFile = aFile;
	}

EXPORT_C TInt TImAttachmentFile::WriteFile (const TDesC8& aBuffer)
	{
	// Write data from aBuffer to file using an intermediate buffer, iDataLine. 
	// This is to reduce the number of file writes. 'Large' files can be written 
	// directly to the file.
	
	TInt err(KErrNone);

	TInt atchDataSize = aBuffer.Length();
	TInt bufferSize = iDataLine.Length();
	TInt spaceOnBuffer = 0;
	TInt bufPos = 0;
	// Fill buffer
	spaceOnBuffer = KDataLineBufferLength - bufferSize;

	// if incoming data is bigger than buffer 
	// we can write it directly to file if packing is not done earier
	if (atchDataSize >= KDataLineBufferLength && spaceOnBuffer == KDataLineBufferLength)
		{
		err=iFile.Write(aBuffer.Mid(bufPos));
		atchDataSize=0;
		return err;
		}

	if (atchDataSize > spaceOnBuffer)
		{
		// The remaining size of the atch is greater than the buffer
		// Append as much data from the atch than will fit in the buffer
		iDataLine.Append(aBuffer.Mid(bufPos,spaceOnBuffer));

		bufPos += spaceOnBuffer;
		atchDataSize -= spaceOnBuffer;

		// Flush buffer to file
		err = WriteFile();
		iDataLine.SetLength(0);
		bufferSize = 0;
		
		// if the size of the attachment is 'large' then write the whole file
		if (atchDataSize >= KDataLineBufferLength)
			{
			err=iFile.Write(aBuffer.Mid(bufPos));
			atchDataSize=0;
			}
		else 
			{
			iDataLine.Append(aBuffer.Mid(bufPos,atchDataSize));
			bufPos += atchDataSize;
			atchDataSize -= atchDataSize;	// atchDataSize should be 0!
			}
		}
	else 
		{
		iDataLine.Append(aBuffer.Mid(bufPos,atchDataSize));
		bufPos += atchDataSize;
		atchDataSize -= atchDataSize;	// atchDataSize should be 0!
		}
	
	return err;
	}

EXPORT_C TInt TImAttachmentFile::WriteFile (const TDesC16& aBuffer)
	{
	// Write data from aBuffer to file using an intermediate buffer, iDataLine. 
	// This is to reduce the number of file writes. 'Large' files can be written 
	// directly to the file.
	
	iFileText.Set(iFile);
	TInt err (KErrNone);

	TInt atchDataSize = aBuffer.Length();
	TInt bufferSize = iDataLine.Length();
	TInt spaceOnBuffer = 0;
	TInt bufPos = 0;
	// Fill buffer
	spaceOnBuffer = KDataLineBufferLength - bufferSize;

	// if incoming data is bigger than buffer 
	// we can write it directly to file if packing is not done earier
	if (atchDataSize >= KDataLineBufferLength && spaceOnBuffer == KDataLineBufferLength)
		{
		err=iFileText.Write(aBuffer.Mid(bufPos));
		atchDataSize=0;
		return err;
		}

	if (atchDataSize > spaceOnBuffer)
		{
		// The remaining size of the atch is greater than the buffer
		// Append as much data from the atch than will fit in the buffer
		iDataLine.Append(aBuffer.Mid(bufPos,spaceOnBuffer));

		bufPos += spaceOnBuffer;
		atchDataSize -= spaceOnBuffer;

		// Flush buffer to file
		err = WriteFile();
		iDataLine.SetLength(0);
		bufferSize = 0;
		
		// if the size of the attachment is 'large' then write the whole file
		if (atchDataSize >= KDataLineBufferLength)
			{
			err=iFileText.Write(aBuffer.Mid(bufPos));
			atchDataSize=0;
			}
		else 
			{
			iDataLine.Append(aBuffer.Mid(bufPos,atchDataSize));
			bufPos += atchDataSize;
			atchDataSize -= atchDataSize;	// atchDataSize should be 0!
			}
		}
	else 
		{
		iDataLine.Append(aBuffer.Mid(bufPos,atchDataSize));
		bufPos += atchDataSize;
		atchDataSize -= atchDataSize;	// atchDataSize should be 0!
		}
	
	return err;
	}


TInt TImAttachmentFile::WriteFile ()
	{
	return iFile.Write(iDataLine);
	}

EXPORT_C TInt TImAttachmentFile::CloseFile()
	{
	// Check if writing to a file, in which case make sure iDataLine buffer..
	// has been flushed.
	TInt err = KErrNone;
	if (iWritingToFile)
		err = FlushFile();
	Reset();
	iFile.Close();
	return err;
	}

TInt TImAttachmentFile::FlushFile()
	{
	if (iDataLine.Length() > 0)
		{
		WriteFile();
		}
	return iFile.Flush();	
	}

EXPORT_C TInt TImAttachmentFile::DeleteAttachment(const TDesC& aDirPath, const TDesC& aFileName)
	{
	TInt err(AppendValidFile(aDirPath, aFileName, iFullFilePath));
	if (err!=KErrNone)
		return err;
	return (iFileSession.Delete(iFullFilePath));
	}

TInt TImAttachmentFile::AppendValidFile(const TDesC& aDirPath, const TDesC& aFileName, TFileName& rFullFilePath)
	{	
	// if the resultant filename is too long, report it,
	// trimming the filename is not an option, apparently.

	rFullFilePath=aDirPath;
	if (aDirPath.Length() + aFileName.Length() > KMaxFileName)
		{
		return KErrBadName;
		}
	else
		{
		rFullFilePath.Append(aFileName);
		}
	return KErrNone;	
	}

