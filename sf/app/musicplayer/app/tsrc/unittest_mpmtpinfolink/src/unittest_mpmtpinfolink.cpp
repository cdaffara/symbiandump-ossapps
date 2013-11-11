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
* Description: Unit test for MpMtpInfoLink
*
*/

#include <QGraphicsSceneMouseEvent>
#include <QTranslator>
#include <QLocale>

#include "stub/inc/hblabel.h"
#include "stub/inc/qdesktopservices.h"
#include "stub/inc/mpsettingsmanager.h"
#include "unittest_mpmtpinfolink.h"


// Do this so we can access all member variables.
#define private public
#define protected public
#include "mpmtpinfolink.h"
#undef protected
#undef private

//This so we can test private functions
#include "../../src/mpmtpinfolink.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpMtpInfoLink tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmtpinfolink.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMtpInfoLink::TestMpMtpInfoLink()
    : mTest(0),
      mMpTranslator(0)
{
}

TestMpMtpInfoLink::~TestMpMtpInfoLink()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMtpInfoLink::initTestCase()
{
    QString lang = QLocale::system().name();
    QString path = QString("z:/resource/qt/translations/");
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator(this);
    translatorLoaded = mMpTranslator->load(path + "musicplayer_" + lang);
    if ( translatorLoaded ) {
        qApp->installTranslator(mMpTranslator);
    }
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMtpInfoLink::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMtpInfoLink::init()
{
    mTest = new MpMtpInfoLink();
}

/*!
 Called after every testfunction.
 */
void TestMpMtpInfoLink::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
*/
void TestMpMtpInfoLink::testConstructor()
{
    QVERIFY(mTest->textWrapping() == Hb::TextWrapAnywhere );
}

/*!
 testMousePressEvent.
 */
void TestMpMtpInfoLink::testMousePressEvent()
{
    //MpMtpInfoLink::mousePressEvent does nothing, included just to cover all functions.
    QGraphicsSceneMouseEvent *event = new QGraphicsSceneMouseEvent();
    mTest->mousePressEvent( event );
    QVERIFY( mTest );
}

/*!
 Tests HandleOpen.
 */
void TestMpMtpInfoLink::testMouseReleaseEvent()
{
    QGraphicsSceneMouseEvent *event = new QGraphicsSceneMouseEvent();

    //Mock label rect: x=10, y=10, width=10, height=10);
    QPointF releasePointOut(5,5);
    QPointF releasePointIn(15,15);

    //Verify releasing the mouse outside the label
    event->setLastPos( releasePointOut );
    mTest->mouseReleaseEvent( event );
    QCOMPARE( QDesktopServices::instance()->mOpenUrlCount, 0);
    //Verify releasing the mouse inside the label
    event->setLastPos( releasePointIn );
    mTest->mouseReleaseEvent( event );
    QCOMPARE( QDesktopServices::instance()->mOpenUrlCount, 1);
}

/*!
 testSetMtpInfoText
 */
void TestMpMtpInfoLink::testSetMtpInfoText( )
{
    //Verify with an url
    QVERIFY( mTest->plainText().length() == 0 );
    QVERIFY( mTest->html().length() > 0 );

}

//end of file
