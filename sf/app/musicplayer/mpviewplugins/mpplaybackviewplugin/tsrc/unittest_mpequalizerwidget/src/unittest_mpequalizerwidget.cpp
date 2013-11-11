/*
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
* Description: Unit test for mpequalizerwidget
*
*/

#include <QMetaType>
#include <QtGui>
#include <hbradiobuttonlist.h>
#include <QTranslator>
#include <QLocale>

#include "unittest_mpequalizerwidget.h"

// Do this so we can access all member variables.
#define private public
#include "mpequalizerwidget.h"
#undef private

//This so we can test private functions
#include "../../src/mpequalizerwidget.cpp"



/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpEqualizerWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpequalizerwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpEqualizerWidget::TestMpEqualizerWidget()
    : mTest(0),
      mMpTranslator(0)
{
}

TestMpEqualizerWidget::~TestMpEqualizerWidget()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEqualizerWidget::initTestCase()
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
void TestMpEqualizerWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEqualizerWidget::init()
{
    MpEngineFactory::createSharedEngine();
    mTest = new MpEqualizerWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpEqualizerWidget::cleanup()
{
    delete mTest;
    mTest = 0;
    MpEngineFactory::close();
}

/*!
 Tests constructor.
 */
void TestMpEqualizerWidget::testInitialize()
{
    QVERIFY(mTest->mMpEngine != 0 );    
}

/*!
 Tests testPrepare.
 */
void TestMpEqualizerWidget::testPrepareDialog()
{
    mTest->prepareDialog();
    QVERIFY(mTest->mOriginalPresetIndex == 0 );    
    QVERIFY(mTest->timeout() == HbDialog::NoTimeout);
    QVERIFY(mTest->dismissPolicy() == HbDialog::NoDismiss);
    QVERIFY(mTest->frameType() == HbPopup::Strong);
    QVERIFY(mTest->headingWidget() != 0);
    QVERIFY(mTest->actions().count() != 0);
    QVERIFY(mTest->mPresetsList != 0);
    QCOMPARE(qobject_cast<HbRadioButtonList*>(mTest->contentWidget()), mTest->mPresetsList);
    QVERIFY(mTest->mPresetsList->selected() == 0);
    QCOMPARE(mTest->mPresetsList->items().at(0), hbTrId("txt_mus_list_off"));
    QCOMPARE(mTest->mPresetsList->items().at(1), hbTrId("txt_mus_list_bass_booster"));
    QCOMPARE(mTest->mPresetsList->items().at(2), hbTrId("txt_mus_list_classical"));
    QCOMPARE(mTest->mPresetsList->items().at(3), hbTrId("txt_mus_list_jazz"));
    QCOMPARE(mTest->mPresetsList->items().at(4), hbTrId("txt_mus_list_pop"));
    QCOMPARE(mTest->mPresetsList->items().at(5), hbTrId("txt_mus_list_rock"));
    QCOMPARE(mTest->mPresetsList->items().at(6), QString("New Preset"));
}

/*!
 Tests presetSelected.
 */
void TestMpEqualizerWidget::testPresetSelected()
{
    //Off preset must call DisableEqualizer
    mTest->presetSelected(0);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);

    //Any other preset must be applied
    mTest->presetSelected(3);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
}

/*!
 Tests cancelSelected.
 */
void TestMpEqualizerWidget::testCancelSelected()
{
    mTest->prepareDialog();
    
    //Cancel, when Current preset selected != Original Preset
    
    //When Original Preset == -1 "Off"
    mTest->mMpEngine->mActivePreset = 4;
    mTest->mOriginalPresetIndex = 0;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPresetIndex);
    
    //When Original Preset != -1 "Any other different than Off"
    mTest->mMpEngine->mActivePreset = 4;
    mTest->mOriginalPresetIndex = 2;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPresetIndex);
        
    //Cancel, when Current preset selected == Original Preset
    mTest->mMpEngine->mActivePreset = 3;
    mTest->mOriginalPresetIndex = 3;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPresetIndex);
}

/*!
 Tests okSelected.
 */
void TestMpEqualizerWidget::testOkSelected()
{
    mTest->mMpEngine->mActivePreset = 2;
    mTest->mOriginalPresetIndex = -1;
    mTest->okSelected(true);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPresetIndex);   
}

/*!
 Tests getLogicalName.
 */
void TestMpEqualizerWidget::testGetLocalizedString()
{
    QString localizedString;
    
    // test Bass Booster
    localizedString = mTest->getLocalizedString( QString("Bass Booster") );
    QCOMPARE(localizedString, hbTrId("txt_mus_list_bass_booster"));

    // test Classic
    localizedString = mTest->getLocalizedString( QString("Classic") );
    QCOMPARE(localizedString, hbTrId("txt_mus_list_classical"));

    // test Jazz
    localizedString = mTest->getLocalizedString( QString("Jazz") );
    QCOMPARE(localizedString, hbTrId("txt_mus_list_jazz"));

    // test Pop
    localizedString = mTest->getLocalizedString( QString("Pop") );
    QCOMPARE(localizedString, hbTrId("txt_mus_list_pop"));

    // test Rock
    localizedString = mTest->getLocalizedString( QString("Rock") );
    QCOMPARE(localizedString, hbTrId("txt_mus_list_rock"));

    // test new preset
    localizedString = mTest->getLocalizedString( QString("New Preset") );
    QVERIFY(localizedString.isEmpty());
}

// End of file
