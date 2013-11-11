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
 * Description: Menu View Builder
 *
 */

#include <QDebug>
#include <QActionGroup>
#include <QGraphicsWidget>
#include <HbAction>
#include <HbGroupBox>
#include <HbListView>
#include <HbToolBar>
#include <HbView>
#include <HbWidget>
#include <HbStaticVkbHost>
#include <HbMainWindow>
#include <HbInputMethod>
#include <HbToolBarExtension>

#include "hsmenuviewbuilder.h"
#include "hsmenustates_global.h"

static const char* DOCUMENT_BASE_NAME_MAP
        [InvalidOperationalContext] =
                            /*HsItemViewContext,    HsEmptyLabelContext*/
{
     "labeledlistview",     "emptylabeledview"
};

static const QString DOCUMENT_NAME_PREFIX(QLatin1String(":/xml/"));
static const QString DOCUMENT_NAME_EXT(QLatin1String(".docml"));
static const QString COMMON_OBJECTS_DOCUMENT_BASE_NAME(
        QLatin1String("common_objects"));
static const QString VIEW_NAME(QLatin1String("view"));
static const QString LIST_VIEW_NAME(QLatin1String("listView"));
static const QString VIEW_LABEL_NAME(QLatin1String("label"));

/*!
    \class HsMenuViewBuilder
    \ingroup group_hsmenustateplugin

    \brief Menu View Builder

    Reads UI object from Application Library docml

    \sa HsMenuView
*/




/*!
 \return Pointer to the view resulting from last \a build call or NULL if
 the \a build has not yet been called.
 Memory ownership is not changed.
 */
HbView *HsMenuViewBuilder::currentView()
{
    HbView *const view =
        qobject_cast<HbView *>(currentLoader()->findWidget(VIEW_NAME));

    if (view != NULL && mStateContext != HsInstalledAppsContext) {
        view->setToolBar(mToolBar);
    }
    return view;
}

/*!
 \return Pointer to list view resulting from last \a build call or NULL if
 the \a build has not yet been called.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbListView *HsMenuViewBuilder::currentListView()
{
    HbListView *const listView =
        qobject_cast<HbListView *>(
                currentLoader()->findWidget(LIST_VIEW_NAME));

    return listView;
}

/*!
 \return Pointer to the view label resulting from last \a build call. It is
 guaranteed to be not NULL if the \a build was called for the context
 related to view including label.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbGroupBox *HsMenuViewBuilder::currentViewLabel()
{
    HbGroupBox *viewLabel =
            qobject_cast<HbGroupBox *>(
                    currentLoader()->findWidget(VIEW_LABEL_NAME));

    return viewLabel;
}

/*!
 \return Pointer to All Applications Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allAppsAction() const
{
    return qobject_cast<HbAction *>(mCommonObjectsLoader->findObject(
                                        ALL_APPS_ACTION_NAME));
}

/*!
 \return Pointer to All Collections Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allCollectionsAction() const
{
    return qobject_cast<HbAction *>(mCommonObjectsLoader->findObject(
                                        ALL_COLLECTIONS_ACTION_NAME));
}

/*!
 \return Pointer to Search Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::searchAction() const
{
    return qobject_cast<HbAction *>(mCommonObjectsLoader->findObject(
                                        SEARCH_ACTION_NAME));
}

/*!
 \return Pointer to Ovi Store Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::oviStoreAction() const
{
    return qobject_cast<HbAction *>(mCommonObjectsLoader->findObject(
                                        OVI_STORE_ACTION_NAME));
}

/*!
 \return Pointer to Operator Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::operatorAction() const
{
    return qobject_cast<HbAction *>(mCommonObjectsLoader->findObject(
                                        OPERATOR_ACTION_NAME));
}

/*!
 \return Pointer to the main view toolbar.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbToolBar *HsMenuViewBuilder::toolBar() const
{
    return mToolBar;
}

/*!
 \return Pointer to the toolbar extension.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbToolBarExtension *HsMenuViewBuilder::toolBarExtension() const
{
    return mToolBarExtension;
}

/*!
 \return Pointer to the toolbar extension action.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::toolBarExtensionAction() const
{
    return mToolBarExtensionAction;
}

/*!
 \return Action group for \a allAppsState and \a allCollectionsState action.
 */
