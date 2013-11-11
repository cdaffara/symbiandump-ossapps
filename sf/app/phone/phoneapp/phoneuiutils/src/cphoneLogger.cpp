/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneLogger class.
*
*/


// INCLUDE FILES
// System includes
#include <f32file.h> // for TParse, in Flogger stuff
#include <flogger.h>
#include <pevirtualengine.h>
#include <featmgr.h>        // for FeatureManager

#include "phoneconstants.h"
#include "phonelogger.h"

// Local includes
#include "phonestatestrings.h" // also includes StringPool.h
#include "phonestatestringsgsm.h" // also includes StringPool.h
#include "phoneloggerviewcommands.h" // also includes StringPool.h
#include "phoneui.pan"

// Class signature
#include "cphonelogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneLogger::CPhoneLogger
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructed with destructionPriority = -1, CPhoneLogger will then be 
// destroyed after the CCoeAppUi object is destroyed.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneLogger::CPhoneLogger( TUid aUid ) : CCoeStatic( aUid, -1 )
    {
    // Logging levels for each component is setted here
    iLogLevelArray[ EPhoneAbstractFactoryClient ] = ENo;
    iLogLevelArray[ EPhoneControl ] = EAll;
    iLogLevelArray[ EPhonePhoneapp ] = EAll;
    iLogLevelArray[ EPhoneUIStates ] = EAll;
    iLogLevelArray[ EPhoneUIView ] = EAll;
    iLogLevelArray[ EPhoneUIUtils ] = EAll;
    iLogLevelArray[ PhoneUIVoIPExtension ] = EAll;
    iLogLevelArray[ EPhoneMediatorCenter ] = EAll;
    }


// Destructor
EXPORT_C
CPhoneLogger::~CPhoneLogger()
    {
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
    iPool.Close();
#endif
    }

// -----------------------------------------------------------------------------
// CPhoneLogger::CheckLogLevel
// Checks if component that tries to log has big enough log level.
// -----------------------------------------------------------------------------
//
TBool CPhoneLogger::CheckLogLevel(
    TPhoneUILoggingLevel aLevel, 
    TPhoneUILoggingComponent aLoggingComponent )
    {
        __ASSERT_DEBUG( aLoggingComponent < iLogLevelArray.Count(),
        Panic( EPhoneUtilsIndexOutOfBounds ) );
        
    return ( aLevel <= iLogLevelArray[aLoggingComponent] );
    }

// -----------------------------------------------------------------------------
// CPhoneLogger::LogIt
// Messages to the log file.
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogIt( TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START( list, aFmt );

    TBuf<KMaxLogLineLength> buf;

    buf.AppendFormatList( aFmt, list, this );
    VA_END( list );

    WriteComment( buf );

    }
#else 
void CPhoneLogger::LogIt( TRefByValue<const TDesC> /*aFmt*/, ...) {}
#endif

// -----------------------------------------------------------------------------
// CPhoneLogger::LogIt
// Writes messages to the log.
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
EXPORT_C void CPhoneLogger::LogIt(
    TPhoneUILoggingLevel aLevel, 
    TPhoneUILoggingComponent aLoggingComponent, 
    TRefByValue<const TDesC> aFmt, ...)
    {
    if ( this == NULL ) return;
    if ( CheckLogLevel( aLevel, aLoggingComponent ) )
        {
        VA_LIST list;
        VA_START( list, aFmt );

        TBuf<KMaxLogLineLength> buf;

        buf.AppendFormatList( aFmt, list, this );
        VA_END( list );

        WriteComment( buf );  
        }   
    }
#else 
EXPORT_C void CPhoneLogger::LogIt(
    TPhoneUILoggingLevel /*aLevel*/, 
    TPhoneUILoggingComponent, 
    TRefByValue<const TDesC> /*aFmt*/, ...) {}
#endif

// -----------------------------------------------------------------------------
// CPhoneLogger::WriteComment
// Do a write of the supplied data, literally where possible
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::WriteComment( const TDesC& aComment )
    {
#if ( PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING )
    RDebug::Print( aComment );
#endif
#if (PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
    RFileLogger::Write( 
        KPhoneLogFolder(),
        KPhoneLogFileName(),
        EFileLoggingModeAppend,
        aComment);
#endif
    }
