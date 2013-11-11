/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Localisation strings
*
*/

#ifndef GLXLOCALISATIONSTRINGS_H_
#define GLXLOCALISATIONSTRINGS_H_

#include "hbglobal.h"

//-------------------------------------------------------------------------------------------------------//
//-----------------------------START OF OPTIONS LOCALISATION STRING----------------------------------
//-------------------------------------------------------------------------------------------------------//

//Options list item
#define GLX_OPTION_SHARE               hbTrId("txt_photos_opt_share") 

//Options list item. Opens submenu where slide show can be played or settings edited or viewed.
#define GLX_OPTION_SLIDESHOW           hbTrId("txt_photos_opt_slideshow")
//sub menu options for slideshow play

#define GLX_OPTION_SS_PLAY             hbTrId("txt_photos_opt_sub_play")

//sub menu options for slideshow
#define GLX_OPTION_SS_SETTINGS         hbTrId("txt_photos_opt_sub_settings")

//
#define GLX_OPTION_ADD_TO_ALBUM        hbTrId("txt_photos_opt_add_to_album")

//
#define GLX_OPTION_SETTINGS            hbTrId("txt_common_opt_settings")

//Options list item. Note! Only use this text ID if there are no icons.
//Erases something completely from the phone, list, or memory card, 
//for example. Delete should only be used when something in removed permanently.                                            
#define GLX_OPTION_DELETE              hbTrId("txt_common_opt_delete")

//Options list item. Marks all the items in the view for editing, transferring or removing.
#define GLX_OPTION_MARK_ALL            hbTrId("txt_photos_opt_mark_all")

//Options list item. Unmarks all the items in the view.
#define GLX_OPTION_UN_MARK_ALL         hbTrId("txt_photos_opt_unmark_all")

//Options list item. Removes selected item(s) from the folder.
#define GLX_OPTION_REMOVE_FROM_ALBUM   hbTrId("txt_photos_opt_remove_from_album")

//
#define GLX_OPTION_NEW_ALBUM           hbTrId("txt_photos_opt_new_album")

//3D effect option to allow user to enable and disable 3D Grid Effect.
//Submenu option for 3D effect option
//Off sub menu option for 3D effect option
#define GLX_GRID_OPT_ON                hbTrId("txt_photos_opt_view_sub_on")

#define GLX_GRID_OPT_OFF               hbTrId("txt_photos_opt_sub_off")

#define GLX_GRID_OPT_EFFECT            hbTrId("txt_photos_opt_3d_effect")


//-----------------------------END OF OPTIONS LOCALISATION STRING----------------------------------------

//-------------------------------------------------------------------------------------------------------//
//-----------------------------START OF MENU LOCALISATION STRING----------------------------------------
//-------------------------------------------------------------------------------------------------------//

//context menu item share , invoke share UI
#define GLX_MENU_SHARE                 hbTrId("txt_photos_menu_share")

//context menu item to play slideshow
#define GLX_MENU_SLIDESHOW             hbTrId("txt_photos_menu_slideshow")

//context menu item for adding current image to album(s)
#define GLX_MENU_ADD_TO_ALBUM          hbTrId("txt_photos_menu_add_to_album")

//Item specific menu item. Note! Only use this text ID if there are no icons. 
//Erases something completely from the phone, list, or memory card, for example. 
//Delete should only be used when something in removed permanently.
#define GLX_MENU_DELETE                hbTrId("txt_common_menu_delete")

//
#define GLX_MENU_RENAME                hbTrId("txt_common_menu_rename_item")

#define GLX_MENU_OPEN                  hbTrId("txt_common_menu_open")

//Full Screen Menu
#define GLX_MENU_USE_IMAGE          hbTrId("txt_photos_opt_use_image")
#define GLX_MENU_SET_WALLPAPER      hbTrId("txt_photos_opt_sub_wallpaper")
#define GLX_MENU_ROTATE             hbTrId("txt_photos_opt_rotate")
#define GLX_MENU_90_CW              hbTrId("txt_photos_opt_sub_90cw")
#define GLX_MENU_90_CCW             hbTrId("txt_photos_opt_rotate_sub_90ccw")
#define GLX_MENU_CROP               hbTrId("txt_photos_opt_crop") 

//-----------------------------END OF MENU LOCALISATION STRING-------------------------------------------


