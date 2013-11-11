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
 * Description: Search View Builder
 *
 */

#include <HbSearchPanel>
#include <HbView>
#include <HbListView>
#include <HbGroupBox>
#include "hssearchviewbuilder.h"

const char *searchDocmlFile           = ":/xml/searchview.docml";
const char *searchViewLabeledList     = "searchViewLabeledList";
const char *searchViewList            = "searchViewList";
const char *searchViewLabeledEmpty    = "searchViewLabeledEmpty";
const char *searchViewEmpty           = "searchViewEmpty";


/*!
    \class HsSearchViewBuilder
    \ingroup group_hsmenustateplugin

    \brief Search View Builder

    Reads UI object from Application Library Search docml

    \sa HsMenuView
*/

/*!
 Constructor. Loads UI objects from docml.
 */
HsSearchViewBuilder::HsSearchViewBuilder() : mLoader(new HbDocumentLoader()),
    mLabledContext(false)
{
    bool loadStatusOk(false);
    mLoadedObjects =
            mLoader->load(searchDocmlFile, &loadStatusOk).toSet();
    Q_ASSERT(loadStatusOk);
}

/*!
 Destructor. Deletes seto of loaded objects.
 */
HsSearchViewBuilder::~HsSearchViewBuilder()
{
    qDeleteAll(mLoadedObjects);
}

/*!
 \return Pointer to the view.
 Memory ownership is not changed.
 */
HbView *HsSearchViewBuilder::searchView()
{
    return qobject_cast<HbView *>(mLoader->findWidget("view"));
}

/*!
 \return Pointer to list view.
 The pointer is valid until the HsSearchViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbListView *HsSearchViewBuilder::searchListView()
{
    return qobject_cast<HbListView *>(mLoader->findWidget("listView"));
}

/*!
 \return Pointer to search panel.
 The pointer is valid until the HsSearchViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbSearchPanel *HsSearchViewBuilder::searchPanel()
{
    return qobject_cast<HbSearchPanel *>(mLoader->findWidget("searchPanel"));
}

/*!
 \return Pointer to the view label.
 The pointer is valid until the HsSearchViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbGroupBox *HsSearchViewBuilder::searchViewLabel()
{
    return qobject_cast<HbGroupBox *>(mLoader->findWidget("label"));
}

/*!
 Switches HsSearchViewBuilder to labeled view layout.
 */
void HsSearchViewBuilder::setSearchLabledContext()
{
    mLabledContext = true;
}

/*!
 \return true if section was succesfull loaded.
 Loads proper empty docml section.
 */
bool HsSearchViewBuilder::loadViewEmptySection()
{
    bool loadStatusOk(false);
    if(mLabledContext) {
        mLoadedObjects |= mLoader->load(
                searchDocmlFile,
                searchViewLabeledEmpty,
                &loadStatusOk).toSet();
    } else {
        mLoadedObjects |= mLoader->load(
                searchDocmlFile,
                searchViewEmpty,
                &loadStatusOk).toSet();
    }
    Q_ASSERT(loadStatusOk);
    return loadStatusOk;
}

/*!
 \return true if section was succesfull loaded.
 Loads proper list docml section.
 */
bool HsSearchViewBuilder::loadViewListSection()
{
    bool loadStatusOk(false);
    if(mLabledContext) {
        mLoadedObjects |= mLoader->load(
                searchDocmlFile,
                searchViewLabeledList,
                &loadStatusOk).toSet();
    } else {
        mLoadedObjects |= mLoader->load(
                searchDocmlFile,
                searchViewList,
                &loadStatusOk).toSet();
    }
    Q_ASSERT(loadStatusOk);
    return loadStatusOk;
}
