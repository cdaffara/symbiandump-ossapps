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

#ifndef CNTVIEWPARAMS_H
#define CNTVIEWPARAMS_H

#include <QMap>
#include <QVariant>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

enum CntViewParamId
{
    EViewId = 0,
    ESelectedAction,
    ESelectedContact,
    ESelectedGroupContact,
    ESelectedDetail,
    ESelectionMode,
    EMyCard,
    EExtraAction,
	ECustomParam = 100
};

enum CntViewId
{
    noView=0, //exit
    namesView,
    contactCardView,
    serviceContactCardView,
    serviceAssignContactCardView,
    myCardView,
    myCardSelectionView,
    serviceContactSelectionView,
    collectionView,
    collectionFavoritesView,
    favoritesMemberView,
    editView,
    serviceEditView,
    serviceSubEditView,
    emailEditorView,
    namesEditorView,
    urlEditorView,
    companyEditorView,
    phoneNumberEditorView,
    onlineAccountEditorView,
    noteEditorView,
    familyDetailEditorView,
    addressEditorView,
    dateEditorView,
    imageEditorView,
    serviceContactFetchView,
    groupEditorView,
    groupMemberView,
    groupActionsView,
    historyView,
    importsView,
    settingsView,
    ringToneFetcherView,
    serviceGroupMemberView,
    customPhonebookView = 100, // reserved for internal view plugins, DO NOT USE THESE
    customView = 200 // for all other view plugins
};

typedef QMap<int, QVariant> CntViewParameters;

Q_DECLARE_METATYPE(QContact)
Q_DECLARE_METATYPE(QContactDetail)

#endif // CNTVIEWPARAMS_H
