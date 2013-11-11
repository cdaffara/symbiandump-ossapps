/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         Helper functions for MMSClientMTMBCTester
*
*/










// INCLUDE FILES
#include "MMSClientMTMBCTesterController.h"
#include <utf.h>
#include <f32file.h>
#include <apparc.h>


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ReadConfigFile
// Read data into memory from the test module configuration file
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::ReadConfigFileL( TFileName& aIniFile )
    {
    // Connect to the file
    RFs fs;
	User::LeaveIfError( fs.Connect() );
	RFile file;
	User::LeaveIfError( file.Open( fs, aIniFile, EFileRead ) );
    
    // Read the content of the file into a buffer
    TInt fileSize;
	User::LeaveIfError( file.Size( fileSize ) );
    iFileBuf = HBufC8::NewL( fileSize );
    TPtr8 modBuffer = iFileBuf->Des();
	User::LeaveIfError( file.Read( modBuffer ) );

	// Close the file and clean up
    file.Close();
    fs.Close();
    }


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetItemFromFile
// Get a wanted item from the configuration file
// -----------------------------------------------------------------------------
//
HBufC* CMMSClientMTMBCTesterController::GetItemFromFile( const TDesC8& aID,
                                                         HBufC* iBuf)
	{
	// Needed variables
	_LIT( KAllocationFailure, "*** Memory allocation FAILED: %d ***" );
	TPtrC8 tempFile = iFileBuf->Des();
   	TRAPD ( error1, iBuf = HBufC::NewL( iBufSize ) );
   	if ( error1 != KErrNone )
   		{
   		iLogger->Log( KAllocationFailure, error1 );
   		return NULL;
   		}
	TPtrC8 prtPropPart;
	TInt line;
	TInt start;
	TInt end;
	
	// Find the wanted item
	line = tempFile.Find( aID );
	TPtrC8 foundLine = tempFile.Mid( line, KLineLength );
	start = foundLine.Locate( '[' );
	end = foundLine.Locate( ']' );
	
	// Get the data
	prtPropPart.Set( foundLine.Mid( start+1, end-start-1 ) );
	
	// Make sure we can use the item and return it
	_LIT( KConversionFailure, "*** ConvertToUnicodeFromUtf8 FAILED: %d ***" );
	TInt size = prtPropPart.Length();
	if ( size > iBufSize - iExtraSpace )
		{
		delete iBuf;
        iBuf = NULL;
        iBufSize = 0;
        TRAPD ( error2, iBuf = HBufC::NewL( size + iExtraSpace ) );
        if ( error2 != KErrNone )
        	{
        	iLogger->Log( KAllocationFailure, error2 );
        	return NULL;
        	}
        iBufSize = size;
        TPtr16 modBuf = iBuf->Des();
		TInt rcode = CnvUtfConverter::ConvertToUnicodeFromUtf8( modBuf,
		                                                        prtPropPart );
		if (rcode != KErrNone)
			{
			iLogger->Log( KConversionFailure, rcode );
			}
		iBuf->Des().ZeroTerminate();
		}
	else 
		{
		TPtr16 modBuf = iBuf->Des();
		TInt rcode = CnvUtfConverter::ConvertToUnicodeFromUtf8( modBuf,
		                                                        prtPropPart );
		if (rcode != KErrNone)
			{
			iLogger->Log( KConversionFailure, rcode );
			}
		iBuf->Des().ZeroTerminate();
		}
	return iBuf;
	}


// End of file: MMSClientMTMBCTesterTools.cpp
