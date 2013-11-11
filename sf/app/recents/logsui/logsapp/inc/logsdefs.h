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

#ifndef LOGSDEFS_H
#define LOGSDEFS_H

//#ifndef QT_TESTLIB_LIB 
const char logsRecentCallsViewFile[] = ":/docml/recentCallsView.docml";
const char logsDetailsViewFile[] = ":/docml/detailsView.docml";
const char logsMatchedCallsViewFile[] = ":/docml/matchesView.docml";

//view ids
const char logsRecentCallsViewId[] = "logs_recent_calls_view";
const char logsDetailsViewId[] = "logs_details_view";
const char logsMatchesViewId[] = "logs_matches_view";

//widget ids
const char logsListWidgetId[] = "logs_list_view";
const char logsDetailsListWidgetId[] = "logs_details_list_view";
const char logsGroupboxViewNameId[] = "logs_groupbox_recent_calls";
const char logsGroupboxDetailViewNameId[] = "logs_groupbox_remote_party_info";
const char logsDetailsViewMenuId[] = "logs_details_view_menu";
const char logsShowFilterMenuId[] = "logs_show_filter_menu";
const char logsLabelEmptyListId[] = "logs_label_empty_list";
const char logsToolbarId[] = "logs_toolbar";
const char logsButtonAddToContactsId[] = "logs_button_addtocontacts";
const char logsPageIndicatorId[] = "logs_page_indicator";
const char logsContentId[] = "logs_content";

//action ids
const char logsCommonVideoCallMenuActionId[] = "logs_act_video_call";
const char logsCommonMessageMenuActionId[] = "logs_act_create_message";
const char logsCommonAddToContactsMenuActionId[] = "logs_act_add_to_contacts";

const char logsDetailsViewVoiceCallMenuActionId[] = "logs_act_voice_call";
const char logsDetailsViewInternetCallMenuActionId[] = "logs_act_internet_call";
const char logsDetailsOpenContactMenuActionId[] = "logs_act_open_contact";
const char logsDetailsCopyNumberMenuActionId[] = "logs_act_copy_number";
const char logsRecentViewClearListMenuActionId[] = "logs_act_clear_list";
const char logsRecentViewContactSearchMenuActionId[] = "logs_act_contact_search";

const char logsShowFilterReceivedMenuActionId[] = "logs_act_received";
const char logsShowFilterDialledMenuActionId[] = "logs_act_dialled";
const char logsShowFilterMissedMenuActionId[] = "logs_act_missed";
const char logsShowFilterRecentMenuActionId[] = "logs_act_recent";


//list layout names from *.widgetml
const char logsListDefaultLayout[] = "default";
const char logsListLandscapeLayout[] = "logsLandscape";
const char logsListLandscapeDialpadLayout[] = "logsLandscapeDialpad";

//view layout *.docml section names
const char logsViewDefaultSection[] = "default";
const char logsViewPortraitDialpadSection[] = "portrait_dialpad";
const char logsViewLandscapeDialpadSection[] = "landscape_dialpad";


//view identifiers
enum LogsAppViewId {
    LogsUnknownViewId,
    LogsRecentViewId,
    LogsDetailsViewId,
    LogsMatchesViewId
};

const int logsContactSearchPermanentlyDisabled = 0;
const int logsContactSearchEnabled = 1;
const int logsContactSearchDisabled = 2;



// Interface related constants
const char logsServiceName[] = "logs";

//param names below are deprecated
const char logsViewIndexParam[] = "view_index";
const char logsShowDialpadParam[] = "show_dialpad";
const char logsDialpadTextParam[] = "dialpad_text";

// Activity related constants
const char logsActivityIdViewRecent[] = "LogsViewRecent";
const char logsActivityIdViewCalled[] = "LogsViewCalled";
const char logsActivityIdViewReceived[] = "LogsViewReceived";
const char logsActivityIdViewMissed[] = "LogsViewMissed";
const char logsActivityIdViewMatches[] = "LogsViewMatches";
const char logsActivityIdViewDetails[] = "LogsViewDetails";

const char logsActivityParamShowDialpad[] = "show_dialpad"; // val type bool
const char logsActivityParamDialpadText[] = "dialpad_text"; // val type string
const char logsActivityParamInternalViewId[] = "view_id"; // val type int

// Command line args
const char logsBootupStartCmdLineArg[] = "-logsbootup";


#endif // LOGSDEFS_H
