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


//  EXTERNAL INCLUDES
#include <s32file.h>
#include <charconv.h>
#include <bautils.h>
#include <pathinfo.h>
#include <utf.h>
#include <obexbaseobject.h>
#include <centralrepository.h>
#include <driveinfo.h>

#include "rsutils.h"
#include "btprotocolconsts.h"
#include "cbtonquery.h"
#include "clog.h"
 
namespace
	{
	const TInt KSegBufChunkSize=2048;
#ifdef ENABLE_LOGGING // for removing compile warnings
	const TInt KLogBufferSize = 50;
#endif
	}



LOCAL_C TInt SearchAndReplace(TDes &aString, const TDesC &aSearch, const TDesC &aReplace)
{
	TInt nPos, nLengthLeft;

	nLengthLeft = aString.MaxLength() - aString.Length();

	nPos = aString.Find(aSearch);
	while (nPos != KErrNotFound)
	{
		if ((nLengthLeft - (aReplace.Length() - aSearch.Length())) < 0)
		{
			return KErrGeneral;
		}
		aString.Replace (nPos, aSearch.Length(), aReplace);
		nLengthLeft = aString.MaxLength() - aString.Length();
		nPos = aString.Find(aSearch);
	}
	return KErrNone;
}

LOCAL_C void SearchAndReplaceL(CBufBase& aString, TDesC8& aSearch, TDesC8& aReplace)
{
	TInt nExtraLength = 0;
	TInt nLength = aString.Size();
	TInt nTempPos = 0;

	HBufC8* buf = HBufC8::NewLC(nLength);
	TPtr8 ptr(buf->Des());
	aString.Read(0, ptr, nLength);

	TInt nPos = 0;
	while (nPos < nLength)
	{
		nTempPos = ptr.Mid(nPos).Find(aSearch);
		if (nTempPos == KErrNotFound)
			break;
		nPos += nTempPos;
		aString.Delete(nPos+nExtraLength, aSearch.Length());
		aString.InsertL(nPos+nExtraLength, aReplace);
		nExtraLength += aReplace.Length()-aSearch.Length();
		nPos++;
	}

	CleanupStack::PopAndDestroy(buf);
}

LOCAL_C void SearchAndReplace(TDes &aString, TChar aChar, TChar aReplace)
{
	TBuf<1> aSrc1, aRep2;
	aSrc1.Append(aChar);
	aRep2.Append(aReplace);

	SearchAndReplace(aString, aSrc1, aRep2);
}

void RsUtils::CleanFileName(const TDesC &aOldFname, TDes& aNewFname)
{
LOG1("RsUtils::CleanFileName aOldFname: %S", &aOldFname);
	TInt nLen;

	const TUint colon=TChar(':');

	if (aOldFname.Length() == 0 || aNewFname.Length() == 0)
		return;

	nLen = aOldFname.Length();
	if(nLen > 3 && aOldFname[1] == colon)
	{
		// strip off c:\ at the front
		aNewFname = aOldFname.Right(nLen-3);
	}
	else
		aNewFname=aOldFname;

	// Change \ into /
	SearchAndReplace (aNewFname, '\\', '/');
	LOG1("RsUtils::CleanFileName aNewFname: %S", &aNewFname);
}


LOCAL_C void ConvertFrom8To16Bit(const TDesC8& aSrc, TDes8& aDest)
{
	const TInt srcLen = aSrc.Length();
	const TInt destMaxLen = aDest.MaxSize();

	if (srcLen*2 > destMaxLen)
		return;

	aDest.SetLength(srcLen*2);
	aDest.FillZ();
	for (TInt i = 0; i < srcLen; i++)
	{
		aDest[2*i] = aSrc[i];
		aDest[2*i+1] = 0x00;
	}
}

