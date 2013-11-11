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
#include <remconcoreapitarget.h> 
#include <remconinterfaceselector.h> 
#include <QTimer>
#include <hbvolumesliderpopup.h>

#include "irapplication.h"
#include "irplaycontroller.h"
#include "irmediakeyobserver.h"

const int KMaxVolume = 100;
const int KMinVolume = 0;
const int KVolumeStep = 10;
const int KFirstTimerExpiryInterval = 1;// Expire immediately
const int KTimerExpiryInterval = 1000 / 6; // KAknStandardKeyboardRepeatRate

IRMediaKeyObserver::IRMediaKeyObserver(IRApplication* aApplication) : iInterfaceSelector(NULL)
           , iVolumeControlTimer(NULL), iVolumeSlider(NULL)
           , iIncreseVolume(true), iPlayController(NULL)
{
    iPlayController = aApplication->getPlayController();

    iVolumeControlTimer = new QTimer(this);
    connect(iVolumeControlTimer, SIGNAL(timeout()), this, SLOT(volumeCallback()));

    iVolumeSlider = new HbVolumeSliderPopup();
    iVolumeSlider->setZValue(2.0);
    connect(iVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
    iVolumeSlider->setVisible(false);

    TRAP_IGNORE(initializeL());
}

IRMediaKeyObserver::~IRMediaKeyObserver()
{
    delete iVolumeSlider;
    iVolumeSlider = NULL;

    delete iInterfaceSelector;
    iInterfaceSelector = NULL;
}


/*
 * Description : handle the event from media key
 */
void IRMediaKeyObserver::MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
                                      TRemConCoreApiButtonAction aButtonAct)
{
    switch (aOperationId)
    {
    case ERemConCoreApiVolumeUp:
    case ERemConCoreApiVolumeDown:
        if (iVolumeControlTimer->isActive())
        {
            iVolumeControlTimer->stop();
        }

        if (ERemConCoreApiVolumeUp == aOperationId)
        {
            iIncreseVolume = true;
        }
        else
        {
            iIncreseVolume = false;
        }

        showVolumeSlider();
        switch (aButtonAct)
        {
        case ERemConCoreApiButtonClick:
            iVolumeSlider->setValue(changingVolume());
            break;
        case ERemConCoreApiButtonPress:
            iVolumeControlTimer->start(KFirstTimerExpiryInterval);
            break;
        case ERemConCoreApiButtonRelease:
            // Need not to stop volume control timer here because
            // it's stopped above.
        default:
            break;
        }
        break;

    case ERemConCoreApiPause:
    case ERemConCoreApiStop:
        if (aButtonAct == ERemConCoreApiButtonClick)
        {
            iPlayController->stop(EIRQUserTerminated);
        }
        break;

    case ERemConCoreApiPausePlayFunction:
    case ERemConCoreApiPlay:
        if (aButtonAct == ERemConCoreApiButtonClick)
        {
            if (iPlayController->isStopped())
            {
                // Scenario 1: In station view, play the station highlighted.
                // Scenario 2: In non-station view, play the last played station.
                emit playPausePressed();
            }
        }
        break;
    default:
        break;
    }
}

/*
 * Description : handle the play command
 */
void IRMediaKeyObserver::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED(aSpeed);
    MrccatoCommand(ERemConCoreApiPlay, aButtonAct);
}

void IRMediaKeyObserver::initializeL()
{
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    // Instance of CRemConCoreApiTarget For Handling Media Keys
    CRemConCoreApiTarget* coreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
    CleanupStack::PushL( coreTarget );
    iInterfaceSelector->OpenTargetL();
    CleanupStack::Pop(coreTarget);
}

/*
 * Description : slot function for volume timer timeout.
 *               Set volume to media player through play controller.
 * Parameters  : aDirection : true up, false down.
 */
void IRMediaKeyObserver::volumeCallback()
{
    iVolumeControlTimer->setInterval(KTimerExpiryInterval);

    iVolumeSlider->setValue(changingVolume());
}

/*
 * Description : return the volume value being about to change. 
 */
int IRMediaKeyObserver::changingVolume()
{
    int volume = iVolumeSlider->value();
    if (iIncreseVolume)
    {
        volume = volume + KVolumeStep;
        if (volume > KMaxVolume)
        {
            volume = KMaxVolume;
        }
    }
    else
    {
        volume = volume - KVolumeStep;
        if (volume < KMinVolume)
        {
            volume = KMinVolume;
        }
    }
    return volume;
}

/*
 * Description : slot function for signal sliderPositionChanged(int) of volume popup.
 *               Set volume to media player through play controller.
 * Parameters  : aVolume : the volume value. between 0 and 100
 */
void IRMediaKeyObserver::volumeChanged(int aVolume)
{
    iPlayController->setVolume(aVolume);

    QString str = QString::number(aVolume);
    str.append("%");
    iVolumeSlider->setText(str);
}

/*
 * Description : slot function for signal triggered() of volume action.
 *               show the volume popup.
 */
void IRMediaKeyObserver::showVolumeSlider()
{
    if (!iVolumeSlider->isVisible())
    {
        int val = iPlayController->getVolume();
        QString str = QString::number(val);
        str.append("%");
        iVolumeSlider->setValue(val);
        iVolumeSlider->setText(str);
        iVolumeSlider->show();
    }
}
