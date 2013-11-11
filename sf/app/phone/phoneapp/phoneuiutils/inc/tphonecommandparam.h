/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for all command parameter classes.
*
*/

#ifndef TPHONECOMMANDPARAM_H
#define TPHONECOMMANDPARAM_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TPhoneCommandParam;

// DATA TYPES
typedef TPhoneCommandParam TPhoneUICommandParam;
typedef TPhoneCommandParam TPhoneEngineCommandParam;

// CLASS DECLARATION

/**
*  A base class for command parameter.
*  
*/
class TPhoneCommandParam
    {
    public: // enum

    /** TPhoneParamId is list of possible phone ui command parameters */
    enum TPhoneParamId
        {
        EPAParamIdCallId = 200,
        EPAParamIdKeyCode,
        EPAParamIdDtmfString,
        EPAParamIdPhoneNumber,
        EPAParamIdAudioIHFMode,
        EPAUIParamIdMOTerminationIndicator,
        EPAUIParamIdMOSettingsIndication,
        EPAUIParamCallControlState,
        EPAUIParamIdMenuBar,
        EPAUIParamIdChangeMenuBar,
        EPAUIParamIdChangeToolBar,
        EPAUIParamIdRemoteInfo,
        EPAUIParamIdInformationBanner,
        EPAUIParamIdCallStatusIndicator,    
        EPAUIParamIdTerminationNote,
        EPAUIParamIdViewId,
        EPAUIParamIdKeyEvent,
        EPAUIParamIdServiceReqResult,
        EPAUIParamIdToolBar,
        EPAUIParamIdEmergencyPhase,
        EPAUIParamIdKeyCapture,    
        EPAUIParamIdHscsd,
        EPAUIParamIdUid,        
        EPAUIParamAppInfo,
        EPAUIParamIdSoftkeyName,
        EPAUIParamIdGlobalIndicator, // for displaying global note
        EPAUIParamIdCallGuard,

    // ----- USED TYPES ---------
        EPhoneParamIdInteger,
        EPhoneParamIdMenu,
        EPhoneParamIdBoolean,
        EPhoneParamIdKeyEvent,
        EPhoneParamIdKeyCapture,
        EPhoneParamIdString,
        EPhoneParamIdNote,
        EPhoneParamIdGlobalNote,
        EPhoneParamIdCallHeaderData,
        EPhoneParamIdEmergencyCallHeaderData,
        EPhoneParamIdIncallIndicatorData,
        EPhoneParamIdAppInfo,
        EPhoneParamIdQuery,
        EPhoneParamIdMessageEditor,
        EPhoneParamIdCallStateData,
        EPhoneParamIdSingleItemFetch,
        EPhoneParamIdBitmap,
        EPhoneParamIdSfiData,
        EPhoneParamIdProgressNote,
        EPhoneParamIdAudioOutput,
        EPhoneParamIdAudioAvailable,
        EPhoneParamIdRingTone,
        EPhoneParamIdReconnectQuery,
        EPhoneParamIdVideoCallSetupFailedCreateVoiceCallToTheSameContactQuery,
        EPhoneParamRebootQuery,
        EPhoneParamIdUnattendedTransferAcceptanceQuery,
        EPhoneParamIdCustomDialer,
        EPhoneParamIdGlobalWaitNote,
        EPhoneNumberEntryObserver,
        EPhoneParamIdPointer,
        // Add new params before this
        EPhoneParamIdLast
        };

    public: 
        
        /**
        * Method for checking the command parameter id.
        * 
        * @return Returns an id of the command parameter in question.
        */
        IMPORT_C TPhoneParamId ParamId() const;

        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCommandParam();


    protected:

        /**
        * Param ID.
        */
        TPhoneParamId iParamId;
    };

#endif // TPHONECOMMANDPARAM_H   
            
// End of File