HBufC16* RsUtils::MakeXHTMLSafeL(const TDesC16& aString)
{
	// Change XHTML special characters to their printable equivalents
	CBufSeg* seg=CBufSeg::NewL(KSegBufChunkSize);
	CleanupStack::PushL(seg);

	seg->InsertL(0, &aString[0], aString.Length()*2);

	TBuf8<16> src;
	TBuf8<18> rep;

	_LIT8(KAmpersand, "&amp;");
	ConvertFrom8To16Bit(KAmpersand, rep);
	ConvertFrom8To16Bit(_L8("&"), src);
	SearchAndReplaceL(*seg, src, rep);

	_LIT8(KLessthan, "&lt;");
	ConvertFrom8To16Bit(KLessthan, rep);
	ConvertFrom8To16Bit(_L8("<"), src);
	SearchAndReplaceL(*seg, src, rep);

	_LIT8(KGreaterthan, "&gt;");	
	ConvertFrom8To16Bit(KGreaterthan, rep);
	ConvertFrom8To16Bit(_L8(">"), src);
	SearchAndReplaceL(*seg, src, rep);

	_LIT8(KApostrophe, "&apos;");
	ConvertFrom8To16Bit(KApostrophe, rep);
	ConvertFrom8To16Bit(_L8("\'"), src);
	SearchAndReplaceL(*seg, src, rep);

	_LIT8(KDoubleQuote, "&quot;");
	ConvertFrom8To16Bit(KDoubleQuote, rep);
	ConvertFrom8To16Bit(_L8("\""), src);
	SearchAndReplaceL(*seg, src, rep);

	// Replace the many types of newlines with the XHTML break sequence
	_LIT8(KTokenNewLine8,"<br/>");
	ConvertFrom8To16Bit(KTokenNewLine8, rep);

	ConvertFrom8To16Bit(_L8("\r\n"), src);
	SearchAndReplaceL(*seg, src, rep);

	ConvertFrom8To16Bit(_L8("\n"), src);
	SearchAndReplaceL(*seg, src, rep);

	TUint16 newLine=0x2029;
	src.Copy(((TUint8*)&newLine),2);
	SearchAndReplaceL(*seg, src, rep);

	newLine=0x2028;
	src.Copy(((TUint8*)&newLine),2);
	SearchAndReplaceL(*seg, src, rep);


	TInt len = seg->Size()/2;
	HBufC16* retBuf = HBufC16::NewL(len);
	TPtr16 ptr16(retBuf->Des());
	ptr16.SetLength(len);	
	seg->Read(0, &ptr16[0], len*2);

	CleanupStack::PopAndDestroy(seg);

	return retBuf;

}


HBufC8* RsUtils::ConvertFromUnicodeToUtf8LC(const TDesC16& aUnicode)
{
	CBufSeg* seg=CBufSeg::NewL(KSegBufChunkSize);
	CleanupStack::PushL(seg);

	const TInt KNumCharConvert = 30;

	HBufC8* buf = HBufC8::NewLC(KNumCharConvert*10);
	TPtr8 ptr(buf->Des());

	TInt nInsertPos = 0;
	TInt nSrcPos = 0;
	TInt nLength = KNumCharConvert;
	TInt nSrcLength = aUnicode.Length();
	TInt nUnconvCharsLeft = 0;

	//start loop
	while (nSrcPos < nSrcLength)
	{
		ptr.SetLength(0);
		nLength = Min(KNumCharConvert, nSrcLength-nSrcPos);
		
		//convert to utf8
		nUnconvCharsLeft = CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aUnicode.Mid(nSrcPos, nLength));
		
		//append converted text to a buffer
		seg->InsertL(nInsertPos, ptr);
		
		nInsertPos += ptr.Length();
		nSrcPos += (nLength-nUnconvCharsLeft);
	}	
	CleanupStack::PopAndDestroy(buf);

	//copy converted data to a buffer to return 
	nLength = seg->Size();
	HBufC8* retBuf = HBufC8::NewL(nLength);
	TPtr8 ptrRet(retBuf->Des());
	seg->Read(0, ptrRet, nLength);
	*retBuf=ptrRet;

	CleanupStack::PopAndDestroy(seg);

	CleanupStack::PushL(retBuf);
	return retBuf;
}

// correct order one...
HBufC8* RsUtils::ConvertToUtf8SafeLC(const TDesC16& aUnicode)
{
	if (!aUnicode.Length())
	{
		HBufC8* buf = HBufC8::NewLC(1);
		return buf;
	}

	HBufC16* buf1 = MakeXHTMLSafeL(aUnicode);
	CleanupStack::PushL(buf1);
	HBufC8* retBuf = ConvertFromUnicodeToUtf8LC(*buf1);

	CleanupStack::Pop(retBuf);
	CleanupStack::PopAndDestroy(buf1);
	CleanupStack::PushL(retBuf);
	return retBuf;
}


