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
* Description: Unit test for mpsettingsaudioeffectswidget
*
*/

#include <QMetaType>
#include <QtGui>
#include <QTranslator>
#include <QLocale>

#include "unittest_mpsettingsaudioeffectswidget.h"
#include "mpenginefactory.h"
#include "mpengine.h"

// Do this so we can access all member variables.
#define private public
#include "MpSettingsAudioEffectsWidget.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpSettingsAudioEffectsWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsettingsaudioeffectswidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpSettingsAudioEffectsWidget::TestMpSettingsAudioEffectsWidget()
    : mTest(0),
      mMpTranslator(0)
{
}

TestMpSettingsAudioEffectsWidget::~TestMpSettingsAudioEffectsWidget()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSettingsAudioEffectsWidget::initTestCase()
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
void TestMpSettingsAudioEffectsWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsAudioEffectsWidget::init()
{
    MpEngineFactory::createSharedEngine();
    mTest = new MpSettingsAudioEffectsWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpSettingsAudioEffectsWidget::cleanup()
{
    delete mTest;
    mTest = 0;
    MpEngineFactory::close();
}

/*!
 Tests InitializeSlider.
 */
void TestMpSettingsAudioEffectsWidget::testInitializeSlider()
{
    //test slider widget is set up correctly in initialize()
    mTest->initialize();
    QVERIFY(mTest->mMpEngine != 0 );

    MpEngineFactory::sharedEngine()->setLoudness( false );

    QCOMPARE(mTest->mModel->rowCount(),2 );
    QCOMPARE(mTest->mModel->columnCount(),1 );

    QCOMPARE(  mTest->mModel->item( 0 )->type(), HbDataFormModelItem::SliderItem );
    QCOMPARE(  mTest->mModel->item( 1 )->type(), HbDataFormModelItem::ToggleValueItem );

    //slider
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("minimum"), QVariant(-100) );
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("maximum"), QVariant(100) );
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("majorTickInterval"), QVariant(100) );
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("minorTickInterval"), QVariant(25) );
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("singleStep"), QVariant(25) );
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("toolTipVisible"), QVariant(false) );   
    int position = Hb::SliderTicksRight;
    QCOMPARE((mTest->mModel->item( 0 )->contentWidgetData("tickPosition")).toInt(), position);
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("sliderPosition"), QVariant(mTest->mMpEngine->balance()) ); //this needs to be checked
    QCOMPARE(  mTest->mModel->item( 0 )->contentWidgetData("trackFilled"), QVariant(false) );

}

/*!
 Tests InitializeLoudnessOff.
 */
void TestMpSettingsAudioEffectsWidget::testInitializeLoudnessOff()
{
    //test loudness HbDataFormModelItem is set up correctly in initialize()
    MpEngineFactory::sharedEngine()->setLoudness( false );
    mTest->initialize();
    QVERIFY(  mTest->mModel->item( 1 )->type() == HbDataFormModelItem::ToggleValueItem );
    QCOMPARE(  mTest->mModel->item( 1 )->contentWidgetData("text"), QVariant(hbTrId( "txt_mus_setlabel_loudness_val_off" )) );
    QCOMPARE(  mTest->mModel->item( 1 )->contentWidgetData("additionalText"), QVariant(hbTrId( "txt_mus_setlabel_loudness_val_on" )) );
}

/*!
 Tests InitializeLoudnessOn.
 */
void TestMpSettingsAudioEffectsWidget::testInitializeLoudnessOn()
{
    //test loudness HbDataFormModelItem is set up correctly in initialize()
    MpEngineFactory::sharedEngine()->setLoudness( true );
    mTest->initialize();
    QVERIFY(  mTest->mModel->item( 1 )->type() == HbDataFormModelItem::ToggleValueItem );
    QCOMPARE(  mTest->mModel->item( 1 )->contentWidgetData("text"), QVariant(hbTrId( "txt_mus_setlabel_loudness_val_on" )) );
    QCOMPARE(  mTest->mModel->item( 1 )->contentWidgetData("additionalText"), QVariant(hbTrId( "txt_mus_setlabel_loudness_val_off" )) );

}

/*!
 Tests FilterSignals.
 */
void TestMpSettingsAudioEffectsWidget::testFilterSignals()
{   
    //filtersignals should not change loudness value
    MpEngineFactory::sharedEngine()->setLoudness( false );
    mTest->initialize();    
    QModelIndex index1 = mTest->mModel->indexFromItem((mTest->mModel->item(1)));
    mTest->filterSignals(index1,index1);
    QCOMPARE(MpEngineFactory::sharedEngine()->loudness(), false);
   
}




// End of file
