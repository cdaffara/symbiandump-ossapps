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

#ifndef HSMENUSTATES_GLOBAL_H
#define HSMENUSTATES_GLOBAL_H

#include <QDebug>
#include <QtGlobal>
#include "hsmenuservice_global.h"

#ifndef MENUSTATES_UNITTEST
#define HS_STATES_TEST_CLASS(className)
#else
#define HS_STATES_TEST_CLASS(className) class className;
#endif

#ifndef MENUSTATES_UNITTEST
#define HS_STATES_TEST_FRIEND_CLASS(className)
#else
#define HS_STATES_TEST_FRIEND_CLASS(className) friend class className;
#endif

static const char *const HS_APP_CHECK_LIST_LAYOUT =
    ":/xml/hsappchecklist.docml";
static const char *const HS_WIDGET_PREVIEW_DIALOG_LAYOUT =
    ":/xml/hswidgetpreviewdialog.docml";
static const char *const HS_WIDGET_PREVIEW_DIALOG_NAME="preview_dialog";
static const char *const HS_WIDGET_PREVIEW_LABEL_NAME="label";
static const char *const HS_WIDGET_PREVIEW_ICON_BOX_NAME="icon_box";
static const char *const HS_PREVIEW_ATTRIBUTE_NAME="preview_image_name";

static const char *const HS_ARRANGE_DIALOG_LAYOUT =
    ":/xml/hsarrangedialog.docml";
static const char *const HS_ARRANGE_DIALOG_NAME="arrange_dialog";
static const char *const HS_ARRANGE_LIST_NAME="arrange_list";

static const char *const HS_DETAILS_DIALOG_LAYOUT =
    ":/xml/hsdetailsdialog.docml";
static const char *const HS_DETAILS_DIALOG_NAME="details_dialog";

static const char *const HS_UNINSTALL_DIALOG_LAYOUT =
    ":/xml/hsuninstalldialog.docml";
static const char *const HS_UNINSTALL_DIALOG_NAME ="uninstall_dialog";
static const char *const HS_UNINSTALL_DIALOG_DELETE_LABEL ="delete_message_label";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_LABEL ="confirmation_dialog_label";
static const char *const HS_UNINSTALL_DIALOG_LIST_VIEW_LABEL ="list_view_label";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_ACTION ="qtl_dialog_softkey_2_left";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_ICON ="icon";

static const char *const HS_INSTALLATION_LOG_DIALOG_LAYOUT =":/xml/hsinstallationlogdialog.docml";
static const char *const HS_INSTALLATION_LOG_DIALOG_NAME="installation_log_dialog";
static const char *const HS_INSTALLATION_LOG_DIALOG_LOG_LABEL="log_label";

static const char *const HS_COLLECTION_DIALOG_LAYOUT =":/xml/hscollectionslistdialog.docml";
static const char *const HS_COLLECTION_DIALOG_NAME="collection_list_dialog";
static const char *const HS_COLLECTION_DIALOG_LIST_VIEW="listView";
static const char *const HS_COLLECTION_DIALOG_CREATE_NEW_ACTION="qtl_dialog_softkey_2_left";
static const char *const HS_COLLECTION_DIALOG_CANCEL_ACTION="qtl_dialog_softkey_2_right";

#endif //HSMENUSTATES_GLOBAL_H
