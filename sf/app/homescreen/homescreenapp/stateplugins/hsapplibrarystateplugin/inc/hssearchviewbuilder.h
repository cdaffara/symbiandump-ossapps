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

#ifndef HSSEARCHVIEWBUILDER_H_
#define HSSEARCHVIEWBUILDER_H_

#include <HbDocumentLoader>
#include "hsmenustates_global.h"

class HbSearchPanel;
class HbView;
class HbListView;
class HbGroupBox;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsSearchViewBuilder
{

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    HsSearchViewBuilder();
    virtual ~HsSearchViewBuilder();

    HbView *searchView();
    HbListView *searchListView();
    HbSearchPanel *searchPanel();
    HbGroupBox *searchViewLabel();
    void setSearchLabledContext();
    bool loadViewEmptySection();
    bool loadViewListSection();

private:
    QSet<QObject *> mLoadedObjects;
    QSharedPointer<HbDocumentLoader> mLoader;
    bool mLabledContext;
};

#endif /* HSSEARCHVIEWBUILDER_H_ */
