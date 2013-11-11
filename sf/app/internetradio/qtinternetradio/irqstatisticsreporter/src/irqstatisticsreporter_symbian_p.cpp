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
* Description:  Qt wrapper class implementation for CIRReportGenerator
*
*/
#include <e32cmn.h>
#include "irsessionlogger.h" // CIRReportGenerator
#include "irqisdsclient.h"
#include "irqsettings.h"
#include "irqnetworkcontroller.h"
#include "irreportsettings.h"
#include "irqnwkinfoobserver.h"
#include "irqstatisticsreporter_symbian_p.h"
#include "irqnetworkcontroller.h"
#include "irqlogger.h"

//Network Indicator
_LIT(KGPRS,"gprs");
_LIT(KWCDMA,"wcdma");
_LIT(KWLAN,"wlan");
_LIT(KCDMA2000,"cdma2000");
_LIT(KEDGE,"edge");

//Music Shop Type
_LIT(KFind,"find");
_LIT(KLaunch,"launch");

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporterPrivate::IRQStatisticsReporterPrivate():mBody(NULL),
                                               mIsdsClient(NULL),
                                               mNwkController(NULL),
                                               mNwkInfoObserver(NULL),
                                               mSessionOngoing(false)
{

}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporterPrivate::~IRQStatisticsReporterPrivate()
{
    if(mBody)
    {
        mBody->SetDbStatusObserser(NULL);
        mBody->Close();
    }
        
    if(mIsdsClient)
    {
        mIsdsClient->closeInstance();
    }

    if(mNwkController)
    {
        mNwkController->closeInstance();
    }

    if(mNwkInfoObserver)
    {
        mNwkInfoObserver->closeInstance();
    }     
}

bool IRQStatisticsReporterPrivate::init()
{
    if(NULL == mBody)
    {
        TRAP_IGNORE( mBody = CIRReportGenerator::OpenL());
        if(mBody)
        {
            mBody->SetDbStatusObserser(this);
        }
    }
    
    if(NULL == mIsdsClient)
    {    
        mIsdsClient = IRQIsdsClient::openInstance();   
    }  
    
    if(NULL == mNwkController)
    {
        mNwkController = IRQNetworkController::openInstance();
    }
    

    IRQSettings *settingManager = IRQSettings::openInstance();
    if(settingManager)
    {
        QString logFilePath = settingManager->privatePath();
        QString logFileName = QString::fromUtf16(KGZipLogFileName().Ptr(), KGZipLogFileName().Length());
        iLogFile = logFilePath + logFileName;
        settingManager->closeInstance();
    }

    
    if(NULL == mNwkInfoObserver)
    {
        mNwkInfoObserver = IRQNwkInfoObserver::openInstance();
        startMonitoringNwkInfo();
    } 
        
    if( mBody
        && mIsdsClient 
        && mNwkController
        && mNwkInfoObserver
        && !iLogFile.isEmpty())
    {
        return true;
    }
    else
    {
        return true;
    }
}    

// ---------------------------------------------------------------------------
// marks the start of a session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::markSessionStart()
{
    if(mBody && mSessionOngoing)
    {
        mBody->MarkSessionStart();
    }
}

