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

#include <xqsettingsmanager.h>
#include <xqsysinfo.h>
#include <gsmerror.h>
#include <exterror.h>
#include <etelmm.h>
#include <btsapdomainpskeys.h>
#include "ut_cpplugincommon.h"
#include "cpplugincommon.h"
#include "qtestmains60.h"

/*!
  UT_CpPluginCommon::UT_CpPluginCommon
 */
UT_CpPluginCommon::UT_CpPluginCommon() 
    : mWrapper(NULL)
{
}

/*!
  UT_CpPluginCommon::~UT_CpPluginCommon
 */
UT_CpPluginCommon::~UT_CpPluginCommon()
{
    delete mWrapper;
}

/*!
  UT_CpPluginCommon::init
 */
void UT_CpPluginCommon::init()
{
    initialize();

    mWrapper = new SettingsWrapper();
}

/*!
  UT_CpPluginCommon::cleanup
 */
void UT_CpPluginCommon::cleanup()
{
    reset();
    
    delete mWrapper;
    mWrapper = NULL;
}

/*!
  UT_CpPluginCommon::t_memleak
 */
void UT_CpPluginCommon::t_memleak()
{
    
}

/*!
  UT_CpPluginCommon::t_showCallDuration
 */
void UT_CpPluginCommon::t_showCallDuration()
{
    EXPECT(XQSettingsManager, readItemValue).
        returns(QVariant(23324));
    
    mWrapper->showCallDuration();
    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_setShowCallDuration
 */
void UT_CpPluginCommon::t_setShowCallDuration()
{
    mWrapper->setShowCallDuration(true);
    mWrapper->setShowCallDuration(false);
    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_readSoftRejectText
 */
void UT_CpPluginCommon::t_readSoftRejectText()
{
    QString text="";
    bool userDefined=true;
    EXPECT(XQSettingsManager, readItemValue).
            returns(QVariant(true));
    
    EXPECT(XQSettingsManager, readItemValue).
            returns(QVariant("text"));
    
    mWrapper->readSoftRejectText(text, userDefined);
    
    userDefined=false;
    EXPECT(XQSettingsManager, readItemValue).
                    returns(QVariant(false));
    EXPECT(XQSettingsManager, readItemValue).
            returns(QVariant("text"));
    mWrapper->readSoftRejectText(text, userDefined);
    
    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_writeSoftRejectText
 */
void UT_CpPluginCommon::t_writeSoftRejectText()
{
    QString text="";
    bool userDefined=true;
    mWrapper->writeSoftRejectText(text, userDefined);
    userDefined=false;
    mWrapper->writeSoftRejectText(text, userDefined);
    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_numberGroupingSupported
 */
void UT_CpPluginCommon::t_numberGroupingSupported()
{
    EXPECT(XQSettingsManager, readItemValue).
            returns(QVariant(true));
    
    mWrapper->numberGroupingSupported();
    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_voipSupported
 */

void UT_CpPluginCommon::t_voipSupported()
{
    EXPECT(XQSysInfo, isSupported).with(
            KFeatureIdCommonVoip).returns(false);
    QCOMPARE( Tools::voipSupported(), false );
    EXPECT(XQSysInfo, isSupported).with(
            KFeatureIdCommonVoip).returns(true);
    QCOMPARE( Tools::voipSupported(), true );
    
    QVERIFY( verify() );
}

/*!
  UT_CpPluginCommon::t_videoSupported
 */
void UT_CpPluginCommon::t_videoSupported()
{
    EXPECT(XQSysInfo, isSupported).with(
            KFeatureIdCsVideoTelephony).returns(true);
    QCOMPARE( Tools::videoSupported(), true );
    EXPECT(XQSysInfo, isSupported).with(
            KFeatureIdCsVideoTelephony).returns(false);
    QCOMPARE( Tools::videoSupported(), false );
    
    QVERIFY( verify() );
}

/*!
  UT_CpPluginCommon::t_readVtVideoSending
 */
void UT_CpPluginCommon::t_readVtVideoSending()
{
    EXPECT(XQSettingsManager, readItemValue).
            returns(QVariant(1));
    mWrapper->readVtVideoSending();
    QVERIFY( verify() );
}

/*!
  UT_CpPluginCommon::t_writeVtVideoSending
 */
void UT_CpPluginCommon::t_writeVtVideoSending()
{
    EXPECT(XQSettingsManager, writeItemValue);//.
            //times(2);
    int i=0;
    mWrapper->writeVtVideoSending(i);
    QVERIFY( verify() );
}

/*!
  UT_CpPluginCommon::t_isFeatureCallWaitingDistiquishNotProvisionedEnabled
 */
void UT_CpPluginCommon::t_isFeatureCallWaitingDistiquishNotProvisionedEnabled()
{
    EXPECT(XQSettingsManager, readItemValue).
                returns(QVariant(1));
    mWrapper->isFeatureCallWaitingDistiquishNotProvisionedEnabled();
    QVERIFY( verify() );
}

/*!
  UT_CpPluginCommon::t_isPhoneOffline
 */
void UT_CpPluginCommon::t_isPhoneOffline()
{
    EXPECT(XQSettingsManager, readItemValue).
                returns(QVariant(1));
     mWrapper->isPhoneOffline();

    QVERIFY(verify());
}

/*!
  UT_CpPluginCommon::t_isOngoingCall
 */
void UT_CpPluginCommon::t_isOngoingCall()
{
    EXPECT(XQSettingsManager, readItemValue).
                    returns(QVariant(1));
    mWrapper->isOngoingCall();
    QVERIFY( verify() );
}

QTEST_MAIN_S60(UT_CpPluginCommon)
