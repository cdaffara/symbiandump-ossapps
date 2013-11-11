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
* Description: Music Player global popup handler.
*
*/

// INCLUDE FILES
#include <hblabel.h>
#include <hbaction.h>
#include <hbfontspec.h>
#include <hbmessagebox.h>
#include <hbprogressdialog.h>
#include <hbnotificationdialog.h>

#include "mpglobalpopuphandler.h"
#include "mpsettingsmanager.h"
#include "mpenginefactory.h"
#include "mpsongscanner.h"
#include "mpmtpinfolink.h"
#include "mptrace.h"

/*!
    \class MpGlobalPopupHandler
    \brief Music Player global popup handler.

    This class controls the asynchronous operation of all global dialogs. 
*/

// Popups launched by this class
const QString KScanProgressDialog = QString( "ScanProgressDialog" );
const QString KDiskFullDialog = QString( "DiskFullDialog" );
const QString KScanFinished = QString( "ScanFinishedDialog" );
const QString KUnableToContinueDueUSB = QString( "UnableToContinueDueUSB" );
const QString KUsbBlockingNote = QString( "UsbBlockingNote" );
const QString KMTPInfoDialog = QString( "MTPInfoDialog" );

// Popups Actions
const QString KYes = QString( "yes" );
const QString KNo = QString( "no" );

/*!
 Constructs the collection popup handler.
 */
MpGlobalPopupHandler::MpGlobalPopupHandler( QObject *parent )
    : QObject( parent ),
      mOutstandingPopup( 0 ),
      mMpEngine( 0 ),
      mMpSongScanner( 0 )
{
    TX_ENTRY
    mMpEngine = MpEngineFactory::sharedEngine();
    connect( mMpEngine, SIGNAL( libraryAboutToUpdate() ), this, SLOT( handleLibraryAboutToUpdate() ) );
    connect( mMpEngine, SIGNAL( unableToCotinueDueUSB() ), this, SLOT( launchUnableToCotinueDueUsb() ) );
    connect( mMpEngine, SIGNAL( usbSynchronizationStarted() ), this, SLOT( launchUsbBlockingNote() ) );
    connect( mMpEngine, SIGNAL( usbSynchronizationFinished() ), this, SLOT( closeUsbBlockingNote() ) );
    TX_EXIT
}

/*!
 Destructs the collection popup handler.
 */
MpGlobalPopupHandler::~MpGlobalPopupHandler()
{
    TX_ENTRY
    delete mOutstandingPopup;
    TX_EXIT
}

/*!
 Closes any active popup
 */
void MpGlobalPopupHandler::cancelOngoingPopup()
{
    TX_ENTRY
    if ( mOutstandingPopup ) {
         mOutstandingPopup->close();
    }
    TX_EXIT
}

/*!
 Slot called upon MpEngine signal libraryAboutToUpdate(). Used to connect with SongScanner.
 */
