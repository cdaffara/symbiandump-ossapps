/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles phone indicators.
*
*/

#include "phoneindicatorcontroller.h"
#include "phoneindicators.h"
#include "phonevisibilityhandler.h"
#include "qtphonelog.h"

#include <QByteArray>
#include <hbicon.h>

#ifdef Q_OS_SYMBIAN
#include <logsmodel.h>
#include <logsevent.h>
#include <logsabstractmodel.h>
#include <logsfilter.h>
#include <LogsDomainCRKeys.h>
#include <ctsydomaincrkeys.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <xqaiwdecl.h>
#endif

Q_DECLARE_METATYPE(LogsEvent *)


namespace PhoneIndicatorControllerKeys{
    const XQSettingsKey missedCallsSettingsKey( XQSettingsKey::TargetCentralRepository, 
        KCRUidLogs.iUid, KLogsNewMissedCalls );

    const XQSettingsKey unconditionalCFKey( XQSettingsKey::TargetCentralRepository, 
        KCRUidCtsyCallForwardingIndicator.iUid, KCtsyUnconditionalCFStatus );

    const XQSettingsKey currentCFKey( XQSettingsKey::TargetCentralRepository, 
            KCRUidCtsyCallForwardingIndicator.iUid, KCtsyCallForwardingIndicator );     
}



PhoneIndicatorController::PhoneIndicatorController(
    PhoneVisibilityHandler& visibilityHandler, QObject *parent):
    QObject(parent), m_logsModel(0), m_missedCallsFilter(0), 
    m_request(0), m_phoneVisibilityHandler(visibilityHandler)
{
    PHONE_TRACE
#ifdef Q_OS_SYMBIAN
    m_setManager = new XQSettingsManager(this);
    connect( m_setManager, 
            SIGNAL( valueChanged(const XQSettingsKey & ,const QVariant &)),
                this, SLOT(updateMissedCallIndicator(XQSettingsKey, 
                        QVariant)));
    
    connect( m_setManager, 
            SIGNAL( valueChanged(const XQSettingsKey & ,const QVariant &)),
                this, SLOT(updateDivertIndicator(XQSettingsKey, 
                        QVariant)));
    
    fetchMissedCalls(
            m_setManager->readItemValue(
                    PhoneIndicatorControllerKeys::missedCallsSettingsKey).toInt());
    
    m_setManager->startMonitoring( PhoneIndicatorControllerKeys::missedCallsSettingsKey );
    m_setManager->startMonitoring( PhoneIndicatorControllerKeys::unconditionalCFKey, 
            XQSettingsManager::TypeByteArray);
    m_setManager->startMonitoring( PhoneIndicatorControllerKeys::currentCFKey,
            XQSettingsManager::TypeByteArray);
    
    qRegisterMetaType<PhoneIndicatorInfo>("PhoneIndicatorInfo");    
    QByteArray bytes = m_setManager->readItemValue(
            PhoneIndicatorControllerKeys::unconditionalCFKey).toByteArray();
    if ( m_setManager->error() == XQSettingsManager::NoError ){
        int cfStatus=0;
        memcpy(&cfStatus, bytes.data_ptr()->array, sizeof(int));
        updateDiverIndicator(cfStatus & KCFVoiceForwarded);
    }
#endif
    connect(&m_indicator,SIGNAL(userActivated(QString,QVariantMap)),
            this,SLOT(handleInteraction(QString,QVariantMap)));
}

PhoneIndicatorController::~PhoneIndicatorController()
{   
#ifdef Q_OS_SYMBIAN
    
    XQPublishAndSubscribeSettingsKey missedCallCountKey(
                    (qint32)KCRUidLogs.iUid, (quint32)KLogsNewMissedCalls );
    m_setManager->stopMonitoring(missedCallCountKey);
    delete m_setManager;
    delete m_missedCallsFilter;
    delete m_logsModel;
#endif
}

