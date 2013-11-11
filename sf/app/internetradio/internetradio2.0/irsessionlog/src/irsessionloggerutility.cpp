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


#include <s32strm.h>

#include "irdebug.h"
#include "irreportsettings.h"
#include "irsessionloggerutility.h"

const TInt KMaxUrlLength = 256;
const TInt KMaxTimeStampLength = 64;

// ============================== Function declaration =======================

// ---------------------------------------------------------------------------
// Function: NewL
// Two phase constructor returns the instance of CIRSessionLogger
// ---------------------------------------------------------------------------
//
CIRSessionLogger* CIRSessionLogger::NewL()
    {
    IRLOG_DEBUG( "CIRSessionLogger::NewL" );
    CIRSessionLogger* self = CIRSessionLogger::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSessionLogger::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function: NewLC
// Two phase constructor creates the instance of CIRSessionLogger
// ---------------------------------------------------------------------------
//
CIRSessionLogger* CIRSessionLogger::NewLC()
    {
    IRLOG_DEBUG( "CIRSessionLogger::NewLC" );
    CIRSessionLogger* self = new ( ELeave ) CIRSessionLogger;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRSessionLogger::NewLC - Exiting." );
    return self;
    }    

// ---------------------------------------------------------------------------
// Function: ~CIRSessionLogger
// default destructor
// ---------------------------------------------------------------------------
//
CIRSessionLogger::~CIRSessionLogger()    
    {
    iRServer.Reset();
    iRServer.Close();    
	iRSongRecogLog.Reset();
	iRSongRecogLog.Close();	
    }

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::ConstructL()        
    {
    //No implementation
    }

// ---------------------------------------------------------------------------
// Function: CIRSessionLogger
// default constructor
// ---------------------------------------------------------------------------
//
CIRSessionLogger::CIRSessionLogger()    
    {
    //No implementation
    }

// ---------------------------------------------------------------------------
// Function: EndLogging
// logs the termination condition
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::EndLogging()
    {
    IRLOG_DEBUG( "CIRSessionLogger::EndLogging" );    
    if ( iSessionMarked )
        {
        HandleStopEvents();
        }
    else
        {
        iDuration = 0;    
        }
    switch ( iTerminateBy )
        {
        case EUserTerminated:
            {
            //error condition for termination is disconnected by the user
            iTerminatedBy.Copy( KUSER );
            break;
            }
        case ENoConnectionToServer:
            {
            //error condition for termination is no connection to server            
            iTerminatedBy.Copy( KNOCONNECTIONTOSERVER );
            break;
            }
        case ENoConnectionToNetwork:
            {
            //error condition for termination is no connection to network
            iTerminatedBy.Copy( KNOCONNECTIONTONETWORK );
            break;
            }
        default:
            {
            break;    
            }                                
        }
    IRLOG_DEBUG( "CIRSessionLogger::EndLogging - Exiting." );        
    }

    
// ---------------------------------------------------------------------------
// Function: HandleStopEvents
// logs the termination condition
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::HandleStartEvents()
    {
    if ( !iIsSessionProgressing )
        {
        iActiveSessionTime.UniversalTime();
        iIsSessionProgressing = ETrue;
        }
    }
    
// ---------------------------------------------------------------------------
// Function: HandleStartEvents
// logs the termination condition
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::HandleStopEvents()
    {
    if ( iIsSessionProgressing )
        {
        TTime endtime;    
        //endtime has session end time in GMT format
        endtime.UniversalTime();
        //session duration compensation value is calculated 
        //as difference between Compensation Time [time when it stopped] and
        //end time
        TTimeIntervalSeconds timeelaspsed;
        endtime.SecondsFrom( iActiveSessionTime, timeelaspsed );
        //compensation value session duration is converted to integer
        iDuration += timeelaspsed.Int();
        iIsSessionProgressing = EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// Function: MarkSession
// Marks the start of a session
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::MarkSession()
    {
    iSessionMarked = ETrue;    
    iDuration = 0;
    HandleStartEvents();
    }

// ---------------------------------------------------------------------------
// Function: UnMarkSession
// UnMarks the start of a sessiom
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UnMarkSession()
    {
    iSessionMarked = EFalse;    
    }
    
// ---------------------------------------------------------------------------
// Function: SessionMarked
// Returns the session mark flag
// ---------------------------------------------------------------------------
//    
TBool CIRSessionLogger::SessionMarked()
    {
    return iSessionMarked;    
    }    
// ---------------------------------------------------------------------------
// Function: ResetResource
// reset data from channel server
// ---------------------------------------------------------------------------
//    
void CIRSessionLogger::ResetResource()
    {
    IRLOG_DEBUG( "CIRSessionLogger::ResetResource" );
    //reseting of resource
    iRServer.Reset();
	iRSongRecogLog.Reset();
    }
// ---------------------------------------------------------------------------
// Function: IsSessionProgressing
// checks whether session is progressing or not
// ---------------------------------------------------------------------------
//
TBool CIRSessionLogger::IsSessionProgressing()
    {
    return iIsSessionProgressing;    
    }
    
// ---------------------------------------------------------------------------
// Function: CompensationTime
// Returns the session mark flag
// ---------------------------------------------------------------------------
//    
TTime CIRSessionLogger::CompensationTime()
    {
    return iActiveSessionTime;    
    }
    
// ---------------------------------------------------------------------------
// Function: LogServerResult
// logs server connection result
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::LogServerResult( const TDesC& aUrl, TResponse aResponse )
    {
    IRLOG_INFO3( "CIRSessionLogger::LogServerResult - %S ( %d )", &aUrl, aResponse );
	TIRServer tempServer;
    if ( aUrl.Length() <= KMaxUrlLength )
        {
        //channel server url is copied
		tempServer.iServerUrl.Copy(aUrl);
        }
    switch ( aResponse )
        {
        //connected to the channel server
        case EIRConnected:
            {
			tempServer.iServerResult.Copy(KConnected);
            }
            break;
        //not connected to channel server
        //since server is not responding
        case EIRFull:
            {
			tempServer.iServerResult.Copy(KFull);
            }
            break;
        //not connected to channel server
        //since connection time out     
        case EIRConnTimeOut:
            {
			tempServer.iServerResult.Copy(KConnTimeOut);
            }
            break;
        //not connected to channel server ( may be invalid urls )
        case EIRConnFailed:
            {
			tempServer.iServerResult.Copy(KConnectionFailed);
            }
            break;
        default:
            {
            break;    
            }    
        }
	UpdateSessionLog(tempServer);
    IRLOG_DEBUG( "CIRSessionLogger::LogServerResult - Exiting." );            
	}

// ---------------------------------------------------------------------------
// Function: LogSongRecog
// logs song recog
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::LogSongRecog(const TDesC& aTimeStamp)
    {
    if( aTimeStamp.Length() <= KMaxTimeStampLength )
        {
        TIRSongRecogLog tempSongRecogLog;
        tempSongRecogLog.iTimeStamp.Copy(aTimeStamp);   
        TRAP_IGNORE(iRSongRecogLog.AppendL(tempSongRecogLog));
        }    

    }

// ---------------------------------------------------------------------------
// Function: GetConnectedFrom
// connected information is stored
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::GetConnectedFrom(TConnectedFrom aConnectedFrom)
	{
	IRLOG_DEBUG( "CIRSessionLogger::GetConnectedFrom" );
	switch(aConnectedFrom)
		{
		case EIRIsds:
			{
			iConnectedFrom.Copy(KISDS);
			break;
			}
		case EIRPresetAdhoc:
			{
			iConnectedFrom.Copy(KPRESETADHOC);
			break;
			}
		case EIRPresetIsds:
			{
			iConnectedFrom.Copy(KPRESETISDS);
			break;
			}
        case EIRHistoryAdhoc:
            {
            iConnectedFrom.Copy(KHISTORYADHOC);
            break;
            }
        case EIRHistoryIsds:
            {
            iConnectedFrom.Copy(KHISTORYISDS);
            break;
            }
        case EIRAdhocManual:
            {
            iConnectedFrom.Copy(KADHOCMANUAL);
            break;
            }
        case EIRAdhocExternal:
            {
            iConnectedFrom.Copy(KADHOCEXTERNAL);
            break;
            }
        default:
            {
            break;    
            }                                    
        }
    IRLOG_DEBUG( "CIRSessionLogger::GetConnectedFrom - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function: UpdateSessionLog
// update the session logs
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateSessionLog( TIRServer& aServer )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateSessionLog" );

    TBool found = EFalse;

    // Only one entry per URL may be in the session log.
    for ( TInt i = 0; i < iRServer.Count() && !found; i++ )
        {
        TIRServer& server = iRServer[i];
        if ( server.iServerUrl == aServer.iServerUrl )
            {
            // If an entry for this URL already exists, it is replaced
            // by the newer status code.
            server = aServer;
            found = ETrue;
            }
        }
    
    if ( !found )
        {
        // Creates a new entry into the session log for this URL.
        TRAP_IGNORE( iRServer.AppendL( aServer ) )    
        }
    }

// ---------------------------------------------------------------------------
// Function: UpdateSessionId
// updates the session id
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateSessionId( TInt aSessionId )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateSessionId" );
    iSessionId = aSessionId;    
    }
  
// ---------------------------------------------------------------------------
// Function: UpdateTerminationStatus
// updates the termination status
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateTerminationStatus( TTerminationStatus aTerminatedBy )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateTerminationStatus" );
    iTerminateBy = aTerminatedBy;    
    }

// ---------------------------------------------------------------------------
// Function: UpdateChannelID
// updates the channel id
// ---------------------------------------------------------------------------
//    
void CIRSessionLogger::UpdateChannelID( TInt aChanneldID )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateChannelID" );
    iChannelId = aChanneldID;    
    }

// ---------------------------------------------------------------------------
// Function: UpdateCurrentNetwork
// updates the current network
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateCurrentNetwork( TInt aCurrentNetwork )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateCurrentNetwork" );
    iSessionCurrentNetwork = aCurrentNetwork;
    }

