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

#ifndef HSMENUVIEWBUILDER_H
#define HSMENUVIEWBUILDER_H

#include <QFlags>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QSharedPointer>
#include <QString>
#include <qnamespace.h>
#include <HbDocumentLoader>
#include "hsmenustates_global.h"

class QActionGroup;
class HbAction;
class HbGroupBox;
class HbLineEdit;
class HbListView;
class HbToolBar;
class HbView;
class HbWidget;
class HbToolBarExtension;

enum HsStateContext {
    HsAllAppsContext = 0,
    HsAllCollectionsContext,
    HsInstalledAppsContext,
    HsCollectionContext,
    InvalidStateContext
};

enum HsOperationalContext {
    HsItemViewContext = 0,
    HsEmptyLabelContext,
    InvalidOperationalContext
};

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuViewBuilder
{
public:
    HsMenuViewBuilder();
    ~HsMenuViewBuilder();

    // mandatory context independent widgets accessors
    HbAction *allAppsAction() const;
    HbAction *allCollectionsAction() const;
    HbAction *searchAction() const;
    HbAction *oviStoreAction() const;
    HbAction *operatorAction() const;
    HbToolBar *toolBar() const;
    QActionGroup *toolBarActionGroup() const;
    HbToolBarExtension *toolBarExtension() const;
    HbAction *toolBarExtensionAction() const;

    // mandatory context dependent widgets accessors
    HbView *currentView();
    HbListView *currentListView();


    // optional widgets accessors
    HbGroupBox *currentViewLabel();

    void setStateContext(HsStateContext stateContext);
    void setOperationalContext(HsOperationalContext operationalContext);

private:
    typedef QPair<HsStateContext, HsOperationalContext> Context;
    typedef QMap<Context, QSharedPointer<HbDocumentLoader> > LoaderMap;

    QSharedPointer<HbDocumentLoader>
            parseDocument(const QString &documentBaseName);

    QSharedPointer<HbDocumentLoader> readContextConfiguration();

    Context context() const;

    QSharedPointer<HbDocumentLoader> currentLoader();


    QSet<QObject *> mLoadedObjects;


    const QString ALL_APPS_ACTION_NAME;
    const QString ALL_COLLECTIONS_ACTION_NAME;
    const QString SEARCH_ACTION_NAME;
    const QString OVI_STORE_ACTION_NAME;
    const QString OPERATOR_ACTION_NAME;

    QActionGroup *mToolBarActionGroup;
    HbToolBar *mToolBar;
    HbToolBarExtension *mToolBarExtension;
    HbAction *mToolBarExtensionAction;

    HsStateContext mStateContext;
    HsOperationalContext mOperationalContext;

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

    LoaderMap mLoaderMap;
    QSharedPointer<HbDocumentLoader> mCommonObjectsLoader;
};

#endif // HSMENUVIEWBUILDER_H