void PhoneIndicatorController::setActiveCallData()
{
    PHONE_TRACE
    m_cli = hbTrId("txt_phone_dblist_ongoing_call");
    m_callImage = "qtg_mono_call";
}

void PhoneIndicatorController::clearActiveCallData()
{
    PHONE_TRACE
    m_cli.clear();
    m_callImage.clear();
    disableActiveCallIndicator();
}

void PhoneIndicatorController::enableActiveCallIndicator()
{
    PHONE_TRACE
    if (!m_cli.isEmpty()){
        QString indicatorType(indicatorName(PhoneActiveCallIndicator));
        QVariantMap parameters;
        
        parameters.insert(
                QVariant(HbIndicatorInterface::PrimaryTextRole ).toString(),
                m_cli);
        
        parameters.insert(QVariant( HbIndicatorInterface::DecorationNameRole).toString(),
                m_callImage);
        
        m_indicator.activate(indicatorType, parameters);
    }
}
void PhoneIndicatorController::disableActiveCallIndicator()
{
    PHONE_TRACE
    QString indicatorType(indicatorName(PhoneActiveCallIndicator));
    m_indicator.deactivate(indicatorType);
}

void PhoneIndicatorController::updateMissedCallIndicator(
        const XQSettingsKey &key, const QVariant &value)
{ 
#ifdef Q_OS_SYMBIAN
    PHONE_TRACE
    if (compareKeys(key, PhoneIndicatorControllerKeys::missedCallsSettingsKey) ){
        QString indicatorType(indicatorName(PhoneMissedCallIndicator));
        m_indicator.deactivate(indicatorType);
        // deleting null pointer has no effect.
        delete m_missedCallsFilter;
        m_missedCallsFilter = NULL;
        delete m_logsModel;
        m_logsModel = NULL;
        fetchMissedCalls(value.toInt());
    }   
#endif  
}

void PhoneIndicatorController::updateDivertIndicator(
        const XQSettingsKey &key, const QVariant &value)
{
    PHONE_TRACE
    if ( compareKeys( key, PhoneIndicatorControllerKeys::currentCFKey ) ){
        PHONE_DEBUG("currentCfKey");
        int cfStatus = value.toByteArray().toInt();
        updateDiverIndicator( cfStatus & KCFVoiceForwarded);
    } else if (compareKeys(key, PhoneIndicatorControllerKeys::unconditionalCFKey )){        
        QByteArray bytes = value.toByteArray();
        // Convert QByteArray elements into integer
        // Conversation does not care about sign bit
        int status;
        memcpy(&status, bytes.data_ptr()->array, sizeof(int));
        updateDiverIndicator(status & KCFVoiceForwarded);
    }
}

void PhoneIndicatorController::setMissedallIndicatorData()
{
#ifdef Q_OS_SYMBIAN
    PHONE_TRACE
    QString indicatorType(indicatorName(PhoneMissedCallIndicator));
    
    int missedCallCount = m_setManager->readItemValue( 
            PhoneIndicatorControllerKeys::missedCallsSettingsKey,
                XQSettingsManager::TypeInt ).toInt();
    
    if ( missedCallCount > 0 ) {
        QVariantMap parameters;
        LogsEvent *event = qVariantValue<LogsEvent*>(m_missedCallsFilter->data(
                m_missedCallsFilter->index(0,0),
                LogsAbstractModel::RoleFullEvent));
        if (event){
            // First row
            parameters.insert( 
                    QVariant( HbIndicatorInterface::PrimaryTextRole ).toString(),
                    ( hbTrId("txt_phone_dblist_ln_missed_calls", 
                            missedCallCount)));
            
            if ( missedCallCount >= event->duplicates() ){
                QString lastMissedCallFrom;
                if ( event->isRemotePartyPrivate() ){
                    lastMissedCallFrom = hbTrId("txt_phone_other_unknown_number");
                } else {
                    lastMissedCallFrom = event->remoteParty().isEmpty() ?
                        event->number() : event->remoteParty();
                }
                parameters.insert( 
                        QVariant( HbIndicatorInterface::SecondaryTextRole ).toString(), 
                        lastMissedCallFrom );
            }
            QString iconName = "qtg_mono_missed_call_unseen";
            parameters.insert(
                    QVariant( HbIndicatorInterface::DecorationNameRole ).toString(),
                    iconName );
        }
        m_indicator.activate(indicatorType, parameters);
    }
#endif
}

