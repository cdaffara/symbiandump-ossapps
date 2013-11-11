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

#if !defined (__MIUTATCH_H__)
#define __MIUTATCH_H__


#if !defined (__S32FILE_H__)
#include <s32file.h>
#endif

const TInt KDataLineBufferLength = 4096;
_LIT(KImcvAttachFormatStr, "%S(%d)%S");

/**
@internalTechnology
@released
*/
enum TImAttachmentFileState
	{ 
	EFileNotOpen, 
	EFileIsOpen, 
	EFileIsClosed,		
	EFileIsCorrupt,		// UU data being decoded is corrupt
	EFileIsIncomplete,	// file write failed
	EFileFailedToOpen,	// can't open attach file
	EFileTopIncomplete	// File is incomplete due to top
	};

class TImAttachmentFile		// utility class to handle file operations with Email attachments
/**
@internalAll
@released
*/
    {
public:
 	enum TImFileOpenMode
		{
		/**
		File opened for read mode
		*/
		EImFileRead = 0,
		/**
		File opened for write mode
		*/
		EImFileWrite,
		};   
public:
    IMPORT_C TImAttachmentFile(RFs& aFileSession);  // constructor
    IMPORT_C ~TImAttachmentFile();  // destructor

    IMPORT_C TInt MakeDir (const TDesC& aDirPath);
    IMPORT_C TInt OpenFile (const TDesC& aDirPath ,const TDesC& aFileName );
    IMPORT_C TInt CreateFile (const TDesC& aDirPath ,const TDesC& aFileName );
    IMPORT_C TInt ReadFile (TDes8& rBuffer, TInt aNumBytes );
    IMPORT_C TInt WriteFile (const TDesC8& aBuffer);
    IMPORT_C TInt WriteFile (const TDesC16& aBuffer);
    IMPORT_C TInt CloseFile();
	IMPORT_C TInt DeleteAttachment(const TDesC& aDirPath,const TDesC& aFileName);
	
	IMPORT_C void SetFileHandle(RFile& aFile,TImFileOpenMode aFileMode);
						
	TInt AppendValidFile(const TDesC& aDirPath, const TDesC& aFileName, TFileName& rFullFilePath);
private:
	void Reset();
	TInt WriteFile ();
    TInt ReadFile ();
	TInt FlushFile();
	TPtrC8 SelectFileData(TInt& abufLen, TInt& aDataLen);

private:
    RFs&		    iFileSession;
	RFile			iFile;
	TFileText		iFileText;
	TFileName		iFullFilePath;
	TBuf8<KDataLineBufferLength>		iDataLine;
	TBool			iWritingToFile;
	TInt			iSpaceOnCache;
	TInt			iPositionInCache;
    };

#endif
