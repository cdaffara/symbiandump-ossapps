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

#ifndef RADIOUILOADER_H_
#define RADIOUILOADER_H_

// System includes
#include <HbDocumentLoader>

// Constants

namespace DOCML
{
    static const QLatin1String FILE_MAINVIEW                    ( ":/layout/mainview.docml" );
    static const QLatin1String FILE_STATIONSVIEW                ( ":/layout/stationsview.docml" );
    static const QLatin1String FILE_HISTORYVIEW                 ( ":/layout/historyview.docml" );

    // Generic XML UI definitions
    static const QLatin1String NAME_VIEW                        ( "view" );
    static const QLatin1String SECTION_LAZY_LOAD                ( "lazy_load" );
    static const QLatin1String SECTION_PORTRAIT                 ( "portrait" );
    static const QLatin1String SECTION_LANDSCAPE                ( "landscape" );
    static const QLatin1String NAME_LOUDSPEAKER_ACTION          ( "loudspeaker_action" );
    static const QLatin1String NAME_EXIT_ACTION                 ( "exit_action" );

    // RadioMainView
    static const QLatin1String MV_NAME_HISTORYVIEW_ACTION       ( "mv:historyview_action" );
    static const QLatin1String MV_NAME_CAROUSEL_BACKGROUND      ( "mv:carousel_background" );
    static const QLatin1String MV_NAME_CAROUSEL_OVERLAY         ( "mv:carousel_overlay" );
    static const QLatin1String MV_NAME_STATION_CAROUSEL         ( "mv:station_carousel" );
    static const QLatin1String MV_NAME_FREQUENCY_BACKGROUND     ( "mv:frequency_background" );
    static const QLatin1String MV_NAME_FREQUENCY_STRIP          ( "mv:frequency_strip" );
    static const QLatin1String MV_NAME_STATIONS_BUTTON          ( "mv:stations_button" );
    static const QLatin1String MV_NAME_FAVORITE_BUTTON          ( "mv:favorite_button" );
    static const QLatin1String MV_NAME_SPEAKER_BUTTON           ( "mv:loudspeaker_button" );
    static const QLatin1String MV_NAME_PREV_BUTTON              ( "mv:prev_button" );
    static const QLatin1String MV_NAME_NEXT_BUTTON              ( "mv:next_button" );
    static const QLatin1String MV_NAME_SCAN_ACTION              ( "mv:scan_action" );
    static const QLatin1String MV_NAME_CAROUSEL_RT_MENU         ( "mv:carousel_rt_menu" );
    static const QLatin1String MV_SECTION_NO_ANTENNA            ( "mv:no_antenna" );
    static const QLatin1String MV_SECTION_SEEKING               ( "mv:seeking" );
    static const QLatin1String MV_SECTION_SCANNING              ( "mv:scanning" );
    static const QLatin1String MV_SECTION_NO_FAVORITES          ( "mv:no_favorites" );
    static const QLatin1String MV_SECTION_NORMAL                ( "mv:normal" );

    // RadioStationsView
    static const QLatin1String SV_NAME_HEADING_BANNER           ( "sv:heading_banner" );
    static const QLatin1String SV_NAME_STATIONS_LIST            ( "sv:stations_list" );
    static const QLatin1String SV_NAME_FAVORITES_BUTTON         ( "sv:favorite_stations_button" );
    static const QLatin1String SV_NAME_LOCALS_BUTTON            ( "sv:local_stations_button" );
    static const QLatin1String SV_NAME_SCAN_ACTION              ( "sv:scan_action" );
    static const QLatin1String SV_NAME_MULTI_SELECTION_ACTION   ( "sv:multi_selection_action" );
    static const QLatin1String SV_NAME_SCAN_BUTTON              ( "sv:scan_button" );
    static const QLatin1String SV_NAME_PLAY_ACTION              ( "sv:play_action" );
    static const QLatin1String SV_NAME_RENAME_ACTION            ( "sv:rename_action" );
    static const QLatin1String SV_NAME_FAVORITE_ACTION          ( "sv:toggle_favorite_action" );
    static const QLatin1String SV_NAME_DELETE_ACTION            ( "sv:delete_action" );

