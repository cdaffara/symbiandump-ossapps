/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPEProfileSettingMonitor
*                class
*
*/


#ifndef CPEPROFILESETTINGMONITOR_H
#define CPEPROFILESETTINGMONITOR_H

//  INCLUDE FILES
#include <e32base.h>

// Profiles headers
#include <Profile.hrh>
#include <MProfile.h>
#include <MProfileEngine.h>
#include <MProfileTones.h>
#include <MProfileName.h>
#include <TProfileToneSettings.h>
#include <MProfileChangeObserver.h>
#include <CProfileChangeNotifyHandler.h>
#include "cpecenrepmonitor.h"

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;

// CLASS DECLARATION

/**
*  CPEProfileSettingMonitor
*  Used to retrieve profiles settings and provide callback to profiles engine
*  for notification of changes to profiles
*
*  @lib Phoneenginebase.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPEProfileSettingMonitor ) : public CPECenRepMonitor, public MProfileChangeObserver
{
public:
    static CPEProfileSettingMonitor* NewL( MPEPhoneModelInternal& aModel );
    ~CPEProfileSettingMonitor();
public: // From MProfileChangeObserver
    virtual void HandleActiveProfileEventL( TProfileEvent aProfileEvent, TInt aProfileId );
protected:
    CPEProfileSettingMonitor( MPEPhoneModelInternal& aModel );
    void ConstructL();
    void GetProfileSettingsL();
    // From CPECenRepMonitor
    void UpdateL();
private: // Not owned by this object
    MPEPhoneModelInternal& iModel; 
private: // Owned by this object
    MProfileEngine* iProfileEngine;  
    CProfileChangeNotifyHandler* iProfileEventHandler;
};

#endif // CPEPROFILESETTINGMONITOR_H