//-------------------------------------------------------------------------------------------------------//
//-----------------------------VIEW SPECIFIC LOCALISATION STRING-----------------------------------------
//-------------------------------------------------------------------------------------------------------//

//*******************************GRID VIEW***********************************************

//sub heading lable for mark mode.
#define GLX_SELECT_IMAGES               hbTrId("txt_photos_subhead_select_images")

//for single item selection
#define GLX_SELECT_IMAGE                hbTrId("txt_photos_subhead_select_image")

//Label field in all grid view to display no of items plus album name in case of album grid view. whre %1 is album name and %L2 is no of images in the album.
#define GLX_ALBUM_NAME_COUNT_LABEL      "txt_photos_subhead_1_l2"

//to show the total image count in grid view
#define GLX_GRID_VIEW_COUNT_LABEL       "txt_photos_subhead_all_l2" 

//to show the total album count in Album list view
#define GLX_ALBUM_LIST_COUNT_LABEL      "txt_photos_subhead_albums_l2"

//mark all label to be shown in marking mode view
#define GLX_LABEL_MARK_ALL              hbTrId("txt_photos_list_mark_all")

//mark all label to be shown in marking mode view
#define GLX_LABEL_MARK_COUNT            "txt_photos_list_l1l2" 

#define GLX_GRID_NO_IMAGE               hbTrId("txt_photos_info_no_content")

//*******************************DETAILS VIEW**********************************************

//Name Field inside the text editor in the Details View
#define GLX_DETAILS_NAME              hbTrId("txt_photos_list_name")

//Description Field inside the text editor in the Details View
#define GLX_DETAILS_DESCRIPTION       hbTrId("txt_photos_list_description")

//Details view "Date" Label to show the captured date of the image, where %1 is the date
#define GLX_DETAILS_DATE               "txt_photos_list_date_1"

//Details view "time" Label to show the captured time of the image , where %1 is the time
#define GLX_DETAILS_TIME               "txt_photos_list_time_1"

//%ln is the size specfied in KiloBytes unit
//%ln is the size specfied in MegaBytes unit
//%ln is the size specfied in GigaBytes unit
#define GLX_DETAILS_SIZE_KB             "txt_photos_list_ln_kb"
#define GLX_DETAILS_SIZE_MB             "txt_photos_list_ln_mb"
#define GLX_DETAILS_SIZE_GB             "txt_photos_list_ln_gb" 
#define GLX_DETAILS_SIZE_BYTES          "txt_photos_list_sizeln_bytes"


//*******************************SLIDESHOW VIEW/SETTINGS VIEW*******************************

#define GLX_LABEL_TRANSITION_EFFECT  hbTrId("txt_photos_setlabel_transistion_effect")
#define GLX_VAL_SMOOTH_FADE          hbTrId("txt_photos_setlabel_transistion_effect_val_smooth")
#define GLX_EFFECTS_FLIP             hbTrId("txt_photos_setlabel_transistion_effect_val_flip_in")
#define GLX_EFFECTS_ZOOMIN_ZOOMOUT   hbTrId("txt_photos_setlabel_transistion_effect_val_zoom_in")
#define GLX_EFFECTS_ZOOM_AND_PAN     hbTrId("txt_photos_setlabel_transistion_effect_val_zoom_an")

#define GLX_LABEL_TRANSITION_DELAY   hbTrId("txt_photos_setlabel_transistion_delay")
#define GLX_VAL_SLOW                 hbTrId("txt_photos_setlabel_transistion_delay_val_slow")
#define GLX_VAL_MEDIUM               hbTrId("txt_photos_setlabel_transistion_delay_val_medium")
#define GLX_VAL_FAST                 hbTrId("txt_photos_setlabel_transistion_delay_val_fast")

#define GLX_UNABLE_PLAY_SLIDESHOW      hbTrId("txt_photos_info_unable_to_play_slideshow")
#define GLX_NOIMAGE_PLAY_SLIDESHOW     hbTrId( "txt_photos_info_no_images_to_play_slideshow" )

//-----------------------------END OF VIEW SPECIFIC LOCALISATION STRING----------------------------------


//-------------------------------------------------------------------------------------------------------//
//-----------------------------DIALOG/INFO LOCALISATION STRING-----------------------------------------
//-------------------------------------------------------------------------------------------------------//

