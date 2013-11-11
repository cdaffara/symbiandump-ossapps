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

#include "ut_networkpluginform.h"
#include "qtestmains60ui.h"
#include "cpitemdatahelper.h"
#include "psetwrapper.h"
#include "cpplugincommon.h"
#define private public
#include "cpnetworkpluginform.h"


class CPsetContainer
{
public:
    CPsetContainer(){};
    ~CPsetContainer(){};
};

const QString KIconGsmForbidden("qtg_small_network_off"); 
const QString KIconGsmNotForbidden("qtg_small_network");
const QString KIconWcdmaForbidden("qtg_small_wcdma_off"); 
const QString KIconWcdmaNotForbidden("qtg_small_wcdma");
const QString KIconGsmAndWcdmaForbidden("qtg_small_pd_wcdma_off"); 
const QString KIconGsmAndWcdmaNotForbidden("qtg_small_pd_wcdma");

/*!
  UT_CpNetworkPluginForm::UT_CpNetworkPluginForm
 */
UT_CpNetworkPluginForm::UT_CpNetworkPluginForm() 
    : m_networkPluginForm(NULL), i(0)
{
    SmcDefaultValue<PSetNetworkWrapper::ErrorCode>::SetL(PSetNetworkWrapper::ErrNoError);
}

/*!
  UT_CpNetworkPluginForm::~UT_CpNetworkPluginForm
 */
UT_CpNetworkPluginForm::~UT_CpNetworkPluginForm()
{

}

/*!
  UT_CpNetworkPluginForm::init
 */
void UT_CpNetworkPluginForm::init()
{
    initialize();

    QVERIFY(!m_networkPluginForm);
    CPsetContainer tmpPsetContainer;
    m_networkWrapper = new PSetNetworkWrapper(tmpPsetContainer);
    EXPECT(PSetWrapper, networkWrapper).returns(m_networkWrapper);
    EXPECT(PSetNetworkWrapper, isManualNetworkSelectionSupported).returns(true);
    if(i) {
        m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeAutomatic;
    } else {
        m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeManual;
    }
    i++;
    EXPECT(PSetNetworkWrapper, getNetworkSelectionMode).willOnce(invoke(this, &updateNetworkSelectionMode));
    m_networkPluginForm = new CpNetworkPluginForm();
    QVERIFY(m_networkPluginForm);
}


/*!
  UT_CpNetworkPluginForm::cleanup
 */
void UT_CpNetworkPluginForm::cleanup()
{
    reset();
    delete m_networkPluginForm;
    m_networkPluginForm = NULL;
    delete m_networkWrapper;
    m_networkWrapper = NULL;
}


/*!
  UT_CpNetworkPluginForm::updateNetworkSelectionMode
 */
void UT_CpNetworkPluginForm::updateNetworkSelectionMode(
    PSetNetworkWrapper::NetworkSelectionMode& mode)
{
    mode = m_NetworkSelectionMode;
}

/*!
  UT_CpNetworkPluginForm::t_memleak
 */
void UT_CpNetworkPluginForm::t_memleak()
{
    
}


/*!
  UT_CpNetworkPluginForm::t_networkModeStateChanged
 */
