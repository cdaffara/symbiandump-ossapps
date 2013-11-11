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


#include <bautils.h>

#include "rsutils.h"

//  CONSTANTS
namespace
	{
	_LIT( KBackslash, "\\" );	
	_LIT( KMatchPattern, "*" );
	}


TBool RsUtils::FileExists(const TDesC& aFileName, RFs& aFs)
	{	
	return ( BaflUtils::FileExists( aFs, aFileName ) );
	}

TBool RsUtils::PathExists(const TDesC& aFilePath, RFs& aFs)
	{		
	if( aFilePath.Right( 1 ) == KBackslash )
		{				
		return ( BaflUtils::PathExists( aFs, aFilePath ) );
		}
	else
		{
		TFileName filePath = aFilePath;
		filePath.Append( KBackslash );
		return ( BaflUtils::PathExists( aFs, filePath ) );
		}
	}

TInt RsUtils::DriveFreeSpace( TChar aDrive, RFs& aFs, TUint32& aSpace )
	{
	TInt mmcDriveNum;
	TInt err = aFs.CharToDrive( aDrive, mmcDriveNum );
	if( !err )
		{
		TVolumeInfo volInfo;
		err = aFs.Volume( volInfo, mmcDriveNum );
		aSpace = volInfo.iFree;
		}		
	return err;
	}
			

TInt RsUtils::CumulativeFileSize( const TDesC& aFileName, RFs& aFs, TInt& aCumulativeSize )
	{
	TEntry fileEntry;
	TInt err = aFs.Entry( aFileName, fileEntry );
	if( !err )
		{
		aCumulativeSize += fileEntry.iSize;	
		}
	return err;
	}	
	
TBool RsUtils::FileInDir( const TDesC& aFileName, const TDesC& aDirName )	
	{
	TFileName dir = aDirName;
	if( dir.Right( 1 ) != KBackslash )
		{
		dir.Append( KBackslash );	
		}
	dir.Append( KMatchPattern );	
	TBool ret = ( aFileName.Match( dir ) == KErrNotFound ) ? EFalse : ETrue;	
	return ret;
	}
	
TBool RsUtils::FileOnDrive( TChar aDrive, const TDesC& aFileName )
	{
	TParsePtrC parse( aFileName );	
	TBool res = ( parse.Drive().LocateF( aDrive ) == KErrNotFound ) ?  EFalse : ETrue;
	return res;
	}

//  End of File
