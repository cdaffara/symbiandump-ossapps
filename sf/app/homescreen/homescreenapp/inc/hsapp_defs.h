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
 * Description: declaration of functions providing string constants
 *
 */

#ifndef HSAPP_DEFS_H
#define HSAPP_DEFS_H

namespace Hs {
    const char widgetPathAttributeName[] = "widget:path";
    const char widgetLibraryAttributeName[] = "widget:library";
    const char widgetUriAttributeName[] = "widget:uri";
    const char applicationTypeName[] = "application";
    const char urlTypeName[] = "url";
    const char collectionTypeName[] = "collection";
    const char collectionDownloadedTypeName[] = "collection::downloaded";
    const char menuCollectionsTypeName[] = "menucollections";
    const char widgetTypeName[] = "widget";
    const char templatedApplicationTypeName[] = "templatedApplication";
    const char widgetParam[] = "widgetparam:";
    const char openActionIdentifier[] = "open";
    const char removeActionIdentifier[] = "remove";
    const char itemIdKey[] = "item_id";
    const char collectionIdKey[] = "collection_id";
    const char collectionSortOrderKey[] = "collection_sort_order";
    const char appSortOrderKey[] = "app_sort_order";
    const char entryTypeNameKey[] = "entry_type_name";
    const char applicationUidEntryKey[] = "application:uid";
    const char urlEntryKey[] = "url";
    const char defaultCollectionIconId[] = "qtg_large_applications_user";
    const char newIconId[] = "qtg_small_new_event";
    const char menuModeType[] = "menu_mode_type";
    const char swTypeKey[] = "apptype";
    const char javaSwType[] = "java";
    const char uninstallError[] = "uninstall_error";
    static const char *const HS_CWRT_APP_TYPE="cwrt";
    static const char *const HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE=
        "wrtwidgetuiplugin";
    const char packageTypeName[] = "package";
    const char appSettingsPlugin[] = "app_settings_plugin";
    const char groupNameAttributeName[] = "appgroup_name";
    const char groupAppLibRecentView[] = "AppLibRecentView";
    const char componentIdAttributeName[] = "component_id";
    const char entryShortName[] = "short_name";
    
    const char translationFilename[] = "widget:translation_file";
    const char activityHsIdleView[] = "HsIdleView";
    const char activityAppLibMainView[] = "AppLibMainView";
    

    const char tsDeviceDialogUri[] = 
        "com.nokia.taskswitcher.tsdevicedialogplugin/1.0";

    const char library[] = "library";
    const char uri[] = "uri";
    const char preferences[] = "preferences";
    const char homescreenData[] = "homescreenData";

    const char snapEnabled[] = "snapenabled";
    const char snapForce[] = "snapforce";
    const char snapGap[] = "snapgap";
    const char pageMargin[] = "pagemargin";
    const char snapLineFadeinDuration[] = "snaplinefadeinduration";
    const char snapLineFadeoutDuration[] = "snaplinefadeoutduration";

    const int softwareUpdateApplicationUid  = 0x2001FE2F;
    const int oviLauncherApplicationUid  = 0x2002D07F;
    
    const char drivesIconIdsAttributeName[] = "drivesIconIds";

    // Sort attribute
    enum HsSortAttribute {
        NoHsSortAttribute,
        AscendingNameHsSortAttribute,
        DescendingNameHsSortAttribute,
        LatestOnTopHsSortAttribute,
        OldestOnTopHsSortAttribute,
        CustomHsSortAttribute
    };
    
    enum HsMenuMode {
        NormalHsMenuMode,
        AddHsMenuMode
    };
    
    enum HsContextAction {
        AddToHomeScreenContextAction = 1,
        AddToCollectionFromApplicationsViewContextAction,
        AddToCollectionFromCollectionViewContextAction,
        UninstallContextAction, 
        AppSettingContextAction,
        RemoveFromCollectionContextAction,
        AppDetailsContextAction,
        OpenAppContextAction,
        OpenCollectionContextAction,
        DeleteCollectionContextAction,
        RenameCollectionContextAction
    };

} // namespace Hs

#endif // HSAPP_DEFS_H