void PhoneIndicatorController::updateDiverIndicator(bool activeDiverts)
{
    PHONE_TRACE
    QString indicatorType(indicatorName(PhoneDivertIndidicator));
    if(activeDiverts){
        QVariantMap parameters;
        
        // First row
       parameters.insert( 
               QVariant( HbIndicatorInterface::PrimaryTextRole ).toString(),
               hbTrId("txt_phone_dblist_active_diverts" ));
        
        QString iconName = "qtg_mono_call_diverted";
        parameters.insert(
                QVariant( HbIndicatorInterface::DecorationNameRole ).toString(),
                iconName );
        
        m_indicator.activate( indicatorType, parameters );
    }else{
        m_indicator.deactivate( indicatorType );
    }        
}

bool PhoneIndicatorController::compareKeys(
        const XQSettingsKey &first, const XQSettingsKey &second)
{   
    PHONE_TRACE
    return ( first.key() == second.key() && first.uid() == second.uid() );
}

void PhoneIndicatorController::handleInteraction(QString /*type*/,QVariantMap data)
{
    PHONE_TRACE
    
    if (data.contains(QLatin1String("interaction")) && 
        data.value(QLatin1String("interaction")).canConvert<int>()) {
        
        XQApplicationManager appManager;
        QList<QVariant> args;
        QString service;
        QString interface;
        QString operation;
        QVariantHash hash;
        QVariantMap map;
        int interaction = data.value("interaction").toInt();
        
        switch(interaction){
            case OpenMissedCallView:
                service = "logs";
                interface = XQI_LOGS_VIEW;
                operation = XQOP_LOGS_SHOW;
                map.insert(XQLOGS_VIEW_INDEX , QVariant((int)XQService::LogsViewMissed));
                map.insert(XQLOGS_SHOW_DIALPAD, QVariant(false));
                map.insert(XQLOGS_DIALPAD_TEXT, QVariant(QString()));
                args.append(QVariant(map));
                break;
            case OpenCallUi:            
                m_phoneVisibilityHandler.bringToForeground();
                break;
            case OpenDiverSettingsView:
                interface = "com.nokia.symbian.ICpPluginLauncher";
                operation = "launchSettingView(QString,QVariant)";
                args << QVariant("cptelephonyplugin.dll");            
                hash["view"] = "divert_view";
                hash["heading"] = "txt_phone_subhead_telephone";
                args << hash;
                break;
            default:            
                break;
        }
        
        delete m_request;
        m_request = service.isEmpty() ? 
            appManager.create( interface, operation, false):
            appManager.create(service, interface, operation, false);
        if ( m_request == NULL ){
            return;       
        }   
        m_request->setArguments(args);
        m_request->send();
    }
}

void PhoneIndicatorController::fetchMissedCalls(int count)
{
    PHONE_TRACE1(count)
    
    if(count > 0 ){
        if(!m_logsModel){
            m_logsModel = new LogsModel(LogsModel::LogsFullModel);
        }
        if(!m_missedCallsFilter){
            m_missedCallsFilter = new LogsFilter(LogsFilter::Missed);
            connect( m_missedCallsFilter, 
                SIGNAL(rowsInserted(const QModelIndex &, int, int )),
                    this, SLOT(setMissedallIndicatorData()));
            m_missedCallsFilter->setSourceModel(m_logsModel);
        }
        m_missedCallsFilter->setMaxSize(count + 1);
    }
}