//Wait note. Displayed when something is being deleted.
//Delete multiple images confirmation dialog
//Deleting...
//Delete selected items?
#define GLX_DELETE_PROGRESS              hbTrId("txt_common_info_deleting")

#define GLX_MULTIPLE_DELETE              hbTrId("txt_photos_info_delete_selected_items")

// progressing not to be shown while removing selected images from an album
#define GLX_REMOVE_IMAGES                hbTrId("txt_photos_info_removing_images")    

#define GLX_DIALOG_ALBUM_PROMPT           hbTrId("txt_photos_title_enter_name")

//Name Prompt
#define GLX_DIALOG_NAME_PROMPT            hbTrId("txt_photos_dialog_image_name")

//Description Prompt
#define GLX_DIALOG_DESCRIPTION            hbTrId("txt_photos_dialog_description")


#define GLX_DIALOG_ALBUM_NAME_DEFAULT    hbTrId("txt_photos_dialog_enter_name_entry_album")

#define GLX_DIALOG_DELETE_ONE_ITEM       hbTrId("txt_photos_info_delete_1")

//Name already in use alert when user tries to name/reanme the album with an already existing album name
#define GLX_NAME_ALREADY_EXIST           "txt_photos_info_name_1_already_in_use" 

#define GLX_PROCESSING                   hbTrId( "txt_common_info_processing" ) 

#define GLX_ADDING_IMAGES                hbTrId( "txt_photos_info_adding_images" )

#define GLX_IMAGES_ADDED                 "txt_photos_dpopinfo_images_added_to_1"

#define GLX_REFRESHING                   hbTrId("txt_photos_info_refreshing_your_media")

//favorite album name for select album popup
#define GLX_ALBUM_FAV                hbTrId("txt_photos_list_favorite")

//-----------------------------END OF DIALOG/INFO LOCALISATION STRING----------------------------------

//-------------------------------------------------------------------------------------------------------//
//-----------------------------BUTTON LOCALISATION STRING-----------------------------------------
//-------------------------------------------------------------------------------------------------------//

//Button. Note! Use this text ID only if there are max. two buttons in the dialog. 
//When this button is pressed, the focused item or operation will be selected or accepted.
#define GLX_BUTTON_OK                 hbTrId("txt_common_button_ok") 

//Button. Note! Use this text ID only if there are max. two buttons in the dialog. 
//Allows the user to cancel the action or process.
#define GLX_BUTTON_CANCEL            hbTrId("txt_common_button_cancel")

#define GLX_BUTTON_SELECT            hbTrId("txt_common_button_select")

#define GLX_BUTTON_HIDE              hbTrId("txt_common_button_hide")

#define GLX_BUTTON_NEW               hbTrId("txt_photos_button_new")

#define GLX_BUTTON_PLAY              hbTrId("txt_common_button_play_video")

#define GLX_BUTTON_ADD               hbTrId("txt_common_button_add") 

#define GLX_BUTTON_DELETE            hbTrId("txt_common_button_delete_toolbar") 

#define GLX_BUTTON_REMOVE            hbTrId("txt_common_button_remove") 

#define GLX_BUTTON_SHARE             hbTrId("txt_photos_button_share") 

//-----------------------------END OF BUTTON LOCALISATION STRING----------------------------------

//TITLE RELATED and DEFAULT STRINGS

#define GLX_CAPTION                    hbTrId("txt_short_caption_photos")
#define GLX_TITLE                      hbTrId("txt_photos_title_photos")
#define GLX_PLACES                     "txt_photos_dblist_places_ln"
#define GLX_ITEMS_CAMERA               "txt_photos_dblist_val_ln_items"
#define GLX_ITEMS_MYFAV                "txt_photos_subtitle_ln_items"
#define GLX_SUBTITLE_CAM_GRIDVIEW      hbTrId("txt_photos_subtitle_my_camera")
#define GLX_SUBTITLE_MYFAV_GRIDVIEW    hbTrId("txt_photos_subtitle_my_favorites")
#define GLX_ALBUM_SELECTION_TITLE      hbTrId("txt_photos_dialog_select_album") 
#define GLX_IMAGE_VIEWER               hbTrId("txt_photos_title_image_viewer")  

#endif /* GLXLOCALISATIONSTRINGS_H_ */
