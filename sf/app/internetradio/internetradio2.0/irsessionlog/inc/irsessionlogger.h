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


#ifndef C_IRSESSIONLOGGER_H
#define C_IRSESSIONLOGGER_H

#include <f32file.h>

#include "irsessionparams.h"
#include "mirphoneinfoobserver.h"

class CIRSessionDb;
class CIRSessionLogger;
class CIRSettings;
class CIRNmsLogDb;
class CIRNmsLogger;
class CPhoneInfo;
class MIRLogDbStatusObserver;

/**
 * This class is used to provide usage log, various function are to log session
 * information as well as others, like browse log information. Session logs can be 
 * stored in internel data base for last 5 session, it is deleted only if the 
 * response is given. If more than 5 session is happening at any point of time
 * first one based on session id is removed.
 * When you end a session a gzip file is generated in private path
 * This zip file has ( xml file ) session log in xml format
 * This is single ton implementation use DeleteInstance to delete its 
 * instance
 *
 * @lib irsessionlog.lib
 * @since S_60 3.0
 *
 */

//============================ class declaration ========================

NONSHARABLE_CLASS(CIRReportGenerator) : public CObject,
                                        public MIRPhoneInfoObserver
    {
    //member function
public:

    /**
    * Function: NewL
    * Two phase constructor returns the instance of CIRReportGenerator
    * this is a single ton implementation
    * @note Deleting this object directly is not allowed. Please use Close().
    * @note CleanupClosePushL( generator ) must be used if the object is to be pushed onto cleanup stack.
    * @returns instance of CIRReportGenerator
    */    
    IMPORT_C static CIRReportGenerator* OpenL();
    
private:    
    /**
    * Function: ~CIRReportGenerator
    * default destructor
    */    
    ~CIRReportGenerator();
    
    /**
    * Function: ConstructL
    * Two phase constructor
    */    
    void ConstructL();
    
    /**
    * Function: CIRReportGenerator
    * default constructor
    */    
    CIRReportGenerator();

public:

    /**
    * Function: LogServerResult
    * logs server connection result
    * @param URL to which connection is attempted
    * @param response from server
    */        
    IMPORT_C void LogServerResult( const TDesC& aUrl, TResponse aResult );    
    
    /**
    * Function: LogSongRecog
    * logs song recog
    */  
    IMPORT_C void LogSongRecog();
    
    /**
    * Function: UpdateCurrentBrowseNetwork
    * logs network during browsing
    * @param mcc+ mnc code of current network
    */  
    IMPORT_C void UpdateCurrentBrowseNetwork(TInt aNetwork);
        
    /**
    * Function: LogBrowse
    * logs browse url and status information
    * @param browse url
    * @param status information
    */  
    IMPORT_C void LogBrowse(const TDesC8& aUrl,TInt aStatus);
            
    /**
    * Function: SessionStartedL
    * starts a new session
    */  
    IMPORT_C void SessionStartedL();
    /**
    * Function: NmsLogStartedL
    * starts a new nmslog
    */  
    IMPORT_C void NmsLogStartedL();
    /**
    * Function: SessionEnd
    * marks the a session
    */      
    IMPORT_C void SessionEndL();
    
    /**
    * Function: MarkSessionStart
    * marks the a start of a session
    */  
    IMPORT_C void MarkSessionStart();
    
    /**
    * Function: HandleStopEvents
    * function handle session state change events
    * @param ETrue if session is progressing and EFalse if not progressing
    */
    IMPORT_C void HandleStopEvents(TBool aIsSessionProgressing );
    
    /**
    * Function: UpdateTerminatedBy
    * updates the termination status
    * @param terminated by information
    */  
    IMPORT_C void UpdateTerminatedBy(TTerminationStatus aTerminatedBy);
    
    /**
    * Function: UpdateConnectedFrom
    * connected information is stored
    * @param connected from information
    */      
    IMPORT_C void UpdateConnectedFrom( TConnectedFrom aConnectedFrom );
    
    /**
    * Function: BrowseUrl
    * logs browse url and result
    * @param URL to which connection is attempted
    * @param response from server
    */    
    IMPORT_C void BrowseUrl( const TDesC& aBrowseUrl, TInt aStatus );
    
    /**
    * Function: UpdateChannelID
    * updates the channel id
    * @param channel id
    */    
    IMPORT_C void UpdateChannelID( TInt aChanneldID );
    
    /**
    * Function: UpdateCurrentNetwork
    * updates the current network
    * @param current network
    */    
    IMPORT_C void UpdateCurrentNetwork( TInt aCurrentNetwork );
    
    /**
    * Function: UpdateHomeOperator
    * updates the home network
    * @param home network
    */    
    IMPORT_C void UpdateHomeOperator( TInt aHomeOperator );
    
    /**
    * Function: UpdateConnectionType
    * updates the connection type
    * @param connection type
    */    
    IMPORT_C void UpdateConnectionType( const TDesC& aConnectionType );    
    
    /**
    * Function : ReponseFromISDSObtained
    * This function is called when session log is successfully sent
    * removes all session log entries from sessiondb
    */    
    IMPORT_C void ReponseFromISDSObtained();
    
    
    /**
     * The following methods are for Nms Event Usage Logging 
     */
    
    
    /**
     * Function: UpdateNmsCurrentNetwork
     * logs network during Music Store launching
     * @param mcc+ mnc code of current network
     */    
    IMPORT_C void UpdateNmsCurrentNetwork( TInt aNetwork );
    
    /**
     * Function: UpdateNmsHomeOperator
     * logs network during Music Store launching
     */    
    IMPORT_C void UpdateNmsHomeOperator( TInt aNetwork );
    
    /**
     * Function: WriteNmsLogtoXmlL
     * logs nmsEvents to Xml file
     */    
    IMPORT_C void WriteNmsLogtoXmlL();
    /**
    * Function: UpdateNmsType
    * updates the Nms type
    * @param Nms type
    */    
    IMPORT_C void UpdateNmsType( const TDesC& aNmsType );        
    /**
    * Function: UpdateNmsChannelID
    * updates the Nms channel id
    * @param Nms channel id
    */    
    IMPORT_C void UpdateNmsChannelID( TInt aChanneldID );
    /**
    * Function: SetDbStatusObserser
    * @param Db Status Observer
    */    
    IMPORT_C void SetDbStatusObserser( MIRLogDbStatusObserver *aDbStatusObserver );
    
    /**
    * Function: ReportSent();
    * notify that the usage report is just sent out to isds server
    */    
    IMPORT_C void ReportSent();    
private:

    /**
    * Function: CollectSettingsData
    * collect information during starting of session
    * like irid, version information.
    */        
    void CollectSettingsData();
    
    /**
    * Function: WriteToXmlL
    * gathers the information and generates sesion log file and zip file
    * file will be stored in private path
    */    
    void WriteToXmlL();        
    
    /**
    * Function: WriteSessionData
    * gathers session log data and writes to XML file
    * @param no of urls tried to connect in that particular session
    */        
    void WriteSessionData(TInt aIndex);
    
    /**
    * Function: WriteBrowseLog
    * gathers browse log data and writes to XML file
    */    
    void WriteBrowseLog();
    
    /**
    * Function: ZipFileL
    * generates the zip file from the input xml file
    * output file is in gzip format and is stored in private path
    */    
    void ZipFileL();
    
    /**
    * Function: FormatDateTime
    * formats the date time given in "%d-%d-%d %d:%d:%d GMT\n" format
    * copies the result to a descriptor
    * @param reference of descriptor to which the data is to be copied
    * @param time to be copied
    */    
    void FormatDateTime( TDes& aBuffer, TDateTime aDateTime );
    
    /**
    * Function : WriteSessionLog
    * Write buffer content to log file
    */
    void WriteBufferToLogFile();
    
    /**
    * Function : UnForceSessionEndedL
    * Write session log
    * @param instance of session which ended
    */
    void UnForceSessionEndedL( CIRSessionLogger* aSession );

    /**
     * The following methods are for Nms Event Usage Logging 
     */
    
    /**
    * Function: WriteNmsLogData
    * gathers NmsLog data and writes to XML file
    * @param position of the log in the log data array
    */      
    void WriteNmsLogData( TInt aIndex );
    
    /**
     * From MIRPhoneInfoObserver
     */
    void ImeiUpdatedL( const TDesC& aImei );
    
private:
    /**
    * opsetting instance 
    */
    CIRSettings* iSettingsData;
    
    /**
    * Phone Info 
    */  
    CPhoneInfo* iPhoneInfo;
    
    /**
    * instance of session logger
    */
    CIRSessionLogger* iSessionLog;
    /**
    * instance of nms logger
    */
    CIRNmsLogger* iNmsLog;
    
    /**
    * instance of sessiondb
    */ 
    CIRSessionDb* iLogSessionDb;
    /**
    * instance of NmsLogdb
    */ 
    CIRNmsLogDb* iNmsLogDb;
    /**
    * instance of Db Status Observer
    */
    MIRLogDbStatusObserver *iDbStatusObserver;
    /**
    * Indicate whether Nms Log Db has been updated
    */
    TBool iNmsLogDbUpdated;
    /**
    * Indicate whether Session Log Db has been updated
    */
    TBool iSessionLogDbUpdated;
    /**
    * Indicate whether usage report sent to isds server has received successful response
    */
    TBool iUnComfirmedReportSent;
    /**
    * it holds the sessions
    */
    CArrayPtrFlat<CIRSessionLogger> *iSessionData;
    /**
    * it holds the NmsLog
    */
    CArrayPtrFlat<CIRNmsLogger> *iNmsLogData;
    
    /**
    * position in xml file
    */
    TInt iFileWritePos;

    /**
    * stores the network during browsing
    */
    TInt iCurrentBrowseNetwork;
    
    /**
    * Checks whether any error has occured while 
    * writing or retriving from data base
    */
    TBool iSessionLogUpdationFailed;
    /**
    * Checks whether any error has occured while 
    * writing or retriving from data base
    */
    TBool iNmsLogUpdationFailed;
    
    /**
    * Checks whether db creation is failure or success
    */
    TBool iSessionDbCreationFailed;
    /**
    * Checks whether db creation is failure or success
    */
    TBool iNmsDbCreationFailed;
    /*
    * sets when an nms event is generated
    */
    TBool iNmsEventFlag;
    /**
    * log file
    */
    RFile iLogFile;
    
    /*
    * file session
    */
    RFs iFsSession;
    
    /**
    * unicode buffer
    */ 
    //"MAX_URL_CHARACTOR_NUMBER = 255" is defined 
    // in iropenwebaddressview.cpp, it is 8bit len char.
    // the memory should be double when tranfer 8bit to 16bit.  
    TBuf<512> iUniCodeBuf;  //Unicode
    
    /**
    * utf 8 buffer
    */
    TBuf8<256> iUTF8Buffer; //UTF-8
    
    /**
    * version number
    */
    TBuf<5> iReportVersion;
    
    /**
    * irid
    */
    TBuf<30>  iIRId;
    
    /**
    * Base64 encoded IMEI
    */
    HBufC *iImeiBase64;
    
    /**
    * session database file
    */
    TFileName iSessFile;
    /**
    * session database file
    */
    TFileName iNmsLogFile;
    
    /**
    * log files path
    */
    TFileName iLogFilePath;
    
    /**
    * instance of browse log    
    */
    TIRBrowseLog iRBrowseLog;
    
    /**
    * session start time
    */
    TTime iSessionStartTime;
    
    /**
     * Timestamp
    */
    TTime iSongRecogTimestamp;  
    
    /**
    * time stamp
    */
    TTime iBrowseLogTimeStamp;
    
    /**
    * date
    */
    TDateTime iDate;
    
    /**
    * array for storing browse log
    */    
    RArray<TIRBrowseLog> iBrowseLog;
    
    /**
    * number of browse logs written to xml
    */
    TInt iBrowseLogsCntToXml;

    
    /**
     * The following methods are for Nms Event Usage Logging 
     */
        
    /**
     * Timestamp
     */
    TTime iNmsTimestamp; 
    
    /**
     * Type of Event ( i.e., "find" or "launch" )
     */
    TInt iNmsType;
    
    /**
     * Nms Event Current netwrok type
     */
    TInt iNmsCurrentNetwork;
    
    /**
     * Nms Event Home Operator
     */
    TInt iNmsHomeOperator;   
    };

#endif //C_IRSESSIONLOGGER_H
