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
* Description:  The implementation for presentation elements.
*
*/


#include <coemain.h>
#include <ezgzip.h>
#include <sysutil.h>
#include <utf.h>
#include <etel3rdparty.h>
#include <imcvcodc.h>
#include "irdebug.h"
#include "irreportsettings.h"
#include "irsessiondb.h"
#include "irsessionlogger.h"
#include "irsessionloggerutility.h"
#include "irsettings.h"
#include "irnmslogdb.h"
#include "irnmslogger.h"
#include "irdbstatusobserver.h"
#include "irphoneinfo.h"

const TUint KFieldPosition = 5;
const TInt KAmpMaxLength = 5;
const TInt KFormatBuffMaxLength = 6;
const TInt KMaxUrlSize = 256;
const TUint KSessionLogGranularity = 4;
const TUint KNmsLogGranularity = 5;
const TUint KSessionLogThreshold = 5; // Session Log Maximum 15
const TUint KNmsLogThreshold = 5;     // Nms Log Maximum 15

// ---------------------------------------------------------------------------
// Function: OpenL
// Two phase constructor returns the instance of CIRReportGenerator
// this is a single ton implementation
// ---------------------------------------------------------------------------
//
EXPORT_C CIRReportGenerator* CIRReportGenerator::OpenL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::OpenL" );
    CIRReportGenerator* reportGenerator = 
            reinterpret_cast<CIRReportGenerator*>( Dll::Tls() );
    
    if ( reportGenerator )
        {
        User::LeaveIfError( reportGenerator->Open() );
        IRLOG_INFO( "CIRReportGenerator::OpenL - \
                Opened singleton for new user." );
        }
    else
        {
        reportGenerator = new ( ELeave ) CIRReportGenerator;
        CleanupClosePushL( *reportGenerator );
        reportGenerator->ConstructL();
        User::LeaveIfError( Dll::SetTls( reportGenerator ) );
        CleanupStack::Pop( reportGenerator );
        IRLOG_INFO( "CIRSessionLogger::OpenL - \
                Created the CIRReportGenerator singleton." );
        }
    IRLOG_DEBUG( "CIRReportGenerator::OpenL - Exiting." );
    return reportGenerator;
    }


// ---------------------------------------------------------------------------
// Function: ~CIRReportGenerator
// default destructor
// ---------------------------------------------------------------------------
//
CIRReportGenerator::~CIRReportGenerator()    
    {
    IRLOG_DEBUG( "CIRReportGenerator::~CIRReportGenerator" );
    
    delete iImeiBase64;
    iImeiBase64 = NULL;
    
    delete iPhoneInfo;
    iPhoneInfo = NULL;
    
    //operation settings instance is removed
    if ( iSettingsData )
        {
        iSettingsData->Close(); // delete is handled like this.
        }
    delete iSessionLog;
    delete iNmsLog;
    
    //CIRSessionDb object is deleted
    delete iLogSessionDb;

    //CIRNmsLogDb object is deleted
    delete iNmsLogDb;
    //session data is also removed
    if ( iSessionData )
        {
        iSessionData->ResetAndDestroy();
        }
    delete iSessionData;
    //NmsLog data is also removed
    if ( iNmsLogData )
        {
        iNmsLogData->ResetAndDestroy();
        }
    delete iNmsLogData;    
    iBrowseLog.Close(); //browse log array is closed
    iLogFile.Close();
    //file session is closed
    iFsSession.Close();
    Dll::FreeTls();
    IRLOG_DEBUG( "CIRReportGenerator::~CIRReportGenerator - Exiting." );    
    }

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor
// ---------------------------------------------------------------------------
//    
void CIRReportGenerator::ConstructL()        
    {
    IRLOG_DEBUG( "CIRReportGenerator::ConstructL" );
    //file session is connected
    //Create the IROperation Settings Instance
    iSettingsData = CIRSettings::OpenL();
    User::LeaveIfError( iFsSession.Connect() );
    //session logger is created
    iSessionLog = CIRSessionLogger::NewL();
       //session logger is created
    iNmsLog = CIRNmsLogger::NewL();
    //session db is created
    iLogSessionDb = CIRSessionDb::NewL();
    //NmsLog db is created
    iNmsLogDb = CIRNmsLogDb::NewL();

    iSessFile =iSettingsData->PrivatePath();
    iSessFile.Append( KSession );
    TInt error = iLogSessionDb->CreateDbConditional( iSessFile );
    if ( error )
        {
        iSessionDbCreationFailed = ETrue;
        }
    else
        {
        iSessionDbCreationFailed = EFalse;    
        }
  
    iNmsLogFile =iSettingsData->PrivatePath();
    iNmsLogFile.Append( KNmsLog );
    TInt nmsError = iNmsLogDb->CreateDbConditional( iNmsLogFile );
    if ( nmsError )
        {
        iNmsDbCreationFailed = ETrue;
        }
    else
        {
        iNmsDbCreationFailed = EFalse;    
        }

  
      //session log for five session are to be stored
      //and is stored in session data      
    iSessionData = new (ELeave) CArrayPtrFlat<CIRSessionLogger>(KSessionLogGranularity);
    iNmsLogData = new ( ELeave ) CArrayPtrFlat<CIRNmsLogger>( KNmsLogGranularity );
        
    iPhoneInfo = CPhoneInfo::NewL(this);
    iPhoneInfo->StartUpdating();
    
    IRLOG_DEBUG( "CIRReportGenerator::ConstructL - Exiting." );    
    }

