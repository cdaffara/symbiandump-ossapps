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
#ifndef IRUIDEFINES_H_
#define IRUIDEFINES_H_

// Docml File
static const char* ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME     = ":/layout/abstractlistviewbase.docml";
static const char* STATION_DETAILS_CONTAINER_LAYOUT_FILENAME   = ":/layout/stationdetailscontainer.docml";
static const char* NOW_PLAYING_VIEW_LAYOUT_FILENAME            = ":/layout/nowplayingview.docml";
static const char* STATION_DETAILS_VIEW_LAYOUT_FILENAME        = ":/layout/stationdetailsview.docml";
static const char* OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME       = ":/layout/openwebaddressview.docml";
static const char* SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME        = ":/layout/searchchannelsview.docml";
static const char* TERMS_CONS_VIEW_LAYOUT_FILENAME             = ":/layout/termsconsview.docml";
static const char* OPEN_WEB_ADDRESS_VIEW_PUSH_BUTTON_CSS       = ":/layout/pushbutton.css";
static const char* OPEN_WEB_ADDRESS_VIEW_PUSH_BUTTON_WIDGETML  = ":/layout/customizer.hbpushbutton.widgetml";

// Common Objects
static const char* EXIT_ACTION          = "exit_act";
static const char* SETTINGS_ACTION      = "setting_act"; 
static const char* GO_TO_STATION_ACTION = "go2station_act";
static const char* OBJECT_TOOLBAR       = "toolbar";
static const char* VIEW_CONTAINER       = "container";

// Common Sections
static const char* PORTRAIT_SEC  = "prt_section";
static const char* LANDSCAPE_SEC  = "lsc_section";


// IrAbstractListViewBase -- Object Name
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_VIEW                        = "ext-IrAbstractListViewBase";
// menu action
// tool bar action
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_COLLECTION_ACTION    = "collections";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_FAVORITE_ACTION      = "favorites";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_GENRE_ACTION         = "genres";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_SEARCH_ACTION        = "search";
// widget
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_HEADINGTEXTLABEL     = "headingText";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_PLAYINGBANNER        = "playingBanner";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_STATIONNAME          = "stationName";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_ARTISTSONGNAME       = "artistSongName";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_LISTVIEW             = "listView";

// IrAbstractListViewBase -- Section Name
static const char* ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION    = "section_no_playingbanner";
static const char* ABSTRACT_LIST_VIEW_BASE_WITH_PLAYINGBANNER_SECTION  = "section_with_playingbanner";
static const char* ABSTRACT_LIST_VIEW_BASE_NO_TOOLBAR_SECTION          = "section_no_toolbar_with_heading";
static const char* ABSTRACT_LIST_VIEW_BASE_WITH_TOOLBAR_SECTION        = "section_with_toolbar_no_heading";
static const char* ABSTRACT_LIST_VIEW_BASE_ADD_TOOLBAR_SECTION         = "section_add_toolbar";

// OpenWebAddressView -- Object Name
static const char* OPEN_WEB_ADDRESS_VIEW_OBJECT_VIEW                       = "ext-IRQOpenWebAddressView";
static const char* OPEN_WEB_ADDRESS_VIEW_OBJECT_DATA_FORM                  = "dataForm";
static const char* OPEN_WEB_ADDRESS_VIEW_OBJECT_PLAY_BUTTON                = "playButton";

// SearchView -- Object Name
static const char* SEARCH_CHANNELS_VIEW_OBJECT_VIEW                   = "ext-IRSearchChannelsView"; 
static const char* SEARCH_CHANNELS_VIEW_SEARCHPANEL_WIDGET            = "search_panel";
static const char* SEARCH_CHANNELS_VIEW_SEARCHLISTVIEW_WIDGET         = "search_listview";
static const char* SEARCH_CHANNELS_VIEW_HEADINGTEXT_WIDGET            = "headingText";

