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
* Description:  This file contains the header file of the CPEExternalDataHandler class
*
*/


#ifndef CPEEXTERNALDATAHANDLER_H
#define CPEEXTERNALDATAHANDLER_H

//  INCLUDE FILES
#include "cpeaudiooutputpreferencemonitor.h"
#include "cpecalldurationdisplaysettingmonitor.h"
#include "cpenetworkregistrationstatusmonitor.h"
#include "cpeprofilesettingmonitor.h"
#include "mpeexternaldatahandler.h"
#include "cpeaccessorymodemonitor.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class MPEDataStore;
class MPEPhoneModelInternal;
class PeNetworkHandler;

// DATA TYPES
// None

// CLASS DECLARATION

/**
*  CPEExternalDataHandler
*  For derivation only, not to be instantiated!
*  A container for Phone Engine base Central Repository monitors and
*  a command class for Central Repository settings.
*
*  @lib Phoneengine.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPEExternalDataHandler )
        : public CBase,
          public MPEExternalDataHandler
    {
    public:

        static CPEExternalDataHandler* NewL( MPEPhoneModelInternal& aModel );
        ~CPEExternalDataHandler();
        
// from base class MPEExternalDataHandler        
        
        /**
        * Gets the current value of the specified setting,
        * if the operation was successful then aValue contains the current valid
        * value of the setting
        * @param aSetting specifies the setting to be read
        * @param aValue the current value of the monitored setting
        * @return Symbian OS error code
        */
        TInt Get( const TPEExternalDataId aSetting, TInt& aValue ) const;

        /**
        * Gets the current value of the specified setting,
        * if the operation was successful then aValue contains the current valid
        * value of the setting
        * @param aSetting specifies the setting to be read
        * @param aValue the current value of the monitored setting
        * @return Symbian OS error code
        */
        TInt GetText( const TPEExternalDataId aSetting, TDesC& aValue ) const;

        /**
        * Sets the value of the specified setting,
        * @param aSetting specifies the setting to be written
        * @param aValue the value of the setting
        * @return Symbian OS error code
        */
        TInt Set( const TPEExternalDataId aSetting, const TInt aValue );

    private:

        CPEExternalDataHandler( MPEDataStore& aEngineInfo );
        void ConstructL( MPEPhoneModelInternal& aModel );

        /**
        * Check if automatic answer is defined for connected accessory.
        * @param aSetting specifies accessory type of setting
        * @param aValue the value of the setting
        * @return Symbian OS error code
        */
        TInt GetAutomaticAnswer( TUint32 aSetting,TInt& aValue ) const;
        
    protected: // Owned by this object

        // Reference to engine info object
        MPEDataStore& iDataStore;

        // Monitor objects
        CPEAudioOutputPreferenceMonitor* iAudioOutputPreferenceMonitor;
        CPECallDurationDisplaySettingMonitor* iCallDurationDisplay;     
        CPENetworkRegistrationStatusMonitor* iNetworkRegistrationStatus;
        CPEProfileSettingMonitor* iProfileSettings;
        CPEAccessoryModeMonitor* iAccessoryModeMonitor;
        PeNetworkHandler* iNetworkHandler;

        // Repository objects
        CRepository* iAccessorySettingsRepository;
        CRepository* iTelephonySettingsRepository;
        CRepository* iTelephonyVariationRepository;
        CRepository* iCoreApplicationRepository;
    };

#endif  //  CPEEXTERNALDATAHANDLER_H