    static const QLatin1String SV_SECTION_SHOW_ALL_STATIONS     ( "show_all_stations" );
    static const QLatin1String SV_SECTION_SHOW_FAVORITES        ( "show_favorites" );
    static const QLatin1String SV_SECTION_SHOW_SCAN_TEXT        ( "show_scan_text" );
    static const QLatin1String SV_SECTION_HIDE_SCAN_TEXT        ( "hide_scan_text" );

    // RadioStationsView Context Menu
    static const QLatin1String NAME_CONTEXT_MENU                ( "sv:context_menu" );
    static const QLatin1String NAME_CONTEXT_RENAME              ( "sv:rename_action" );
    static const QLatin1String NAME_CONTEXT_FAVORITE            ( "sv:toggle_favorite_action" );
    static const QLatin1String NAME_CONTEXT_DELETE              ( "sv:delete_action" );
    static const QLatin1String NAME_INPUT_QUERY                 ( "sv:station_name_query" );

    // Station Carousel
    static const QLatin1String MV_NAME_INFO_FIRST_ROW           ( "mv:InfoFirstRow" );
    static const QLatin1String MV_NAME_INFO_SECOND_ROW          ( "mv:InfoSecondRow" );
    static const QLatin1String MV_NAME_INFO_THIRD_ROW           ( "mv:InfoThirdRow" );
    static const QLatin1String MV_NAME_INFO_FOURTH_ROW          ( "mv:InfoFourthRow" );
    static const QLatin1String MV_NAME_INFO_TEXT                ( "mv:info_text" );

    // History View
    static const QLatin1String HV_NAME_MULTI_DELETE_ACTION      ( "hv:multi_delete_action" );
    static const QLatin1String HV_NAME_HISTORY_LIST             ( "hv:history_list" );
    static const QLatin1String HV_NAME_ALL_SONGS_BUTTON         ( "hv:all_songs_button" );
    static const QLatin1String HV_NAME_TAGGED_SONGS_BUTTON      ( "hv:tagged_songs_button" );
    static const QLatin1String HV_SECTION_SHOW_LIST             ( "hv:show_list" );
    static const QLatin1String HV_SECTION_HIDE_LIST             ( "hv:hide_list" );
    static const QLatin1String HV_SECTION_HISTORY_MODE          ( "hv:history_mode" );
    static const QLatin1String HV_SECTION_FAVORITE_MODE         ( "hv:tagged_mode" );
    static const QLatin1String HV_NAME_CONTEXT_MENU             ( "hv:context_menu" );
    static const QLatin1String HV_NAME_TOGGLE_TAG_ACTION        ( "hv:toggle_tag_action" );
    static const QLatin1String HV_NAME_OVI_STORE_ACTION         ( "hv:ovi_store_action" );

    // History View Context Menu
    static const QLatin1String HV_NAME_CONTEXT_TAG              ( "hv:toggle_tag_action" );
    static const QLatin1String HV_NAME_CONTEXT_SEARCH           ( "hv:search_from_other_store_action" );

}

// Class declaration
class RadioUiLoader : public HbDocumentLoader
{
public:

    RadioUiLoader();

    /*!
     * Returns the requested widget casted to correct type
     *
     * @param name Name of the widget
     * @return Pointer to the widget
     */
    template<class T>
    T* findWidget( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findWidget( name ) );
    }

    /*!
     * Returns the requested object casted to correct type
     *
     * @param name Name of the object
     * @return Pointer to the object
     */
    template<class T>
    T* findObject( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findObject( name ) );
    }

};

#endif // RADIOUILOADER_H_
