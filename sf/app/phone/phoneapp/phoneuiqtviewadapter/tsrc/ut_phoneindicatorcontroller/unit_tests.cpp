/*!
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Unit tests for PhoneIndicatorController.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <HbGlobal>
#include <HbMenu>
#include <mockservice.h>

#include <QByteArray>
#include <QKeyEvent>
#include <QSignalSpy>
#include <HbApplication>
#include <xqcallinfo.h>
#include <callinformation.h>
#include <MediatorDomainUIDs.h>
#include <telinformationpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <ccallinfoiter.h>
#include <keyguardaccessapi.h>

#include <LogsDomainCRKeys.h>
#include <ctsydomaincrkeys.h>

#include "phonevisibilityhandler.h"
#include "phoneuiqtviewif_stub.h"

#include <logsmodel.h>
#include "phoneindicatorcontroller.h"
#include "phoneindicators.h"
#include "hbindicatorstubhelper.h"
#include "phoneuitestmain.h"


extern bool m_logsModelCreated;
extern bool m_serviceCreated;
extern bool m_aiwRequestCreated;
extern bool m_aiwRequestSended;
extern int m_modelMaxSize;
extern bool m_bringToForegroundCalled;

class UT_PhoneIndicatorController : public QObject, public MockService
{
    Q_OBJECT
public:
    UT_PhoneIndicatorController();
    virtual ~UT_PhoneIndicatorController();
    
signals:
    void valueChanged(const XQSettingsKey &key ,const QVariant &value);
    void userActivated(QString,QVariantMap data);
    void rowsInserted(const QModelIndex &, int, int );
    
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
    void testIndicatorController();
    void t_setActiveCallData();
    void t_clearActiveCallData();
    void t_enableActiveCallIndicator();
    
    void t_updateMissedCallIndicator();
    void t_updateDivertIndicator();
    void t_setMissedallIndicatorData();
    void t_handleInteraction(); 
    void t_handleInteraction1(); 
    void t_handleInteraction2(); 

private:
    PhoneUIQtViewIFStub *mPhoneUIQtViewIFStub;
    PhoneVisibilityHandler *m_handler;
    PhoneIndicatorController *m_phoneIndicatorController; // class under test
    HbMenu m_menu;  
};

UT_PhoneIndicatorController::UT_PhoneIndicatorController():
        m_phoneIndicatorController(NULL)
{
}

UT_PhoneIndicatorController::~UT_PhoneIndicatorController()
{
    delete m_phoneIndicatorController;
}

void UT_PhoneIndicatorController::initTestCase()
{
    mPhoneUIQtViewIFStub = new PhoneUIQtViewIFStub();
    m_handler = new PhoneVisibilityHandler(*mPhoneUIQtViewIFStub, this);
    m_phoneIndicatorController = new PhoneIndicatorController(*m_handler, this);
}

void UT_PhoneIndicatorController::cleanupTestCase()
{
    delete m_phoneIndicatorController;
    delete m_handler;
    delete mPhoneUIQtViewIFStub;
}

void UT_PhoneIndicatorController::init()
{
}

void UT_PhoneIndicatorController::cleanup()
{
    reset();
}

void UT_PhoneIndicatorController::testIndicatorController()
{
    QVERIFY(m_phoneIndicatorController);
}

void UT_PhoneIndicatorController::t_setActiveCallData()
{
    QVERIFY(m_phoneIndicatorController->m_cli.isEmpty());
    m_phoneIndicatorController->setActiveCallData();
    QVERIFY(!m_phoneIndicatorController->m_cli.isEmpty());
}

void UT_PhoneIndicatorController::t_clearActiveCallData()
{
    m_phoneIndicatorController->clearActiveCallData();
    QVERIFY(m_phoneIndicatorController->m_cli.isEmpty());
}

void UT_PhoneIndicatorController::t_enableActiveCallIndicator()
{   
    m_phoneIndicatorController->enableActiveCallIndicator();
    QVERIFY(!HbIndicatorStubHelper::indicatorActive());
    m_phoneIndicatorController->setActiveCallData();
    m_phoneIndicatorController->enableActiveCallIndicator();
    QVERIFY(HbIndicatorStubHelper::indicatorActive());
    m_phoneIndicatorController->disableActiveCallIndicator();
    QVERIFY(!HbIndicatorStubHelper::indicatorActive());    
}

void UT_PhoneIndicatorController::t_updateMissedCallIndicator()
{
    QObject::connect( this, SIGNAL( valueChanged(const XQSettingsKey & ,const QVariant &)),
            m_phoneIndicatorController, SLOT(updateMissedCallIndicator(XQSettingsKey, 
                    QVariant)));
    const XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
            KCRUidLogs.iUid, 
            KLogsNewMissedCalls );
    emit valueChanged(key, QVariant(1));
    QVERIFY(m_logsModelCreated); 
    QVERIFY(m_modelMaxSize = 2);
}

void UT_PhoneIndicatorController::t_updateDivertIndicator()
{
    QObject::connect( this, SIGNAL( valueChanged(const XQSettingsKey & ,const QVariant &)),
                m_phoneIndicatorController, SLOT(updateDivertIndicator(XQSettingsKey, 
                        QVariant)));
    const XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
            KCRUidCtsyCallForwardingIndicator.iUid, 
            KCtsyUnconditionalCFStatus);
    int x(KCFVoiceForwarded);
    emit valueChanged( key, QVariant(x).toByteArray());
    QVERIFY(HbIndicatorStubHelper::indicatorActive());
    emit valueChanged( key, QVariant(0).toByteArray());
    QVERIFY(!HbIndicatorStubHelper::indicatorActive());
}

void UT_PhoneIndicatorController::t_setMissedallIndicatorData()
{
    // TODO:
    QObject::connect( this, SIGNAL(rowsInserted(const QModelIndex &, int, int )),
                this, SLOT(setMissedallIndicatorData()));  
    QModelIndex index; 
    emit(rowsInserted(index,0,0));
}

void UT_PhoneIndicatorController::t_handleInteraction()
{
    QObject::connect(this,SIGNAL(userActivated(QString,QVariantMap)),
            m_phoneIndicatorController,SLOT(handleInteraction(QString,QVariantMap)));
    
    QVariantMap data;
    data.insert(QLatin1String("interaction"), OpenMissedCallView);
    emit userActivated(QString(),data);
    QVERIFY(m_serviceCreated);
    QVERIFY(m_aiwRequestCreated);
    QVERIFY(m_aiwRequestSended);
}
void UT_PhoneIndicatorController::t_handleInteraction1()
{
    QObject::connect(this,SIGNAL(userActivated(QString,QVariantMap)),
                    m_phoneIndicatorController,SLOT(handleInteraction(QString,QVariantMap)));
    QVariantMap data;
    data.insert(QLatin1String("interaction"), OpenCallUi);
    emit userActivated(QString(),data);
    QVERIFY(m_bringToForegroundCalled);
    
}
void UT_PhoneIndicatorController::t_handleInteraction2()
{
    QObject::connect(this,SIGNAL(userActivated(QString,QVariantMap)),
                m_phoneIndicatorController,SLOT(handleInteraction(QString,QVariantMap)));
        
    QVariantMap data;
    data.insert(QLatin1String("interaction"), OpenDiverSettingsView);
    emit userActivated(QString(),data);
    QVERIFY(m_serviceCreated);
    QVERIFY(m_aiwRequestCreated);
    QVERIFY(m_aiwRequestSended);
}

PHONE_UITEST_MAIN(UT_PhoneIndicatorController)
#include "unit_tests.moc"