// ---------------------------------------------------------------------------
// Function: CIRReportGenerator
// default constructor
// ---------------------------------------------------------------------------
//
CIRReportGenerator::CIRReportGenerator():iPhoneInfo(NULL),
                                         iDbStatusObserver(NULL),
                                         iNmsLogDbUpdated( EFalse ),
                                         iSessionLogDbUpdated( EFalse ),
                                         iUnComfirmedReportSent( EFalse ),
                                         iSessionLogUpdationFailed(EFalse),
                                         iNmsLogUpdationFailed(EFalse),
                                         iSessionDbCreationFailed(EFalse),
                                         iNmsDbCreationFailed(EFalse),
                                         iImeiBase64(NULL)
    {
    //No Implementation    
    }

// ---------------------------------------------------------------------------
// Function: LogServerResult
// logs server connection result
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::LogServerResult( const TDesC& aUrl,
        TResponse aResult )    
    {
    iSessionLog->LogServerResult( aUrl,aResult );
    }

// ---------------------------------------------------------------------------
// Function: LogSongRecog
// logs song recog
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::LogSongRecog()  
    {
    //taking the session start time in GMT    
    iSongRecogTimestamp.UniversalTime();
    TTimeIntervalSeconds offset( iSettingsData->TimeCorrection() );
    iSongRecogTimestamp -= offset;
    iDate = iSongRecogTimestamp.DateTime();
    TBuf<64> songRecogTimestamp;
    FormatDateTime( songRecogTimestamp, iDate );
    iSessionLog->LogSongRecog( songRecogTimestamp );
    }

// ---------------------------------------------------------------------------
// Function: UpdateCurrentBrowseNetwork
// logs network browsing
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateCurrentBrowseNetwork( TInt aNetwork )
    {
    iCurrentBrowseNetwork = aNetwork;
    }

// ---------------------------------------------------------------------------
// Function: LogBrowse
// logs browse url and status information
// ---------------------------------------------------------------------------
//    
EXPORT_C void CIRReportGenerator::LogBrowse( const TDesC8& aUrl, TInt aStatus )    
    {
    IRLOG_DEBUG( "CIRReportGenerator::LogBrowse" );
    //browse url is copied
    iRBrowseLog.iBrowseUrl.Copy( aUrl );
    //time stamp in GMT is onbtained
    iBrowseLogTimeStamp.UniversalTime();

    TInt time_val = 0;
    time_val = iSettingsData->TimeCorrection();
    TTimeIntervalSeconds offset( time_val );

    iBrowseLogTimeStamp -= offset;
    iDate = iBrowseLogTimeStamp.DateTime();
    //formated
    FormatDateTime( iRBrowseLog.iTimeStamp, iDate );
    _LIT( KFormat, "%d" );
    ( iRBrowseLog.iCurrentNetwork ).Format( KFormat,
                                    iCurrentBrowseNetwork );    
    //status is stored
    iRBrowseLog.iStatus = aStatus;
    TRAP_IGNORE( iBrowseLog.AppendL( iRBrowseLog ) )
    IRLOG_DEBUG( "CIRReportGenerator::LogBrowse - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: SessionStartedL
// starts a new session
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::SessionStartedL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::SessionStartedL" );
    //gets the session id for new session
    iSessionLog->ResetResource();
    
    iSessionLog->UpdateSessionId(iSettingsData->GetLogRunningNo());
    iSettingsData->SetLogRunningNoL();
    iFileWritePos = 0;    
    
    iLogFilePath = iSettingsData->PrivatePath();
    iLogFilePath.Append( KLogFileName );
    //taking the session start time in GMT    
    iSessionStartTime.UniversalTime();
    iSessionLog->UpdateSessionStartGMTTime();
    TTimeIntervalSeconds offset( iSettingsData->TimeCorrection() );
    iSessionStartTime -= offset;
    iDate = iSessionStartTime.DateTime();
    FormatDateTime( iSessionLog->StartTime(), iDate );
    iSessionLog->UnMarkSession();
    //collect the neccessary information    
    CollectSettingsData();
    iSessionLogUpdationFailed = EFalse;
    //start adding the intial information in db    
    IRLOG_DEBUG( "CIRReportGenerator::SessionStartedL - Exiting." );    
    }
// ---------------------------------------------------------------------------
// Function: NmsLogStartedL
// starts a new nmslog
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::NmsLogStartedL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::SessionStartedL" );
    iNmsTimestamp.UniversalTime();
    TInt time_val = 0;
    time_val = iSettingsData->TimeCorrection();
    TTimeIntervalSeconds offset( time_val );

    iNmsTimestamp -= offset;
    iDate = iNmsTimestamp.DateTime();
    //formated
    FormatDateTime( iNmsLog->StartTime(), iDate );    
    iNmsLogUpdationFailed = EFalse;
    //start adding the intial information in db    
    IRLOG_DEBUG( "CIRReportGenerator::SessionStartedL - Exiting." );    
    }
    
