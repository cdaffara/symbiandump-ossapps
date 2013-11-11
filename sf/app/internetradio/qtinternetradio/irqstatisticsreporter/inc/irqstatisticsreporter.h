/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Qt wrapper class declaration for CIRReportGenerator
*
*/


#ifndef IRQSTATISTICSREPORTER_H_
#define IRQSTATISTICSREPORTER_H_

#include <QObject>
#include <QString>
#include <QMutex>

#include "irqstatisticsreporterexporter.h"
class IRQStatisticsReporterPrivate;

/*!
 * This class provides usage report related interface.
 *
 * IRQStatisticsReporter follows singleton pattern
 *
 */
class RQSTATISTICSREPORTER_DLL_EXPORT IRQStatisticsReporter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  Get the instance of IRQStatisticsReporter
     *  @return IRQStatisticsReporter*
     */
    static IRQStatisticsReporter* openInstance();

    /*!
     *  Close the instance of IRQStatisticsReporter
     */
    void closeInstance();

private:
    /*!
     * Constructor.
     */
    IRQStatisticsReporter();
    
    /*!
     * Destructor.
     */    
    ~IRQStatisticsReporter();
   
public:    

    /*!
     *
     * <session starttime="R" duration="R" terminatedBy="R" connectedFrom="R" id="R">
     *    <channelid>1234</channelid>
     *    <serverResult url="R">Connected</serverResult>
     *       <connection type="R">
     *          <operator currentNetwork="R" homeOperator=""/>
     *       </connection>
     *    <songRecogLog>
     *        <songRecogEvent timeStamp="R" /> 
     *    </songRecogLog>       
     * </session>
     */  
    

    /*!
     * Function: markSessionStart
     * marks the start of a session, because the session may not be started successfully due to network problem
     */  
    void markSessionStart();
        
    /*!
     * Function: sessionStarted
     * starts a new session
     */
    enum IRConnectedFrom
    {
        //when session started directly from station details in iSDS
        EIRIsds = 0,
        //when session started from a station details saved to iRAPP favorites and the station is not from iSDS (channel_Id = 0 )
        EIRPresetAdhoc,
        //when session started from a station details saved to iRAPP favorites and the station is from iSDS (channel_ID is iSDS channel_ID)
        EIRPresetIsds,
        //when session started from iRAPP 'station play history' and the station is not from iSDS (channel_Id = 0 )
        EIRHistoryAdhoc,
        //when session started from iRAPP 'station play history' and the station is from iSDS (channel_ID is iSDS channel_ID )
        EIRHistoryIsds,
        //when session started from manually entered stream url
        EIRAdhocManual,
        //when iRAPP session started/triggered from any iRAPP external client e.g. Browser or SMS/MMS
        EIRAdhocExternal
    };
    void sessionStarted(const int aChannelId, const IRConnectedFrom aConnectedFrom);

    /*!
     * Function: suspendSession
     * function handle session suspend
     */
    void suspendSession();

    /*!
     * Function: restartSession
     * function handle session restart
     */
    void restartSession();
                
    /*!
     * Function: sessionEnd
     * ends the session
     */    
    enum IRTerminatedType
    {
        EIRUnknownTermination = 0,
        //terminated by user
        EIRUserTerminated,
        //no connection to the server
        EIRNoConnToServer,
        //no connection to the network
        EIRNoConnToNetwork
    };     
    void sessionEnded(const IRTerminatedType aTerminatedBy);

    /*!
     * Function: logServerResult
     * logs server connection result
     * @param aUrl URL to which connection is attempted
     * @param aServerResult response from server
     */    
    enum IRServerResult
    {
        EIRConnected = 0,
        EIRServerFull,
        EIRTimeOut,
        EIRConnectFailed
    };     
    void logServerResult(const QString& aUrl, const IRServerResult aServerResult);  
    
    /*!
     * Function: logSongRecogEvent
     */  
    void logSongRecogEvent();     

    /*!
     * The following methods are for Browse Usage Report 
     *
     *<browseLog>
     *   <browse url="R" timeStamp="R" currentNetwork=""/>
     *</browseLog>
     */  
    void logBrowseEvent(const QString& aUrl);

    /*!
     * The following methods are for Nms Event Usage Logging 
     *
     * <nmsLog>
     *    <nmsEvent timeStamp="R" channelId="" type="R">
     *      <operator currentNetwork="R" homeOperator=""/>
     *    </nmsEvent>
     * </nmsLog>    
     */  
    
    /*!
     * Function: logNmsEvent
     * @param aNmsType Nms type  
     * @param aChannelId Nms channel id         
     */  
    enum IRNmsType
    {
        //find in music shop 
        EIRNmsFind = 0,
        //launch music shop
        EIRNmsLaunch
    };     
    void logNmsEvent(const IRNmsType aNmsType, const int aChannelId);   
    
private:
    IRQStatisticsReporterPrivate * const d_ptr;
    
    static QMutex mMutex;
    static int mRef;
    static IRQStatisticsReporter *mInstance;
    
    Q_DISABLE_COPY(IRQStatisticsReporter)     
};

#endif /* IRQSTATISTICSREPORTER_H_ */