// IRNowplayingView  -- Object Name
static const char* NOW_PLAYING_VIEW_OBJECT_NAME                       = "ext-nowplayingview"; 
// IRNowplayingView  -- widget
static const char* NOW_PLAYING_VIEW_OBJECT_SHARE_STATION_ACTION       = "share_station_act"; 
static const char* NOW_PLAYING_VIEW_OBJECT_MUSICSTORE                 = "musicstore_act";
static const char* NOW_PLAYING_VIEW_OBJECT_IDENTIFYSONG               = "songrecg_act";
static const char* NOW_PLAYING_VIEW_OBJECT_PLAYSTOP                   = "play_stop_act";
static const char* NOW_PLAYING_VIEW_OBJECT_ADDTOFAV                   = "add_2_fav_act";
static const char* NOW_PLAYING_VIEW_OBJECT_FLIP                       = "detail_act";
static const char* NOW_PLAYING_VIEW_OBJECT_STATION_LOGO               = "station_logo";
static const char* NOW_PLAYING_VIEW_OBJECT_SONG_NAME_LABEL            = "song_name_label";
static const char* NOW_PLAYING_VIEW_OBJECT_SONG_NAME_MARQUEE          = "song_name_marquee";
static const char* NOW_PLAYING_VIEW_OBJECT_ARTIST_NAME                = "artist_name";
static const char* NOW_PLAYING_VIEW_OBJECT_STATION_NAME               = "station_name";
#ifdef ADV_ENABLED
static const char* NOW_PLAYING_VIEW_OBJECT_ADVERTISEMENT_IMAGE        = "advertisementImage";
#endif
// IRNowplayingView  -- section
static const char* NOW_PLAYING_VIEW_SONG_RECOG_YES_SEC    = "song_recog_yes_section";
static const char* NOW_PLAYING_VIEW_SONG_RECOG_NO_SEC     = "song_recog_no_section";

// IRStationDetailsView  -- Object Name
static const char* STATION_DETAILS_VIEW_OBJECT_NAME     = "ext-IRStationDetailsView";
// IRStationDetailsView  -- widget
static const char* STATION_DETAILS_VIEW_OBJECT_SCROLL_AREA          = "scrollArea";
static const char* STATION_DETAILS_VIEW_OBJECT_SCROLLAREA_CONTENTS  = "scrollAreaContents";
static const char* STATION_DETAILS_VIEW_OBJECT_STATION_NAME         = "stationName";
static const char* STATION_DETAILS_VIEW_OBJECT_GENRE                = "genre";
static const char* STATION_DETAILS_VIEW_OBJECT_LANGUAGE             = "language";
static const char* STATION_DETAILS_VIEW_OBJECT_COUNTRY              = "country";
static const char* STATION_DETAILS_VIEW_OBJECT_BITE_RATE            = "bitRate";
static const char* STATION_DETAILS_VIEW_OBJECT_DESCRIPTION          = "description";
static const char* STATION_DETAILS_VIEW_OBJECT_STATION_URL          = "stationUrl";

// IRTermsConsView -- Object Name
static const char* TERMS_CONS_VIEW_BASE_OBJECT_VIEW  = "ext-IRTermsConsView";
// IRTermsConsView -- widget
static const char* TERMS_CONS_VIEW_ACCEPT_BTN   = "accept_btn";
static const char* TERMS_CONS_VIEW_DECLINE_BTN  = "decline_btn";
static const char* TERMS_CONS_VIEW_TEXT_EDIT  = "text_edit";

// NowPlayingBanner
static const QString KNowPlayingBannerColorPressed = "qtc_multimedia_trans_pressed";
static const QString KNowPlayingBannerColorNormal = "qtc_multimedia_trans_normal";

#define LOADING_ANIMATION_ICON_SIZE 50

const int KBitmapSize = 59;
const int KMaxLineEditLength = 255;

#endif /* IRUIDEFINES_H_ */