// Logging method == 0 (no logging at all) is handled so
// that no code is generated at all.
#else 
void CPhoneLogger::WriteComment( const TDesC& )
{
//TInt test;
}
#endif

// -----------------------------------------------------------------------------
// CPhoneLogger::Overflow
// Implements overflow handler derived from TDes16Overflow. 
// Used by TDes16::AppendFormatList().
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::Overflow( TDes16& aDes )
    {
    // Overflow has occured - end log line with '...'
    _LIT( KErrOverflowMsg, "..." );
    if ( aDes.MaxLength() >= KErrOverflowMsg().Length() + aDes.Length() )
        {
        aDes.Append( KErrOverflowMsg );
        }
    }
#else
void CPhoneLogger::Overflow( TDes& ) {}
#endif

// -----------------------------------------------------------------------------
// CPhoneLogger::LogMsgFromPhoneUIToPE
// LogPECommand: 
// Creates a PE specified log message by given command and 
// call id and writes it in the log file.
//
// NOTE:
//  If You make changes to enum TPACommandId, please do same 
//  changes to paloggermessagestophoneenginestrings.st and run abld 
//  makefile.
//
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogMsgFromPhoneUIToPE( 
    const TInt aMessage, 
    const TInt /*aCallId*/ )
    {
    // Log only if logging level for control is big enough.
    if ( iLogLevelArray[EPhoneControl] >= EBasic )
        {   
        TBuf<KMaxAppendLength> buf;
        GetPhoneUIToPEStringById( buf, aMessage );
        LogIt( KPEDefaultMsgToPE, &buf );
        }
    }

#else
void CPhoneLogger::LogMsgFromPhoneUIToPE( const TInt, const TInt ) {}
#endif //_DEBUG


// -----------------------------------------------------------------------------
// CPhoneLogger::LogMsgFromControlToView
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogMsgFromControlToView(
    const TInt aMessage, 
    const TInt /*aCallId*/ )
    {
    // Log only if logging level for Phoneapp is big enough.
    if ( iLogLevelArray[EPhoneUIView] >= EBasic )
        {   
        TBuf<KMaxAppendLength> buf;
        GetPhoneUIToViewStringById( buf, aMessage );
        LogIt( KPEDefaultMsgToView, &buf );
        }
    }

#else
// -----------------------------------------------------------------------------
// CPhoneLogger::LogMsgFromControlToView
// -----------------------------------------------------------------------------
//
void CPhoneLogger::LogMsgFromControlToView( const TInt, const TInt ) {}
#endif //_DEBUG

// -----------------------------------------------------------------------------
// CPhoneLogger::LogStateChange
// Writes state chage to log file
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogStateChange( 
    const TInt aCurrentState, 
    const TInt aNewState )
    {
    // Log only if logging level for Phone UI control is big enough.
    if ( ( iLogLevelArray[EPhoneControl] >= EBasic ) ||
        ( iLogLevelArray[EPhoneUIStates] >= EBasic ) )
        {           
        TBuf<KMaxAppendLength> currentState;
        TBuf<KMaxAppendLength> newState;
        
        GetStateName( currentState, aCurrentState );
        GetStateName( newState, aNewState );
        
        LogIt( KPEDefaultStateChange, &currentState, &newState );
        }
    }

#else
void CPhoneLogger::LogStateChange( const TInt, const TInt ) {}
#endif //_DEBUG

// -----------------------------------------------------------------------------
// CPhoneLogger::LogMsgFromPEToPhoneUI
// LogPEMessage: 
// Creates a PE specified log message by given parameters 
// and writes it in the log file.
//
// NOTE:
//  If You make changes to enum TPEMessagesFromPhoneEngine, please do same 
//  changes to phoneloggermessagesfromphoneenginestrings.st and run abld 
//  makefile.
//
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogMsgFromPEToPhoneUI( 
    const TInt aMessage, 
    const TInt aCallId  )
    {
    // Log only if logging level for application control is big enough.
    if ( iLogLevelArray[EPhoneControl] >= EBasic )
        {          
        TBuf<KMaxAppendLength> buf;
        GetPEToPhoneUIStringById( buf, aMessage );
        LogIt( KPEDefaultMsgToPhoneUI, &buf, aCallId );
        }
    }

