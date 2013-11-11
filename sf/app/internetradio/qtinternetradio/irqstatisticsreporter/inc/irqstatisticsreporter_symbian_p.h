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


#ifndef IRQSTATISTICSREPORTER_SYMBIAN_P_H_
#define IRQSTATISTICSREPORTER_SYMBIAN_P_H_

#include <QObject>
#include <QString>
#include "irqstatisticsreporter.h"
#include "irdbstatusobserver.h"

class CIRReportGenerator;
class IRQNwkInfoObserver;
class IRQNetworkController;
class IRQIsdsClient;

class IRQStatisticsReporterPrivate : public QObject,
                                     public MIRLogDbStatusObserver
{
    Q_OBJECT

public:
    IRQStatisticsReporterPrivate();
    ~IRQStatisticsReporterPrivate();
    
    bool init();
    
public:    
    void markSessionStart();
    void sessionStarted(const int aChannelId, 
                        const IRQStatisticsReporter::IRConnectedFrom aConnectedFrom);
    void suspendSession();
    void restartSession();  
    void sessionEnded(const IRQStatisticsReporter::IRTerminatedType aTerminatedBy); 
    void logServerResult(const QString& aUrl,
                         const IRQStatisticsReporter::IRServerResult aServerResult);  
    void logNmsEvent(const IRQStatisticsReporter::IRNmsType aNmsType,
                     const int aChannelId);
    void logSongRecogEvent();    
    
private:
    /*!
     * From MIRLogDbStatusObserver.
     */       
    void LogDbNeedFlush();
    /*!
     * Function: sendReport
     * send the statistics report to isds server.
     */    
    void sendReport();
    /*!
     * Function: startMonitoringNwkInfo
     * start monitoring nwk info, HomeOperator and CurrentNetwork.
     */      
    void startMonitoringNwkInfo();
           
    void updateConnectionType();    
    void updateTerminatedBy(const IRQStatisticsReporter::IRTerminatedType aTerminatedBy);
    void updateConnectedFrom(const IRQStatisticsReporter::IRConnectedFrom aConnectedFrom);
    void updateNmsType(const IRQStatisticsReporter::IRNmsType aNmsType);
    void doLogNmsEventL(const IRQStatisticsReporter::IRNmsType aNmsType, const int aChannelId); 
    
private slots:
    void updateCurrentNetwork(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC);
    void updateHomeOperator(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC);

private:
    
    CIRReportGenerator *mBody;
    
    /*!
     * Used for sending out report
     */     
    IRQIsdsClient *mIsdsClient;  
     
    /*!
     * Used for querying the connection status
     */    
    IRQNetworkController *mNwkController; 
    
    /*!
     * Used for getting nwk info, HomeOperator and CurrentNetwork
     */      
    IRQNwkInfoObserver *mNwkInfoObserver;
    
    QString iLogFile;

    bool mSessionOngoing;
};

#endif /* IRQSTATISTICSREPORTER_SYMBIAN_P_H_ */
