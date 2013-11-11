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
* Description:  Contains the RsUtils class definitions.
*
*/


#ifndef RSUTILS_H
#define RSUTILS_H

#include <e32std.h>
#include <f32file.h>

/**
* The RsUtils class contains static utility functions useful to the whole application
* such as descriptor manipulations.
*/
class RsUtils
	{
	public:
		
		static TBool FileExists( const TDesC& aFileName, RFs& aFs );
		static TBool PathExists( const TDesC& aFilePath, RFs& aFs );				
		static TInt DriveFreeSpace( TChar aDrive, RFs& aFs, TUint32& aSpace );
		static TInt CumulativeFileSize( const TDesC& aFileName, RFs& aFs, TInt& aCumulativeSize );
		static TBool FileInDir( const TDesC& aFileName, const TDesC& aDirName );
		static TBool FileOnDrive( TChar aDrive, const TDesC& aFileName );
	};

#endif // RSUTILS_H

//  End of File