#else
void CPhoneLogger::LogMsgFromPEToPhoneUI ( const TInt, const TInt ) {}
#endif //_DEBUG

// -----------------------------------------------------------------------------
// CPhoneLogger::LogMsgFromPEToPhoneUIEnd
// -----------------------------------------------------------------------------
//
EXPORT_C
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::LogMsgFromPEToPhoneUIEnd( const TInt aMessage )
    {
    // Log only if logging level for application control is big enough.
    if ( iLogLevelArray[EPhoneControl] >= EBasic )
        {           
        TBuf<KMaxAppendLength> buf;
        GetPEToPhoneUIStringById( buf, aMessage );
        LogIt( KPEDefaultMsgToPhoneUIEnd, &buf );
        }
    }

#else
void CPhoneLogger::LogMsgFromPEToPhoneUIEnd ( const TInt ) {}
#endif //_DEBUG


// -----------------------------------------------------------------------------
// CPhoneLogger::GetPEToPhoneUIStringById
// Provides string representation for TPEMessagesFromPhoneEngine enums by 
// ordinal number.
//
// NOTE:
//  If You make changes to enum TPEMessagesFromPhoneEngine, please do same 
//  changes to phoneloggermessagesfromphoneenginestrings.st and run abld 
//  makefile.
//
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::GetPEToPhoneUIStringById( TDes16& aDes, const TInt aID )
    {
    ASSERT( iPhoneEngine );
    aDes.Copy( iPhoneEngine->NameByMessageFromPhoneEngine( aID ) );
    }

#else
void CPhoneLogger::GetPEToPhoneUIStringById( TDes16& , const TInt ) {}
#endif //_DEBUG


// -----------------------------------------------------------------------------
// CPhoneLogger::GetStateName
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::GetStateName( TDes16& aDes, const TInt aID )
    {
    TStringTable table;
    TInt stateID;
    if ( aID < KPhoneStateProtocolFirst )
        {
        table = PhoneStateStrings::Table;
        stateID = aID;
        }
    else 
        {
        stateID = aID - KPhoneStateProtocolFirst;

        if ( FeatureManager::FeatureSupported( KFeatureIdProtocolGsm ) ||
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma ) )
            {
            table = PhoneStateStringsGSM::Table;
            }
        }

    TRAPD( err, iPool.OpenL( table ) );
    if ( err == KErrNone )
        {
        aDes.Copy( iPool.StringF( stateID, table ).DesC() );
        iPool.Close();
        }

    if ( ( aDes.Length() == 0 ) || ( err != KErrNone ) )
        {
        aDes.AppendNum( (TInt)aID );
        }
    }

#else
void CPhoneLogger::GetStateName( TDes16& , const TInt ) {}
#endif //_DEBUG
/*
// ==========================================================
// GetViewName: 
// Appends view name to given buf.
//
// ==========================================================
#if (PHONEUI_LOGGING_OUTPUT == 1 || PHONEUI_LOGGING_OUTPUT == 2)
void CPhoneLogger::GetViewName( TDes16& aDes, const TInt aID )
    {
    TRAPD( err, iPool->OpenL( PhoneUIViewStrings::Table ) );
    if( err == KErrNone )
        {
        aDes.Copy(iPool->StringF( aID, PAViewStrings::Table ).DesC());
        iPool->Close();
        }

    if( ( aDes.Length() == 0 ) || ( err != KErrNone ) )
        {
        aDes.AppendNum( (TInt)aID );                                                                                            
        }
    }

#else
void CPhoneLogger::GetViewName( TDes16& , const TInt ) {}
#endif //_DEBUG
*/

// -----------------------------------------------------------------------------
// CPhoneLogger::GetPhoneUIToPEStringById
// Provides string representation for TPACommandId enums by 
// ordinal number.
//
// NOTE:
//  If You make changes to enum TPACommandId, please do same 
//  changes to paloggermessagestophoneenginestrings.st and run abld 
//  makefile.
//
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::GetPhoneUIToPEStringById( TDes16& aDes, const TInt aID )
    {
    ASSERT( iPhoneEngine );
    aDes.Copy( iPhoneEngine->NameByMessageToPhoneEngine( aID ) );
    }

