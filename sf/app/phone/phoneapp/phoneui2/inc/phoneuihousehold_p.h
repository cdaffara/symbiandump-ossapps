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
* Description:  UI house hold private for PhoneUI
*
*/

#ifndef PHONEUIHOUSEHOLDPRIVATE_H
#define PHONEUIHOUSEHOLDPRIVATE_H

#include <QtGui>
#include <pevirtualengine.h>
#include "mphonepubsubobserver.h"
#include "cphonerecoverysystem.h"

class MPEPhoneModel;
class CPhoneUIController;
class HbMainWindow;
class PhoneUIKeyEventAdapter;
class PhoneUiCommandAdapter;
class QtCall;
class PhoneUIQtViewAdapter;
class HbTranslator;

class PhoneUiHouseHoldPrivate : 
    private MPhonePubSubObserver
{

public:
    
    PhoneUiHouseHoldPrivate (HbMainWindow &window);

    ~PhoneUiHouseHoldPrivate ();
    
    /**
    * Performs startup signalling. 
    *
    * Calls either idle or security signalling.
    */
    static TInt DoStartupSignalL( TAny* aAny );
    
    /**
    * Idle signalling.
    */
    TInt DoStartupSignalIdleL();
    
    /**
    * Security signalling. 
    */
    TInt DoStartupSignalSecurityL();
  
  // From MPhonePubSubObserver
  /**
  * This function is called when there is property value change.
  * @param aCategory Category of the property
  * @param aKey Property key that is changed
  * @param aValue New property value
  */
  void HandlePropertyChangedL( 
      const TUid& aCategory,
      const TUint aKey,
      const TInt aValue );

private:
    
   /**
    * Constructor
    */
    void ConstructL();
    
private:
    // Startup states.
    // EPhoneStartupInitial - Initial state.
    // EPhoneStartedUp - Phone application is ready.
    // EPhoneIdleStartedUp - Idle application is ready.
    enum
        {
        EPhoneStartupInitial = 0x00000001,
        EPhoneStartedUp = 0x00000002,
        EPhoneIdleStartedUp = 0x00000004
        };

    
    // Application (Phone and Idle) startup status.
    TInt iAppsReady;

    // Recovery id for startup signalling.
    TRecoveryId iStartupSignalRecoveryId;

    //Boolean flag. ETrue iff we have reached light idle
    TBool iLightIdleReached;

    CPhoneUIController* iPhoneUIController;
    
    HbMainWindow &m_window;

    PhoneUIKeyEventAdapter* iKeyEventAdapter;
    
    PhoneUiCommandAdapter* iCommandAdapter;
    
    PhoneUIQtViewAdapter* iViewAdapter;
    
    //List for translators
    QList<HbTranslator*> m_translators;
    
};

#endif
