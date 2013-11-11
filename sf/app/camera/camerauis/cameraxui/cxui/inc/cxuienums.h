/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXUIENUMS_H
#define CXUIENUMS_H


namespace CxUi
{
    enum CameraView
    {
        //! Viewfinder view for still capture
        StillPreCaptureView   = 0,

        //! Viewfinder view for video capture
        VideoPreCaptureView = 1,

        //! Preview for captured image
        StillPostCaptureView = 2,

        //! Preview for captured image
        VideoPostCaptureView = 3
    };
}

// Application layout strings
namespace CxUiLayout
{

    // Application layout xml files
    static const char *VIDEO_1ST_XML = ":/xml/view_video_precapture.docml";
    static const char *STILL_1ST_XML = ":/xml/view_still_precapture.docml";
    static const char *POSTCAPTURE_XML = ":/xml/view_postcapture.docml";

    static const char *SCENEMODE_SETTING_XML = ":/xml/setting_scenemode.docml";

    static const char *ERROR_POPUP_XML = ":/xml/errornote_popup.docml";
    static const char *SETTINGS_SLIDER_DIALOG_XML = ":/xml/setting_slider.docml";

    static const char *FULL_SCREEN_POPUP_XML = ":/xml/full_screen_popup.docml";


    // Graphics
    static const char *TRANSPARENT_BACKGROUND_GRAPHIC = "qtg_fr_popup_trans";

    // Sections
    static const char *STILL_PRE_CAPTURE_INDICATORS_SECTION = "still_capture_indicators";
    static const char *STILL_PRE_CAPTURE_WIDGETS_SECTION = "still_capture_with_widgets";
    static const char *VIDEO_PRE_CAPTURE_INDICATORS_SECTION = "video_capture_indicators";
    static const char *VIDEO_PRE_CAPTURE_WIDGETS_SECTION = "video_capture_with_widgets";
    static const char *POST_CAPTURE_WIDGETS_SECTION = "post_capture_with_widgets";
    static const char *STILL_PRE_CAPTURE_EMBEDDED_SECTION = "still_embedded_mode";
    static const char *STILL_PRE_CAPTURE_STANDALONE_SECTION = "still_standalone_mode";
    static const char *VIDEO_PRE_CAPTURE_EMBEDDED_SECTION = "video_embedded_mode";
    static const char *VIDEO_PRE_CAPTURE_STANDALONE_SECTION = "video_standalone_mode";

    static const char *VIDEO_PRE_CAPTURE_IDLE       = "video_idle";
    static const char *VIDEO_PRE_CAPTURE_RECORDING  = "video_recording";
    static const char *VIDEO_PRE_CAPTURE_PAUSED     = "video_paused";

    // Widgets
    // In order to be able to refer to a widget, we need to know its object name.
    static const char *STILL_PRE_CAPTURE_VIEW = "still_capture_view";
    static const char *STILL_PRE_CAPTURE_TOOLBAR = "toolbar";
    static const char *STILL_PRE_CAPTURE_FLASH_ACTION = "cxui_action_flash";
    static const char *STILL_PRE_CAPTURE_SCENE_MODE_ACTION = "cxui_action_scenemodes";
    static const char *STILL_PRE_CAPTURE_ROOT_WIDGET = "viewfinder_widget";
    static const char *STILL_PRE_CAPTURE_VIEWFINDER = "transparent";
    static const char *STILL_PRE_CAPTURE_ZOOM_SLIDER = "zoom";
    static const char *STILL_PRE_CAPTURE_IMAGE_QUALITY_ACTION = "cxui_action_image_quality";
    static const char *STILL_PRE_CAPTURE_GOTO_VIDEO_ACTION = "cxui_action_goto_video";
    static const char *STILL_PRE_CAPTURE_IMAGES_LEFT_LABEL = "images_remaining";
    static const char *STILL_PRE_CAPTURE_QUALITY_ICON = "quality_indicator";
    static const char *STILL_PRE_CAPTURE_FACE_TRACKING_ICON = "face_tracking_indicator";
    static const char *STILL_PRE_CAPTURE_POST_CAPTURE_ACTION = "cxui_action_stillpostcapture";
    static const char *STILL_PRE_CAPTURE_INDICATOR_CONTAINER = "indicatorContainer";
    static const char *STILL_PRE_CAPTURE_IMAGES_LEFT_CONTAINER = "images_left_container";
    static const char *STILL_PRE_CAPTURE_GEOTAGGING_INDICATOR_ICON = "still_geotagging_indicator";
    static const char *STILL_PRE_CAPTURE_FLASHBLINK_INDICATOR_ICON = "flash_indicator";

    static const char *STILL_PRE_CAPTURE_SELFTIMER_CONTAINER = "still_precapture_selftimer_container";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_COUNTER = "still_selftimer_counter";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_START_BUTTON = "still_selftimer_start_button";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_CANCEL_BUTTON = "still_selftimer_cancel_button";

    static const char *VIDEO_PRE_CAPTURE_VIEW = "video_capture_view";

    static const char *SETTINGS_DIALOG_OK_ACTION = "action_ok";

