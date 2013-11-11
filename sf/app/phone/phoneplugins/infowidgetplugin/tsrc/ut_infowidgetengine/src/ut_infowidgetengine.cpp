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
* Description:
*
*/
#include "ut_infowidgetengine.h"
#include "infowidgetengine.h"
#include "qtestmains60.h"
#include "infowidgetpreferences.h"

// mocked dependencies of the class under test
#include "infowidgetnetworkhandler.h"
#include "infowidgetsathandler.h"

const QString KSpnName("Spn name");    // 

const QString KHomeZoneTextTag("HomeZoneText0");    // max length 13
const TNWViagIndicatorType 
    KHomeZoneIndicatorType = ENWViagIndicatorTypeHomeZone;

const QString KMcnName("McnNameMcnNameMcnNam");     // max length 20 
const TNWMCNIndicatorType 
    KMcnIndicatorType = ENWMCNIndicatorTypeActive;

/*!
  UT_InfoWidgetEngine::UT_InfoWidgetEngine
 */
UT_InfoWidgetEngine::UT_InfoWidgetEngine() 
    : 
    m_infoWidgetEngine(0)
{
    
}

/*!
  UT_InfoWidgetEngine::~UT_InfoWidgetEngine
 */
UT_InfoWidgetEngine::~UT_InfoWidgetEngine()
{
    delete m_infoWidgetEngine;
}

/*!
  UT_InfoWidgetEngine::init
 */