// ---------------------------------------------------------------------------
// starts a new session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::sessionStarted(const int aChanneldId, 
    const IRQStatisticsReporter::IRConnectedFrom aConnectedFrom)
{
    if(!mSessionOngoing)
    {
        if(mBody)
        {
        	updateConnectedFrom(aConnectedFrom);
            mBody->UpdateChannelID(aChanneldId);
            updateConnectionType();
    
            TRAPD(error,mBody->SessionStartedL());
            if(KErrNone == error)
            {
                mSessionOngoing = true;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// function handle session suspend
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::suspendSession()
{
    if(mBody && mSessionOngoing)
    {
        mBody->HandleStopEvents(EFalse);
    }
}

// ---------------------------------------------------------------------------
// function handle session restart
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::restartSession()
{
    if(mBody && mSessionOngoing)
    {
        mBody->HandleStopEvents(ETrue);
    }
}

// ---------------------------------------------------------------------------
// ends the session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::sessionEnded(const IRQStatisticsReporter::IRTerminatedType aTerminatedBy)
{
    if(mBody && mSessionOngoing)
    {
        updateTerminatedBy(aTerminatedBy);
        TRAP_IGNORE(mBody->SessionEndL());
    }
    mSessionOngoing = false;
}

// ---------------------------------------------------------------------------
// logs server connection result
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::logServerResult(const QString& aUrl,
    const IRQStatisticsReporter::IRServerResult aServerResult)
{
    if(mBody && mSessionOngoing)
    {
        TPtrC url(reinterpret_cast<const TUint16*>(aUrl.utf16()));
        switch(aServerResult)
        {
            case IRQStatisticsReporter::EIRConnected:
                mBody->LogServerResult(url,EIRConnected);
                break;
            case IRQStatisticsReporter::EIRServerFull:
                mBody->LogServerResult(url,EIRFull);
                break;
            case IRQStatisticsReporter::EIRTimeOut:
                mBody->LogServerResult(url,EIRConnTimeOut);
                break;
            case IRQStatisticsReporter::EIRConnectFailed:
                mBody->LogServerResult(url,EIRConnFailed);
                break;
            default:
                mBody->LogServerResult(url,EIRConnFailed);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// logs nmsEvents to Xml file
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::logNmsEvent(const IRQStatisticsReporter::IRNmsType aNmsType,
                       const int aChanneldId)
{
    TRAP_IGNORE(doLogNmsEventL(aNmsType,aChanneldId));
}

// ---------------------------------------------------------------------------
// logs nmsEvents to Xml file
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::doLogNmsEventL(const IRQStatisticsReporter::IRNmsType aNmsType,
                       const int aChanneldId)
{
    if(mBody)
    {
    	updateNmsType(aNmsType);
        mBody->UpdateNmsChannelID(aChanneldId);
        mBody->NmsLogStartedL();
        mBody->WriteNmsLogtoXmlL();
    }
}

// ---------------------------------------------------------------------------
// logs songRecog event to Xml file 
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::logSongRecogEvent()
{
    if(mBody && mSessionOngoing)
    {
        mBody->LogSongRecog();
    }
}

// ---------------------------------------------------------------------------
// updates the current network
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateCurrentNetwork(const QString &aCurrentNwkMCC,
                           const QString &aCurrentNwkMNC)
{
    QString nwkCode = aCurrentNwkMCC + aCurrentNwkMNC;
    if(mBody)
    {
        mBody->UpdateCurrentNetwork(nwkCode.toInt());
        mBody->UpdateCurrentBrowseNetwork(nwkCode.toInt());
        mBody->UpdateNmsCurrentNetwork(nwkCode.toInt());
    }
}

// ---------------------------------------------------------------------------
// updates the home network
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateHomeOperator(const QString &aHomeNetworkMCC,
                           const QString &aHomeNetworkMNC)
{
    QString nwkCode = aHomeNetworkMCC + aHomeNetworkMNC;
    if(mBody)
    {    
        mBody->UpdateHomeOperator(nwkCode.toInt());
        mBody->UpdateNmsHomeOperator(nwkCode.toInt());  
    }  
}

// ---------------------------------------------------------------------------
// updates the termination status
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateTerminatedBy(const IRQStatisticsReporter::IRTerminatedType aTerminatedBy)
{
    if(mBody)
    {
        switch(aTerminatedBy)
        {
            case IRQStatisticsReporter::EIRUserTerminated:
                mBody->UpdateTerminatedBy(EUserTerminated);
                break;
            case IRQStatisticsReporter::EIRNoConnToServer:
                mBody->UpdateTerminatedBy(ENoConnectionToServer);
                break;
            case IRQStatisticsReporter::EIRNoConnToNetwork:
                mBody->UpdateTerminatedBy(ENoConnectionToNetwork);
                break;
            default:
                mBody->UpdateTerminatedBy(EUserTerminated);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// updateds connected information
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateConnectedFrom(const IRQStatisticsReporter::IRConnectedFrom aConnectedFrom)
{
    if(mBody)
    {
        switch(aConnectedFrom)
        {
            case IRQStatisticsReporter::EIRIsds:
                mBody->UpdateConnectedFrom(EIRIsds);
                break;
            case IRQStatisticsReporter::EIRPresetAdhoc:
                mBody->UpdateConnectedFrom(EIRPresetAdhoc);
                break;
            case IRQStatisticsReporter::EIRPresetIsds:
                mBody->UpdateConnectedFrom(EIRPresetIsds);
                break;
            case IRQStatisticsReporter::EIRHistoryAdhoc:
                mBody->UpdateConnectedFrom(EIRHistoryAdhoc);
                break;
            case IRQStatisticsReporter::EIRHistoryIsds:
                mBody->UpdateConnectedFrom(EIRHistoryIsds);
                break;
            case IRQStatisticsReporter::EIRAdhocManual:
                mBody->UpdateConnectedFrom(EIRAdhocManual);
                break;
            case IRQStatisticsReporter::EIRAdhocExternal:
                mBody->UpdateConnectedFrom(EIRAdhocExternal);
                break;                                
            default:
                mBody->UpdateConnectedFrom(EIRIsds);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// updates the connection type
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateConnectionType()
{
    if(mBody && mNwkController)
    {
        IRQConnectionType connectionType = mNwkController->identifyConnectionType();
        switch(connectionType)
        {
            case EIRQGprs:
                mBody->UpdateConnectionType(KGPRS);
                break;
            case EIRQWcdma:
                mBody->UpdateConnectionType(KWCDMA);
                break;
            case EIRQCdma2000:
                mBody->UpdateConnectionType(KCDMA2000);
                break;                
            case EIRQWiFi:
                mBody->UpdateConnectionType(KWLAN);
                break;
            case EIRQEdge:
                mBody->UpdateConnectionType(KEDGE);
                break;
            default:
                mBody->UpdateConnectionType(KNullDesC);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// updates the nokia music shop access type
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::updateNmsType(const IRQStatisticsReporter::IRNmsType aNmsType)
{
    if(mBody)
    {
        switch(aNmsType)
        {
            case IRQStatisticsReporter::EIRNmsFind:
                mBody->UpdateNmsType(KFind);
                break;
            case IRQStatisticsReporter::EIRNmsLaunch:
                mBody->UpdateNmsType(KLaunch);
                break;
            default:
                mBody->UpdateNmsType(KNullDesC);
                break;
        }
    }
}


// ---------------------------------------------------------------------------
// From MIRLogDbStatusObserver
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::LogDbNeedFlush()
{
    sendReport();
}

// ---------------------------------------------------------------------------
// Send the statistics report to isds server
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::sendReport()
{
    if(mNwkController)
    {
    	  if(mNwkController->getNetworkStatus() 
    	     && mIsdsClient
    	     && !iLogFile.isEmpty())
    	  {
            mIsdsClient->isdsPostLog(iLogFile);
            if(mBody)
            {
                mBody->ReportSent();
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  Start to monitor network info changes
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporterPrivate::startMonitoringNwkInfo()
{
    if(mNwkInfoObserver)
    {
        mNwkInfoObserver->startMonitorNwkInfo();	
        connectAndAssert(mNwkInfoObserver, SIGNAL(currentNwkChanged(const QString, const QString)), 
            this, SLOT(updateCurrentNetwork(const QString, const QString)));
        connectAndAssert(mNwkInfoObserver, SIGNAL(homeNwkChanged(const QString, const QString)),
            this, SLOT(updateHomeOperator(const QString, const QString)));   	
    }
}
