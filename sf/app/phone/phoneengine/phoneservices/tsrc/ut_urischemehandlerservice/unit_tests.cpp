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
* Description:  Unit tests for URI Scheme Handler Service.
*
*/

#include <QtTest/QtTest>
#include <mockservice.h>
#include <xqrequestinfo.h>
#include <HbDeviceMessageBox>
#include "urischemehandlerservice.h"
#include "telurischemeparser.h"
#include "ctiurischemeparser.h"
#include "mock_mpecallcontrolif.h"
#include "mock_mpecallsettersif.h"

const QString KGlobalPhoneNumber = "+358401234567890";
const QString KTelUriWithGlobalNumber("tel:" + KGlobalPhoneNumber);

void setPhoneNumber(
    const QString &uri,
    QString &phoneNumber,
    QString &dtmfString())
{
    Q_UNUSED(uri)
    Q_UNUSED(dtmfString)
    phoneNumber = KGlobalPhoneNumber;
}

class UT_UriSchemeHandlerService : public QObject, public MockService
{
    Q_OBJECT
    
public:
    UT_UriSchemeHandlerService();
    virtual ~UT_UriSchemeHandlerService();

private slots:
    void init();
    void cleanup();
    
    void t_viewUnsupportedScheme();
    void t_viewSupportedSchemeCapabilityCheckFails();
    void t_viewSupportedSchemeParsingFails();
    void t_viewSupportedUriUserCancelsCall();
    void t_viewSupportedUriUserAcceptsCall();
    void t_viewSupportedUriCallRequestFails();
    
private:
    UriSchemeHandlerService *m_uriHandlerService;
    CPECallControlIFMock *m_callControlIfMock;
    CPECallSettersIFMock *m_callSettersIfMock;
};


UT_UriSchemeHandlerService::UT_UriSchemeHandlerService()
    :
    m_uriHandlerService(NULL), 
    m_callControlIfMock(NULL), 
    m_callSettersIfMock(NULL)
{
}


UT_UriSchemeHandlerService::~UT_UriSchemeHandlerService()
{
    delete m_uriHandlerService;
    delete m_callControlIfMock;
    delete m_callSettersIfMock;
}


void UT_UriSchemeHandlerService::init()
{
    initialize();
    
    QT_TRAP_THROWING(SmcDefaultValue<XQRequestInfo>::SetL(XQRequestInfo()))
    
    m_callControlIfMock = new CPECallControlIFMock();
    m_callSettersIfMock = new CPECallSettersIFMock();
    m_uriHandlerService = new UriSchemeHandlerService(
        *m_callControlIfMock, *m_callSettersIfMock);
}


void UT_UriSchemeHandlerService::cleanup()
{
    reset();
    
    delete m_uriHandlerService;
    m_uriHandlerService = NULL;
    delete m_callControlIfMock;
    m_callControlIfMock = NULL;
    delete m_callSettersIfMock;
    m_callSettersIfMock = NULL;
    
    SmcDefaultValue<XQRequestInfo>::Reset();
}


void UT_UriSchemeHandlerService::t_viewUnsupportedScheme()
{
    QString unknownUri("mailto:alice@wonderland");
    
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(false);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).times(0);
    
    bool handled = m_uriHandlerService->view(unknownUri);
    
    QCOMPARE(handled, false);
    QVERIFY(verify());
}


void UT_UriSchemeHandlerService::t_viewSupportedSchemeCapabilityCheckFails()
{
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(true);
    QSet<int> invalidCapabilities;
    invalidCapabilities.insert(ECapabilityNetworkServices);
    EXPECT(XQRequestInfo, clientCapabilities).returns(invalidCapabilities);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).times(0);
    
    bool handled = m_uriHandlerService->view(KTelUriWithGlobalNumber);
    
    QCOMPARE(handled, false);
    QVERIFY(verify());
}


void UT_UriSchemeHandlerService::t_viewSupportedSchemeParsingFails()
{
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(true);
    QSet<int> validCapabilities;
    validCapabilities.insert(ECapabilityNetworkServices);
    validCapabilities.insert(ECapabilityNetworkControl);
    EXPECT(XQRequestInfo, clientCapabilities).returns(validCapabilities);
    EXPECT(TelUriSchemeParser, parseUri).returns(false);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).times(0);
    
    bool handled = m_uriHandlerService->view(KTelUriWithGlobalNumber);
    
    QCOMPARE(handled, false);
    QVERIFY(verify());    
}


void UT_UriSchemeHandlerService::t_viewSupportedUriUserCancelsCall()
{
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(true);
    QSet<int> validCapabilities;
    validCapabilities.insert(ECapabilityNetworkServices);
    validCapabilities.insert(ECapabilityNetworkControl);
    EXPECT(XQRequestInfo, clientCapabilities).returns(validCapabilities);
    EXPECT(TelUriSchemeParser, parseUri).returns(true);
    EXPECT(HbDeviceMessageBox, setTimeout).with(0);
    EXPECT(HbDeviceMessageBox, isAcceptAction).returns(false);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).times(0);
    
    bool handled = m_uriHandlerService->view(KTelUriWithGlobalNumber);
    
    QCOMPARE(handled, false);
    QVERIFY(verify());
}


void UT_UriSchemeHandlerService::t_viewSupportedUriUserAcceptsCall()
{
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(true);
    QSet<int> validCapabilities;
    validCapabilities.insert(ECapabilityNetworkServices);
    validCapabilities.insert(ECapabilityNetworkControl);
    EXPECT(XQRequestInfo, clientCapabilities).returns(validCapabilities);
    EXPECT(TelUriSchemeParser, parseUri)
        .willOnce(invoke(setPhoneNumber))
        .returns(true);
    EXPECT(HbDeviceMessageBox, setTimeout).with(0);
    EXPECT(HbDeviceMessageBox, isAcceptAction).returns(true);
    
    _LIT(KExpectedDialString, "+358401234567890");
    EXPECT(CPECallSettersIFMock, SetPhoneNumber).with(KExpectedDialString());
    EXPECT(CPECallSettersIFMock, SetCallTypeCommand).with(EPECallTypeCSVoice);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).returns(KErrNone);
    
    bool handled = m_uriHandlerService->view(KTelUriWithGlobalNumber);
    
    QCOMPARE(handled, true);
    QVERIFY(verify());
}


void UT_UriSchemeHandlerService::t_viewSupportedUriCallRequestFails()
{
    EXPECT(TelUriSchemeParser, isSupportedUriScheme).returns(true);
    QSet<int> validCapabilities;
    validCapabilities.insert(ECapabilityNetworkServices);
    validCapabilities.insert(ECapabilityNetworkControl);
    EXPECT(XQRequestInfo, clientCapabilities).returns(validCapabilities);
    EXPECT(TelUriSchemeParser, parseUri)
        .willOnce(invoke(setPhoneNumber))
        .returns(true);
    EXPECT(HbDeviceMessageBox, setTimeout).with(0);
    EXPECT(HbDeviceMessageBox, isAcceptAction).returns(true);
    
    _LIT(KExpectedDialString, "+358401234567890");
    EXPECT(CPECallSettersIFMock, SetPhoneNumber).with(KExpectedDialString());
    EXPECT(CPECallSettersIFMock, SetCallTypeCommand).with(EPECallTypeCSVoice);
    EXPECT(CPECallControlIFMock, HandleDialServiceCall).returns(KErrNoMemory);
    
    bool handled = m_uriHandlerService->view(KTelUriWithGlobalNumber);
    
    QCOMPARE(handled, false);
    QVERIFY(verify());
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    UT_UriSchemeHandlerService tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "unit_tests.moc"
