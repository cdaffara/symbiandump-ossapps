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
* Description: Adjusts visibility of Phone app in device lock situations.
*
*/

#ifndef PHONEVISIBILITYHANDLER_H
#define PHONEVISIBILITYHANDLER_H

#include <QObject>

#include "mphonepubsubobserver.h"
#include <qvaluespacesubscriber.h>
#include <qvaluespacepublisher.h>

using namespace QtMobility;

class CEikonEnv;
class PhoneUIQtViewIF;

/*!
 * \class PhoneVisibilityHandler
 * \brief Adjusts visibility of Phone application in device lock situations.
*/
class PhoneVisibilityHandler : 
    public QObject,
    public MPhonePubSubObserver
{
    Q_OBJECT
    
    enum AdjustAction {
        ForceOnTop,
        BringForwards,
        KeepCurrentPos,
        SendToBack
    };

public:
    
    /**
     * Constructor.
     */
    PhoneVisibilityHandler(PhoneUIQtViewIF &view, QObject *parent = 0);

    /**
     * Destructor.
     */
    ~PhoneVisibilityHandler();

public:
    /**
     * Control visiblity
     */
    void bringToForeground();
    void sendToBackground(bool homeScreenForeground);
    void hideDeviceDialogs(bool hide);
    
    /**
     * Return true if phone is visible application
     */
    bool phoneVisible();
    
public: // from MPhonePubSubObserver

    /**
    * This function is called when there is property value change.
    * @param aCategory Category of the property
    * @param aKey Property key that is changed
    * @param aValue New property value
    */
    void HandlePropertyChangedL( 
        const TUid& aCategory,
        const TUint aKey,
        const TInt aValue);
        
private:
    /**
     * Handle keyguard according phone visiblity
     * and keyguard previous state
     */
    bool disableKeyGuard();
    void enableKeyGuard();

    /**
     * Number of currently ongoing call's
     */
    int ongoingCalls();
    
    /**
     * Adjusts visibility based on current call and device security statuses.
     */
    void adjustVisibility(AdjustAction action);
    
private slots:

    /**
     * QValueSpaceSubscriber has notified that Car Mode value has changed by calling this slot.
     */
    void carModeChanged();
    
private:
    /** Phone view */
    PhoneUIQtViewIF &m_view;
    
    /** Cached pointer to Eikon environment. Not own.*/
    CEikonEnv *m_eikonEnv;
    
    /** Car mode effects visibility */
    bool m_carModeEnabled;
    
    /** Hide device dialogs */
    bool m_hideDeviceDialogs;
    
    /** Device lock enabled */
    bool m_deviceLockEnabled;
    
    /** Subscribes to Car Mode changes */
    QValueSpaceSubscriber* m_carModeSubscriber;
    
    /** Car Mode publisher */
    QValueSpacePublisher* m_carModePublisher;
    
    /** Key guard status before phone came top */
    bool m_keyguardOnBeforeForeground;
};

#endif // PHONEVISIBILITYHANDLER_H