#else
void CPhoneLogger::GetPhoneUIToPEStringById( TDes16& , const TInt ) {}
#endif //_DEBUG

// -----------------------------------------------------------------------------
// CPhoneLogger::GetPhoneUIToViewStringById
// Provides string representation for TPACommandId enums by 
// ordinal number.
//
// NOTE:
//  If You make changes to enum TPACommandId, please do same 
//  changes to paloggermessagestophoneenginestrings.st and run abld 
//  makefile.
//
// -----------------------------------------------------------------------------
//
#if (PHONEUI_LOGGING_OUTPUT == RDEBUG_LOGGING || PHONEUI_LOGGING_OUTPUT == FILE_LOGGING)
void CPhoneLogger::GetPhoneUIToViewStringById( TDes16& aDes, const TInt aID )
    {
    TRAPD( err, iPool.OpenL( PhoneLoggerviewCommands::Table ) );
    if ( err == KErrNone )
        {
        aDes.Copy( iPool.StringF( aID, 
            PhoneLoggerviewCommands::Table ).DesC() );
        iPool.Close();
        }

    if ( ( aDes.Length() == 0 ) || ( err != KErrNone ) )
        {
        aDes.AppendNum( (TInt)aID );
        }
    }

#else
void CPhoneLogger::GetPhoneUIToViewStringById( TDes16& , const TInt ) {}
#endif //_DEBUG

// ---------------------------------------------------------
// CPhoneLogger::SetPhoneEngine
// ---------------------------------------------------------
//
EXPORT_C void CPhoneLogger::SetPhoneEngine( 
    MPEPhoneModel* aPhoneEngine )
    {
    iPhoneEngine = aPhoneEngine;
    }

// -----------------------------------------------------------------------------
// CPhoneLogger::CPhoneMethodLogger
// Method logger contructor. Logs method start.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneMethodLogger::CPhoneMethodLogger(
    TPhoneUILoggingComponent aLoggingComponent, 
    TPtrC aString )
    {
    iLoggingComponent = aLoggingComponent;
    iString = aString;
    TBuf<KMaxLogLineLength> str;
    switch( iLoggingComponent )
        {
        case EPhoneControl:
            str.Append( KPhoneUIControl );
            break;
        
        case EPhonePhoneapp:
            str.Append( KPhoneUI );
            break;
            
        case EPhoneUIView:
            str.Append( KPhoneUIView );
            break;
            
        case EPhoneUIStates:
            str.Append( KPhoneUIStates );
            break;
            
        case EPhoneUIUtils:
            str.Append( KPhoneUIUtils );
            break;

        case EPhoneMediatorCenter:
            str.Append( KPhoneMediatorCenter );
            break;
                        
        default:
            break;
        }
        
    str.Append( iString );
    str.Append( KLessThan );
    
    CPhoneLogger * logger = static_cast<CPhoneLogger*>(
        CCoeEnv::Static( KUidPhoneUILoggerSingleton ) );
    if ( logger )
        {
        logger->LogIt( EMethodStartEnd, iLoggingComponent, str );
        }
    }

// Destructor
EXPORT_C CPhoneMethodLogger::~CPhoneMethodLogger()
    {
    TBuf<KMaxLogLineLength> str;
    switch( iLoggingComponent )
        {
        case EPhoneControl:
            str.Append( KPhoneUIControl );
            break;
        
        case EPhonePhoneapp:
            str.Append( KPhoneUI );
            break;
            
        case EPhoneUIView:
            str.Append( KPhoneUIView );
            break;
            
        case EPhoneUIStates:
            str.Append( KPhoneUIStates );
            break;
            
        case EPhoneUIUtils:
            str.Append( KPhoneUIUtils );
            break;

        case EPhoneMediatorCenter:
            str.Append( KPhoneMediatorCenter );
            break;
            
        default:
            break;
        }
    str.Append( iString );
    str.Append( KGreaterThan );

    CPhoneLogger * logger = static_cast<CPhoneLogger*>(
        CCoeEnv::Static( KUidPhoneUILoggerSingleton ) );
    if ( logger )
        {
        logger->LogIt( EMethodStartEnd, iLoggingComponent, str );
        }
    }

//  End of File  
 
