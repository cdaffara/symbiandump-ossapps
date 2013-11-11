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


#ifndef C_IRSESSIONLOGGERUTILITY_H
#define C_IRSESSIONLOGGERUTILITY_H

#include <e32base.h>

#include "irsessionparams.h"

class RReadStream;
class RWriteStream;

/**
* This class can hold data for session log 
*/

NONSHARABLE_CLASS( CIRSessionLogger ) : public CBase
    {
public:
    /**
    * Function: NewL
    * Two phase constructor returns the instance of CIRSessionLogger
    * @returns instance of CIRSessionLogger
    */
    static CIRSessionLogger* NewL();

    /**
    * Function: NewLC
    * Two phase constructor creates the instance of CIRSessionLogger
    * @returns instance of CIRSessionLogger
    */
    static CIRSessionLogger* NewLC(); 

    /**
    * Function: ~CIRSessionLogger
    * default destructor
    */    
    ~CIRSessionLogger();
    
private:
    /**
    * Function: ConstructL
    * Two phase constructor
    */    
    void ConstructL();
    
    /**
    * Function: CIRSessionLogger
    * default constructor
    */    
    CIRSessionLogger();    

public:
    /**
    * Function: EndLogging
    * logs the termination condition
    */
    void EndLogging();
    
    /**
    * Function: HandleStartEvents
    * functtion handles start events
    */
    void HandleStartEvents();
    
    /**
    * Function: HandleStopEvents
    * functtion handles stop events
    */
    void HandleStopEvents();
    
    /**
    * Function: MarkSession
    * Marks the start of a session
    */
    void MarkSession();

    /**
    * Function: UnMarkSession
    * UnMarks the start of a sessiom
    */
    void UnMarkSession();
    
    /**
    * Function: SessionMarked
    * Returns whether session is marked or not
    * @return session marked flag
    */    
    TBool SessionMarked();
    
    /**
    * Function: LogServerResult
    * logs server connection result
    * @param URL to which connection is attempted
    * @param response from server
    */
    void LogServerResult( const TDesC& aUrl, TResponse aResult );    
	
    /**
    * Function: LogSongRecog
    * logs song recog
    * @param aTimeStamp song recog timestamp.
    */	
	void LogSongRecog( const TDesC& aTimeStamp );
	
	/**
	* Function: GetConnectedFrom
	* connected information is stored
	* @param connected from information
    */
	void GetConnectedFrom( TConnectedFrom aConnectedFrom );
    
    /**
    * Function: UpdateSessionLog
    * update the session logs
    * @param TIRServer structure which holds url response and cound
    */
    void UpdateSessionLog( TIRServer& aServer );
    
    /**
    * Function: ResetResource
    * reset data from channel server
    */
    void ResetResource();

    /**
    * Function: IsSessionProgressing
    * checks whether session is active or not
    * @return EFalse if session is currently not progressing
    */
    TBool IsSessionProgressing();

    /**
    * Function: CompensationTime
    * returns time at which session become active
    * @return time at which session become active
    */    
    TTime CompensationTime();

    /**
    * Function: UpdateSessionId
    * updates the session id
    * @param session id
    */
    void UpdateSessionId( TInt aSessionId );
    
    /**
    * Function: UpdateTerminationStatus
    * updates the termination status
    * @param terminated by information
    */    
    void UpdateTerminationStatus( TTerminationStatus aTerminatedBy );
    
    /**
    * Function: UpdateChannelID
    * updates the channel id
    * @param channel id
    */    
    void UpdateChannelID( TInt aChanneldID );
    
    /**
    * Function: UpdateCurrentNetwork
    * updates the current network
    * @param current network
    */
    void UpdateCurrentNetwork( TInt aCurrentNetwork );
    
    /**
    * Function: UpdateHomeOperator
    * updates the home network
    * @param home network
    */
    void UpdateHomeOperator( TInt aHomeOperator );
    
    /**
    * Function: UpdateConnectionType
    * updates the connection type
    * @param connection type
    */
    void UpdateConnectionType( const TDesC& aConnectionType );
    
    /**
    * Function : UpdateSessionStartGMTTime
    * function updates session start time with current GMT time
    */
    void UpdateSessionStartGMTTime();
    
    /**
    * Function: TerminationStatus
    * returns termination status
    * @return termination status
    */
    TDesC& TerminationStatus();
        
    /**    
    * Function: ChannelID
    * return the channel id
    * @return channel id
    */    
    TInt ChannelID() const;
    
    /**
    * Function: CurrentNetwork
    * returns the Current Network
    * @return Current Network
    */    
    TInt CurrentNetwork() const;
    
    /**
    * Function: HomeOperator
    * returns the home network
    * @return home network
    */
    TInt HomeOperator() const;
    
    /**
    * Function: ConnectionType
    * returns the connection type
    * @return connection type
    */    
    TDesC& ConnectionType();
    
    /**
    * Function: StartTime
    * returns the start time
    * @return start time
    */    
    TDes& StartTime();
    
    /**
    * Function: ConnectionType
    * returns the connection type
    * @return connection type
    */    
    TDesC& ConnectedFrom();
    
    /**
    * Function: SessionId
    * returns the session id
    * @return session id
    */    
    TInt SessionId() const;
    
    
    /**
    * Function: SessionDuration
    * returns the duration of the session
    * @return duration of the session
    */    
    TInt SessionDuration() const;
    
    /**
    * Function: FileName
    * returns file name
    * @return file name
    */
    TFileName FileName();
    
    /** 
    * Function: DateTime
    * returns session start time
    * @return session start time
    */
    TTime DateTime();
    
    /** 
    *  Function: SessionData
    *  function returns instance session log array
    *  @return session log RArray of session log
    */
    const RArray<TIRServer>& SessionData()const;

    /** 
    *  Function: SongRecogLog
    *  function returns instance song recog log array
    *  @return song recog log RArray of song recog log
    */
    const RArray<TIRSongRecogLog>& SongRecogLog()const;
    
    /**
    * Function: GetBackedUp
    * Back up the given session, copies the data of the session
    * @param session logs instance which to be copied
    */
    void GetBackedUp( CIRSessionLogger& aSession );
    
    /**
    * Function: ExternalizeL
    * externialize the stream to the given stream
    * @param write stream
    */
    void ExternalizeL( RWriteStream& aWriteStream );
    
    /**
    * Function: InternalizeL
    * gets the data from the stream
    * @param read stream
    */
    void InternalizeL( RReadStream& aReadStream );
    
    /**
    * Function: DurationValue
    * Duration value
    * @return duration
    */
    TTimeIntervalSeconds DurationValue();
    
private:  
    /**                                    
    * file name 
    */                                    
    TFileName iFilePath;
    
    /**
    * session start time
    */
    TTime iStartSessionTime;
    
    /**
    * time current session become active started
    */
    TTime iActiveSessionTime;
    
    /**
    * session duration
    */
    TTimeIntervalSeconds iDurationValue;
    
    /**
    * start time is stored
    */
    TBuf<64> iStartTime;
    
    /**
    * terminated by status is stored
    */
	TBuf<32> iTerminatedBy;
    
    /**
    * connected from information
    */
	TBuf<32> iConnectedFrom;
    
    /**
    * connection type information    
    */
	TBuf<32> iSessionConnectionType;
    
    /**
    * session duration
    */
    TInt iDuration;
    
    /**
    * Termination status
    */
    TTerminationStatus iTerminateBy;
    
    /**
    * Session id
    */
    TInt iSessionId;
    
    
    /**
    * NmsLog id
    */
    TInt iNmsLogId;
    
    /**
    * channel id
    */
    TInt iChannelId;
    
    /**
    * home operator mcc+mnc
    */
    TInt iHomeOperator;
    
    /**
    * current operator mcc+mnc
    */
    TInt iSessionCurrentNetwork;
    
    /**
    * Checks whether session is actually started 
    */    
    TBool iSessionMarked;    
    
    /**
    * Checks whether application is playing or not 
    */    
    TBool iIsSessionProgressing;    
    
    /**
    * session log holder's array
    */
    RArray<TIRServer> iRServer;
		
    /**
    * song recog log holder's array
    */
    RArray<TIRSongRecogLog> iRSongRecogLog;
    };

#endif //C_IRSESSIONLOGGERUTILITY_H
