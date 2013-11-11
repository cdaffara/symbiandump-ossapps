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

#ifndef NMMAILBOXLISTVIEW_H_
#define NMMAILBOXLISTVIEW_H_

#include "nmbaseview.h"
#include <nmactionobserver.h>
#include <QModelIndex>

class HbListView;
class HbDocumentLoader;
class NmUiEngine; 
class NmApplication;
class NmUiStartParam;
class NmMailboxListModel;
class NmActionResponse;
class HbMenu;
class HbAbstractViewItem;
class NmAction;

class NmMailboxListView : public NmBaseView
{
    Q_OBJECT
public:
    NmMailboxListView(
        NmApplication &application,
        NmUiStartParam* startParam,
        NmUiEngine &uiEngine,
        NmMailboxListModel &mailboxListModel,
        HbDocumentLoader *documentLoader,
        QGraphicsItem *parent = 0);
    ~NmMailboxListView();
    void reloadViewContents(NmUiStartParam* startParam);
    NmUiViewId nmailViewId() const;
    void viewReady();

public slots:
    void itemActivated(const QModelIndex &index);
    void openSelectedMailBox();

private slots:
    void refreshList();

private:
    void loadViewLayout();

private:
    NmApplication &mApplication;
    QObjectList mWidgetList;
    HbListView *mMailboxListWidget;     // Not owned
    NmUiEngine &mUiEngine;              // Not owned
    NmMailboxListModel &mListModel;     // Not owned
    HbMenu *mItemContextMenu;           // Owned
    HbDocumentLoader *mDocumentLoader;  // Owned
    QModelIndex mActivatedIndex;
    bool mViewReady;
};

#endif /* NMMAILBOXLISTVIEW_H_ */
