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
* Description:  Unit tests for PhoneMessageController.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <HbGlobal>
#include <mockservice.h>
#include <settingsinternalcrkeys.h>
#include "phonetestmain.h"
#include "phoneapplauncher.h"
#include "phonemessagecontroller.h"
#include "tphonecmdparamsfidata.h"
#include "phoneconstants.h"
#include "cphonecenrepproxy.h"

const QString KNumber = "1234567";
const QString KName = "Tester";
const QString KUserDefinedSoftRejectText = "user defined text";

void enableUserDefinedSoftRejectText(
    const TUid & aUid,
    const TUint aId,
    TInt & aValue)
{
    Q_UNUSED(aUid)
    Q_UNUSED(aId)
    
    aValue = 1;
}


void setUserDefinedSoftRejectText(
    const TUid & aUid,
    const TUint aId,
    TDes & aValue)
{
    Q_UNUSED(aUid)
    Q_UNUSED(aId)
    
    aValue.Copy(KUserDefinedSoftRejectText.utf16());
}


class UT_PhoneMessageController : public QObject, public MockService
{
    Q_OBJECT
public:
    UT_PhoneMessageController();
    virtual ~UT_PhoneMessageController();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup(); 
    
private slots:
    void t_openSoftRejectEditorDefaultText();
    void t_openSoftRejectEditorUserDefinedText();

private:
    PhoneAppLauncher *m_launcher;
    PhoneMessageController *m_messageController; // class under test
};


UT_PhoneMessageController::UT_PhoneMessageController()
    :
    m_launcher(NULL),
    m_messageController(NULL)
{
}


UT_PhoneMessageController::~UT_PhoneMessageController()
{
    delete m_messageController;
    delete m_launcher;
}


void UT_PhoneMessageController::initTestCase()
{
    m_launcher = new PhoneAppLauncher(this);
    m_messageController = new PhoneMessageController(*m_launcher, this);
}


void UT_PhoneMessageController::cleanupTestCase()
{
    delete m_messageController;
    m_messageController = NULL;
    delete m_launcher;
    m_launcher = NULL;
}


void UT_PhoneMessageController::init()
{
    initialize();
}


void UT_PhoneMessageController::cleanup()
{
    reset();
}


void UT_PhoneMessageController::t_openSoftRejectEditorDefaultText()
{
    const QString KDefaultSoftRejectText = 
        hbTrId("txt_phone_setlabel_soft_reject_val_default_text");
    EXPECT(PhoneAppLauncher, launchMessaging)
        .with(KNumber, KName, KDefaultSoftRejectText);
    
    TPhoneCmdParamSfiData sfiParam;
    sfiParam.SetNumber(TBuf16<20>(KNumber.utf16()));
    sfiParam.SetName(TBuf16<20>(KName.utf16()));
    m_messageController->openSoftRejectMessageEditor(&sfiParam);
    
    QVERIFY(verify());
}


void UT_PhoneMessageController::t_openSoftRejectEditorUserDefinedText()
{
    EXPECT(PhoneAppLauncher, launchMessaging)
        .with(KNumber, KName, KUserDefinedSoftRejectText);
    EXPECT(CPhoneCenRepProxy, GetInt)
        .with(KCRUidTelephonySettings, KSettingsSoftRejectDefaultInUse, 0)
        .willOnce(invoke(enableUserDefinedSoftRejectText));
    EXPECT(CPhoneCenRepProxy, GetString)
        .with(KCRUidTelephonySettings, KSettingsSoftRejectText, KNullDesC())
        .willOnce(invoke(setUserDefinedSoftRejectText));
    
    TPhoneCmdParamSfiData sfiParam;
    sfiParam.SetNumber(TBuf16<20>(KNumber.utf16()));
    sfiParam.SetName(TBuf16<20>(KName.utf16()));
    m_messageController->openSoftRejectMessageEditor(&sfiParam);
    
    QVERIFY(verify());
}


PHONE_TEST_MAIN(UT_PhoneMessageController)
#include "unit_tests.moc"