// ---------------------------------------------------------------------------
// Function: SessionEnd
// marks the a session
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::SessionEndL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::SessionEndL" );
    iSessionLog->EndLogging();
    UnForceSessionEndedL(iSessionLog);
    IRLOG_DEBUG( "CIRReportGenerator::SessionEndL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: UpdateTerminatedBy
// updates the termination status
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateTerminatedBy( TTerminationStatus 
    aTerminatedBy )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateTerminatedBy" );
    iSessionLog->UpdateTerminationStatus( aTerminatedBy );      
    }

// ---------------------------------------------------------------------------
// Function: GetConnectedFrom
// connected information is stored
// ---------------------------------------------------------------------------
//    
EXPORT_C void CIRReportGenerator::UpdateConnectedFrom(TConnectedFrom aConnectedFrom)
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateConnectedFrom" );
    iSessionLog->GetConnectedFrom( aConnectedFrom );
    }
    
// ---------------------------------------------------------------------------
// Function: BrowseUrl
// logs browse url and result
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::BrowseUrl( const TDesC& aBrowseUrl,
                                                    TInt aStatus )
    {
    IRLOG_DEBUG( "CIRReportGenerator::BrowseUrl" );
    _LIT( KAmp, "&amp;" );
    TBuf8<KAmpMaxLength> amp;
    amp.Copy( KAmp );    
    TBuf8<KMaxUrlSize> url;
    url.Copy( aBrowseUrl );
    _LIT8( KAmpnd, "&" );
    TInt pos = url.Find( KAmpnd );
    if ( pos > 0 )
        {
        url.Replace( pos, 1, amp );    
        }
    LogBrowse( url, aStatus );
    IRLOG_DEBUG( "CIRReportGenerator::BrowseUrl - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: UpdateChannelID
// updates the channel id
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateChannelID( TInt aChanneldID )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateChannelID" );
    iSessionLog->UpdateChannelID( aChanneldID );
    }

// ---------------------------------------------------------------------------
// Function: UpdateCurrentNetwork
// updates the current network
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateCurrentNetwork( TInt aCurrentNetwork )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateCurrentNetwork" );
    iSessionLog->UpdateCurrentNetwork( aCurrentNetwork );
    }

// ---------------------------------------------------------------------------
// Function: UpdateHomeOperator
// updates the home network
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateHomeOperator( TInt aHomeOperator )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateHomeOperator" );
    iSessionLog->UpdateHomeOperator( aHomeOperator );
    }

// ---------------------------------------------------------------------------
// Function: MarkSessionStart
// updates the termination status
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::MarkSessionStart()
    {
    iSessionStartTime.UniversalTime();

    TInt time_val = 0;
    time_val = iSettingsData->TimeCorrection();
    TTimeIntervalSeconds offset( time_val );
    
    iSessionStartTime -= offset; 
    iDate = iSessionStartTime.DateTime();
    FormatDateTime( iSessionLog->StartTime(), iDate );
    //setting session start time
    iSessionLog->UpdateSessionStartGMTTime();
    iSessionLog->MarkSession();
    }