void RsUtils::FindWidthHeightForPrinting(TInt &aWidth, TInt &aHeight, const TInt aMaxWidth, const TInt aMaxHeight)
{
	if (aWidth*aMaxHeight == aHeight*aMaxWidth)
	{
		aWidth = aMaxWidth;
		aHeight = aMaxHeight;
	}
	else if (aWidth*aMaxHeight > aHeight*aMaxWidth)
	{
		aHeight = (aMaxWidth*aHeight)/aWidth;
		aWidth = aMaxWidth;
	}
	else
	{
		aWidth = (aMaxHeight*aWidth)/aHeight;
		aHeight = aMaxHeight;
	}
}




// All the helper functions


// returns the file name

TFileName RsUtils::FileName( const TFileName& aFullName )
	{
	TParse parse;
	parse.Set(aFullName, NULL, NULL);
	return parse.NameAndExt();
	}


/** Test whether a path points to ROM. The file does not need to exist
 */
TBool RsUtils::PathIsInRom(const TFileName &aFileName)
	{
	LOG("RsUtils::PathIsInRom begin");				
	TParsePtrC parseSource( aFileName );
	TInt driveId;
	TFileName path;
	DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveId );
	PathInfo::GetFullPath( path, driveId, PathInfo::ERomRootPath );
	TParsePtrC parseRom( path );

	TBool res = ( parseSource.Drive().CompareF( parseRom.Drive() ) == 0 ) ?  ETrue : EFalse;	
	LOG1("RsUtils::PathIsInRom res: %d", res);
	return res;				
	}

/** Returns a fullname with a drive not in rom
 */
TFileName RsUtils::PathNotRom(const TFileName &aFileName)
	{
	LOG1("RsUtils::PathNotRom aFileName: %S", &aFileName);
	
	if( PathIsInRom( aFileName ) )
		{
		LOG("RsUtils::PathNotRom file in rom");
		TParse parse;

		TInt driveId;
		TFileName path;
		DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, driveId );
		PathInfo::GetRootPath( path, driveId );
		TParsePtrC parsePhoneDrive( path );

		TFileName drive = parsePhoneDrive.Drive();
		parse.Set(drive, &aFileName, NULL);
		return parse.FullName();
		}
	else
		{
		LOG("RsUtils::PathNotRom file NOT in rom");
		return aFileName;
		}
	}



TBool RsUtils::FileExists(const TDesC& aFileName, RFs& aFs)
{	
	return (BaflUtils::FileExists(aFs, aFileName));
}

TInt RsUtils::FileSize(const TDesC& aFileName, TInt& aSize, RFs& aFs)
{
	TEntry theFile;
	TInt error = aFs.Entry(aFileName, theFile);
	aSize = theFile.iSize;
	return error;
}



HBufC* RsUtils::ConcatenateStringsLC(const TDesC& aDes1, const TDesC& aDes2)
{
	const TInt newLen=aDes1.Length()+aDes2.Length();
	HBufC* newBuf=HBufC::NewLC(newLen);
	*newBuf=aDes1;
	TPtr newPtr(newBuf->Des());
	newPtr.Append(aDes2);
	*newBuf=newPtr;
	return newBuf;
}

HBufC* RsUtils::ConcatenateStringsLC(const TDesC& aDes1, const TDesC& aDes2, const TDesC& aDes3)
{
	HBufC* twoSt=ConcatenateStringsLC(aDes1,aDes2);
	HBufC* allThree=ConcatenateStringsLC(*twoSt,aDes3);
	CleanupStack::Pop(allThree);
	CleanupStack::PopAndDestroy(twoSt);
	CleanupStack::PushL(allThree);
	return allThree;
}


// WARNING: This function uses User::WaitForRequest,
// so check carefully where to use it to avoid View Server 11 panic
TBool RsUtils::IsBluetoothOn()
	{
	const TUid KPowerModeSettingNotifierUid = {0x100059E2}; // 	Notifier id for BT power mode setting
	TPckgBuf<TBool> pckg( ETrue ); 			// Input parameter
	TPckgBuf<TBool> resultPckg( EFalse );	// Output parameter	
	RNotifier notif;
	if( notif.Connect() == KErrNone )
		{		
		TRequestStatus stat;
		notif.StartNotifierAndGetResponse(stat, KPowerModeSettingNotifierUid, pckg, resultPckg);
		User::WaitForRequest(stat);
		notif.CancelNotifier(KPowerModeSettingNotifierUid);
		notif.Close();
		}
	return resultPckg();
	}

