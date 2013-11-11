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
* Description: Unit test for MpMediaWallDocumentLoader
*/

#include "unittest_mpglobalpopuphandler.h"
#include "mpsongscanner.h"
#include "mpengine.h"
#include <hbpopup.h>
#include <hbprogressdialog.h>
#include <hbnotificationdialog.h>
#include <hblabel.h>
#include <QTranslator>
#include <QLocale>

/*
* Following methods were skipped on purpose since theres nothing valuable to test
* CloseUsbBlockingNote()
* HanldeMTPInfoDialogFinished
*/

// Do this so we can access all member variables.
#define private public
#include "mpglobalpopuphandler.h"
#undef private

#include "../../../src/mpglobalpopuphandler.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpGlobalPopupHandler tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpglobalpopuphandler.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpGlobalPopupHandler::TestMpGlobalPopupHandler()
    : mTest(0),      
      mMpTranslator(0)
{
}

TestMpGlobalPopupHandler::~TestMpGlobalPopupHandler()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpGlobalPopupHandler::initTestCase()
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
void TestMpGlobalPopupHandler::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpGlobalPopupHandler::init()
{
    mTest = new MpGlobalPopupHandler( new QObject() );
}

/*!
 Called after every testfunction.
 */
void TestMpGlobalPopupHandler::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test handleLibraryAboutToUpdate
 */
void TestMpGlobalPopupHandler::testHandleLibraryAboutToUpdate() 
{
    mTest->mMpSongScanner = 0;
    mTest->mMpEngine->mSongScanner = new MpSongScanner();
    
    mTest->handleLibraryAboutToUpdate();
    QCOMPARE( mTest->mMpSongScanner,  mTest->mMpEngine->mSongScanner );    
}

/*!
  test scanCountChanged
 */
void TestMpGlobalPopupHandler::testScanCountChanged() 
{
    HbProgressDialog* dialog = new HbProgressDialog( HbProgressDialog::WaitDialog );
    dialog->setModal( true );
    dialog->setText( QString("") );
    dialog->setObjectName( KScanProgressDialog );
        
    mTest->mOutstandingPopup = dialog;
    
    mTest->scanCountChanged( 2 );
    HbProgressDialog *dialogNew = qobject_cast<HbProgressDialog *>( mTest->mOutstandingPopup );
    QCOMPARE( dialogNew->text(),  hbTrId( "txt_mus_info_ln_songs_found" , 2 ));  

}

/*!
  test handleScanFinished
 */
void TestMpGlobalPopupHandler::testHandleScanFinished() 
{
    HbProgressDialog* dialog = new HbProgressDialog( HbProgressDialog::WaitDialog );
    dialog->setModal( true );
    HbLabel *title = new HbLabel( hbTrId( "testScanCountChanged" ) );
    title->setFontSpec(HbFontSpec(HbFontSpec::Primary));

    dialog->setHeadingWidget( title );
    dialog->setText( QString("") );
    dialog->setObjectName( KScanProgressDialog );
        
    mTest->mOutstandingPopup = dialog;    
    mTest->handleScanFinished( MpSongScanner::ScanErrorNone, 0 );
    QVERIFY( mTest->mOutstandingPopup->objectName() == KScanFinished );    
    QVERIFY( qobject_cast<HbNotificationDialog *>( mTest->mOutstandingPopup )->title() == hbTrId( "txt_mus_dpophead_refresh_complete" ) );
    
    mTest->mOutstandingPopup = dialog;
    mTest->handleScanFinished( MpSongScanner::ScanGeneralError, 0 );
    QVERIFY( mTest->mOutstandingPopup->objectName() == KScanFinished );
    QVERIFY( qobject_cast<HbNotificationDialog *>( mTest->mOutstandingPopup )->title() == hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
    
    mTest->mOutstandingPopup = dialog;
    mTest->handleScanFinished( MpSongScanner::ScanErrorDiskFull, 0 );
    QVERIFY( mTest->mOutstandingPopup->objectName() == KDiskFullDialog );

}

/*!
  test launchUnableToCotinueDueUsb
 */
void TestMpGlobalPopupHandler::testLaunchUnableToCotinueDueUsb()
{
    mTest->launchUnableToCotinueDueUsb();
    QVERIFY( mTest->mOutstandingPopup->objectName() == KUnableToContinueDueUSB );
}

/*!
  test launchUsbBlockingNote
 */
void TestMpGlobalPopupHandler::testLaunchUsbBlockingNote()
{
    mTest->launchUsbBlockingNote();
    QVERIFY( mTest->mOutstandingPopup->objectName() == KUsbBlockingNote );
}


/*!
  test launchMTPInfoDialog
 */
void TestMpGlobalPopupHandler::testLaunchMTPInfoDialog()
{
    mTest->launchMTPInfoDialog();
    QVERIFY( mTest->mOutstandingPopup->objectName() == KMTPInfoDialog );
}


/*!
  test outstandingPopupClosing
 */
void TestMpGlobalPopupHandler::testOutstandingPopupClosing()
{
    HbPopup *dialog = new HbPopup();
    connect( dialog, SIGNAL( aboutToClose() ), mTest, SLOT( outstandingPopupClosing() ) );
    mTest->mOutstandingPopup = dialog; 
    
    dialog->close();
    delete dialog;
    QVERIFY( mTest->mOutstandingPopup == 0 );       
}

/*!
  test launchScanFinishedDialog
 */
void TestMpGlobalPopupHandler::testLaunchScanFinishedDialog()
{
    mTest->launchScanFinishedDialog( true, 1 );
    QVERIFY( mTest->mOutstandingPopup->objectName() == KScanFinished );
    QVERIFY( qobject_cast<HbNotificationDialog *>( mTest->mOutstandingPopup )->title() == hbTrId( "txt_mus_dpophead_refresh_complete" ) );

    mTest->launchScanFinishedDialog( false, 1 );
    QVERIFY( mTest->mOutstandingPopup->objectName() == KScanFinished );
    QVERIFY( qobject_cast<HbNotificationDialog *>( mTest->mOutstandingPopup )->title() == hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
}


/*!
  test launchDiskFullDialog
 */

void TestMpGlobalPopupHandler::testLaunchDiskFullDialog() 
{
    mTest->launchDiskFullDialog();
    QVERIFY( mTest->mOutstandingPopup->objectName() == KDiskFullDialog );
}

/*!
  test setOutstandingPopup
 */
void TestMpGlobalPopupHandler::testSetOutstandingPopup() 
{
    HbPopup *dialog = new HbPopup();
    mTest->mOutstandingPopup = 0;    
    mTest->setOutstandingPopup( dialog );    
    QVERIFY( mTest->mOutstandingPopup == dialog );
}


// End of file
