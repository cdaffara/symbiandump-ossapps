/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: MMCScBkupLogger implementation
*
*
*/

#include "MMCScBkupLogger.h"

// System includes
#include <coemain.h>
#include <e32svr.h>
#include <flogger.h>

#if defined(__MMCSCBKUPLOGGING_ENABLED__)

// Constants
const TInt KMMCScBkupLoggingMaxLogTextLength = 1024;
const TInt KMMCScBkupLoggingMaxLogSize = 10240;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// MMCScBkupLogger::LogRaw()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupLogger::LogRaw( const TDesC& aData )
    {
#if defined(__MMCSCBKUPLOGGING_MODE_FILE__)

    _LIT( KMMCScBkupLoggingDir,      "FMBkup" );
    _LIT( KMMCScBkupLoggingFileName, "FMBkup.txt" );
    
    /////////////
    // FLOGGER
    /////////////
    RFileLogger::Write( KMMCScBkupLoggingDir, KMMCScBkupLoggingFileName, EFileLoggingModeAppend, aData );

#elif defined(__MMCSCBKUPLOGGING_MODE_RDEBUG__)

    /////////////
    // RDEBUG
    /////////////
    _LIT( KLogComponentName, "[MMC] ");
    HBufC* printBuf = HBufC::New( aData.Length() + KLogComponentName().Length() );
    if  ( printBuf )
        {
        TPtr pBuf( printBuf->Des() );
        pBuf.Append( KLogComponentName );
        pBuf.Append( aData );
        RDebug::Print( *printBuf );
        delete printBuf;
        }
    else
        {
        RDebug::Print( aData );
        }

#endif
    }


// ---------------------------------------------------------------------------
// MMCScBkupLogger::Log()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupLogger::Log( TRefByValue<const TDesC> aFmt, ... )
    {
	VA_LIST list;
	VA_START( list, aFmt );
	
    HBufC* buf = HBufC::New( KMMCScBkupLoggingMaxLogTextLength );
    if  ( buf )
        {
        TPtr pBuf( buf->Des() );
        pBuf.FormatList(aFmt, list);
        LogRaw( *buf );
        delete buf;
        }
    }


// ---------------------------------------------------------------------------
// MMCScBkupLogger::DataType()
// 
// 
// ---------------------------------------------------------------------------
const TDesC& MMCScBkupLogger::DataType( TMMCScBkupOwnerDataType aType )
    {
    switch( aType )
        {
    case EMMCScBkupOwnerDataTypeDataOwner:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypeDataOwner");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypeJavaData:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypeJavaData");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypePublicData:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypePublicData");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypeSystemData:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypeSystemData");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypeActiveData:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypeActiveData");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypePassiveData:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypePassiveData");
        return KRet;
        }
    case EMMCScBkupOwnerDataTypeAny:
        {
        _LIT(KRet, "EMMCScBkupOwnerDataTypeAny");
        return KRet;
        }
    default:
        break;
        }

    _LIT(KRet, "UNKNOWN!");
    return KRet;
    }


// ---------------------------------------------------------------------------
// MMCScBkupLogger::FriendlyNameForSID()
// 
// 
// ---------------------------------------------------------------------------
const TDesC& MMCScBkupLogger::FriendlyNameForSID( TSecureId aSID )
    {
    switch( aSID.iId )
        {
    case 0x10202be9:
        {
        _LIT(KRet, "[CentRep]");
        return KRet;
        }
    case 0x10008d38:
        {
        _LIT(KRet, "[FavouritesSrv]");
        return KRet;
        }
    case 0x101f51f2:
        {
        _LIT(KRet, "[RightsServer]");
        return KRet;
        }
    case 0x101f7993:
        {
        _LIT(KRet, "[VpnManager]");
        return KRet;
        }
    case 0x101fd288:
        {
        _LIT(KRet, "[EventMediator]");
        return KRet;
        }
    default:
        break;
        }

    _LIT(KRet, "");
    return KRet;
    }


// ---------------------------------------------------------------------------
// MMCScBkupLogger::LogFile()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupLogger::LogFile( TRefByValue<const TDesC> aFmt, ... )
    {
	VA_LIST list;
	VA_START( list, aFmt );

    HBufC* buf = HBufC::New( KMMCScBkupLoggingMaxLogTextLength );
    if  ( buf )
        {
        TPtr pBuf( buf->Des() );
        pBuf.FormatList(aFmt, list);
#ifdef _DEBUG
        LogRaw( *buf );
#endif
        RFile64 file;
        RFs& fsSession(CCoeEnv::Static()->FsSession());
        TEntry entry;
        TInt error;
        
        if  ( fsSession.Entry( KMMCScBkupLoggingFullPathAndName, entry ) == KErrNone )
            {
            error = file.Open( fsSession, KMMCScBkupLoggingFullPathAndName, EFileWrite | EFileShareExclusive );
            }
        else
            {
            entry.iSize = 0;
            error = file.Create( fsSession, KMMCScBkupLoggingFullPathAndName, EFileWrite | EFileShareExclusive );
            }
        
        if(error == KErrNone)
            {
            TInt64 size = 0;
            
            error = file.Size( size );
            if(error == KErrNone && size < KMMCScBkupLoggingMaxLogSize)
                {
                HBufC8* buf8 = HBufC8::New( KMMCScBkupLoggingMaxLogTextLength );
                if(buf8)
                    {
                    const TText8 KLineFeed( '\n' );
                    TPtr8 data( buf8->Des() );
                    
                    data.Copy(buf->Des());
                    data.Append(KLineFeed);
                    file.Write(static_cast<TInt64>(entry.iSize), data);
                    delete buf8;
                    }
                }
            }
        file.Close();
        delete buf;
        }
    }

#endif

