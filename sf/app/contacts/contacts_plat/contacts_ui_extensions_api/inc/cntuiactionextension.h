/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Tags for contact card UI control. Use these in mobility plugin metaData() to specify 
 *              which action details UI should show.
 *
 */

#ifndef CNTUIEXTENSIONGROUP_H
#define CNTUIEXTENSIONGROUP_H

/*
 * Tags for QContactAction metaData call. Tags describe which UI element they are affecting to.
 * Action can be visualised with UI item that has three customised areas: icon, title text and value text.
 * Icon reserves small space from left edge of item, title text fills top part of free space from item and 
 * value text fills lower part of free space from item.
 * 
 * You can create generic contact or my card action(detail independent) by checking if
 * QContactDetail in isDetailSupported is null. These items are created only once for view.
 * You cannot use KCntUiActionMetaTitleTextDetail or KCntUiActionMetaValueTextDetail with this
 * action UI item, since it's detail independent.
 * 
 * If you want to create action only to "My card" view, you have to create QContactManager instance and
 * and compare its "cm->selfContactId()" to local id of QContact parameter in isDetailSupported 
 * (contact.localId()). You cannot use KCntUiActionMetaTitleTextDetail or KCntUiActionMetaValueTextDetail
 * with this action UI item, since it's detail independent.
 */

// Default Symbian database, use this as uri parameter for QContactManager::fromUri.
const QString ACTION_EXT_SYMBIAN_BACKEND = "qtcontacts:symbian:";

// First line in UI item
// This tag is used to specify localized freetext(QString) description for action, eg. "Send message". This
// or KCntUiActionMetaTitleTextDetail tag must be specified if UI should show action. If both set
// this is used.
const QString KCntUiActionMetaTitleText = "titleText";

// First line in UI item
// This tag is used to specify data field to be shown from detail, eg. QContactPhoneNumber::FieldNumber. 
// This or KCntUiActionMetaTitleText tag must be specified if UI should show action. If both set
// KCntUiActionMetaTitleText is used.
// Note: Does not work with general contact actions(isDetailSupported has empty detail parameter)
const QString KCntUiActionMetaTitleTextDetail = "titleTextDetail";


// Second line in UI item
// This tag is used to specify which field to show from detail in UI, eg. QContactPhoneNumber::FieldNumber.
// UI can show either this or KCntUiActionMetaValueText tag. If both set this is used.
// Note: Does not work with general contact actions(isDetailSupported has empty detail parameter)
const QString KCntUiActionMetaValueTextDetail = "valueTextDetail";

// Second line in UI item
// This tag can be used to specify localized freetext(QString) to UI.
// UI can show either this or KCntUiActionMetaValueTextDetail tag. If both set KCntUiActionMetaValueTextDetail
// is used.
const QString KCntUiActionMetaValueText = "valueText";

// Text for long press menu in UI
// This tag can be used to specify localized freetext(QString) to UI.
const QString KCntUiActionMetaValueTextLongPress = "longPressText";


// This tag is used to specify custom icon for action
// Icon must be QIcon.
// TODO Supported icon dimension?
const QString KCntUiActionMetaIcon = "icon";

#endif