TLanguage RsUtils::GetPhoneLanguage()
{
	return User::Language();
}


TInt RsUtils::CreateDataFolder( RFs& aSession, TDes& aPath, TDriveNumber aDrive )
	{
	LOG1("RsUtils::CreateDataFolder aDrive: %d", aDrive);
	_LIT( KColon, ":" );
	TChar drive;
	TInt err = aSession.DriveToChar( aDrive, drive );							
	if( !err )
		{
		err = aSession.CreatePrivatePath( aDrive );
		if( !err )
			{
			TFileName path;			
			aSession.PrivatePath( path );
			aPath.Zero();
			aPath.Append( drive );
			aPath.Append( KColon );
			aPath.Append( path );														
			}		
		}	
	LOG1("RsUtils::CreateDataFolder end with aPath: %S", &aPath);
	LOG1("RsUtils::CreateDataFolder end with err: %d", err);
	return err;
	}


TInt RsUtils::ValidateCacheFile( RFs& aSession, const TDesC& aFileName, 
							TTimeIntervalMinutes aValidPeriod, TBool& aDeleted )
	{
	aDeleted = EFalse;
	TEntry entry;
	TInt err = aSession.Entry( aFileName, entry );
	
	if( !err )
		{
		TTime now;
		now.UniversalTime();
		now -= aValidPeriod;		
		if( entry.iModified < now )
			{
			// cache file was not used for a aValidPeriod period of time; delete it
			err = aSession.Delete( aFileName );
			if( !err ) aDeleted = ETrue;	
			}		
		}
	LOG1("RsUtils::ValidateCacheFile aDeleted: %d", aDeleted);
	LOG1("RsUtils::ValidateCacheFile err: %d", err);	
	return err;	
	}

void RsUtils::LogL( const CBufBase& aBuffer )
	{
	#ifdef ENABLE_LOGGING
	#define _BUFBASE_LOGGING_
	#ifdef _BUFBASE_LOGGING_
	LOG("BUFBASE LOG BEGIN");	
	HBufC8* buffer=HBufC8::NewLC(KLogBufferSize);
	TPtr8 bufferPtr(buffer->Des());
	TInt sizeleft=aBuffer.Size();
	TInt amountToRead=KLogBufferSize;
	TInt location=0;
	while(sizeleft>0)
		{
		if(sizeleft<KLogBufferSize)
			amountToRead=sizeleft;
		aBuffer.Read(location,bufferPtr,amountToRead);			
		LogL( *buffer );												
		sizeleft-=amountToRead;
		location+=amountToRead;
		}
	CleanupStack::PopAndDestroy(); // buffer	
	LOG("BUFBASE LOG END");
	#endif // _BUFBASE_LOGGING_
	#endif // ENABLE_LOGGING	
	}

void RsUtils::LogL( CObexBaseObject& aBuffer )
	{
	#ifdef ENABLE_LOGGING
	//#define _OBEXBASE_LOGGING_
	#ifdef _OBEXBASE_LOGGING_
	LOG("OBEXBASE LOG BEGIN");
	LOG1("CObexBaseObject::Name length: %d", aBuffer.Name().Length());
	LogL(aBuffer.Name());
	LOG1("CObexBaseObject::Type length: %d", aBuffer.Type().Length());
	LogL(aBuffer.Type());
	LOG1("CObexBaseObject::Length: %d", aBuffer.Length());
	LOG1("CObexBaseObject::Description length: %d", aBuffer.Description().Length());
	LogL(aBuffer.Description());
	LOG1("CObexBaseObject::Target length: %d", aBuffer.Target().Length());
	LogL(aBuffer.Target());
	const RPointerArray<HBufC8>* http = aBuffer.Http();
	if( http )
		{
		LOG1("http->Count: %d", http->Count());
		for( TInt i = 0; i < http->Count(); i++ )
			{
			LOG1("i: %d", i);
			LogL(*((*http)[i]));		
			}	
		}	
	LOG1("CObexBaseObject::AppParam length: %d", aBuffer.AppParam().Length());
	LogL(aBuffer.AppParam());	
			
		
	LOG("OBEXBASE LOG END");
	#endif // _OBEXBASE_LOGGING_
	#endif // ENABLE_LOGGING		
	}
	