// ---------------------------------------------------------------------------
// Function: UpdateHomeOperator
// updates the home network
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateHomeOperator( TInt aHomeOperator )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateHomeOperator" );
    iHomeOperator = aHomeOperator;
    }

// ---------------------------------------------------------------------------
// Function: UpdateConnectionType
// updates the connection type
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateConnectionType( const TDesC& aConnectionType )
    {
    IRLOG_DEBUG( "CIRSessionLogger::UpdateConnectionType" );
    iSessionConnectionType.Copy( aConnectionType );
    }
    
// ---------------------------------------------------------------------------
// Function : UpdateSessionStartGMTTime
// function updates session start time with current GMT time
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::UpdateSessionStartGMTTime()
    {
    iStartSessionTime.UniversalTime();    
    }
    
// ---------------------------------------------------------------------------
// Function: TerminationStatus
// returns termination status
// ---------------------------------------------------------------------------
//
TDesC& CIRSessionLogger::TerminationStatus()
    {
    IRLOG_DEBUG( "CIRSessionLogger::TerminationStatus" );
    return iTerminatedBy;    
    }

// ---------------------------------------------------------------------------
// Function: ChannelID
// returns the channel id
// ---------------------------------------------------------------------------
//    
TInt CIRSessionLogger::ChannelID() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::ChannelID" );
    return iChannelId;
    }