// ---------------------------------------------------------------------------
// Function: HandleStopEvents
// function handles session state changes
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::HandleStopEvents( TBool aIsSessionProgressing )
    {
    IRLOG_DEBUG( "CIRReportGenerator::HandleStopEvents" );
    if ( !aIsSessionProgressing )
        {
        iSessionLog->HandleStopEvents();
        }
    else
        {
        iSessionLog->HandleStartEvents();
        }
    IRLOG_DEBUG( "CIRReportGenerator::HandleStopEvents - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: UpdateConnectionType
// updates the connection type
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateConnectionType( const TDesC& 
    aConnectionType )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateConnectionType" );
    iSessionLog->UpdateConnectionType( aConnectionType );
    }

// ---------------------------------------------------------------------------
// Function: UpdateNmsType
// updates the connection type
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateNmsType( const TDesC& aNmsType )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateNmsType" );
    iNmsLog->UpdateNmsType( aNmsType );
    }

// ---------------------------------------------------------------------------
// Function: SetDbStatusObserser
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::SetDbStatusObserser( 
        MIRLogDbStatusObserver *aDbStatusObserver )
{
    iDbStatusObserver = aDbStatusObserver;
}    

// ---------------------------------------------------------------------------
// Function: reportSent
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::ReportSent()
{
    iUnComfirmedReportSent = ETrue;
}    


// ---------------------------------------------------------------------------
// Function : ReponseFromISDSObtained
// This function is called when session log is successfully sent
// removes all session log entries from sessiondb
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::ReponseFromISDSObtained()
    {
    IRLOG_DEBUG( "CIRReportGenerator::ReponseFromISDSObtained" );
    iUnComfirmedReportSent = EFalse;
    TRAP_IGNORE( iLogSessionDb->DeleteAllSessionL() )
    TRAP_IGNORE( iNmsLogDb->DeleteAllNmsLogL() )
    for ( TInt i = 0; i < iBrowseLogsCntToXml; i++ )
        {
        if ( iBrowseLog.Count() > 0 )
            iBrowseLog.Remove( 0 );
        }
    }    

// ---------------------------------------------------------------------------
// Function: CollectSettingsData
// collect information during starting of session
// like irid, version information.
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::CollectSettingsData()
    {
    IRLOG_DEBUG( "CIRReportGenerator::CollectSettingsData" );
    _LIT( KData, "1.0" );
    _LIT( KIRID, "NO IRID" );
    //gets irid from opsetting
    TRAP_IGNORE( iIRId.Copy( iSettingsData->GetIRIDL() ) );
    //if length of irid is zero "no irid" is assigned
    if ( ( iIRId ).Length() == 0 )
       {
       iIRId.Copy( KIRID );
       }
    //version copied
    iReportVersion.Copy( KData );
    IRLOG_DEBUG( "CIRReportGenerator::CollectSettingsData - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: WriteToXmlL
// gathers the information and generates sesion log file and zip file
// file will be stored in private path
// ---------------------------------------------------------------------------
//    
void CIRReportGenerator::WriteToXmlL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteToXmlL" );    
    //1. log file is created
    User::LeaveIfError( iLogFile.Replace( iFsSession,
            iLogFilePath, EFileWrite ) );   
    iLogFile.Seek( ESeekStart,iFileWritePos );
    
    
    //2. Write file header.
    iUniCodeBuf.Copy(KXmlHeader);
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer,iUniCodeBuf );   
    WriteBufferToLogFile();
    
    //3. Write Version + IRID + IMEI
    iUniCodeBuf.Copy( KRPSTARTTAG );
    iFileWritePos =  iUniCodeBuf.Find( KField );
    //3.1 Write Version
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, iReportVersion );
    iFileWritePos = iUniCodeBuf.Find( KField );
    //3.2 Write IRID
    RBuf tempIRID;
    TRAP_IGNORE( tempIRID.CreateL( iSettingsData->GetIRIDL() ) );
    if ( ( tempIRID ).Length() != 0 )
        {
        iIRId.Copy( tempIRID );
        }
    tempIRID.Close();
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, iIRId );
    //3.3 Write IMEI
    iFileWritePos = iUniCodeBuf.Find(KField);
    if ( NULL == iImeiBase64 )
        {
        iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, KNullDesC );
        }
    else
        {
        iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, *iImeiBase64 );     
        }    
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    WriteBufferToLogFile();
    
    //4. Write session log
    if ( !iSessionLogUpdationFailed )
        {
        for( TInt count = 0; count<(iSessionData->Count()); count++ )
            {
            WriteSessionData( count );      
            }
        }
    
    //5. Write browse log           
    WriteBrowseLog();

    //6. Write nms log  
    if ( !iNmsLogUpdationFailed )
        {
        TInt nmsLogCount = iNmsLogData->Count();
        if(nmsLogCount > 0)
            {
            iUniCodeBuf.Copy(KNMSLOGSTARTTAG);
            CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
            WriteBufferToLogFile();
    
            for(TInt count = 0; count < nmsLogCount; count++)
                {
                WriteNmsLogData(count);     
                }
            iUniCodeBuf.Copy(KNMSLOGENDTAG);
            CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
            WriteBufferToLogFile(); 
            }   
        }

    //7. Write file ending
    iUniCodeBuf.Copy( KRPENDTAG );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    WriteBufferToLogFile();
    
    iLogFile.Close();
    ZipFileL();
    
    iBrowseLogsCntToXml = iBrowseLog.Count();
    
    IRLOG_DEBUG( "CIRReportGenerator::WriteToXmlL - Exiting." );
    }
