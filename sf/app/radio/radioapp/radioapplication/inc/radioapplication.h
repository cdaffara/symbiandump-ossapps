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

#ifndef _RADIOAPPLICATION_H_
#define _RADIOAPPLICATION_H_

// System includes
#include <HbApplication>

//#define QT_SHAREDPOINTER_TRACK_POINTERS // Debugging support for QSharedPointer
#include <QSharedPointer>

// Forward declarations
class RadioWindow;
class RadioStationModel;
class XQSettingsManager;

#ifdef BUILD_WIN32
    class Win32Window;
#endif // BUILD_WIN32

// Class declaration
class RadioApplication : public HbApplication
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioApplication )

public:

    explicit RadioApplication( int &argc, char *argv[] );

    ~RadioApplication();

private slots:

    void construct();
    void checkOfflineMode();
    void handleOfflineQueryAnswer();

private:

    void init();
    bool isInOfflineMode() const;
    void askOfflineModePermission( const QString& question );

private: // data

#ifdef BUILD_WIN32
    QScopedPointer<Win32Window>         mWin32Window;
#endif // BUILD_WIN32

    /**
     * Application main window
     * Own
     */
    QScopedPointer<RadioWindow>         mMainWindow;

    /**
     * Pointer to settings manager
     * Own.
     */
    XQSettingsManager*                  mSettingsManager;

    bool                                mInitialisationDone;

};

#endif // _RADIOAPPLICATION_H_