// ---------------------------------------------------------------------------
// Function: CurrentNetwork
// returns the Current Network
// ---------------------------------------------------------------------------
//    
TInt CIRSessionLogger::CurrentNetwork() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::CurrentNetwork" );
    return iSessionCurrentNetwork;
    }

// ---------------------------------------------------------------------------
// Function: HomeOperator
// returns the home network
// ---------------------------------------------------------------------------
//
TInt CIRSessionLogger::HomeOperator() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::HomeOperator" );
    return iHomeOperator;
    }

// ---------------------------------------------------------------------------
// Function: ConnectionType
// returns the connection type
// ---------------------------------------------------------------------------
//    
TDesC& CIRSessionLogger::ConnectionType()
    {
    IRLOG_DEBUG( "CIRSessionLogger::ConnectionType" );
    return iSessionConnectionType;
    }

// ---------------------------------------------------------------------------
// Function: StartTime
// returns the start time
// ---------------------------------------------------------------------------
//    
TDes& CIRSessionLogger::StartTime()
    {
    IRLOG_DEBUG( "CIRSessionLogger::StartTime" );
    return iStartTime;    
    }

// ---------------------------------------------------------------------------
// Function: ConnectionType
// returns the connection type
// ---------------------------------------------------------------------------
//    
TDesC& CIRSessionLogger::ConnectedFrom()
    {
    IRLOG_DEBUG( "CIRSessionLogger::ConnectedFrom" );
    return iConnectedFrom;
    }
    
