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

#ifndef MPGLOBALPOPUPHANDLER_H
#define MPGLOBALPOPUPHANDLER_H

#include <QObject>

class HbPopup;
class HbAction;
class MpEngine;
class MpSongScanner;

class MpGlobalPopupHandler : public QObject
{
    Q_OBJECT

public:
    MpGlobalPopupHandler( QObject *parent = 0 );
    virtual ~MpGlobalPopupHandler();

    void cancelOngoingPopup();

public slots:

    // Scanning related dialogs
    void handleLibraryAboutToUpdate();
    void launchScanDialog();
    void scanCountChanged( int count );
    void handleScanFinished( int error, int itemsAdded );

    // Usb related dialogs
    void launchUnableToCotinueDueUsb();
    void launchUsbBlockingNote();
    void closeUsbBlockingNote();
    void launchMTPInfoDialog();
    void hanldeMTPInfoDialogFinished( HbAction *selectedAction );

    void outstandingPopupClosing();

private:
    void launchScanFinishedDialog( bool ok, int itemsAdded );
    void launchDiskFullDialog();
    void setOutstandingPopup( HbPopup *popup );

private:
    HbPopup                     *mOutstandingPopup;              // Own
    MpEngine                    *mMpEngine;                      // Not own
    MpSongScanner               *mMpSongScanner;                 // Not own
};

#endif  // MPGLOBALPOPUPHANDLER_H

