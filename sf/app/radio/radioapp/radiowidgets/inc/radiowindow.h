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

#ifndef _RADIOWINDOW_H_
#define _RADIOWINDOW_H_

// System includes
#include <HbMainWindow>
#include <HbEffect>
#include <QWeakPointer>
#include <QScopedPointer>
#include <QSharedPointer>

// User includes
#include "radiowidgetsexport.h"

// Forward declarations
class RadioViewBase;
class RadioUiEngine;
class HbVolumeSliderPopup;
class HbMessageBox;
class HbNotificationDialog;

typedef QScopedPointer<HbVolumeSliderPopup> VolumeSliderPtr;

/**
 * QWeakPointer is used to store the views because it tracks the deletion of the object and nulls
 * the reference. Transient view like RadioHistoryView is destroyed after they are closed
 * and QWeakPointer will notice it.
 */
typedef QWeakPointer<RadioViewBase> ViewPtr;

// Class declaration
class WIDGETS_DLL_EXPORT RadioWindow : public HbMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioWindow )

public:

    RadioWindow( QWidget* parent = 0 );

    ~RadioWindow();

    void showErrorMessage( const QString& text );

    void init();

    QString orientationSection();

public slots:

    void activateMainView();

    void activateStationsView();

    void activateHistoryView();

private slots:

    void initView();
    void updateOrientation( Qt::Orientation orientation );
    void showVolumeLevel( int volume );
    void updateAntennaStatus( bool connected );

private:

// New functions

    void activateView( RadioViewBase* view, const QString& docmlFile, Hb::ViewSwitchFlags flags = Hb::ViewSwitchDefault );

private: // data

    /*!
     * Pointer to the UI engine
     * Own, shared with views
     */
    QSharedPointer<RadioUiEngine>       mUiEngine;

    /**
     * Tuning view.
     * Own.
     */
    ViewPtr                             mMainView;

    /**
     * Stations view
     * Own.
     */
    ViewPtr                             mStationsView;

    /**
     * Play history view
     * Own.
     */
    ViewPtr                             mHistoryView;

    /**
     * Pointer to the volume slider
     * Own.
     */
    VolumeSliderPtr                     mVolSlider;
};


#endif // _RADIOWINDOW_H_