// ---------------------------------------------------------------------------
// Function: SessionId
// returns the session id
// ---------------------------------------------------------------------------
//    
TInt CIRSessionLogger::SessionId() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::SessionId" );
    return iSessionId;
    }
    
// ---------------------------------------------------------------------------
// Function: SessionDuration
// returns the duration of the session
// ---------------------------------------------------------------------------
//    
TInt CIRSessionLogger::SessionDuration() const
    {
    IRLOG_DEBUG( "CIRSessionLogger::SessionDuration" );
    return iDuration;
    }

// ---------------------------------------------------------------------------
// Function: FileName
// returns file name
// ---------------------------------------------------------------------------
//
TFileName CIRSessionLogger::FileName()
    {
    IRLOG_DEBUG( "CIRSessionLogger::FileName" );
    return iFilePath;    
    }

// ---------------------------------------------------------------------------
// Function: DateTime
// returns session start time
// ---------------------------------------------------------------------------
//
TTime CIRSessionLogger::DateTime()
    {
    return iStartSessionTime;    
    }
    

// ---------------------------------------------------------------------------
//  Function: SessionData
//  function returns instance session log array
// ---------------------------------------------------------------------------
//
const RArray<TIRServer>& CIRSessionLogger::SessionData()const
    {
    IRLOG_DEBUG( "CIRSessionLogger::SessionData" );
    return iRServer;    
    }

// ---------------------------------------------------------------------------
//  Function: SongRecogLog
//  function returns instance song recog log array
// ---------------------------------------------------------------------------
//
const RArray<TIRSongRecogLog>& CIRSessionLogger::SongRecogLog()const
    {
    IRLOG_DEBUG( "CIRSessionLogger::SongRecogLog" );
    return iRSongRecogLog;    
    }

// ---------------------------------------------------------------------------
//  Function: DurationValue
//  function returns duration value
// ---------------------------------------------------------------------------
//
TTimeIntervalSeconds CIRSessionLogger::DurationValue()
    {
    return iDurationValue;    
    }
    
// ---------------------------------------------------------------------------
// Function: GetBackedUp
// Back up the session log of a session
// ---------------------------------------------------------------------------
//    
void CIRSessionLogger::GetBackedUp( CIRSessionLogger& aSession )
    {
    //copies the file path
    iFilePath = aSession.FileName();
    //session start time
    iStartSessionTime = aSession.DateTime();
    //duration 
    iDurationValue = aSession.DurationValue();
    //start time 
    iStartTime.Copy( aSession.StartTime() );
    //termination status
    iTerminatedBy.Copy( aSession.TerminationStatus() );
    //connected from information
    iConnectedFrom.Copy( aSession.ConnectedFrom() );
    //connection type
    iSessionConnectionType.Copy( aSession.ConnectionType() );
    //session duration
    iDuration = aSession.SessionDuration();
    //state of the player
    iIsSessionProgressing = aSession.IsSessionProgressing();
    //Compensation time
    iActiveSessionTime = aSession.CompensationTime();
    //session id
    iSessionId = aSession.SessionId();
    //channel id
    iChannelId = aSession.ChannelID();
    //home operator
    iHomeOperator = aSession.HomeOperator();
    //current network
    iSessionCurrentNetwork = aSession.CurrentNetwork();
    //session mark checking flag
    iSessionMarked = aSession.SessionMarked();
    
    ResetResource();
    //getting session data    
    for ( TInt iter = 0 ;iter < aSession.SessionData().Count(); iter++ )
        {
        //session data
        iRServer.Append( aSession.SessionData().operator[]( iter) );
        }
    //getting song recog log data  
    for(TInt iter = 0 ;iter < aSession.SongRecogLog().Count(); iter++)
        {
        //song recog log data
        iRSongRecogLog.Append(aSession.SongRecogLog().operator[](iter));
        }	
	}
			
