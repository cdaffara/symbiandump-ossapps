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
* Description:  
*
*/


#ifndef RSUTILS_H
#define RSUTILS_H

#include <e32std.h>
#include <f32file.h>

#include "mbtonnotify.h"

class CObexBaseObject;

/**
* \brief Useful utilities
*
* The RsUtils class contains static utility functions useful to the whole application
* such as descriptor manipulations.
*/
class RsUtils
	{
	public:

		static TBool IsBluetoothOn(); // Check comments .cpp file
		static HBufC16* MakeXHTMLSafeL(const TDesC16& aString);
		static HBufC8* ConvertToUtf8SafeLC(const TDesC16& aUnicode);
		static void CleanFileName(const TDesC &aOldFname, TDes& aNewFname);
		static void FindWidthHeightForPrinting(TInt &aWidth, TInt &aHeight, const TInt aMaxWidth, const TInt aMaxHeight);
		static HBufC8* ConvertFromUnicodeToUtf8LC(const TDesC16& aUnicode); 
		static TBool PathIsInRom(const TFileName &aFileName);
		static TFileName PathNotRom(const TFileName &aFileName);
		static TFileName FileName( const TFileName& aFullName );		
		static TBool FileExists(const TDesC& aFileName, RFs& aFs);
		static TInt FileSize(const TDesC& aFileName, TInt& aSize, RFs& aFs);				
		static TLanguage GetPhoneLanguage();		
		static TInt CreateDataFolder( RFs& aSession, TDes& aPath, TDriveNumber aDrive = EDriveC );
		static TInt ValidateCacheFile( RFs& aSession, const TDesC& aFileName, 
								TTimeIntervalMinutes aValidPeriod, TBool& aDeleted );
		static HBufC* ConcatenateStringsLC(const TDesC& aDes1, const TDesC& aDes2);
		static HBufC* ConcatenateStringsLC(const TDesC& aDes1, const TDesC& aDes2, const TDesC& aDes3);
		static void LogL( const CBufBase& aBuffer );
		static void LogL( CObexBaseObject& aBuffer );
		static void LogL( const TDesC8& aBuffer );	
		static void LogL( const TDesC16& aBuffer );	
		static void LogFileL( const TFileName& aName );
		static void GetLocalBTNameL(TDes8& aName);
	    
	};

#endif  // RSUTILS_H 

//  End of File
