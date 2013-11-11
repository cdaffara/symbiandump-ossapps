/*
* Copyright (c) 2004 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony logger
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "cvtlogger.h"


// LOCAL CONSTANTS
_LIT( KVtEngLogConfig, "c:\\Logs\\VT\\VTLOGCONF.txt" );
_LIT( KVtEngLogFolder, "c:\\Logs\\VT" );
_LIT( KVtEngLogFolderLogger, "VT" );
_LIT( KVtEngLogPrefix, "VT: " );
_LIT( KVtEngLogExtension, ".txt" );
_LIT( KVtLoggerEnterFunc, "<" );
_LIT( KVtLoggerExitFunc, ">" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtLogger::CVtLogger
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtLogger::CVtLogger() : CActive( CActive::EPriorityStandard - 1 ),
    iLogLevel( ELogLogEverything )
    {
    }

// -----------------------------------------------------------------------------
// CVtLogger::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtLogger::ConstructL( )
    {
    User::LeaveIfError( iFs.Connect() );
    TFindFile find( iFs );
    TUint logLevel( 0 );
    TInt ret = iFs.Att(KVtEngLogFolder, logLevel );
    if ( ret == KErrNone )
        {
        if ( find.FindByPath( KVtEngLogConfig, NULL ) == KErrNone )
            {
            CActiveScheduler::Add( this );
            ReadCongigFile();
            }
        if ( iLogLevel & ELogToFile )
            {
            RThread thread;
            iFileName.Copy( thread.Name() );
            iFileName.Append( KVtEngLogExtension );
            }
        }
    Dll::SetTls( this );
    }

// -----------------------------------------------------------------------------
// CVtLogger::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtLogger* CVtLogger::NewL( )
    {
    CVtLogger* self = new( ELeave ) CVtLogger;

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CVtLogger::~CVtLogger
// Destructor
// -----------------------------------------------------------------------------
//
CVtLogger::~CVtLogger()
    {
    Cancel();
    iFs.Close();
    Dll::SetTls( NULL );
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Writes to log.
// -----------------------------------------------------------------------------
//
void CVtLogger::Print( TDesC& aDes )
    {
    if ( iLogLevel & ELogTime && !(iLogLevel & ELogToFile) )
        {
        TTime time;
        time.HomeTime();
        const TDateTime dt= time.DateTime();

        iTempBuf.Zero();
        iTempBuf.AppendNum( dt.Minute() );
        iTempBuf.Append(':');
        iTempBuf.AppendNum( dt.Second() );
        iTempBuf.Append(':');
        iTempBuf.AppendNum( dt.MicroSecond() );
        iTempBuf.Append(' ');

        const TInt currentLength( iTempBuf.Length() );

        iTempBuf.Append( aDes.Left( Min( iTempBuf.MaxLength() - currentLength, aDes.Length() ) ) );
        RDebug::Print( iTempBuf );
        }
    else
        {
        if ( iLogLevel & ELogToFile )
            {
            RFileLogger::Write(
                KVtEngLogFolderLogger,
                iFileName,
                EFileLoggingModeAppend,
                aDes );
            }
        else
            {
            RDebug::Print( aDes );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Writes to log.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print(
    TInt aLevel,
    TPtrC aBuffer )
    {
    CVtLogger* logger = CVtLogger::Logger();
    if ( logger && logger->CheckLevels( aLevel ) )
        {
        logger->iBuffer.Copy( KVtEngLogPrefix );
        logger->iBuffer.Append( aBuffer);
        logger->Print( logger->iBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Writes to log.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print(
    TInt aLevel,
    TPtrC aBuffer,
    TInt aValue )
    {
    CVtLogger* logger = CVtLogger::Logger();
    if ( logger && logger->CheckLevels( aLevel ) )
        {
        logger->iBuffer.Zero();
        logger->iBuffer.AppendNum( aValue );
        logger->iBuffer.Format( aBuffer, aValue );
        logger->iBuffer.Insert( 0, KVtEngLogPrefix );
        logger->Print( logger->iBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Writes to log.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print(
    TInt aLevel,
    TPtrC aBuffer,
    TInt aValue1,
    TInt aValue2 )
    {
    CVtLogger* logger = CVtLogger::Logger();
    if ( logger && logger->CheckLevels( aLevel ) )
        {
        logger->iBuffer.Format( aBuffer, aValue1, aValue2 );
        logger->iBuffer.Insert( 0, KVtEngLogPrefix );
        logger->Print( logger->iBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CVtLogger::Print
// Writes to log.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Print(
            TInt aLevel,
            const TBool aEntry,
            TPtrC aBuffer,
            TInt aRetVal )
    {
    CVtLogger* logger = CVtLogger::Logger();
    if ( logger && logger->CheckLevels( aLevel ) )
        {
        logger->iBuffer.Copy( KVtEngLogPrefix );
        if ( aEntry )
            {
            logger->iBuffer.Append( aBuffer );
            logger->iBuffer.Append( KVtLoggerEnterFunc );
            }
        else
            {
            if ( aRetVal != KVtDebugReturnValNotApplicable )
                {
                logger->iTempBuf.Format( aBuffer, aRetVal );
                logger->iBuffer.Append( logger->iTempBuf );
                }
            else
                {
                logger->iBuffer.Append( aBuffer );
                }
            logger->iBuffer.Append( KVtLoggerExitFunc );
            }
        logger->Print( logger->iBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CVtLogger::Logger
// Returns logger.
// -----------------------------------------------------------------------------
//
CVtLogger* CVtLogger::Logger()
    {
    TAny* tls = Dll::Tls();
    if ( tls )
        {
        return reinterpret_cast<CVtLogger*>( tls );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVtLogger::Initialize
// Initialize logger.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Initialize()
    {
    CVtLogger* logger = NULL;
    if ( Dll::Tls() == NULL )
        {
        TRAPD( err, logger = CVtLogger::NewL() );
        if ( err == KErrNone )
            {
            Dll::SetTls( logger );
            }
        }
    else
        {
        logger = CVtLogger::Logger();
        }
    if ( logger )
        {
        (logger->iAccessCount)++;
        }

    }

// -----------------------------------------------------------------------------
// CVtLogger::Uninitialize
// Uninitialize logger.
// -----------------------------------------------------------------------------
//
EXPORT_C void CVtLogger::Uninitialize()
    {
    TAny* tls = Dll::Tls();
    if ( tls )
        {
        CVtLogger* logger = CVtLogger::Logger();
        if( logger )
            {
            TInt count( --(logger->iAccessCount) );
            if ( !count )
                {
                delete reinterpret_cast<CVtLogger*>( tls );
                Dll::SetTls( NULL );
                }
            }
        }

    }

void CVtLogger::ReadCongigFile()
    {
#ifdef _DEBUG
    RDebug::Print(_L(" VT: ReadConfigFile" ) );
#endif
    RFile config;
    if ( config.Open( iFs, KVtEngLogConfig, EFileShareAny ) == KErrNone )
        {
        TBuf8<16> buf;
        if ( config.Read( buf ) == KErrNone )
            {
            TLex8 lex( buf );
            TUint logLevel( 0 );
            if ( lex.Val( logLevel, EHex ) == KErrNone )
                {
                iLogLevel = logLevel;
                }
            }
        config.Close();
        }
    iFs.NotifyChange( ENotifyEntry, iStatus, KVtEngLogConfig() );
    SetActive();
    }

void CVtLogger::RunL()
    {
    if ( iStatus == KErrNone )
        {
        ReadCongigFile();
        }
    }

void CVtLogger::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }

// -----------------------------------------------------------------------------
// CVtLogger::Flags
// Returns flags.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVtLogger::Flags()
    {
    CVtLogger* logger = CVtLogger::Logger();
    if ( logger )
        {
        return logger->iLogLevel;
        }
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CVtLogger::CheckLevels
// Check debug level.
// -----------------------------------------------------------------------------
//
TBool CVtLogger::CheckLevels( TInt aLevels ) const
    {
    if ( aLevels & iLogLevel )
        {
        return ETrue;
        }
    return EFalse;
    }
#pragma CTC ENDSKIP
//  End of File