void MpGlobalPopupHandler::handleLibraryAboutToUpdate()
{
    TX_ENTRY
    if ( !mMpSongScanner && mMpEngine->songScanner() ) {
        mMpSongScanner = mMpEngine->songScanner();
        connect( mMpSongScanner, SIGNAL( scanStarted() ), this, SLOT( launchScanDialog() ) );
        connect( mMpSongScanner, SIGNAL( scanCountChanged( int ) ), this, SLOT( scanCountChanged( int ) ) );
        connect( mMpSongScanner, SIGNAL( scanFinished( int, int ) ), 
                 this, SLOT( handleScanFinished( int, int ) ) );
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MpSongScanner indicating start of
 scanning process.
 */
void MpGlobalPopupHandler::launchScanDialog()
{
    TX_ENTRY
    HbProgressDialog* scanProgressDialog = new HbProgressDialog( HbProgressDialog::WaitDialog );
    connect( scanProgressDialog, SIGNAL( cancelled() ), mMpSongScanner, SLOT( cancelScan() ) );
    scanProgressDialog->setModal( true );
    HbLabel *title = new HbLabel( hbTrId( "txt_mus_title_refreshing" ) );
    title->setFontSpec(HbFontSpec(HbFontSpec::Primary));

    scanProgressDialog->setHeadingWidget( title );
    scanProgressDialog->setText( QString("") );
    scanProgressDialog->setAttribute( Qt::WA_DeleteOnClose );
    scanProgressDialog->setObjectName( KScanProgressDialog );
    setOutstandingPopup( scanProgressDialog );
    scanProgressDialog->show();
    TX_EXIT
}

/*!
 Slot called upon notification from MpSongScanner indicating the number of
 songs scanned so far.
 */
void MpGlobalPopupHandler::scanCountChanged(int count)
{
    TX_ENTRY_ARGS("count " << count )
    if ( mOutstandingPopup && ( mOutstandingPopup->objectName() == KScanProgressDialog ) ) {
        HbProgressDialog *dialog = qobject_cast<HbProgressDialog *>( mOutstandingPopup );
        QString added;
        added = hbTrId( "txt_mus_info_ln_songs_found" , count );
        dialog->setText( added );
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MpSongScanner. Used to close current scan progress note,
 and display scan results.
 
 */
void MpGlobalPopupHandler::handleScanFinished( int error, int itemsAdded )
{
    TX_ENTRY_ARGS("error: " << error << " Items added: " << itemsAdded )

    if ( mOutstandingPopup && ( mOutstandingPopup->objectName() == KScanProgressDialog ) ) {
        HbProgressDialog *dialog = qobject_cast<HbProgressDialog *>( mOutstandingPopup );
        disconnect( dialog, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
        mOutstandingPopup = 0;
        dialog->close();
    }

    switch( error ) {
        case MpSongScanner::ScanErrorNone :
            launchScanFinishedDialog( true , itemsAdded );
            break;
        case MpSongScanner::ScanGeneralError :
            launchScanFinishedDialog( false, itemsAdded );
            break;
        case MpSongScanner::ScanErrorDiskFull :
            launchDiskFullDialog();
            break;
        case MpSongScanner::ScanInterrupted :
        default:
            //When scan interrupted (DiskEvent) just dimsiss the scanning progress note.
            break;
    }
    TX_EXIT
}

/*!
 Slot called when MpEngine emits unableToCotinueDueUSB() signal
 */
void MpGlobalPopupHandler::launchUnableToCotinueDueUsb()
{
    HbMessageBox *dialog = new HbMessageBox( HbMessageBox::MessageTypeInformation );
    dialog->setText( hbTrId( "txt_mus_info_usb_conn_in_progress" ) );
    dialog->setModal( true );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->setObjectName( KUnableToContinueDueUSB );
    setOutstandingPopup( dialog );
    dialog->show();
}

/*!
 Slot called when MpEngine emits usbSynchronizationStarted() signal
 */
void MpGlobalPopupHandler::launchUsbBlockingNote()
{
    HbProgressDialog *usbBlockingNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    usbBlockingNote->setModal( true );
    if ( usbBlockingNote->actions().count() ) {
        //Hide cancel action.
        usbBlockingNote->actions().at( 0 )->setVisible( false );
    }
    usbBlockingNote->setDismissPolicy( HbPopup::NoDismiss );
    usbBlockingNote->setText( hbTrId( "txt_mus_info_usb_conn_in_progress" ) );
    usbBlockingNote->setAttribute( Qt::WA_DeleteOnClose );
    usbBlockingNote->setObjectName( KUsbBlockingNote );
    setOutstandingPopup( usbBlockingNote );
    usbBlockingNote->show();
}

/*!
 Slot called when MpEngine emits usbSynchronizationFinished() signal
 */
void MpGlobalPopupHandler::closeUsbBlockingNote()
{
    if ( mOutstandingPopup && ( mOutstandingPopup->objectName() == KUsbBlockingNote ) ) {
        HbProgressDialog *dialog = qobject_cast<HbProgressDialog *>( mOutstandingPopup );
        dialog->cancel();
    }
}

/*!
  Slot to launch the MTP educating info dialog
  This dialog would be displayed after any manual refresh (from options menu),
  or after a refreshing originated by USBMassStorage disconnection.
 */
void MpGlobalPopupHandler::launchMTPInfoDialog()
{
    TX_ENTRY
    HbDialog *dialog = new HbDialog();
    dialog->setContentWidget( new MpMtpInfoLink() );
    dialog->setModal( true );
    dialog->setDismissPolicy( HbPopup::NoDismiss);
    dialog->setTimeout( HbPopup::NoTimeout );
    HbAction *action;
    action = new HbAction( hbTrId( "txt_common_button_yes" ) );
    action->setObjectName( KYes );
    dialog->addAction( action );
    action = new HbAction( hbTrId( "txt_common_button_no" ) );
    action->setObjectName( KNo );
    dialog->addAction( action );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->setObjectName( KMTPInfoDialog );
    setOutstandingPopup( dialog );
    dialog->open( this, SLOT( hanldeMTPInfoDialogFinished( HbAction* ) ) );
    TX_EXIT
}

/*!
 Slot to be called when MtpInfoDialog has finished.
 */
void MpGlobalPopupHandler::hanldeMTPInfoDialogFinished( HbAction *selectedAction )
{
    TX_ENTRY
    if ( selectedAction && selectedAction->objectName() == KNo ) {
        MpSettingsManager::instance()->stopShowingMtpInfo();
    }
    TX_EXIT
}

/*!
 Slot to be called when a popup is getting closed. Usefull when a dialog is closed before it finishes
 (dialog not closed by a direct user action).
 */
void MpGlobalPopupHandler::outstandingPopupClosing()
{
    TX_ENTRY
    HbPopup *popup = qobject_cast<HbPopup *>( sender() );
    if ( popup ) {
        Q_ASSERT( popup == mOutstandingPopup );
        mOutstandingPopup = 0;
    }
    TX_EXIT
}

/*!
 \internal
 Launches Scan Finished Notification.
 */
void MpGlobalPopupHandler::launchScanFinishedDialog( bool ok , int itemsAdded )
{
    HbNotificationDialog *finishedDialog = new HbNotificationDialog();
    finishedDialog->setModal(true);
    finishedDialog->setAttribute( Qt::WA_DeleteOnClose );
    finishedDialog->setObjectName( KScanFinished );
    // Connect aboutToClose with outstandingPopupClosing() first, and then with launchMTPInfoDialog
    // in order to get finishDialog cleared before MtpInfoDialog is launched.
    setOutstandingPopup( finishedDialog );

    // Educating user (of MTP information) dialog is displayed
    // only when the following conditions are met:
    // 1. A "manual" refresh operation completes successfully
    // 2. Refresh operation finds at least one new item

    if( ok ) {
        finishedDialog->setIcon( HbIcon( QString("qtg_large_ok") ) );
        finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_complete" ) );
        if ( MpSettingsManager::showMtpInfo() && !mMpSongScanner->isAutomaticScan() && itemsAdded ) {
            connect( finishedDialog, SIGNAL( aboutToClose() ), this, SLOT( launchMTPInfoDialog() ) );
        }
    }
    else {
        finishedDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
        finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
    }

    finishedDialog->show();
}

/*!
 \internal
 Launches DiskFull Notification
 */
void MpGlobalPopupHandler::launchDiskFullDialog()
{
    TX_ENTRY
    HbMessageBox *diskFullDialog = new HbMessageBox();
    diskFullDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
    diskFullDialog->setText( hbTrId( "txt_mus_info_out_of_disk_space" ) );
    diskFullDialog->setTimeout( HbPopup::NoTimeout);
    diskFullDialog->setAttribute( Qt::WA_DeleteOnClose );
    diskFullDialog->setObjectName( KDiskFullDialog );
    setOutstandingPopup( diskFullDialog );
    diskFullDialog->show();
    TX_EXIT
}

/*!
 \internal
 sets \a popup as the current outstanding popup and cancels any other previous popup.
 */
void MpGlobalPopupHandler::setOutstandingPopup( HbPopup *popup )
{
    TX_ENTRY
    if ( mOutstandingPopup ) {
        TX_LOG_ARGS( "Warning: Multiple popups attempted to be displayed" );
        if ( mOutstandingPopup->objectName() == KScanFinished ) {
            disconnect( mOutstandingPopup, SIGNAL( aboutToClose() ), this, SLOT( launchMTPInfoDialog() ) );
        }
        mOutstandingPopup->close();
    }

    connect( popup, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    mOutstandingPopup = popup;
    TX_EXIT
}

