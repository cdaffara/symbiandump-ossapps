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
#include "ut_logspredictivelatin12keytranslator.h"
#include "logspredictivelatin12keytranslator.h"

#include <hbinputsettingproxy.h>
#include <QtTest/QtTest>
#include <QSignalSpy>


void UT_LogsPredictiveLatin12KeyTranslator::initTestCase()
{
}

void UT_LogsPredictiveLatin12KeyTranslator::cleanupTestCase()
{
}


void UT_LogsPredictiveLatin12KeyTranslator::init()
{
    mTranslator = new LogsPredictiveLatin12KeyTranslator( 
            HbInputSettingProxy::instance()->globalInputLanguage() );
}

void UT_LogsPredictiveLatin12KeyTranslator::cleanup()
{
    delete mTranslator;
    mTranslator = 0;
}

void UT_LogsPredictiveLatin12KeyTranslator::testConstructor()
{
    QVERIFY( mTranslator );
    QVERIFY( !mTranslator->mSpecialKeyMap.isEmpty() );
    
}

void UT_LogsPredictiveLatin12KeyTranslator::testTranslateText()
{
    QVERIFY( mTranslator->translateText( QString( "Nokia" ) ) == QString( "66542" ) );
    QVERIFY( mTranslator->translateText( QString( "Aarne" ) ) == QString( "22763" ) );
    QVERIFY( mTranslator->translateText( QString( "Bertta" ) ) == QString( "237882" ) );
    QVERIFY( mTranslator->translateText( QString( "Celsius" ) ) == QString( "2357487" ) );
    QVERIFY( mTranslator->translateText( QString( "Daavid" ) ) == QString( "322843" ) );
    QVERIFY( mTranslator->translateText( QString( "Eemeli" ) ) == QString( "336354" ) );
    QVERIFY( mTranslator->translateText( QString( "Faarao" ) ) == QString( "322726" ) );
    QVERIFY( mTranslator->translateText( QString( "Gideon" ) ) == QString( "443366" ) );
    QVERIFY( mTranslator->translateText( QString( "Heikki" ) ) == QString( "434554" ) );
    QVERIFY( mTranslator->translateText( QString( "Iivari" ) ) == QString( "448274" ) );
    QVERIFY( mTranslator->translateText( QString( "Jussi" ) ) == QString( "58774" ) );
    QVERIFY( mTranslator->translateText( QString( "Kalle" ) ) == QString( "52553" ) );
    QVERIFY( mTranslator->translateText( QString( "Lauri" ) ) == QString( "52874" ) );
    QVERIFY( mTranslator->translateText( QString( "Matti" ) ) == QString( "62884" ) );
    QVERIFY( mTranslator->translateText( QString( "Niilo" ) ) == QString( "64456" ) );
    QVERIFY( mTranslator->translateText( QString( "Otto" ) ) == QString( "6886" ) );
    QVERIFY( mTranslator->translateText( QString( "Paavo" ) ) == QString( "72286" ) );
    QVERIFY( mTranslator->translateText( QString( "Kuu" ) ) == QString( "588" ) );
    QVERIFY( mTranslator->translateText( QString( "Risto" ) ) == QString( "74786" ) );
    QVERIFY( mTranslator->translateText( QString( "Sakari" ) ) == QString( "725274" ) );
    QVERIFY( mTranslator->translateText( QString( "Tyyne" ) ) == QString( "89963" ) );
    QVERIFY( mTranslator->translateText( QString( "Urho" ) ) == QString( "8746" ) );
    QVERIFY( mTranslator->translateText( QString( "Vihtori" ) ) == QString( "8448674" ) );
    QVERIFY( mTranslator->translateText( QString( "Wiski" ) ) == QString( "94754" ) );
    QVERIFY( mTranslator->translateText( QString( "Yrjö" ) ) == QString( "9756" ) );
    QVERIFY( mTranslator->translateText( QString( "Tseta" ) ) == QString( "87382" ) );
    QVERIFY( mTranslator->translateText( QString( "Åke" ) ) == QString( "253" ) );
    QVERIFY( mTranslator->translateText( QString( "Äiti" ) ) == QString( "2484" ) );
    QVERIFY( mTranslator->translateText( QString( "Öljy" ) ) == QString( "6559" ) );
    QVERIFY( mTranslator->translateText( QString( "Adam" ) ) == QString( "2326" ) );
    QVERIFY( mTranslator->translateText( QString( "Bertil" ) ) == QString( "237845" ) );
    QVERIFY( mTranslator->translateText( QString( "Cesar" ) ) == QString( "23727" ) );
    QVERIFY( mTranslator->translateText( QString( "David" ) ) == QString( "32843" ) );
    QVERIFY( mTranslator->translateText( QString( "Erik" ) ) == QString( "3745" ) );
    QVERIFY( mTranslator->translateText( QString( "Filip" ) ) == QString( "34547" ) );
    QVERIFY( mTranslator->translateText( QString( "Gustav" ) ) == QString( "487828" ) );
    QVERIFY( mTranslator->translateText( QString( "Helge" ) ) == QString( "43543" ) );
    QVERIFY( mTranslator->translateText( QString( "Ivar" ) ) == QString( "4827" ) );
    QVERIFY( mTranslator->translateText( QString( "Johan" ) ) == QString( "56426" ) );
    QVERIFY( mTranslator->translateText( QString( "Kalle" ) ) == QString( "52553" ) );
    QVERIFY( mTranslator->translateText( QString( "Ludvig" ) ) == QString( "583844" ) );
    QVERIFY( mTranslator->translateText( QString( "Martin" ) ) == QString( "627846" ) );
    QVERIFY( mTranslator->translateText( QString( "Niklas" ) ) == QString( "645527" ) );
    QVERIFY( mTranslator->translateText( QString( "Olof" ) ) == QString( "6563" ) );
    QVERIFY( mTranslator->translateText( QString( "Petter" ) ) == QString( "738837" ) );
    QVERIFY( mTranslator->translateText( QString( "Quintus" ) ) == QString( "7846887" ) );
    QVERIFY( mTranslator->translateText( QString( "Rudolf" ) ) == QString( "783653" ) );
    QVERIFY( mTranslator->translateText( QString( "Sigurd" ) ) == QString( "744873" ) );
    QVERIFY( mTranslator->translateText( QString( "Tore" ) ) == QString( "8673" ) );
    QVERIFY( mTranslator->translateText( QString( "Urban" ) ) == QString( "87226" ) );
    QVERIFY( mTranslator->translateText( QString( "Viktor" ) ) == QString( "845867" ) );
    QVERIFY( mTranslator->translateText( QString( "Wilhelm" ) ) == QString( "9454356" ) );
    QVERIFY( mTranslator->translateText( QString( "Xerxes" ) ) == QString( "937937" ) );
    QVERIFY( mTranslator->translateText( QString( "Yngve" ) ) == QString( "96483" ) );
    QVERIFY( mTranslator->translateText( QString( "Zäta" ) ) == QString( "9282" ) );
    QVERIFY( mTranslator->translateText( QString( "Ärlig" ) ) == QString( "27544" ) );
    QVERIFY( mTranslator->translateText( QString( "Östen" ) ) == QString( "67836" ) );
    QVERIFY( mTranslator->translateText( QString( "Alpha" ) ) == QString( "25742" ) );
    QVERIFY( mTranslator->translateText( QString( "Bravo" ) ) == QString( "27286" ) );
    QVERIFY( mTranslator->translateText( QString( "Charlie" ) ) == QString( "2427543" ) );
    QVERIFY( mTranslator->translateText( QString( "Delta" ) ) == QString( "33582" ) );
    QVERIFY( mTranslator->translateText( QString( "Echo" ) ) == QString( "3246" ) );
    QVERIFY( mTranslator->translateText( QString( "Foxtrot" ) ) == QString( "3698768" ) );
    QVERIFY( mTranslator->translateText( QString( "Golf" ) ) == QString( "4653" ) );
    QVERIFY( mTranslator->translateText( QString( "Hotel" ) ) == QString( "46835" ) );
    QVERIFY( mTranslator->translateText( QString( "India" ) ) == QString( "46342" ) );
    QVERIFY( mTranslator->translateText( QString( "Juliet" ) ) == QString( "585438" ) );
    QVERIFY( mTranslator->translateText( QString( "Kilo" ) ) == QString( "5456" ) );
    QVERIFY( mTranslator->translateText( QString( "Lima" ) ) == QString( "5462" ) );
    QVERIFY( mTranslator->translateText( QString( "Mike" ) ) == QString( "6453" ) );
    QVERIFY( mTranslator->translateText( QString( "November" ) ) == QString( "66836237" ) );
    QVERIFY( mTranslator->translateText( QString( "Oscar" ) ) == QString( "67227" ) );
    QVERIFY( mTranslator->translateText( QString( "Papa" ) ) == QString( "7272" ) );
    QVERIFY( mTranslator->translateText( QString( "Quebec" ) ) == QString( "783232" ) );
    QVERIFY( mTranslator->translateText( QString( "Romeo" ) ) == QString( "76636" ) );
    QVERIFY( mTranslator->translateText( QString( "Sierra" ) ) == QString( "743772" ) );
    QVERIFY( mTranslator->translateText( QString( "Tanga" ) ) == QString( "82642" ) );
    QVERIFY( mTranslator->translateText( QString( "Uniform" ) ) == QString( "8643676" ) );
    QVERIFY( mTranslator->translateText( QString( "Vârlan" ) ) == QString( "827526" ) );
    QVERIFY( mTranslator->translateText( QString( "Victor" ) ) == QString( "842867" ) );
    QVERIFY( mTranslator->translateText( QString( "Whiskey" ) ) == QString( "9447539" ) );
    QVERIFY( mTranslator->translateText( QString( "Xray" ) ) == QString( "9729" ) );
    QVERIFY( mTranslator->translateText( QString( "Yankee" ) ) == QString( "926533" ) );
    QVERIFY( mTranslator->translateText( QString( "Zulu" ) ) == QString( "9858" ) );
    
    QString uni;
    uni.append(QChar(0x0219));
    uni.append(QChar(0x4E0F));
    QString result("7");
    
    QCOMPARE(mTranslator->translateText(uni), result);
    
    QString uni1;
    uni1.append(QChar(0x4E0E));
    uni1.append(QChar(0x4E0F));
    QString result1("");
    QCOMPARE(mTranslator->translateText(uni1), result1);
    
    QString result2;
    QCOMPARE(mTranslator->translateText(uni1), result2);

}