void RsUtils::LogL( const TDesC8& aBuffer )
	{
	#ifdef ENABLE_LOGGING
	HBufC16* bufC16 = HBufC16::NewLC( aBuffer.Length() );
	TPtr16 ptr16 = bufC16->Des();
	ptr16.Copy( aBuffer );
	LogL( *bufC16 );
	CleanupStack::PopAndDestroy(); // bufC16
	#endif // ENABLE_LOGGING	
	}

void RsUtils::LogL( const TDesC16& aBuffer )
	{
	#ifdef ENABLE_LOGGING
	TInt sizeleft = aBuffer.Length();
	TInt amountToRead = KLogBufferSize;
	TInt location = 0;
	while( sizeleft > 0 )
		{
		if( sizeleft < KLogBufferSize )
			amountToRead = sizeleft;					
		HBufC16* buffer = aBuffer.Mid( location, amountToRead ).AllocL();
		_LIT( KFormattedString, "ACHTUNG!!! FORMATTED STRING ACHTUNG!!!" );
		_LIT( KFormatMark, "%" );
		if( buffer->Find( KFormatMark ) != KErrNotFound )
			{
			delete buffer;
			buffer = NULL;
			buffer = KFormattedString().AllocL();
			}	
		LOG1("%S", buffer);													
		delete buffer;
		buffer = NULL;												
		sizeleft -= amountToRead;
		location += amountToRead;
		}
	#endif // ENABLE_LOGGING
	}
	
void RsUtils::LogFileL( const TFileName& aName )
	{
	#ifdef ENABLE_LOGGING
	//#define _FILE_LOGGING_
	#ifdef _FILE_LOGGING_
	LOG("FILE LOG BEGIN");
	
	RFile file;
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	User::LeaveIfError( file.Open( fs, aName, EFileStream|EFileRead|EFileShareAny ) );
	CleanupClosePushL( file );
	TInt fileSize;
	User::LeaveIfError( file.Size( fileSize ) );		
	HBufC8* buffer=HBufC8::NewLC( KLogBufferSize );
	TPtr8 bufferPtr( buffer->Des() );
	TInt sizeleft = fileSize;
	TInt amountToRead = KLogBufferSize;
	TInt location = 0;
	while( sizeleft > 0 )
		{
		if( sizeleft < KLogBufferSize )
			amountToRead = sizeleft;				
		
		User::LeaveIfError( file.Read( location, bufferPtr, amountToRead ) );
		LogL( *buffer );												
		sizeleft -= amountToRead;
		location += amountToRead;
		}			
	CleanupStack::PopAndDestroy( 3 ); // buffer, file, fs	
			
	LOG("FILE LOG END");
	#endif // _FILE_LOGGING_	
	#endif // ENABLE_LOGGING
	}

//--------------------------------------------------------------------------------------------
//
// RsUtils::GetLocalBTNameL
//
//--------------------------------------------------------------------------------------------
void RsUtils::GetLocalBTNameL(TDes8& aName)
	{
	LOG("[RsUtils::GetLocalBTNameL] BEGIN");
	RSocketServ socketServ;	
	User::LeaveIfError( socketServ.Connect() );
	TProtocolDesc pInfo;
	TBuf<0x100> name;
	TInt error = KErrNone;

	_LIT( BTLinkManager, "BTLinkManager" );	
	error = socketServ.FindProtocol( BTLinkManager(), pInfo );
	LOG1("[RsUtils::GetLocalBTNameL] socketServ.FindProtocol ends with: %d", error );
	if( KErrNone == error )
		{    
		RHostResolver hostResolver;	
		error = hostResolver.Open( socketServ, pInfo.iAddrFamily, pInfo.iProtocol );
		LOG1("[RsUtils::GetLocalBTNameL] hostResolver.Open ends with: %d", error );
		if( KErrNone == error  )
			{    
			hostResolver.GetHostName( name );       
			hostResolver.Close();   	
			}
		aName.Copy( name );
		}
	if ( error != KErrNone )
		{
		aName.Zero();
		// KServiceClass == own ID
		aName.AppendNum(KServiceClass, EHex);
		}
	socketServ.Close();
	LOG82("[CBtPrintingDevice::GetLocalBTNameL]\t GetLocalBTNameL(%S) %d", &aName, error);	
	LOG("[RsUtils::GetLocalBTNameL] END");

	}

//  End of File
