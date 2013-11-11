/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include <implementationproxy.h>
#include <f32file.h>

#include "irdebug.h"
#include "irrecognizer.h"

// CONSTANTS

// If the file name length > 4, the file extension might be valid                  
const TInt KPlsFileExtensionsMightBeValid = 4;  
const TInt KM3uFileExtensionsMightBeValid = 4;

//extension of File to be recognised.
_LIT(KPlsExtension, ".pls");
//Mime type of the .pls file
_LIT8(KPlsMimeType,"audio/x-scpls");

_LIT(KM3uExtension, ".m3u");


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// RecognizerEx::RecognizerEx()
// constructs the object
// ---------------------------------------------------------------------------
//
CApaRecognizerEx::CApaRecognizerEx():CApaDataRecognizerType(KIRRecognizerDllUid, 
	CApaDataRecognizerType::EHigh)
    {
    IRLOG_DEBUG( "CApaRecognizerEx::CApaRecognizerEx()" );
    // It only supports 1 mime type
    iCountDataTypes = 1;
    IRLOG_DEBUG( "CApaRecognizerEx::CApaRecognizerEx() - Exiting." );
    }

// ---------------------------------------------------------------------------
// RecognizerEx::~RecognizerEx()
// Destroys the object
// ---------------------------------------------------------------------------
//
CApaRecognizerEx::~CApaRecognizerEx()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::~CApaRecognizerEx" );
    // no implementation
    }


// ---------------------------------------------------------------------------
// RecognizerEx::CreateRecognizerL()
// Returns pointer to the new object
// ---------------------------------------------------------------------------
//
CApaDataRecognizerType* CApaRecognizerEx::CreateRecognizerL()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::CreateRecognizerL" );
    return new (ELeave) CApaRecognizerEx();
    }


// ---------------------------------------------------------------------------
// RecognizerEx::PreferredBufSize()
// Returns preferred buffer size
// ---------------------------------------------------------------------------
//
TUint CApaRecognizerEx::PreferredBufSize()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::PreferredBufSize" );
    return 0;
    }

// ---------------------------------------------------------------------------
// RecognizerEx::SupportedDataTypeL()
// Returns supported mime type
// ---------------------------------------------------------------------------
//
TDataType CApaRecognizerEx::SupportedDataTypeL(TInt aIndex) const
    {
    IRLOG_DEBUG( "CApaRecognizerEx::SupportedDataTypeL" );
    if (0 == aIndex)
	{
        return TDataType(KPlsMimeType);
	}
	else
    {
        ASSERT(0);
        return TDataType(KNullDesC8);
    }
    }

// ---------------------------------------------------------------------------
// RecognizerEx::DoRecognizeL()
// Recognizes the file by name and/or head buffer
// ---------------------------------------------------------------------------
//
void CApaRecognizerEx::DoRecognizeL(const TDesC& aName, 
	const TDesC8& aBuffer)
    {
	IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL" );    
    // To keep code simple, we only check file name extension
    TInt len = aBuffer.Length();
    if (aName.Length()>KPlsFileExtensionsMightBeValid)
		{
		//Compare the extension of the file to be recognised with .pls
		if (aName.Right(KPlsFileExtensionsMightBeValid).CompareF(
			KPlsExtension)==0)
	    {
			iConfidence=ECertain;   // is certainly something totally new
			iDataType=TDataType(KPlsMimeType);
			IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL - Exiting (1)." );
			return;
		}
		else if (aName.Right(KM3uFileExtensionsMightBeValid).CompareF(
            KM3uExtension)==0)
		{
		    RecognizeM3uFileL(aName);
		    IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL - Exiting (2)." );
		    return;
		}
	    IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL - Exiting (3)." );					
		}
    }

void CApaRecognizerEx::RecognizeM3uFileL(const TDesC& aFileName)
{
    _LIT8(KHttpProtocol, "http");
    _LIT8(KMmsProtocol, "mms");
    _LIT8(KRtspProtocol, "rtsp");

    RFs fs;
    User::LeaveIfError(fs.Connect());
    RFile file;
    TInt ret = file.Open(fs, aFileName, EFileRead);
    if (KErrNone != ret)
    {
        fs.Close();
        return;
    }
    
    RBuf8 content;
    CleanupClosePushL(content);
    TInt maxLen = 0;
    file.Size(maxLen);
    content.CreateL(maxLen);
    file.Read(content);
    
    //try to use descriptor method to parse the buffer
    if (CheckStreamingLinksL(content, KHttpProtocol) ||
        CheckStreamingLinksL(content, KMmsProtocol) ||
        CheckStreamingLinksL(content, KRtspProtocol))
    {
        iConfidence = ECertain;
        iDataType = TDataType(KPlsMimeType);
    }
    
    CleanupStack::PopAndDestroy();
    file.Close();
    fs.Close();
}

TBool CApaRecognizerEx::CheckStreamingLinksL(TDes8& aBuffer, const TDesC8& aProtocol)
{
    TBool ret = EFalse;
    _LIT8(KNewLine, "\n");
    
    //initially, remain points to aBuffer
    TPtrC8 remain(aBuffer);
    TInt newLinePos = remain.Find(KNewLine);
    
    while (KErrNotFound != newLinePos)
    {
        //copy a new line to RBuf8
        RBuf8 left;
        CleanupClosePushL(left);
        left.CreateL(remain.Left(newLinePos));
        left.TrimLeft();
        left.LowerCase();
        
        //after trim left, does this line start with aProtocol?
        if (left.Left(aProtocol.Length()) == aProtocol)
        {
            ret = ETrue;
        }
        CleanupStack::PopAndDestroy();
        
        if (ret)
        {
            break;
        }
        else
        {
            //remain points to the right part
            remain.Set(remain.Right(remain.Length() - newLinePos - 1));
            newLinePos = remain.Find(KNewLine);
        }
    }
    
    if (!ret && remain.Length() > 0)
    {
        //last line doesn't end with '\n'
        RBuf8 last;
        CleanupClosePushL(last);
        last.CreateL(remain);
        last.TrimLeft();
        last.LowerCase();
        if (last.Left(aProtocol.Length()) == aProtocol)
        {
            ret = ETrue;
        }
        CleanupStack::PopAndDestroy();
    }
    
    return ret;
}

// -----------------------------------------------------------------------------
// ImplementationTable
// violates PC lint error :: Warning 611: Suspicious cast
// required by the ECOM framework to correctly identify the instantiation 
// method pointer to provide to a client's resolution request.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KIRRecognizerImplementationUid, 
    	CApaRecognizerEx::CreateRecognizerL)
    };
    
// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Provides access to the implementationtable
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
	TInt& aTableCount)
    {
    IRLOG_DEBUG( "ImplementationGroupProxy" );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    IRLOG_DEBUG( "ImplementationGroupProxy - Exiting." );
    return ImplementationTable;
    }