void UT_LogsPredictiveLatin12KeyTranslator::testTranslatePattern()
{

    QVERIFY( mTranslator->translatePattern( QString( "1234567890" ) ) == QString( "1234567890" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "+1234567890" ) ) ,QString( "*1234567890" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "*1234567890" ) ) ,QString( "*1234567890" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "#1234567890" ) ), QString( "#1234567890" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "++##" ) ), QString( "**##" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "-+##" ) ), QString( "1*##" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "/+##" ) ), QString( "1*##" ) );
    QCOMPARE( mTranslator->translatePattern( QString( "*+##" ) ), QString( "**##" ) );
    
}

void UT_LogsPredictiveLatin12KeyTranslator::testTranslateChar()
{
    bool ok;
    
    QCOMPARE( mTranslator->translateChar( QChar('+'), ok ), QChar('*') );
    QCOMPARE( mTranslator->translateChar( QChar('*'), ok ), QChar('*') );
    QCOMPARE( mTranslator->translateChar( QChar('#'), ok ), QChar('#') );
    QCOMPARE( mTranslator->translateChar( QChar('0'), ok ), QChar('0') );
    QCOMPARE( mTranslator->translateChar( QChar('a'), ok ), QChar('2') );
    QCOMPARE( mTranslator->translateChar( QChar('A'), ok ), QChar('2') );
    QCOMPARE( mTranslator->translateChar( QChar('1'), ok ), QChar('1') );
    QCOMPARE( mTranslator->translateChar( QChar('ä'), ok ), QChar('2') );
    QCOMPARE( mTranslator->translateChar( QChar('Ä'), ok ), QChar('2') );
    QCOMPARE( mTranslator->translateChar( QChar('-'), ok ), QChar('1') );
    QCOMPARE( mTranslator->translateChar( QChar('%'), ok ), QChar('1') );
    // Romanian unicode character "sh"
    QCOMPARE( mTranslator->translateChar( QChar(0x0219), ok ), QChar('7') );
    // Chinese unicode character
    QChar chn(0x4E0F);
    QCOMPARE( mTranslator->translateChar( QChar(0x4E0F), ok ), QChar() );
    QVERIFY( !ok );


}

void UT_LogsPredictiveLatin12KeyTranslator::testEncodes()
{
    QVERIFY( mTranslator->encodes( QString( "Zulu" ) ) );
    QVERIFY( mTranslator->encodes( QString( "Öistämö" ) ) );
    
    //china
    QString uni1;
    uni1.append(QChar(0x4E0E));
    uni1.append(QChar(0x4E0F));
    QVERIFY( !mTranslator->encodes( uni1) );
    
    const int ucsize = 9;
    const QChar thaiName2[] = {0x0E01,0x0E06,0x0E2F,0x0E0E,0x0E14,0x0E19,0x0E30,0x0E23,0x0E2A };
    QVERIFY( !mTranslator->encodes( QString( thaiName2,ucsize ) ) );
    
}