void UT_CpNetworkPluginForm::t_networkModeStateChanged()
{
    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_networkPluginForm->networkModeStateChanged(0);

    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_networkPluginForm->networkModeStateChanged(1);

    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_networkPluginForm->networkModeStateChanged(2);
    
    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_networkPluginForm->networkModeStateChanged(3);
    
    EXPECT(SettingsWrapper, isPhoneOffline).returns(true);
    m_networkPluginForm->networkModeStateChanged(0);
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_operatorSelectionStateChanged
 */
void UT_CpNetworkPluginForm::t_operatorSelectionStateChanged()
{
    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeAutomatic;
    expect("PSetNetworkWrapper::getNetworkSelectionMode").willOnce(invoke(this, &updateNetworkSelectionMode));
    m_networkPluginForm->operatorSelectionStateChanged();

    EXPECT(SettingsWrapper, isPhoneOffline).returns(false);
    m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeManual;
    expect("PSetNetworkWrapper::getNetworkSelectionMode").willOnce(invoke(this, &updateNetworkSelectionMode));
    m_networkPluginForm->operatorSelectionStateChanged();
    
    EXPECT(SettingsWrapper, isPhoneOffline).returns(true);
    m_networkPluginForm->operatorSelectionStateChanged();
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_networkAccessModeGot
 */
void UT_CpNetworkPluginForm::t_networkAccessModeGot()
{
    m_networkPluginForm->networkAccessModeGot(0);

    m_networkPluginForm->networkAccessModeGot(1);

    m_networkPluginForm->networkAccessModeGot(2);
    
    m_networkPluginForm->networkAccessModeGot(3);
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_availableNetworksGot
 */
void UT_CpNetworkPluginForm::t_availableNetworksGot()
{
    PSetNetworkWrapper::NetworkInfo temp;
    QList<PSetNetworkWrapper::NetworkInfo*> networkInfoList;
    networkInfoList.append(&temp);
    
    EXPECT(HbDialog, open);
    m_networkPluginForm->availableNetworksGot(networkInfoList);
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_networkReqestFailed
 */
void UT_CpNetworkPluginForm::t_networkReqestFailed()
{
    PSetNetworkWrapper::ErrorCode error(PSetNetworkWrapper::ErrCauseCallActive);
    PSetNetworkWrapper::RequestType type(PSetNetworkWrapper::RequestSetNetworkMode);
    EXPECT(PSetNetworkWrapper, getNetworkAccessMode);
    m_networkPluginForm->networkReqestFailed(error, type);
    
    error = PSetNetworkWrapper::ErrNoNetworkService;
    type = PSetNetworkWrapper::RequestSetNetworkMode;
    EXPECT(PSetNetworkWrapper, getNetworkAccessMode);
    m_networkPluginForm->networkReqestFailed(error, type);
    
    error = PSetNetworkWrapper::ErrOfflineOpNotAllowed;
    type = PSetNetworkWrapper::RequestSetNetworkMode;
    EXPECT(PSetNetworkWrapper, getNetworkAccessMode);
    m_networkPluginForm->networkReqestFailed(error, type);
    
    PSetNetworkWrapper::NetworkInfo temp;
    QList<PSetNetworkWrapper::NetworkInfo*> networkInfoList;
    networkInfoList.append(&temp);
    m_networkPluginForm->availableNetworksGot(networkInfoList);
    error = PSetNetworkWrapper::ErrNoNetworkAccess;
    type = PSetNetworkWrapper::RequestSetNetwork;
    EXPECT(HbDialog, open);
    m_networkPluginForm->networkReqestFailed(error, type);
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_userCancel
 */
void UT_CpNetworkPluginForm::t_userCancel()
{
    m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeManual;
    EXPECT(PSetNetworkWrapper, getNetworkSelectionMode).willOnce(invoke(this, &updateNetworkSelectionMode));
    m_networkPluginForm->userCancel();
    
    m_NetworkSelectionMode = PSetNetworkWrapper::SelectionModeAutomatic;
    EXPECT(PSetNetworkWrapper, getNetworkSelectionMode).willOnce(invoke(this, &updateNetworkSelectionMode));
    m_networkPluginForm->userCancel();
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_handleSearchingNetworks
 */
void UT_CpNetworkPluginForm::t_handleSearchingNetworks()
{
    PSetNetworkWrapper::RequestType type(PSetNetworkWrapper::RequestNone);
    m_networkPluginForm->handleSearchingNetworks(type);
    
    type = PSetNetworkWrapper::RequestEnumerateNetworks;
    m_networkPluginForm->handleSearchingNetworks(type);
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPluginForm::t_handleRequestingSelectedNetwork
 */
void UT_CpNetworkPluginForm::t_handleRequestingSelectedNetwork()
{
    m_networkPluginForm->handleRequestingSelectedNetwork(true);
    m_networkPluginForm->handleRequestingSelectedNetwork(false);
}

/*!
  UT_CpNetworkPluginForm::t_handleNetworkChanged
 */
void UT_CpNetworkPluginForm::t_handleNetworkChanged()
{
    PSetNetworkWrapper::NetworkInfo currentInfo;
    PSetNetworkWrapper::RegistrationStatus status(PSetNetworkWrapper::RegisteredOnHomeNetwork);
    m_networkPluginForm->handleNetworkChanged(currentInfo, status); 

    status = PSetNetworkWrapper::RegisteredRoaming;
    currentInfo.m_longName = "test";
    m_networkPluginForm->handleNetworkChanged(currentInfo, status);
    
    status = PSetNetworkWrapper::RegisteredRoaming;
    currentInfo.m_shortName = "test";
    m_networkPluginForm->handleNetworkChanged(currentInfo, status);
}
    
/*!
  UT_CpNetworkPluginForm::t_primaryIconForNetwork
 */
void UT_CpNetworkPluginForm::t_primaryIconForNetwork()
{
    QScopedPointer<PSetWrapper> wrapper(new PSetWrapper()); 
    EXPECT(PSetWrapper, networkWrapper).returns(wrapper.data()); 
    
    QScopedPointer<CpNetworkPluginFormAdapter> 
        formClassAdapter(new CpNetworkPluginFormAdapter());
    PSetNetworkWrapper::NetworkInfo currentInfo;
    
    // Test: forbidden icon flag ON, mode: GSM, status: forbidden 
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeGsm; 
    currentInfo.m_status = PSetNetworkWrapper::StatusForbidden; 
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(true); 
    QString iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconGsmForbidden); 
    
    // Test: forbidden icon flag ON, mode: GSM, status: available
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeGsm; 
    currentInfo.m_status = PSetNetworkWrapper::StatusAvailable; 
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(true); 
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconGsmNotForbidden); 
    
    // Test: forbidden icon flag ON, mode: WCDMA, status: forbidden 
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeWcdma; 
    currentInfo.m_status = PSetNetworkWrapper::StatusForbidden; 
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(true); 
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconWcdmaForbidden); 
     
    // Test: forbidden icon flag ON, mode: WCDMA, status: available
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeWcdma; 
    currentInfo.m_status = PSetNetworkWrapper::StatusAvailable; 
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(true); 
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconWcdmaNotForbidden); 

    // Test: forbidden icon flag ON, mode: GSM and WCDMA, status: forbidden 
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeGsmAndWcdma; 
    currentInfo.m_status = PSetNetworkWrapper::StatusForbidden; 
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(true); 
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconGsmAndWcdmaForbidden); 
     
    // Test: forbidden icon flag ON, mode: GSM and WCDMA, status: available
    currentInfo.m_access = PSetNetworkWrapper::AccessTypeGsmAndWcdma; 
    currentInfo.m_status = PSetNetworkWrapper::StatusAvailable; 
    EXPECT(SettingsWrapper,forbiddenIconSupported).returns(true); 
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, KIconGsmAndWcdmaNotForbidden); 
    
    // Test: forbidden icon flag OFF
    EXPECT(SettingsWrapper, forbiddenIconSupported).returns(false);
    iconName = formClassAdapter->primaryIconForNetwork(currentInfo);
    QCOMPARE(iconName, QString("")); 
}

/*!
  UT_CpNetworkPluginForm::t_chageVisbilityOfManualNetworkSelection
 */
void UT_CpNetworkPluginForm::t_chageVisbilityOfManualNetworkSelection()
{
    m_networkPluginForm->chageVisbilityOfManualNetworkSelection(false);
        
    m_networkPluginForm->chageVisbilityOfManualNetworkSelection(false);
        
    m_networkPluginForm->chageVisbilityOfManualNetworkSelection(true);
    
    m_networkPluginForm->chageVisbilityOfManualNetworkSelection(true);
}


QTEST_MAIN_S60UI(UT_CpNetworkPluginForm)
