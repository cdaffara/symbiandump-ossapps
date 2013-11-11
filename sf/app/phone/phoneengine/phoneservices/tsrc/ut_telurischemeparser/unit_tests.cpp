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
* Description:  Unit tests for TEL URI Parser.
*
*/

#include <QtTest/QtTest>
#include <mockservice.h>
#include "telurischemeparser.h"
#include "mock_mpecallcontrolif.h"
#include "mock_mpecallsettersif.h"

class UT_TelUriSchemeParser : public QObject, public MockService
{
    Q_OBJECT
    
public:
    UT_TelUriSchemeParser();
    virtual ~UT_TelUriSchemeParser();

private slots:
    void init();
    void cleanup();

    void t_isSupportedUriScheme();
    void t_globalNumber();
    void t_localNumber();
    void t_withPauseAndMustChangeSharpDigit();
    void t_localWithAreaSpecifierAndPauseWithSharpDigit();
    void t_visualSeparatorAndMultiplePausesWithStar();
    void t_withAreaSpecifierWithWaitPauseAndStar();
    void t_withIsdnSubAndAndWaitAndPause();
    void t_wrongTelNum();
    void t_wrongPostDial();
    void t_justPauses();
    void t_justWaits();
    void t_justPausesAndWaits();
    void t_uriWrongSchema();
    void t_uriTooLongPhoneNumber();

private:
    TelUriSchemeParser *m_uriParser;
};

UT_TelUriSchemeParser::UT_TelUriSchemeParser()
    :
    m_uriParser(NULL)
{
}

UT_TelUriSchemeParser::~UT_TelUriSchemeParser()
{
    delete m_uriParser;
}

void UT_TelUriSchemeParser::init()
{
    initialize();
    
    m_uriParser = new TelUriSchemeParser();
}

void UT_TelUriSchemeParser::cleanup()
{
    reset();
    
    delete m_uriParser;
    m_uriParser = NULL;
}

void UT_TelUriSchemeParser::t_isSupportedUriScheme()
{
    // valid scheme
    QString uriWithValidScheme(
        QString("tel:") +
        QString("1234567890"));
    bool isValid = m_uriParser->isSupportedUriScheme(uriWithValidScheme);
    
    QCOMPARE(isValid, true);   
        
    // invalid scheme
    QString uriWithWrongScheme(
        QString("cti:") +
        QString("1234567890"));
    isValid = m_uriParser->isSupportedUriScheme(uriWithWrongScheme);
    
    QCOMPARE(isValid, false);   
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_globalNumber()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:+3585551234567");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_localNumber()
{
    QString KExpectedPhoneNumber = "04051234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:04051234567");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_withPauseAndMustChangeSharpDigit()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "p#3233";
    QString telUri("tel:+3585551234567;postd=p%233233");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUri, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_localWithAreaSpecifierAndPauseWithSharpDigit()
{
    QString KExpectedPhoneNumber = "1234";
    QString KExpectedDtmfString = "p#3233";
    QString telUriWithNumber("tel:1234;phone-context=+358555;postd=p#3233");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_visualSeparatorAndMultiplePausesWithStar()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "pp*3233";
    QString telUriWithNumber("tel:+358-555-1234567;postd=pp*3233");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_withAreaSpecifierWithWaitPauseAndStar()
{
    QString KExpectedPhoneNumber = "+1234567890";
    QString KExpectedDtmfString = "pw22p*3";
    QString telUriWithNumber("tel:+1234567890;phone-context=+1234;"
                                "vnd.company.option=foo;postd=w22p*3");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_withIsdnSubAndAndWaitAndPause()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "pwp2233";
    QString telUriWithNumber("tel:+358-555-1234567;isub=5555;postd=wp2233");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_wrongTelNum()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:0w003585551234567;phone-context=+3585551234");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_wrongPostDial()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:+3585551234567;postd=*A123");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_justPauses()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:+3585551234567;postd=pppp");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_justWaits()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:+3585551234567;postd=wwww");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_justPausesAndWaits()
{
    QString KExpectedPhoneNumber = "+3585551234567";
    QString KExpectedDtmfString = "";
    QString telUriWithNumber("tel:+3585551234567;postd=wpwp");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        telUriWithNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_uriWrongSchema()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    
    QString uriWithWrongSchema(
        QString("cti:") +
        QString("1234567890"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        uriWithWrongSchema, phoneNumber, dtmfString);
    
    QCOMPARE(handled, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

void UT_TelUriSchemeParser::t_uriTooLongPhoneNumber()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    
    QString uriWithTooLongPhoneNumber(
        QString("tel:") +
        QString("12345678901234567890123456789012345678901234567890") +
        QString("12345678901234567890123456789012345678901234567890") + 
        QString("1"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool handled = m_uriParser->parseUri(
        uriWithTooLongPhoneNumber, phoneNumber, dtmfString);
    
    QCOMPARE(handled, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}

QTEST_MAIN(UT_TelUriSchemeParser)
#include "unit_tests.moc"