QActionGroup *HsMenuViewBuilder::toolBarActionGroup() const
{
    return mToolBarActionGroup;
}

/*!
 Reads docml configuration corresponding to current context.
 \return Shared pointer to loader serving current context.
 */
QSharedPointer<HbDocumentLoader> HsMenuViewBuilder::currentLoader()
{
    if (!mLoaderMap.contains(context())) {
        QSharedPointer<HbDocumentLoader> loader =
                readContextConfiguration();
        mLoaderMap.insert(context(), loader);
    }

    return mLoaderMap[context()];
}


/*!
  Loads non-context sensitive objects.
 */
HsMenuViewBuilder::HsMenuViewBuilder():
    ALL_APPS_ACTION_NAME("allAppsAction"),
    ALL_COLLECTIONS_ACTION_NAME("allCollectionsAction"),
    SEARCH_ACTION_NAME("searchAction"),
    OVI_STORE_ACTION_NAME("oviStoreAction"),
    OPERATOR_ACTION_NAME("operatorAction"),
    mToolBar(new HbToolBar),
    mToolBarExtension(new HbToolBarExtension),
    mStateContext(HsAllAppsContext),
    mOperationalContext(HsItemViewContext)
{
    mCommonObjectsLoader = parseDocument(COMMON_OBJECTS_DOCUMENT_BASE_NAME);

    mToolBar->addAction(allAppsAction());
    mToolBar->addAction(allCollectionsAction());
    mToolBar->addAction(searchAction());
    mToolBarExtensionAction = mToolBar->addExtension(mToolBarExtension);
    mToolBarExtensionAction->setIcon(HbIcon("qtg_mono_store"));
    mToolBarExtensionAction->setVisible(false);

    mToolBarActionGroup = new QActionGroup(allAppsAction());

    allAppsAction()->setActionGroup(mToolBarActionGroup);
    allCollectionsAction()->setActionGroup(mToolBarActionGroup);
}

/*!
 Deletes loaded objects.
 */
HsMenuViewBuilder::~HsMenuViewBuilder()
{
    delete mToolBarExtension;
    delete mToolBar;
    qDeleteAll(mLoadedObjects);
}

/*!
 Parses requested docml file section and reflects its contents in the
 object state.
 \param documentName Identifies document to parse.
 \return Document loader instance containing parsed
    objects for requested \documentName.
 */
QSharedPointer<HbDocumentLoader> HsMenuViewBuilder::parseDocument(
        const QString &documentBaseName)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuViewBuilder::parseDocument");

    bool loadStatusOk = false;

    QSharedPointer<HbDocumentLoader> loader(new HbDocumentLoader());

    const QString documentFullName = DOCUMENT_NAME_PREFIX +
                                     documentBaseName +
                                     DOCUMENT_NAME_EXT;
    const QObjectList loadedObjects =
            loader->load(documentFullName, &loadStatusOk);

    mLoadedObjects |= loadedObjects.toSet();

    Q_ASSERT_X(loadStatusOk,
               documentFullName.toLatin1().data(),
               "Error while loading docml file.");

    HSMENUTEST_FUNC_EXIT("HsMenuViewBuilder::parseDocument");

    return loader;
}

/*!
 Sets view context. Not reflected in widgets returned by the builder
 until \a build is not run.
 */
void HsMenuViewBuilder::setStateContext(HsStateContext stateContext)
{
    mStateContext = stateContext;
}

/*!
 Sets operational context. Not reflected in widgets returned by the builder
 until \a build is not run.
 */
void HsMenuViewBuilder::setOperationalContext(
    HsOperationalContext operationalContext)
{
    mOperationalContext = operationalContext;
}

/*!
 Reads configuration for requested context and ensures search panel and corresponding
 vkb host are managed properly.
 \return Document loader with objects for current context.
 */
QSharedPointer<HbDocumentLoader> HsMenuViewBuilder::readContextConfiguration()
{
    const QLatin1String documentName(
            DOCUMENT_BASE_NAME_MAP[mOperationalContext]);

    QSharedPointer<HbDocumentLoader> loader =
            parseDocument(QString(documentName));

    return loader;
}

/*!
 \return Current context.
 */
HsMenuViewBuilder::Context HsMenuViewBuilder::context() const
{
    return Context(mStateContext, mOperationalContext);
}
