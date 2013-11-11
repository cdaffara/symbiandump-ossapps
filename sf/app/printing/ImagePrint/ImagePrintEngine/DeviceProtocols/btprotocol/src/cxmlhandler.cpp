/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler for XML file modify. Defines the class to store and modify XML data.
*
*/


#ifdef _DEBUG
#include <driveinfo.h>
#include <pathinfo.h>
#endif

#include "cxmlhandler.h"
#include "clog.h"

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::NewL
//
//--------------------------------------------------------------------------------------------
CXmlHandler* CXmlHandler::NewL()
	{
		CXmlHandler *self = NewLC();
		CleanupStack::Pop();	// self

		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::NewLC
//
//--------------------------------------------------------------------------------------------
CXmlHandler* CXmlHandler::NewLC()
	{
		CXmlHandler *self = new (ELeave) CXmlHandler();
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::~CXmlHandler
//
//--------------------------------------------------------------------------------------------
CXmlHandler::~CXmlHandler()
{
	if(iDataBuf)
		delete iDataBuf;
	iDataBuf = NULL;
	
	Reset();

	iFs.Close();
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ConstructL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::ConstructL()
{
	LOG("[CXmlHandler::ConstructL]\t");
	User::LeaveIfError( iFs.Connect() );
	Reset();
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::InitFromFileL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::InitFromFileL(const TDesC& aFile, const TInt aId)
{
	LOG("[CXmlHandler::InitFromFileL]\t");
	Reset();

	ReadFileL(aFile);
	iBufferId = aId;

	LOG("[CXmlHandler::InitFromFileL]\t Done.");
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::InitFromBufferL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::InitFromBufferL(const TDesC8& aBuffer, const TInt aId)
{
	LOG("[CXmlHandler::InitFromBufferL]\t");

	Reset();

	if(0 >= aBuffer.Length())
	{
		User::Leave(KErrArgument);
	}

	iDataBuf = static_cast<HBufC8*>(User::LeaveIfNull(aBuffer.AllocL()));
	iBufferId = aId;

	LOG("[CXmlHandler::InitFromBufferL]\t Done.");
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::Reset
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::Reset()
{
	LOG("[CXmlHandler::Reset]\t");

	if(iDataBuf)
		delete iDataBuf;
	iDataBuf = NULL;

	iBufferId = KErrNotFound;
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::Reset
//
//--------------------------------------------------------------------------------------------
TInt CXmlHandler::BufferId()
{
	return iBufferId;
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::Reset
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXmlHandler::Buffer()
{
	if(iDataBuf)
		return iDataBuf->Des();

	return KNullDesC8();
}

// PRIVATE

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::CXmlHandler
//
//--------------------------------------------------------------------------------------------
CXmlHandler::CXmlHandler()
{
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::FinalizeBufferL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXmlHandler::FinalizeBufferL(const TDesC8& aTag)
{
	LOG("[CXmlHandler]\t FinalizeBufferL");

	User::LeaveIfNull(iDataBuf);

	// Cut the template to the end tag
	TInt pos = iDataBuf->Find(aTag);
	if(KErrNotFound == pos)
		User::Leave(KErrCorrupt);

	TInt dataLen = pos + aTag.Length();
	TPtrC8 tail = iDataBuf->Left(dataLen);

	return tail;
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::DeleteBufferData
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::DeleteBufferData(TInt aOffset, TInt aLength)
{
	iDataBuf->Des().Delete(aOffset, aLength);

}
//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ReadL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::ReadFileL(const TDesC& aFileName)
{
	LOG("[CXmlHandler]\t ReadFileL");
	// Open the template file and read it into buffer
	RFile templateFile;
	CleanupClosePushL(templateFile);

	User::LeaveIfError( templateFile.Open( iFs, aFileName, EFileRead) );

	TInt fileSize = 0;
	TInt err = templateFile.Size(fileSize);
	if(KErrNone != err)
		fileSize = KBtMaxFileSize;

	iDataBuf = HBufC8::NewL(fileSize);
	TPtr8 bufferPtr( iDataBuf->Des() );

	bufferPtr.Zero();
	templateFile.Read(bufferPtr);

	CleanupStack::PopAndDestroy(&templateFile);
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::GetDataL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXmlHandler::GetDataL(const TDesC8 &aStart, const TDesC8 &aEnd, TInt& aOffset)
{
	User::LeaveIfNull(iDataBuf);
	return ParseValue(iDataBuf->Des(), aStart, aEnd, aOffset);
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::GetDataListL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::GetDataListL(const TDesC8 &aStart, const TDesC8 &aEnd, CDesC8ArrayFlat& aList)
{
	User::LeaveIfNull(iDataBuf);
	ParseValueListL(iDataBuf->Des(), aStart, aEnd, aList);
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ParseValue
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXmlHandler::ParseValue(const TDesC8 &aData, const TDesC8 &aStart, const TDesC8 &aEnd, TInt& aOffset)
{
//	LOG82("[CXmlHandler::ParseValue]\t s: %S, e: %S", &aStart, &aEnd);
	aOffset = aData.Find(aStart);
	if(0 > aOffset)
	{
		return KNullDesC8();
	}

	TInt end = aData.Find(aEnd);
	TInt len = end-aOffset-aStart.Length();
	if(0 >= len)
	{
		aOffset = end;
		return KNullDesC8();
	}
	
	return aData.Mid(aOffset+aStart.Length(), len);
}


//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ParseValueList
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::ParseValueListL(const TDesC8 &aData, const TDesC8 &aStart, const TDesC8 &aEnd, CDesC8ArrayFlat& aList)
{
	LOG("[CXmlHandler::ParseValueListL]\t ");
	TPtrC8 tmpPtr = aData;

	TInt offset = tmpPtr.Find(aStart);
	while(offset != KErrNotFound)
	{
		TInt end = tmpPtr.Find(aEnd);
		if(0 > end)
		{
			return;
		}
		TPtrC8 valuePtr = tmpPtr.Mid(offset+aStart.Length(), end-offset-aStart.Length());

		HBufC8* listEntry = valuePtr.AllocLC();
		aList.AppendL(listEntry->Des());
		CleanupStack::PopAndDestroy();	// listEntry
		
		tmpPtr.Set( tmpPtr.Right(tmpPtr.Length()-(end+aEnd.Length()) ) );
		offset = tmpPtr.Find(aStart);

	}
}


//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ComposeDataL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::ComposeDataL(const TInt aValue, const TDesC8 &aTag)
{
	TBuf8<KDefaultRealWidth> valueStr;
	valueStr.Zero();
	valueStr.AppendNum(aValue);

	ComposeDataL(valueStr, aTag);
}
//--------------------------------------------------------------------------------------------
//
// CXmlHandler::ComposeDataL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::ComposeDataL(const TDesC8 &aData, const TDesC8 &aTag)
{
	User::LeaveIfNull(iDataBuf);

	TInt pos = iDataBuf->Find(aTag);
	if(KErrNotFound == pos)
		User::Leave(KErrCorrupt);

	while(KErrNotFound != pos)
	{
		TInt requiredSize = iDataBuf->Des().Length() + aData.Length();
		if(iDataBuf->Des().MaxLength() < requiredSize)
		{
			iDataBuf = iDataBuf->ReAllocL(requiredSize);
		}
		iDataBuf->Des().Replace(pos, aTag.Length(), aData);
		pos = iDataBuf->Find(aTag);
	}
}

//--------------------------------------------------------------------------------------------
//
// CXmlHandler::InsertDataL
//
//--------------------------------------------------------------------------------------------
void CXmlHandler::InsertDataL(const TDesC8 &aData, const TInt aOffset)
{
	TInt requiredSize = iDataBuf->Des().Length() + aData.Length();
	if(iDataBuf->Des().MaxLength() < requiredSize)
	{
		iDataBuf = iDataBuf->ReAllocL(requiredSize);
	}
	iDataBuf->Des().Insert(aOffset, aData);
}

// End of file
