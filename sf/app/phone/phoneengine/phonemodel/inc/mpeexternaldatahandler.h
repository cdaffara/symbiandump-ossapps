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
* Description:  This file defines the protocol independent interface
*                of the Phone Engine external data store framework for
*                Phone Engine submodule use.
*
*/


#ifndef MPEEXTERNALDATAHANDLER_H
#define MPEEXTERNALDATAHANDLER_H

//  INCLUDE FILES
#include <pevirtualengine.h>

// FORWARD DECLARATIONS
// None

// DATA TYPES
enum TPEExternalDataId
    {
    EPEAudioOutputPreference,
    EPEAutomaticAnswerHeadsetSetting,
    EPEAutomaticAnswerLoopsetSetting,
    EPEAutomaticAnswerCarkitSetting,
    EPEAutomaticAnswerMusicStandSetting,
    EPEAutomaticAnswerWirelessCarkitSetting,
    EPECallDurationDisplaySetting,
    EPEDialPrefixChangeSetting,
    EPEDialPrefixTextSetting,  
    EPETelephonyVariationFlags,
    EPEAccessoryMode,
    EPEEmergencyCallInfo,
    EPENetworkConnectionAllowedSetting
    };

// CLASS DECLARATION

/**
*  MPEExternalDataHandler
*  This internal interface provides for an access to Phone Engines external data
*   store, i.e. settings stored in Central Repository and Publish and Subscribe.
*
*  @lib Phoneengine.lib
*/
NONSHARABLE_CLASS( MPEExternalDataHandler )
    {
    public:

        /**
        * Gets the current value of the specified setting,
        * if the operation was successful then aValue contains the current valid
        * value of the setting
        * @param aSetting specifies the setting to be read
        * @param aValue the current value of the monitored setting
        * @return Symbian OS error code
        */
        virtual TInt Get( const TPEExternalDataId aSetting, TInt& aValue ) const = 0;

        /**
        * Gets the current value of the specified setting,
        * if the operation was successful then aValue contains the current valid
        * value of the setting
        * @param aSetting specifies the setting to be read
        * @param aValue the current value of the monitored setting
        * @return Symbian OS error code
        */
        virtual TInt GetText( const TPEExternalDataId aSetting, TDesC& aValue ) const = 0;

        /**
        * Sets the value of the specified setting,
        * @param aSetting specifies the setting to be written
        * @param aValue the value of the setting
        * @return Symbian OS error code
        */
        virtual TInt Set( const TPEExternalDataId aSetting, const TInt aValue ) = 0;

    };

#endif  //  MPEEXTERNALDATAHANDLER_H
