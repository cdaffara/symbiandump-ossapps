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

// User includes
#include "TestUtils.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vcard3.h>
#endif

#define KBufLength 80
#define KDelim 0xa

CUtils* CUtils::NewL(RTest &aTest)
	{
	CUtils* self = new(ELeave) CUtils(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUtils::CUtils(RTest &aTest)
	: iTest(aTest)
 	{
 	}
 	
CUtils::~CUtils()
	{
	// Remove temporary directory created for unit test.
	if(iFm)
		{
		iFm->RmDir(_L("C:\\PBAPSupport\\"));
		delete iFm;
		}

	if (iSessionConnected)
		{
		iFsSession.Close();
		}
	}
	
void CUtils::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect());
	iSessionConnected = ETrue;
	iFm = CFileMan::NewL(iFsSession);
	iFsSession.MkDir(_L("C:\\PBAPSupport\\"));
	}

/** In the context of file operations, a path was not found or 
a file missing. Report the error. */
void CUtils::ReportFileErrorL(TInt aErr, const TDesC& aFileName)
	{
	if (aErr == KErrNotFound)
		{
		_LIT(KMissingFile, "\r\n** FILE NOT FOUND: %S\r\n\r\n");
		iTest.Printf(KMissingFile, &aFileName);
		}
	else if (aErr == KErrPathNotFound)
		{	// You will get this error if you have not exported the test data files.
		_LIT(KMissingPath, "\r\n** PATH NOT FOUND: %S\r\n\r\n");
		iTest.Printf(KMissingPath, &aFileName);
		}
	else
		{
		_LIT(KError, "Error %d opening file %S\r\n");
		iTest.Printf(KError, aErr, &aFileName);
		}
	__DEBUGGER();
	User::Leave(aErr);
	}

/** Externalises a Versit vCard and all of its sub-entities and properties 
to a file.*/
void CUtils::ExternaliseFileL(CVersitParser& aParser, const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, aFileName, EFileWrite));
	CleanupClosePushL(file);
	aParser.ExternalizeL(file);
	CleanupStack::PopAndDestroy(&file);
	}

/** Internalises a Versit vCard and all of its sub-entities and properties 
from a file. */	
void CUtils::InternaliseFileL(CVersitParser& aParser, const TDesC& aFileName, TBool aUseBaseParser/*=ETrue*/)
	{
	RFile file;
	TInt err = file.Open(iFsSession, aFileName, EFileRead);
	if (err != KErrNone)
		{
		ReportFileErrorL(err, aFileName);
		}
	CleanupClosePushL(file);
	TInt start = 0;
	
	// Internalizing of vCard 3.0 objects is not supported. Use base parser.
	if (aUseBaseParser)
		{
		InternalizeFileL(aParser, file);
		}
	else
		{
		aParser.InternalizeL(file, start);
		}
	CleanupStack::PopAndDestroy(&file);
	}
	
/** Internalises a Versit vCard contained in the incoming stream and parses it. */
void CUtils::InternalizeFileL(CVersitParser& aParser, RFile& aInputFile)
	{
	TInt size;
	if (aInputFile.Size(size) == KErrNone)
		{
		RFileReadStream stream(aInputFile);
		TRAPD(err, aParser.CVersitParser::InternalizeL(stream));
		static_cast<RFileBuf*>(stream.Source())->Reset();
		stream.Close();
		User::LeaveIfError(err);
		}
	}

/** Validates externalized vCard file with expected data. */
void CUtils::CheckVCardL(const TDesC& aFile, const TDesC& aOutputFile)
	{
	TFileName compareFile;
	
	ExistingFileName(compareFile, 'C', aFile);
	CompareFilesL(aOutputFile, compareFile);
	}

/** Compares two vCard files. Line comparison. */
void CUtils::CompareFilesL(const TDesC& aOutputFile, const TDesC& aCompareFile, const TDesC& aInputFile)
	{
	RFile file1,file2;	
	TInt err = file1.Open(iFsSession, aOutputFile, EFileRead);
	if (err != KErrNone)
		{
		ReportFileErrorL(err, aOutputFile);
		}
		
	CleanupClosePushL(file1);
	err = file2.Open(iFsSession, aCompareFile, EFileRead);
	if (err == KErrNotFound)// In this case, the input file and output file should be identical
		{
		err = file2.Open(iFsSession, aInputFile, EFileRead);
		if (err != KErrNone)
			{
			ReportFileErrorL(err, aInputFile);	
			}
		}
		
	// File operation error!	
	if (err != KErrNone)
		{
		ReportFileErrorL(err, aCompareFile);
		}
		
	CleanupClosePushL(file2);
	RFileReadStream stream1(file1);
	RFileReadStream stream2(file2);
	CleanupClosePushL(stream1);
	CleanupClosePushL(stream2);
	TBuf<KBufLength> bufO,bufC;
	
	TInt line = 1;
	do {
		TRAP(err, stream1.ReadL(bufO,TChar(KDelim)));
		if (err == KErrNone || err == KErrEof)
			{
			err = KErrNone;
			TRAP(err, stream2.ReadL(bufC,TChar(KDelim)));
			}
			
		if (err != KErrNone && err != KErrEof)
			{
			User::Leave(err);
			}
			
		// Compare lines in buffers.	
		iTest(!bufO.Compare(bufC));
		
		++line;
		} while (err != KErrEof);
	CleanupStack::PopAndDestroy(4, &file1);
	}