// ---------------------------------------------------------------------------
// Function: WriteNmsLogtoXmlL
// logs nmsEvents to Xml file
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::WriteNmsLogtoXmlL()
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteNmsLogtoXmlL" );
    iNmsEventFlag = ETrue;
    UnForceSessionEndedL(iSessionLog);
    iNmsEventFlag = EFalse;
    IRLOG_DEBUG( "CIRReportGenerator::WriteNmsLogtoXmlL - Leaving" );
    }


// ---------------------------------------------------------------------------
// Function: WriteSessionData
// gathers session log data and writes to XML file
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::WriteSessionData( TInt aIndex )
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteSessionData" );
    
    ASSERT( aIndex >= 0 && aIndex < iSessionData->Count() );
    TBuf< KFormatBuffMaxLength > FormatBuff;
    _LIT( KConvert, "%d" );
    
    // 1. session start time is copied
    iUniCodeBuf.Copy( KSESSIONSTARTTAG );
    
    iFileWritePos = iUniCodeBuf.Find( KField );    
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
        At(aIndex))->StartTime() );
    
    // 2. session duration is copied
    iFileWritePos = iUniCodeBuf.Find( KField );    
    TInt duration = ( iSessionData->At(aIndex))->SessionDuration();
    FormatBuff.Format( KConvert, duration );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    
    // 3. termination status is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
        At(aIndex))->TerminationStatus() );
    
    // 4. connected from information is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
        At(aIndex))->ConnectedFrom() );
    
    // 5. session id is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    FormatBuff.Format( KConvert, ( iSessionData->At( aIndex ) )->SessionId() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    
    // 6. channel id is copied
    iFileWritePos = iUniCodeBuf.Find(KField);
    FormatBuff.Format( KConvert,( iSessionData->At( aIndex ) )->ChannelID() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    
    WriteBufferToLogFile();
        
    // 8. Write ServerResult    
    TInt ServerResultCount = ( iSessionData->At( aIndex ) )->
        SessionData().Count();
    
    for ( TInt count=0; count < ServerResultCount; count++ )
        {
        //channel server urls are copied for the session            
        iUniCodeBuf.Copy( KSERVERRESULTTAG );
        iFileWritePos = iUniCodeBuf.Find( KField );
        iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
        At( aIndex )->SessionData()[count].iServerUrl ) );
            
        //status is copied
        iFileWritePos = iUniCodeBuf.Find( KField );
        iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
            At(aIndex)->SessionData()[count].iServerResult) );
        CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
        //writting the information to the xml file    
        WriteBufferToLogFile();
        }
    
    // 9. connection type is copied
    iUniCodeBuf.Copy( KCONNECTIONTAG );
    iFileWritePos = iUniCodeBuf.Find( KField );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iSessionData->
        At( aIndex ) )->ConnectionType() );
    
    // 10. current network is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    FormatBuff.Format( KConvert, ( iSessionData->At( aIndex ) )->
            CurrentNetwork() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    
    // 11. home operator is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    FormatBuff.Format( KConvert,( iSessionData->At( aIndex ) )->HomeOperator() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    
    WriteBufferToLogFile();
    
    // 12. Write Song Recog Log
    TInt songRecogLogCount = ( iSessionData->At( aIndex ) )->
        SongRecogLog().Count();
    if(songRecogLogCount > 0)
        {
        iUniCodeBuf.Copy(KSONGRECOGLOGSTARTTAG);    
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
        WriteBufferToLogFile(); 
        
        for(TInt count=0; count < songRecogLogCount; count++)
            {        
            iUniCodeBuf.Copy(KSONGRECOGEVENTTAG);
            iFileWritePos = iUniCodeBuf.Find(KField);
            iUniCodeBuf.Replace(iFileWritePos,KFieldPosition,(iSessionData->
            At(aIndex)->SongRecogLog()[count].iTimeStamp));
            CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
            WriteBufferToLogFile();
            }
    
        iUniCodeBuf.Copy(KSONGRECOGLOGENDTAG);
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
        WriteBufferToLogFile();
        }
    
    // write ending tag </session>
    iUniCodeBuf.Copy( KSESSIONENDTAG );
    //tags are also written into xml file
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    WriteBufferToLogFile();
    IRLOG_DEBUG( "CIRReportGenerator::WriteSessionData - Exiting." );            
    }    