    // video
    // In order to be able to refer to a widget, we need to know its object name.
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR = "video_toolbar";
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR_REC = "video_toolbar_recording";
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR_PAUSED = "video_toolbar_paused";
    static const char *VIDEO_PRE_CAPTURE_VIEWFINDER = "video_viewfinder_widget";
    static const char *VIDEO_PRE_CAPTURE_ZOOM_SLIDER = "video_zoom";
    static const char *VIDEO_PRE_CAPTURE_RECORDING_ICON = "video_recording_icon";
    static const char *VIDEO_PRE_CAPTURE_VIDEO_TIME_LABEL = "video_elapsed_time";
    static const char *VIDEO_PRE_CAPTURE_DEFAULT_MENU = "video_view_menu";
    static const char *VIDEO_PRE_CAPTURE_VIDEO_QUALITY_ACTION = "cxui_video_action_quality";
    static const char *VIDEO_PRE_CAPTURE_GOTO_STILL_ACTION = "cxui_video_action_gotostill";
    static const char *VIDEO_PRE_CAPTURE_POST_CAPTURE_ACTION  = "cxui_video_action_showcapturedvideo";
    static const char *VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_TOP = "video_indicator_container_top";
    static const char *VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_BOTTOM = "video_indicator_container_bottom";
    static const char *VIDEO_PRE_CAPTURE_SCENE_MODE_ACTION = "cxui_video_action_scene";

    static const char *VIDEO_PRE_CAPTURE_QUALITY_ICON = "video_quality_indicator";
    static const char *VIDEO_PRE_CAPTURE_GEOTAGGING_INDICATOR_ICON = "video_geotagging_indicator";
    static const char *VIDEO_PRE_CAPTURE_VIDEOAUDIOMUTE_INDICATOR_ICON = "video_audio_mute_indicator";
    static const char *VIDEO_PRE_CAPTURE_STABILITY_INDICATOR_ICON = "video_stability_indicator";

    // standby
    static const char *STANDBY_POPUP = "standbymode_popup";
    static const char *STANDBY_TEXT_WIDGET = "standby_text";

    // Full screen popup
    static const char *FULL_SCREEN_POPUP = "full_screen_popup";
    static const char *FULL_SCREEN_POPUP_TEXT = "full_screen_popup_text";
    static const char *FULL_SCREEN_POPUP_BUTTON = "full_screen_popup_button";

    // error manager
    static const char *ERROR_POPUP = "errornote_popup";
    static const char *ERROR_TEXT_WIDGET = "errornote_text";
    static const char *ERROR_BUTTON_WIDGET = "errornote_button_exit";

    //post capture
    static const char *POSTCAPTURE_VIEW  = "postcapture_view";
    static const char *STILL_POST_CAPTURE_TOOLBAR = "still_postcapture_toolbar";
    static const char *VIDEO_POST_CAPTURE_TOOLBAR = "video_postcapture_toolbar";
    static const char *EMBEDDED_POST_CAPTURE_TOOLBAR = "embedded_postcapture_toolbar";
    static const char *POST_CAPTURE_SNAPSHOT_LABEL = "snapshot_label";

    // common settings dialog layout details
    static const char *SETTINGS_DIALOG_XML = ":/xml/setting.docml";
    static const char *SETTINGS_DIALOG_WIDGET = "settings_dialog";
    static const char *SETTINGS_DIALOG_HEADING_WIDGET = "settings_dialog_heading";
    static const char *SETTINGS_DIALOG_CONTENT_WIDGET = "settings_dialog_list";

    // common slider settings dialog layout details
    static const char *SETTINGS_SLIDER_DIALOG_WIDGET = "slider_settings_dialog";
    static const char *SETTINGS_SLIDER_DIALOG_HEADING_WIDGET = "slider_settings_dialog_heading";
    static const char *SETTINGS_SLIDER_DIALOG_CONTENT_WIDGET = "settings_dialog_slider";

    // scene mode view
    static const char *SCENE_MODE_VIEW = "settings_scenes_view";
    static const char *SCENE_MODE_VIEW_CONTAINER = "scene_view_container";
    static const char *SCENE_MODE_VIEW_BG_IMAGE = "scene_view_bg_icon";
    static const char *SCENE_MODE_VIEW_BG_IMAGE2 = "scene_view_bg_icon2";
    static const char *SCENE_MODE_VIEW_RADIOBUTTONS = "scene_view_radioButtonList";
    static const char *SCENE_MODE_VIEW_OK_BUTTON = "scene_ok_button";
    static const char *SCENE_MODE_VIEW_CANCEL_BUTTON = "scene_cancel_button";
    static const char *SCENE_MODE_VIEW_HEADING_WIDGET = "scene_title";
}

namespace CxUiInternal
{
    // Property for setting id string
    static const char *PROPERTY_KEY_SETTING_ID   = "settingskey";
    // Property for settings grid initiated settings
    static const char *PROPERTY_KEY_SETTING_GRID = "settingsgrid";
    static const char *PROPERTY_KEY_TRUE         = "1";
}

namespace CxuiActivityIds
{
  static const QString STILL_PRECAPTURE_ACTIVITY =  "CameraViewfinderShow";
  static const QString STILL_POSTCAPTURE_ACTIVITY = "CameraPostCaptureShow";

  static const QString VIDEO_PRECAPTURE_ACTIVITY =  "CamcorderViewfinderShow";
  static const QString VIDEO_POSTCAPTURE_ACTIVITY = "CamcorderPostCaptureShow";
}

Q_DECLARE_METATYPE(CxUi::CameraView)

#endif // CXUIENUMS_H