void UT_InfoWidgetEngine::init()
{
    initialize();
    
    SmcDefaultValue<QString>::SetL("");
    SmcDefaultValue<const QString & >::SetL("");
    
    EXPECT(InfoWidgetNetworkHandler, isOnline)
        .returns(true);    
    
    EXPECT(InfoWidgetNetworkHandler, serviceProviderName)
        .returns(KSpnName);
    EXPECT(InfoWidgetNetworkHandler, serviceProviderNameDisplayRequired)
        .returns(true);

    EXPECT(InfoWidgetNetworkHandler, mcnName).returns(KMcnName);
    EXPECT(InfoWidgetNetworkHandler, mcnIndicatorType)
        .returns(KMcnIndicatorType);

    EXPECT(InfoWidgetNetworkHandler, homeZoneIndicatorType)
        .returns(KHomeZoneIndicatorType);
    EXPECT(InfoWidgetNetworkHandler, homeZoneTextTag)
        .returns(KHomeZoneTextTag);
    
    m_infoWidgetEngine = new InfoWidgetEngine();
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::cleanup
 */
void UT_InfoWidgetEngine::cleanup()
{
    reset();
    
    delete m_infoWidgetEngine;
    m_infoWidgetEngine = 0;
}

/*!
  UT_InfoWidgetEngine::t_modelData
 */
void UT_InfoWidgetEngine::t_modelData()
{
    const InfoWidgetEngine::ModelData& data = m_infoWidgetEngine->modelData();
    QVERIFY(data.serviceProviderName() == KSpnName);
    QVERIFY(data.serviceProviderNameDisplayRequired() == true);
    QVERIFY(data.homeZoneTextTag() == KHomeZoneTextTag);
    QVERIFY(data.homeZoneIndicatorType() == KHomeZoneIndicatorType);
    QVERIFY(data.mcnName() == KMcnName);
    QVERIFY(data.mcnIndicatorType() == KMcnIndicatorType);
}

/*!
  UT_InfoWidgetEngine::t_updateNetworkDataToModel
 */
void UT_InfoWidgetEngine::t_updateNetworkDataToModel()
{
    // Test: network status is online
    EXPECT(InfoWidgetNetworkHandler, isOnline)
        .returns(true);    
    
    EXPECT(InfoWidgetNetworkHandler, serviceProviderName)
        .returns(KSpnName);
    EXPECT(InfoWidgetNetworkHandler, serviceProviderNameDisplayRequired)
        .returns(true);

    EXPECT(InfoWidgetNetworkHandler, mcnName).returns(KMcnName);
    EXPECT(InfoWidgetNetworkHandler, mcnIndicatorType)
        .returns(KMcnIndicatorType);

    EXPECT(InfoWidgetNetworkHandler, homeZoneIndicatorType)
        .returns(KHomeZoneIndicatorType);
    EXPECT(InfoWidgetNetworkHandler, homeZoneTextTag)
        .returns(KHomeZoneTextTag);
    
    QSignalSpy spy(m_infoWidgetEngine, SIGNAL(modelChanged()));
    m_infoWidgetEngine->updateNetworkDataToModel();
    
    const int KExpectedNumOfSignalEmissions = 1;
    QCOMPARE(spy.count(), KExpectedNumOfSignalEmissions);
    const QList<QVariant> &arguments = spy.takeFirst(); 
    QCOMPARE(arguments.count(), 0);
    
    QVERIFY(verify());
    
    // Test: network status is offline
    EXPECT(InfoWidgetNetworkHandler, isOnline)
          .returns(false);    
    EXPECT(InfoWidgetNetworkHandler, serviceProviderName).times(0);
    EXPECT(InfoWidgetNetworkHandler, serviceProviderNameDisplayRequired).times(0);
    EXPECT(InfoWidgetNetworkHandler, mcnName).times(0);
    EXPECT(InfoWidgetNetworkHandler, mcnIndicatorType).times(0);
    EXPECT(InfoWidgetNetworkHandler, homeZoneIndicatorType).times(0);
    EXPECT(InfoWidgetNetworkHandler, homeZoneTextTag).times(0);
      
    m_infoWidgetEngine->updateNetworkDataToModel();
    QCOMPARE(spy.count(), KExpectedNumOfSignalEmissions);
    const QList<QVariant> &arguments2 = spy.takeFirst(); 
    QCOMPARE(arguments2.count(), 0);
    
    QVERIFY(verify());
    
    const InfoWidgetEngine::ModelData& data = m_infoWidgetEngine->modelData();
    QVERIFY(data.serviceProviderName() == QString(""));
    QVERIFY(data.homeZoneTextTag() == QString(""));
    QVERIFY(data.mcnName() == QString(""));
    
}

/*!
  UT_InfoWidgetEngine::t_updateSatDataToModel
 */
void UT_InfoWidgetEngine::t_updateSatDataToModel()
{
    EXPECT(InfoWidgetSatHandler, satDisplayText)
        .returns(QString(""));
    
    m_infoWidgetEngine->updateSatDataToModel();
}

/*!
  UT_InfoWidgetEngine::t_updateLineDataToModel
 */
void UT_InfoWidgetEngine::t_updateLineDataToModel()
{
    m_infoWidgetEngine->updateLineDataToModel();
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::t_handleNetworkError
 */
void UT_InfoWidgetEngine::t_handleNetworkError()
{
    m_infoWidgetEngine->handleNetworkError(0, 0);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::t_handleSatError
 */
void UT_InfoWidgetEngine::t_handleSatError()
{
    m_infoWidgetEngine->handleSatError(0, 0);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::t_handleLineError
 */
void UT_InfoWidgetEngine::t_handleLineError()
{
    m_infoWidgetEngine->handleLineError(0, 0);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::t_handlePreferencesChanged
 */
void UT_InfoWidgetEngine::t_handlePreferencesChanged()
{
    InfoWidgetPreferences::Options testOptions; 
    // Preconditions: 
    testOptions &= ~InfoWidgetPreferences::DisplayHomeZone;
    testOptions &= ~InfoWidgetPreferences::DisplayActiveLine;
    testOptions &= ~InfoWidgetPreferences::DisplaySpn;
    
    // Test 1: Mcn ON; SAT OFF   
    testOptions |= InfoWidgetPreferences::DisplayMcn; 
    testOptions &= ~InfoWidgetPreferences::DisplaySatText;
    EXPECT(InfoWidgetNetworkHandler, enableMcn); 
    EXPECT(InfoWidgetSatHandler, connect).with(false);
    m_infoWidgetEngine->handlePreferencesChanged(testOptions); 
    QVERIFY(verify());    
    
    // Test 2: Mcn OFF; SAT ON   
    testOptions &= ~InfoWidgetPreferences::DisplayMcn; 
    testOptions |= InfoWidgetPreferences::DisplaySatText;
    EXPECT(InfoWidgetNetworkHandler, disableMcn); 
    EXPECT(InfoWidgetSatHandler, connect).with(true);
    m_infoWidgetEngine->handlePreferencesChanged(testOptions); 
    QVERIFY(verify());    

}

/*!
  UT_InfoWidgetEngine::t_suspend
 */
void UT_InfoWidgetEngine::t_suspend()
{
    m_infoWidgetEngine->suspend();
        
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetEngine::t_resume
 */
void UT_InfoWidgetEngine::t_resume()
{
    m_infoWidgetEngine->resume();
        
    QVERIFY(verify());
}

QTEST_MAIN_S60(UT_InfoWidgetEngine)