// ---------------------------------------------------------------------------
// Function: ExternalizeL
// externialize the stream to the given stream
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::ExternalizeL( RWriteStream& aWriteStream )
    {
    IRLOG_DEBUG( "CIRSessionLogger::ExternalizeL" );
    TInt length;
    //start time
    length = iStartTime.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iStartTime, length );
    
    //connected from
    length = iConnectedFrom.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iConnectedFrom, length );
    
    //session id
    aWriteStream.WriteInt32L( iSessionId );
    
    //connection type
    length = iSessionConnectionType.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iSessionConnectionType, length );
    
    //channel id
    aWriteStream.WriteInt32L( iChannelId );
    
    //current network
    aWriteStream.WriteInt32L( iSessionCurrentNetwork );
    
    //home operator
    aWriteStream.WriteInt32L( iHomeOperator );
    
    //duration
    aWriteStream.WriteInt32L( iDuration );
    
    //terminated by
    length = iTerminatedBy.Length();
    aWriteStream.WriteInt32L( length );
    aWriteStream.WriteL( iTerminatedBy, length );
    
    //count
    aWriteStream.WriteInt32L( iRServer.Count() );
    
    //session duration
    //Session Data
    for ( TInt iter=0; iter<iRServer.Count(); iter++ )
        {
        //channel Server Url;
        length = iRServer[iter].iServerUrl.Length();
        aWriteStream.WriteInt32L( length );
        aWriteStream.WriteL( iRServer[iter].iServerUrl, length );    
        //channel server result
        length = iRServer[iter].iServerResult.Length();
        aWriteStream.WriteInt32L( length );
        aWriteStream.WriteL( iRServer[iter].iServerResult, length );
        //count
        aWriteStream.WriteInt32L( iRServer[iter].iCount );
        }
	
    //count
    aWriteStream.WriteInt32L(iRSongRecogLog.Count());
    
    //session duration
    //Session Data
    for(TInt iter=0;iter<iRSongRecogLog.Count();iter++)
        {
        //channel Server Url;
        length = iRSongRecogLog[iter].iTimeStamp.Length();
        aWriteStream.WriteInt32L(length);
        aWriteStream.WriteL(iRSongRecogLog[iter].iTimeStamp,length);  
        }	
    IRLOG_DEBUG( "CIRSessionLogger::ExternalizeL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// Function: InternalizeL
// gets the data from the stream
// ---------------------------------------------------------------------------
//
void CIRSessionLogger::InternalizeL( RReadStream& aReadStream )
    {
    IRLOG_DEBUG( "CIRSessionLogger::InternalizeL" );
    TInt length;
    
    //Start Time
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iStartTime, length );
    
    //Connected From    
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iConnectedFrom, length );
    
    //Session Id
    iSessionId=aReadStream.ReadInt32L();
    
    //Session Connection Type
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iSessionConnectionType, length );
    
    //Channel Id
    iChannelId = aReadStream.ReadInt32L();
    
    //Session Current Network
    iSessionCurrentNetwork = aReadStream.ReadInt32L();
    
    //Home Operator
    iHomeOperator = aReadStream.ReadInt32L();
    
    //Duration
    iDuration = aReadStream.ReadInt32L();
    
    //Terminated By
    length = aReadStream.ReadInt32L();
    aReadStream.ReadL( iTerminatedBy, length );
    
    //count
    TInt count = aReadStream.ReadInt32L();
    
    //Session Data
    iRServer.Reset();
    for ( TInt iter=0; iter<count; iter++ )
        {
        TIRServer hold;
        
        //Server Url
        length = aReadStream.ReadInt32L();
        hold.iServerUrl.Zero();
        aReadStream.ReadL( hold.iServerUrl, length );

        //Server Result
        length = aReadStream.ReadInt32L();
        hold.iServerResult.Zero();
        aReadStream.ReadL( hold.iServerResult, length );
        
        //Count
        hold.iCount = aReadStream.ReadInt32L();
        iRServer.Append( hold );
        }

    //count
    count = aReadStream.ReadInt32L();	
    //Song Recog Data
    iRSongRecogLog.Reset();
    for(TInt iter=0;iter<count;iter++)
        {
        TIRSongRecogLog hold;
        
        //Server Url
        length = aReadStream.ReadInt32L();
        hold.iTimeStamp.Zero();
        aReadStream.ReadL(hold.iTimeStamp,length);
        
        iRSongRecogLog.Append(hold);
        }
    
    IRLOG_DEBUG( "CIRSessionLogger::InternalizeL - Exiting." );
    }
