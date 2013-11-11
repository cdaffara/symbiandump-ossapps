/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes
#include "cradioenginelogger.h"
#include "cradioenginetls.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Return the logger instance.
// ---------------------------------------------------------------------------
//
EXPORT_C MRadioEngineLogger* MRadioEngineLogger::Logger()
    {
    return CRadioEngineTls::Logger();
    }

#ifdef LOGGING_ENABLED

#include <bautils.h>

// Indentation
_LIT( KLogSpace,    "                                " ); // 32 empty spaces
const TInt KMaxIndent = 32;

/**
 * Timestamp formatter string
 */
_LIT8( KTimestampFormat, "%02d.%02d.%4d %02d:%02d:%02d.%05d" );

/**
 * Timestamp separator string
 */
_LIT8( KTimestampSeparator, " " );

// Memory card path. Has to contain directory ( \\) or BaflUtils::PathExists fails
_LIT( KMemoryCard, "E:\\" );

_LIT( KLogEnter,     "\\ %S" );
_LIT( KLogExit,      "/ %S" );
_LIT8( KLogLine,     "| " );
_LIT( KLogLeave,     "#+ %S: LEAVE!" );
_LIT( KLogExitRet,   "/ %S, Returning " );

_LIT8( KNewLine, "\r\n" );
_LIT8( KHexFormat, "ptr: 0x%X" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineLogger* CRadioEngineLogger::NewL( RFs& aFs )
    {
    CRadioEngineLogger* self = new (ELeave) CRadioEngineLogger( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineLogger::CRadioEngineLogger( RFs& aFs )
    : iFs( aFs )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineLogger::ConstructL()
    {
    if ( KLogFile().FindF( KMemoryCard ) == 0 )
        {
        TBool readOnly = EFalse;

        // Returns KErrPathNotFound if memory card is not present
        TInt err = BaflUtils::DiskIsReadOnly( iFs, KMemoryCard, readOnly );
        if ( err || readOnly )
            {
            // Log file path points to the memory card and the card is not
            // present or is read only => change the drive to C:
            TFileName fileName( _L( "C:" ) );
            TParsePtrC parse( KLogFile );
            fileName.Append( parse.Path() );
            fileName.Append( parse.NameAndExt() );
            iFileName = fileName.AllocL();
            }
        }

    if ( !iFileName )
        {
        iFileName = KLogFile().AllocL();
        }

    BaflUtils::EnsurePathExistsL( iFs, *iFileName );

    User::LeaveIfError( Dll::SetTls( this ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineLogger::~CRadioEngineLogger()
    {
    iFile.Close();
    delete iFileName;
    }

// ---------------------------------------------------------------------------
// Clear the log by deleting the logfile.
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::ClearLog()
    {
    if ( BaflUtils::FileExists( iFs, *iFileName ) )
        {
        // Returned error code ignored intentionally because there is no way
        // to report an error
        BaflUtils::DeleteFile( iFs, *iFileName );
        }

    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a 8-bit string to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const TDesC8& aMsg )
    {
    AddDesC<TPtrC8, TDesC8>( aMsg );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a 16-bit string to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const TDesC& aMsg )
    {
    // Note! No character conversion is performed while going from 16-bit
    // descriptor to 8-bit descriptor. This is considered acceptable in a
    // debugging utility
    AddDesC<TPtrC, TDesC>( aMsg );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a TInt to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( TInt aInt )
    {
    TBuf8<20> buf8;
    buf8.Num( aInt );
    Add( buf8 );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a TReal to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const TReal& aReal )
    {
    TBuf8<20> buf8;
    buf8.Format( _L8( "%f" ), aReal );
    Add( buf8 );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a c-style string to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const char* aText )
    {
    Add( TPtrC8( reinterpret_cast<const TText8*>( aText ) ) );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a pointer value to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const TAny* aPtr )
    {
    TBuf8<20> buf8;
    buf8.Format( KHexFormat, aPtr );
    Add( buf8 );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a timestamp of current time to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Add( const TTime& aTime )
    {
    TTimestamp timestamp;
    ParseTimestamp( aTime, timestamp );
    Add( timestamp );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds a timestamp of current time to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::AddTimestamp()
    {
    TTime now;
    now.HomeTime();
    return Add( now );
    }

// ---------------------------------------------------------------------------
// Adds a formatted string to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::AddFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    TBuf<KLogBufferLength> buf16;

    // Calls Overflow() if it doesn't fit
    buf16.AppendFormatList( aFmt, list, this );

    VA_END( list );

    Add( buf16 );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds the line indentation with line prefix to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::AddIndent( const TDesC& aMarker )
    {
    Add( aMarker );
    Add( KLogSpace().Mid( 0, iIndent ) );
    Add( KLogLine() );
    return *this;
    }

// ---------------------------------------------------------------------------
// Adds the line indentation to log line
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::AddIndentClear( const TDesC& aMarker )
    {
    Add( aMarker );
    Add( KLogSpace().Mid( 0, iIndent ) );
    return *this;
    }

// ---------------------------------------------------------------------------
// From TDes16Overflow
// Handles the overflow from AppendFormatList()
// ---------------------------------------------------------------------------
//
void CRadioEngineLogger::Overflow( TDes16& /*aDes*/ )
    {
    // aDes contains the part that did fit in the descriptor, but the part that
    // didn't is lost. Modifying the descriptor here would modify it in AddFormat(),
    // but since it gets logged there we only need to add a message about the overflow.
    Add( _L( "FORMAT OVERFLOW! " ) );
    }

// ---------------------------------------------------------------------------
// Increment indentation
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::IncIndent()
    {
    if ( ++iIndent > KMaxIndent )
        {
        iIndent = KMaxIndent;
        }

    return *this;
    }

// ---------------------------------------------------------------------------
// Decrement indentation
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::DecIndent()
    {
    if ( --iIndent < 0 )
        {
        iIndent = 0;
        }

    return *this;
    }

// ---------------------------------------------------------------------------
// Commits the log line to file and RDebug and resets internal buffer
// ---------------------------------------------------------------------------
//
MRadioEngineLogger& CRadioEngineLogger::Commit( TBool aNewLine )
    {
    // Write log buffer to RDebug
    RDebug::RawPrint( iBuf8 );

    // Write log buffer to file
    TInt err = iFile.Open( iFs, *iFileName, EFileWrite );
    if ( err )
        {
        err = iFile.Create( iFs, *iFileName, EFileWrite );
        }

    if ( !err )
        {
        TInt unused = 0;
        if ( iFile.Seek( ESeekEnd, unused ) == KErrNone )
            {
            if ( !iTimeStampWritten )
                {
                // First print a timestamp to log
                TTimestamp timestamp;
                TTime now;
                now.HomeTime();
                ParseTimestamp( now, timestamp );
                iFile.Write( timestamp );
                iFile.Write( KTimestampSeparator );
                iTimeStampWritten = ETrue;
                }

            iFile.Write( iBuf8 );

            if ( aNewLine )
                {
                iFile.Write( KNewLine );
                iTimeStampWritten = EFalse;
                }
            }

        iFile.Close();
        }

    iBuf8.Zero();
    return *this;
    }

// ---------------------------------------------------------------------------
// Returns the amount of characters that still fit in the buffer
// ---------------------------------------------------------------------------
//
TInt CRadioEngineLogger::Available() const
    {
    return iBuf8.MaxLength() - iBuf8.Length();
    }

// ---------------------------------------------------------------------------
// Templated function to add either 8-bit or 16-bit descriptor to buffer
// ---------------------------------------------------------------------------
//
template<class PTR, class DESC>
void CRadioEngineLogger::AddDesC( const DESC& aDesc )
    {
    PTR ptr( aDesc );
    while ( ptr.Length() > Available() )
        {
        PTR writePtr = ptr.Left( Available() );
        iBuf8.Append( writePtr );

        ptr.Set( ptr.Mid( writePtr.Length() ) );
        Commit( EFalse );
        }

    iBuf8.Append( ptr );
    }

// ---------------------------------------------------------------------------
// Parses the timestamp from the given TTime
// ---------------------------------------------------------------------------
//
void CRadioEngineLogger::ParseTimestamp( const TTime& aTime, TTimestamp& aTimestamp )
    {
    TDateTime dateTime = aTime.DateTime();
    aTimestamp.Zero();
    aTimestamp.Format( KTimestampFormat, dateTime.Day() + 1,
                                         dateTime.Month() + 1,
                                         dateTime.Year(),
                                         dateTime.Hour(),
                                         dateTime.Minute(),
                                         dateTime.Second(),
                                         dateTime.MicroSecond() );
    }

// ---------------------------------------------------------------------------
// Constructor. Log method entry
// ---------------------------------------------------------------------------
//
EXPORT_C TRadioMethodLogger::TRadioMethodLogger( const TText* aFunc,
                                                 const TText* aRetFormat )
    : iFunc( aFunc )
    , iRetFormat( aRetFormat )
    {
    MRadioEngineLogger* logger = MRadioEngineLogger::Logger();
    logger->AddIndentClear( KMarkerEngine ).AddFormat( KLogEnter, &iFunc ).Commit();
    logger->IncIndent();
    }

// ---------------------------------------------------------------------------
// Destructor. Log method exit
// ---------------------------------------------------------------------------
//
EXPORT_C TRadioMethodLogger::~TRadioMethodLogger()
    {
    MRadioEngineLogger::Logger()->DecIndent();

    if ( std::uncaught_exception() ) // Leave is an exception
        {
        // The function exited with a leave
        MRadioEngineLogger::Logger()->AddIndentClear( KMarkerEngine ).AddFormat( KLogLeave, &iFunc ).Commit();
        }
    else
        {
        // The function exited normally
        if ( iRetFormat.Length() == 0 )
            {
            MRadioEngineLogger::Logger()->AddIndentClear( KMarkerEngine ).AddFormat( KLogExit, &iFunc ).Commit();
            }
        else
            {
            TBuf<100> format( KLogExitRet );
            format.Append( iRetFormat );
            #if defined( __WINS__ )

                TInt32 retVal = 0;
                _asm( mov retVal, ebx );
                MRadioEngineLogger::Logger()->AddIndentClear( KMarkerEngine ).AddFormat( format, &iFunc, retVal ).Commit();

            #else

                MRadioEngineLogger::Logger()->AddIndentClear( KMarkerEngine ).AddFormat( KLogExit, &iFunc ).Commit();

            #endif
            }
        }
    }

#else // #ifdef LOGGING_ENABLED

// ===========================================================================
// Dummy function definitions to keep the exports unchanged when logging is turned off
// These can not be used.
// ===========================================================================
EXPORT_C TRadioMethodLogger::TRadioMethodLogger( const TText*, const TText* ) {}
EXPORT_C TRadioMethodLogger::~TRadioMethodLogger() {}

#endif // #ifdef LOGGING_ENABLED