// ---------------------------------------------------------------------------
// Function: WriteBrowseLog
// gathers browse log data and writes to XML file
// ---------------------------------------------------------------------------
//    
 void CIRReportGenerator::WriteBrowseLog()
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteBrowseLog" );
    //browse log
    //browse log tag is written
    TInt browseLogCount = iBrowseLog.Count();
    if(browseLogCount > 0)
        {
        iUniCodeBuf.Copy(KBROWSESTARTTAG);
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
        WriteBufferToLogFile();
        for(TInt count = 0; count < browseLogCount; count++)
            {
            //browse log url is copied              
            iUniCodeBuf.Copy(KBROWSETAG);
            iFileWritePos = iUniCodeBuf.Find(KField);
            iUniCodeBuf.Replace(iFileWritePos,KFieldPosition,
                (iBrowseLog).operator[](count).iBrowseUrl);
            iFileWritePos = iUniCodeBuf.Find(KField);
            //browse log time stamp is copied
            iUniCodeBuf.Replace(iFileWritePos,KFieldPosition,
                (iBrowseLog).operator[](count).iTimeStamp);
            iFileWritePos = iUniCodeBuf.Find(KField);
            //browse log current network is copied
            iUniCodeBuf.Replace(iFileWritePos,KFieldPosition,
                (iBrowseLog).operator[](count).iCurrentNetwork);
            CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
            //browse log is written to xml
            WriteBufferToLogFile();
            }
        
        //browse log end tag is copied  
        iUniCodeBuf.Copy(KBROWSEENDTAG);
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iUTF8Buffer,iUniCodeBuf);
        WriteBufferToLogFile();
        }
    IRLOG_DEBUG( "CIRReportGenerator::WriteBrowseLog - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: ZipFileL
// generates the zip file from the input xml file
// output file is in gzip format and is stored in private path
// ---------------------------------------------------------------------------
//    
 void CIRReportGenerator::ZipFileL()
     {
     IRLOG_DEBUG( "CIRReportGenerator::ZipFileL" );
     //input file is the xml file in private path and output is a gzip file stored 
     //private path
     //xml file
    RFile iplogfile;
    //getting path of zip file
    TFileName gzipfilepath = iSessionLog->FileName();
    gzipfilepath.Append( KGZipLogFileName );
    
    iLogFilePath = iSettingsData->PrivatePath();
    iLogFilePath.Append( KLogFileName );

    //input file is opened
    TInt error = iplogfile.Open( iFsSession, iLogFilePath, EFileStream | EFileRead );
    CleanupClosePushL( iplogfile );
    
    if ( KErrNone != error )
        {
        User::LeaveIfError( error );    
        }
    
    //generating zip file
    
    TInt fileSize( 0 );
    TInt err = iplogfile.Size( fileSize );
    
    if ( !err )
        {    
        TParse zipParse;
        zipParse.Set( iLogFilePath, NULL, NULL );
        
        TPtrC zipDrive = zipParse.Drive();
        
        TChar driveChar = zipDrive[0];
        TInt driveNum( 0 );
        RFs::CharToDrive( driveChar, driveNum );
        
        TDriveInfo driveInfo;
        TInt driveErr = iplogfile.Drive( driveNum, driveInfo );
           if ( !driveErr
            && driveNum != EDriveZ
            && !SysUtil::DiskSpaceBelowCriticalLevelL(
                     &CCoeEnv::Static()->FsSession(), iUTF8Buffer.Size(),
                     driveNum ) )
            {
            // disk space is not below critical level
            // Next generate the zip file
            CEZFileToGZip* zip = CEZFileToGZip::NewLC( iFsSession, 
                    gzipfilepath, iplogfile );
     while( zip->DeflateL() )
        {
        }
    CleanupStack::PopAndDestroy( zip );
            }
        else
            {
            // disk space is below critical level
            // Do not generate the zip file
            }
        }

    //file is closed 
    CleanupStack::PopAndDestroy(); // iplogfile
    IRLOG_DEBUG( "CIRReportGenerator::ZipFileL - Exiting." );    
    }
    
// ---------------------------------------------------------------------------
// Function: FormatDateTime
// formats the date time given in "%d-%d-%d %d:%d:%d GMT\n" format
// copies the result to a descriptor
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::FormatDateTime( TDes& aBuffer, TDateTime aDateTime )
    {
    IRLOG_DEBUG( "CIRReportGenerator::FormatDateTime" );          
    _LIT( KFormatTxt, "%d-%d-%d %d:%d:%d GMT\n" );
    aBuffer.Format( KFormatTxt, aDateTime.Year(), TInt( aDateTime.Month() + 1 ), 
        //Format the month as a TInt to preserve locale independence
        //aDateTime.Day()+1, 
        aDateTime.Day() + 1, 
        // Day and month ranges begin at zero ( 0-30 and 0-11 ), 
        // so add one when formatting
        aDateTime.Hour(), aDateTime.Minute(), aDateTime.Second()
        //aDateTime.MicroSecond()
        );
    IRLOG_DEBUG( "CIRReportGenerator::FormatDateTime - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: WriteBufferToLogFile
// Writes the content of iUTF8Buffer to iLogFile
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::WriteBufferToLogFile()
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteBufferToLogFile" );   
    // Get the drive where iLogFile is located.

    TInt driveNumber( 0 );
    TDriveInfo driveInfo;
    
    TInt err = iLogFile.Drive( driveNumber, driveInfo );

    // Write the log file only if:
    // Drive was fetched successflly,
    // the drive is not Z
    // and free disk space is not below critical level ( 128kB )

    if ( !err && driveNumber != EDriveZ )
        {
        TBool ret = ETrue;
        TRAP_IGNORE( ret = SysUtil::DiskSpaceBelowCriticalLevelL( 
        &CCoeEnv::Static()->FsSession(), iUTF8Buffer.Size(), driveNumber ) )
        if ( !ret )
            {
            iLogFile.Write( iUTF8Buffer );
            }
        else
            {
            // Do not write the session log.
            }
        }
    else
        {
        // Do not write the session log.
        }
    IRLOG_DEBUG( "CIRReportGenerator::WriteBufferToLogFile - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: UnForceSessionEndedL
// Handles Unforced end of the session
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::UnForceSessionEndedL( CIRSessionLogger* aSession )
    {
    // 1. retrieve session log from db
    if ( iSessionDbCreationFailed )
        {
        iSessionLogUpdationFailed = ETrue;    
        }
    else
        {
        if ( iNmsEventFlag )
            {
            TRAPD( error,
            //count is stored    
            iLogSessionDb->GetAllSessionL( *iSessionData );
            );
            if ( error )
                {
                iSessionLogUpdationFailed = ETrue;    
                }    
            }
        else
            {
            TRAPD( error,
        //loads the information into the db at the end of the session
        iLogSessionDb->AddSessionStartL( *aSession );
        //count is stored    
        iLogSessionDb->GetAllSessionL( *iSessionData );
        );
        if ( error )
            {
            iSessionLogUpdationFailed = ETrue;    
                }   
            else
                {
                TInt count = iSessionData->Count();
                if ( (count>0 ) && ( count%KSessionLogThreshold == 0 ) )
                    {
                    iSessionLogDbUpdated = ETrue;
                    }
                }
            }
        }

    // 2. retrieve nms log from db  
    if ( iNmsDbCreationFailed )
        {
        iNmsLogUpdationFailed = ETrue;    
        }
    else
        {
        if(!iNmsEventFlag)
            {
            TRAPD( error, iNmsLogDb->GetAllNmsLogL( *iNmsLogData ) );
            if ( error )
                {
                iNmsLogUpdationFailed = ETrue;    
                }    
            }
        else
            {
            TRAPD( error,
            //loads the information into the db at the end of the session
            iNmsLogDb->AddNmsLogStartL( *iNmsLog );
            //count is stored    
            iNmsLogDb->GetAllNmsLogL( *iNmsLogData );
            );
            if ( error )
                {
                iNmsLogUpdationFailed = ETrue;    
                }
            else
                {
                TInt count = iNmsLogData->Count();
                if ( ( count>0 ) && ( count%KNmsLogThreshold == 0 ) )
                    {
                    iNmsLogDbUpdated = ETrue;
                    }
                }
            }
        }
    //xml file is created and is zipped
    WriteToXmlL();
    
    if( iDbStatusObserver && 
           ( iNmsLogDbUpdated ||
             iSessionLogDbUpdated ||
             iUnComfirmedReportSent
           )
      )
      {
          iDbStatusObserver->LogDbNeedFlush();
          iNmsLogDbUpdated = EFalse;
          iSessionLogDbUpdated = EFalse;
      }
    }

/**
 * The following methods are for Nms Event Usage Logging 
 */
// ---------------------------------------------------------------------------
// Function: UpdateNmsCurrentNetwork
// logs network during Music Store launching
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateNmsCurrentNetwork( TInt aNetwork )
    {
    iNmsLog->UpdateCurrentNetwork( aNetwork );
    }
    
// ---------------------------------------------------------------------------
// Function: UpdateNmsHomeOperator
// logs network during Music Store Launching
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateNmsHomeOperator( TInt aNetwork )
    {
    iNmsLog->UpdateHomeOperator( aNetwork );
    }
    
// ---------------------------------------------------------------------------
// Function: UpdateNmsChannelID
// updates the channel id
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRReportGenerator::UpdateNmsChannelID( TInt aChanneldID )
    {
    IRLOG_DEBUG( "CIRReportGenerator::UpdateNmsChannelID" );
    iNmsLog->UpdateChannelID( aChanneldID );
    }

// ---------------------------------------------------------------------------
// Function: WriteNmsLogData
// gathers NmsLog data and writes to XML file
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::WriteNmsLogData(TInt aIndex)
    {
    IRLOG_DEBUG( "CIRReportGenerator::WriteNmsLogData" );
    TBuf<KFormatBuffMaxLength> FormatBuff;
    _LIT( KConvert, "%d" );

    //NmsEvent Tag is written
    iUniCodeBuf.Copy( KNMSEVENTTAG );

    iFileWritePos = iUniCodeBuf.Find( KField );    
    //Nms Event timestamp is copied
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iNmsLogData->
    At(aIndex))->StartTime());

    //channel id is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    ASSERT( aIndex >= 0 && aIndex < iNmsLogData->Count() );
    FormatBuff.Format( KConvert,( iNmsLogData->At(aIndex) )->ChannelID() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );

    //type is copied
    iFileWritePos = iUniCodeBuf.Find( KField );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, ( iNmsLogData->
    At( aIndex ))->NmsType() );
    iFileWritePos = iUniCodeBuf.Find( KField );

    //current network is copied
    ASSERT( aIndex >= 0 && aIndex < iNmsLogData->Count() );
    FormatBuff.Format( KConvert, ( iNmsLogData->At( aIndex ))->
    CurrentNetwork() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );
    iFileWritePos = iUniCodeBuf.Find( KField );

    //home operator is copied
    ASSERT( aIndex >= 0 && aIndex < iNmsLogData->Count() );
    FormatBuff.Format( KConvert, (iNmsLogData->At( aIndex ))->HomeOperator() );
    iUniCodeBuf.Replace( iFileWritePos, KFieldPosition, FormatBuff );

    CnvUtfConverter::ConvertFromUnicodeToUtf8( iUTF8Buffer, iUniCodeBuf );
    //writting the information to the xml file    
    WriteBufferToLogFile();

    IRLOG_DEBUG( "CIRReportGenerator::WriteNmsLogData - Exiting." );
    }   

// ---------------------------------------------------------------------------
// Function: ImeiUpdated
// From MIRPhoneInfoObserver
// ---------------------------------------------------------------------------
//
void CIRReportGenerator::ImeiUpdatedL( const TDesC& aImei )
    {
    if(0 == aImei.Length())
        {
        iPhoneInfo->StartUpdating();
        return;
        }
    // prepare base64 codec input
    HBufC8 *imei = HBufC8::NewLC( aImei.Length() );
    TPtr8 imeiPtr = imei->Des();    
    imeiPtr.Copy( aImei );
    
    // prepare base64 codec output
    HBufC8 *buffer = HBufC8::NewLC( aImei.Length() * 2 );
    TPtr8 bufferPtr = buffer->Des();
    
    // Encoding
    TImCodecB64 base64Codec;
    base64Codec.Initialise();
    base64Codec.Encode( *imei, bufferPtr );
    
    // store to data member
    if(NULL == iImeiBase64)
        {
        iImeiBase64 = HBufC16::NewL( aImei.Length() * 2 );
        }
    TPtr imeiBase64Ptr = iImeiBase64->Des();
    imeiBase64Ptr.Copy( bufferPtr );
    
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( imei );
    }

