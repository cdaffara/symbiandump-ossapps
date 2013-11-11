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
* Description:  Unit tests for CTI URI Parser.
*
*/

#include <QtTest/QtTest>
#include <mockservice.h>
#include "ctiurischemeparser.h"

const QString KCtiUriSchemePrefix = "cti:";

class UT_CtiUriSchemeParser : public QObject, public MockService
{
    Q_OBJECT
    
public:
    UT_CtiUriSchemeParser();
    virtual ~UT_CtiUriSchemeParser();

private slots:
    void init();
    void cleanup();
    
    void t_isSupportedUriScheme();
    void t_ctiUriWithLocalNumber();
    void t_ctiUriWithGlobalNumber();
    void t_ctiUriWithGlobalNumberAndVisualSeparators();
    void t_ctiUriWithGlobalNumberAndToneInput();
    void t_invalidUriWrongSchema();
    void t_invalidUriTooLongPhoneNumber();
    void t_invalidUriMultipleWaitInSuccession();
    void t_invalidUriInvalidCharacters();
    void t_invalidUriToneInputOnly();
    void t_invalidUriOnlyVisualSeparators();
    void t_invalidUriOnlyPausesAndWaits();
    void t_invalidUriToneInputCharactersInNumber();

private:
    CtiUriSchemeParser *m_uriParser;
};


UT_CtiUriSchemeParser::UT_CtiUriSchemeParser()
    :
    m_uriParser(NULL)
{
}


UT_CtiUriSchemeParser::~UT_CtiUriSchemeParser()
{
    delete m_uriParser;
}


void UT_CtiUriSchemeParser::init()
{
    initialize();
    
    m_uriParser = new CtiUriSchemeParser();
}


void UT_CtiUriSchemeParser::cleanup()
{
    reset();
    
    delete m_uriParser;
    m_uriParser = NULL;
}


void UT_CtiUriSchemeParser::t_isSupportedUriScheme()
{
    // valid scheme
    QString uriWithValidScheme(
        QString("cti:") +
        QString("1234567890"));
    bool result = m_uriParser->isSupportedUriScheme(uriWithValidScheme);
    
    QCOMPARE(result, true);   
    
    // invalid scheme
    QString uriWithWrongScheme(
        QString("tel:") +
        QString("1234567890"));
    result = m_uriParser->isSupportedUriScheme(uriWithWrongScheme);
    
    QCOMPARE(result, false);   
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_ctiUriWithLocalNumber()
{
    QString KExpectedPhoneNumber = "0401234567890";
    QString KExpectedDtmfString = "";
    QString ctiUriWithLocalNumber(KCtiUriSchemePrefix + KExpectedPhoneNumber);
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        ctiUriWithLocalNumber, phoneNumber, dtmfString);
    
    QCOMPARE(result, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_ctiUriWithGlobalNumber()
{
    QString KExpectedPhoneNumber = "+358401234567890";
    QString KExpectedDtmfString = "";
    QString ctiUriWithGlobalNumber(KCtiUriSchemePrefix + KExpectedPhoneNumber);
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        ctiUriWithGlobalNumber, phoneNumber, dtmfString);
    
    QCOMPARE(result, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_ctiUriWithGlobalNumberAndVisualSeparators()
{
    QString KExpectedPhoneNumber = "+358401234567890";
    QString KExpectedDtmfString = "";
    QString ctiUriWithVisualSeparators("cti:+358-(40)12345.6789 0");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        ctiUriWithVisualSeparators, phoneNumber, dtmfString);
    
    QCOMPARE(result, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_ctiUriWithGlobalNumberAndToneInput()
{
    QString KExpectedPhoneNumber = "+358401234567890";
    QString KExpectedDtmfString = "w0123456789pp*#";
    QString ctiUriWithNumberAndToneInput("cti:+358401234567890/0123456789,,*#");
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        ctiUriWithNumberAndToneInput, phoneNumber, dtmfString);
    
    QCOMPARE(result, true);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriWrongSchema()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithWrongSchema(
        QString("tel:") +
        QString("1234567890"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithWrongSchema, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriTooLongPhoneNumber()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithTooLongPhoneNumber(
        QString("cti:") +
        QString("12345678901234567890123456789012345678901234567890") +
        QString("12345678901234567890123456789012345678901234567890") + 
        QString("1"));

    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithTooLongPhoneNumber, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriMultipleWaitInSuccession()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithMultipleWaitInSuccession(
        QString("cti:") +
        QString("1234567890//123//"));

    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithMultipleWaitInSuccession, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriInvalidCharacters()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithInvalidCharacters(
        QString("cti:") +
        QString("abcdefghijklmnopqrstuvxyz"));

    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithInvalidCharacters, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);   
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
    
    QString invalidUri(
        QString("cti:") +
        QString("a"));
    result = m_uriParser->parseUri(
        invalidUri, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriToneInputOnly()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithToneInputOnly(
        QString("cti:") +
        QString("/123,321"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithToneInputOnly, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriOnlyVisualSeparators()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithToneInputOnly(
        QString("cti:") +
        QString(" -.()"));

    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithToneInputOnly, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriOnlyPausesAndWaits()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithPausesAndWaitsOnly(
        QString("cti:") +
        QString(",,//"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithPausesAndWaitsOnly, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    QVERIFY(verify());
}


void UT_CtiUriSchemeParser::t_invalidUriToneInputCharactersInNumber()
{
    QString KExpectedPhoneNumber = "";
    QString KExpectedDtmfString = "";
    QString uriWithToneInputCharactersInNumber1(
        QString("cti:") +
        QString("1234567890*#"));
    
    QString phoneNumber = "";
    QString dtmfString = "";
    bool result = m_uriParser->parseUri(
        uriWithToneInputCharactersInNumber1, phoneNumber, dtmfString);
    
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    
    QString uriWithToneInputCharactersInNumber2(
        QString("cti:") +
        QString("*#1234567890"));
    result = m_uriParser->parseUri(
        uriWithToneInputCharactersInNumber2, phoneNumber, dtmfString);
    QCOMPARE(result, false);
    QCOMPARE(phoneNumber, KExpectedPhoneNumber);
    QCOMPARE(dtmfString, KExpectedDtmfString);
    
    QVERIFY(verify());
}

QTEST_MAIN(UT_CtiUriSchemeParser)
#include "unit_tests.moc"