/** Create name and path of externalized vCard file. */
void CUtils::CreateFileName(TFileName& aName, TChar aPrefix, const TDesC& aFile)
	{
	_LIT(KPrefix, "C:\\PBAPSupport\\");
	_LIT(KvCard, ".vcf");
		
	aName = KPrefix;
	if (aPrefix != ' ')
		{
		aName.Append(aPrefix);
		}
		
	aName.Append(aFile);
	aName.Append(KvCard);
	}

/** Create name and path of expected vCard file. */
void CUtils::ExistingFileName(TFileName& aName, TChar aPrefix, const TDesC& aFile)
	{
	_LIT(KePrefix, "Z:\\PBAPSupport\\");
	_LIT(KvCard, ".vcf");
	
	aName = KePrefix;
	if (aPrefix != ' ')
		{
		aName.Append(aPrefix);
		}
		
	aName.Append(aFile);
	aName.Append(KvCard);
	}

/** Internalize and Externalize vCard entities from and to files respectively. */
void CUtils::IntAndExtL(const TFileName& aInputFile, const TFileName& aOutputFile
				, Versit::TVersitCharSet aCharset, Versit::TVersitEncoding aEncoding)
	{	
	CVersitParser* vParser = CreateParserLC();
	InternaliseFileL(*vParser, aInputFile);
	
	vParser->SetDefaultCharSet(aCharset);
	vParser->SetDefaultEncoding(aEncoding);
	
	ExternaliseFileL(*vParser, aOutputFile);
	
	CleanupStack::PopAndDestroy(vParser);
	}

/** Tests if externalized vCard is the same as internalized data. 
Charset: UTF8 and NoEncoding used for Externalize. */
void CUtils::TestFileL(const TDesC& aFile
				, Versit::TVersitCharSet aCharset/*=Versit::EUTF8CharSet*/, Versit::TVersitEncoding aEncoding/*=Versit::ENoEncoding*/)
	{
	TFileName inputFile,outputFile,compareFile;
	ExistingFileName(inputFile, 'I', aFile);
	CreateFileName(outputFile, 'O', aFile);
	ExistingFileName(compareFile, 'C', aFile);
	IntAndExtL(inputFile, outputFile, aCharset, aEncoding);
	CompareFilesL(outputFile, compareFile, inputFile);
	}

/** Create a vCard entity and externalize to a file.
Charset: UTF8 and NoEncoding used for Externalize. */
void CUtils::CreateAndExternaliseVCardL(const TDesC& aVCardText, const TDesC& aFileName, TFileName& aOutputFile
				, Versit::TVersitCharSet aCharset/*=Versit::EUTF8CharSet*/, Versit::TVersitEncoding aEncoding/*=Versit::ENoEncoding*/)
	{
	CVersitParser* vCardParser = CreateParserLC();
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(aVCardText);	
	CParserProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenNOTE, NULL, NULL);
	vCardParser->AddPropertyL(property); //takes ownership
	
	vCardParser->SetDefaultCharSet(aCharset);
	vCardParser->SetDefaultEncoding(aEncoding);
	
	CreateFileName(aOutputFile, 'O', aFileName);
	ExternaliseFileL(*vCardParser, aOutputFile);
	CleanupStack::PopAndDestroy(vCardParser);
	}

/** Create a vCard entity by internalize and externalize to a file.
Charset: UTF8 and NoEncoding used for Externalize. */	
void CUtils::CreateAndExternaliseVCardL(const TDesC8& aVCardText, const TDesC& aFileName, TFileName& aOutputFile
				, Versit::TVersitCharSet aCharset/*=Versit::EUTF8CharSet*/, Versit::TVersitEncoding aEncoding/*=Versit::ENoEncoding*/)
	{
	CVersitParser* vCardParser = CreateParserLC();
	RDesReadStream stream(aVCardText);
	vCardParser->CVersitParser::InternalizeL(stream);
	
	vCardParser->SetDefaultCharSet(aCharset);
	vCardParser->SetDefaultEncoding(aEncoding);
		
	CreateFileName(aOutputFile, 'O', aFileName);
	ExternaliseFileL(*vCardParser, aOutputFile);
	CleanupStack::PopAndDestroy(vCardParser);
	}

/** Create vCard 3.0 parser. */
CVersitParser* CUtils::CreateParserLC()
	{
	CVersitParser* vParser;
	vParser = CParserVCard3::NewL();
	CleanupStack::PushL(vParser);
	return vParser;
	}

