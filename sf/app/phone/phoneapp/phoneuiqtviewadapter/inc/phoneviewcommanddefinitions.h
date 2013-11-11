/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*   Common phone view command definitions.
*
*/


#ifndef PHONEVIEWCOMMANDDEFINITIONS_H
#define PHONEVIEWCOMMANDDEFINITIONS_H

// INCLUDES

#include "phoneconstants.h"

// DATA TYPES

typedef TInt TPhoneViewCommandId;

enum
{
    EPhoneViewRemoveNumberEntry = KPhoneViewCommandCommonFirst,
    EPhoneViewSetNumberEntryVisible,
    EPhoneViewGetNumberFromEntry,
    EPhoneViewGetNumberEntryCount,
    EPhoneViewGetNumberEntryIsUsedStatus,
    EPhoneViewGetNumberEntryIsVisibleStatus,
    EPhoneViewClearNumberEntryContent,
    EPhoneViewCreateCallHeader,
    EPhoneViewCreateEmergencyCallHeader,
    EPhoneViewUpdateCallHeaderCallDuration,
    EPhoneViewRemoveCallHeader,
    EPhoneViewUpdateCba,
    EPhoneViewUpdateBubble,
    EPhoneViewActivateAudioPathUIChanges,
    EPhoneViewActivateAudioAvailabilityUIChanges,
    EPhoneViewActivateMuteUIChanges,
    EPhoneViewShowNote,
    EPhoneViewShowGlobalNote,
    EPhoneViewShowProgressNote,
    EPhoneViewRemoveNote,
    EPhoneViewRemoveProgressNote,
    EPhoneViewRemoveGlobalNote,
    EPhoneViewShowQuery,
    EPhoneViewSetListQueryString, 
    EPhoneViewRemoveQuery,
    EPhoneViewBringPhoneAppToForeground,
    EPhoneViewSendToBackground,
    EPhoneViewGetTopApplicationIsDisplayedStatus, 
    EPhoneViewGetForegroundApplication, 
    EPhoneViewStartCapturingKey,
    EPhoneViewStopCapturingKey,
    EPhoneViewSetGlobalNotifiersDisabled,
    EPhoneViewRemovePhoneDialogs,
    EPhoneViewCreateConference,
    EPhoneViewAddToConference,
    EPhoneViewRemoveFromConference,
    EPhoneViewPrivateFromConference,
    EPhoneViewSelectedConfMember,
    EPhoneViewGetCallIdByState,
    EPhoneViewGetCountOfActiveCalls,
    EPhoneViewSetNaviPaneAudioVolume,
    EPhoneViewHideNaviPaneAudioVolume,
    EPhoneViewGetIsConference,
    EPhoneViewGetCallExistsInConference,
    EPhoneViewRemoveAllCallHeaders,
    EPhoneViewPlayRingTone,
    EPhoneViewMuteRingTone,
    EPhoneViewStopRingTone,
    EPhoneViewMuteRingToneOnAnswer,
    EPhoneViewCipheringInfoChange,
    EPhoneViewLaunchRfsDeep, 
    EPhoneViewLaunchRfsNormal, 
    EPhoneViewOpenSoftRejectEditor,
    EPhoneViewSetSecurityMode, 
    EPhoneViewGetSecurityModeStatus, 
    EPhoneViewEnableKeyLock, 
    EPhoneViewEnableKeyLockWithoutNote, 
    EPhoneViewUpdateFSW,
    EPhoneViewBringIdleToForeground, 
    EPhoneViewBeginUpdate,
    EPhoneViewEndUpdate,
    EPhoneViewRemoveConferenceBubble,
    EPhoneViewSetTouchPaneButtons, 
    EPhoneViewOpenCallHandling, 
    EPhoneViewGetAudioVolumeLevel,
    EPhoneViewRemoveDtmfNote,
    EPhoneViewCancelAllNotications,   
    EPhoneViewDisableKeyLock, 
    EPhoneViewSendAiwCommand, 
    
    EPhoneViewCipheringInfoChangePlayTone, 
    
    EPhoneViewRemoveGlobalWaitNote,
    EPhoneViewSetPhoneCustomization, 
    EPhoneViewSetViewCustomization, 
    EPhoneViewSetButtonCustomization, 
    
    EPhoneViewUpdateContactByString, 
    EPhoneViewAddContactByString, 
    
    EPhoneViewEnableToolbarButton, 
    EPhoneViewHideToolbar, 
    EPhoneViewShowToolbar, 
    EPhoneViewGetExpandedBubbleCallId,
    EPhoneViewOpenDialer,
    EPhoneSetConferenceExpanded,
    EPhoneAppShutDown,
    EPhoneViewBackButtonActive,
    EPhoneViewHsToForegroundAfterCall,
    EPhoneViewOpenContacts,
    EPhoneViewSetIhfFlag,
    EPhoneViewSetMuteFlag,
    EPhoneViewLaunchLogs,
    EPhoneViewSetBlueToothFlag,
    EPhoneViewSetBluetoothAvailableFlag,
    EPhoneViewSetSoftRejectDimmed,
    EPhoneViewDisableDeviceLock,
    EPhoneViewIsRingingTonePlaying,
    EPhoneAmountOfCommands, // Don't remove this, this should always be the last in list.
};


typedef TInt TPhoneViewResponseId;

enum
{
    EPhoneViewResponseFailed = KPhoneViewResponseCommonFirst,
    EPhoneViewResponseSuccess,
    EPhoneViewResponseNumberEntryCleared

};

#endif // PHONEVIEWCOMMANDDEFINITIONS_H

// End of File
