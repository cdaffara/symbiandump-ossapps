/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notification interface for S60 Calendar customizations
*
*/

#ifndef CALENNOTIFICATIONHANDLER_H
#define CALENNOTIFICATIONHANDLER_H

enum TCalenNotification
    {
    ECalenNotifyAll                                = 0, // Receive all notifications
    ECalenNotifyEntryInstanceViewCreated,
    ECalenNotifyEntryInstanceViewCreationFailed,
    ECalenNotifyContextChanged,
    ECalenNotifyExternalDatabaseChanged,
    ECalenNotifyEntrySaved,
    ECalenNotifyEntryDeleted,
    ECalenNotifyMultipleEntriesDeleted,
    ECalenNotifyInstanceSaved,
    ECalenNotifyInstanceDeleted,
    ECalenNotifySystemTimeChanged,
    ECalenNotifySystemLocaleChanged,
    ECalenNotifySystemLanguageChanged,
    ECalenNotifyLostAlarms,
    ECalenNotifyCmdLineLaunch,
    ECalenNotifyAvkonViewSwitchLaunch,
    ECalenNotifySettingsChanged,
    ECalenNotifyViewCreationStarted,
    ECalenNotifyDialogClosed,
    ECalenNotifyCloseDialogs,
    ECalenNotifyCommandFailed,
    ECalenNotifyDeleteFailed,
    ECalenNotifyEntryClosed,
    ECalenNotifySettingsClosed,
    ECalenNotifySettingsCRepKeyChanged,
    ECalenNotifyPluginEnabledDisabled,
    ECalenNotifyEComRegistryChanged,
    ECalenNotifyResourceChanged,
    ECalenNotifyViewPopulationComplete,
    ECalenNotifyAppForegrounded,
    ECalenNotifyAppBackgrounded,
    ECalenNotifyMarkedEntryDeleted,
    ECalenNotifyCheckPluginUnloading,
    ECalenNotifyEntrySent,
    ECalenNotifyCancelDelete,
    ECalenNotifyDayViewClosed,
	ECalenNotifyMapClosed,
	ECalenNotifyCancelMapLaunch,
    ECalenNotifyMissedAlarmViewClosed,
    ECalenNotifyMissedEventViewClosed,
    ECalenNotifyEditorClosedFromViewer,
    /*ECalenNotifyUserDefined1,
    ECalenNotifyUserDefined2,*/
    ECalenNotifyThaiPluginEnabled,
    ECalenNotifyThaiPluginDisabled,
    ECalenNotifyUserDefined3,
    ECalenNotifyUserDefined4,
    ECalenNotifyLast  
    };

/**
 * Class for handling calendar notifications.
 */
class MCalenNotificationHandler
    {
    public:  // New functions
        virtual void HandleNotification( const TCalenNotification aNotification ) = 0;
    };

#endif  // CALENNOTIFICATIONHANDLER_H

// End of file
