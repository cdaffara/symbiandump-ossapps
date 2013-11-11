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
#ifndef IRMEDIAKEYOBSERVER_H_
#define IRMEDIAKEYOBSERVER_H_

#include <QObject>
#include <remconcoreapitargetobserver.h>

class CRemConInterfaceSelector;
class HbVolumeSliderPopup;
class QTimer;
class IRApplication;
class IRPlayController;

/**
 * This class observes the media key events and controls the volume.
 */

class IRMediaKeyObserver : public QObject, public MRemConCoreApiTargetObserver
{
    Q_OBJECT

public:
    explicit IRMediaKeyObserver(IRApplication* aApplication);
    ~IRMediaKeyObserver();

signals:
    void playPausePressed();
    void stopPressed();

public slots:
    void showVolumeSlider();

private slots:
    void volumeCallback();
    int changingVolume();
    void volumeChanged(int aVolume);

private:
    //from base class MRemConCoreApiTargetObserver
    //respond to the media button
    void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
            TRemConCoreApiButtonAction aButtonAct);

    //from base class MRemConCoreApiTargetObserver
    //respond for play command
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
            TRemConCoreApiButtonAction aButtonAct);

    void initializeL();

private:
    /*
     * iSelector
     * Instance of CRemConInterfaceSelector For
     * Handling Media Keys
     */
    CRemConInterfaceSelector* iInterfaceSelector;

    /*
     * Respond continuous volume change
     */
    QTimer* iVolumeControlTimer;

    HbVolumeSliderPopup *iVolumeSlider;
    bool iIncreseVolume;

    IRPlayController *iPlayController;
};

#endif /* IRMEDIAKEYOBSERVER_H_ */
