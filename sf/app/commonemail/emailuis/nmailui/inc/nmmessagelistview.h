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

#ifndef NMMESSAGELISTVIEW_H_
#define NMMESSAGELISTVIEW_H_

#include "nmbaseview.h"
#include <nmactionobserver.h>
#include <QModelIndex>

class HbTreeView;
class HbLabel;
class HbMenu;
class NmUiEngine;
class HbDocumentLoader;
class NmApplication;
class NmUiStartParam;
class NmMessageListModel;
class NmMailboxListModel;
class HbAbstractViewItem;
class NmAction;
class NmActionResponse;
class NmMessageListModelItem;
class HbIconItem;
class HbGroupBox;

class NmMessageListView : public NmBaseView, public NmActionObserver
{
    Q_OBJECT
public:
    NmMessageListView(
        NmApplication &application,
        NmUiStartParam *startParam,
        NmUiEngine &uiEngine,
        NmMailboxListModel &mailboxListModel,
        NmMessageListModel *messageListModel,
        HbDocumentLoader *documentLoader,
        QGraphicsItem *parent = 0);
    ~NmMessageListView();
    NmUiViewId nmailViewId() const;
    void viewReady();
    NmFolderType folderType();
    void okToExitView();
    
public: // From NmActionObserver
    void handleActionCommand(NmActionResponse &menuResponse);

public slots:
    void reloadViewContents(NmUiStartParam *startParam);
    void refreshList();
    
private slots:
    void showItemContextMenu(HbAbstractViewItem *index, const QPointF &coords);
    void itemActivated(const QModelIndex &index);
    void handleSelection();
    void createOptionsMenu();
    void itemsAdded(const QModelIndex &parent, int start, int end);
    void itemsRemoved();
    void showNoMessagesText();
    void hideNoMessagesText();
    void handleSyncStateEvent(NmSyncState syncState, const NmId & mailboxId);
    void folderSelected();
    
private:
    void loadViewLayout();
    void initTreeView();
    void setMailboxName();
    void createToolBar();
    void setFolderName();
    void updateSyncIcon();
    bool eventFilter(QObject *obj, QEvent *event);

private:
    NmApplication &mApplication;
    HbTreeView *mMessageListWidget;         // Not owned
    QObjectList mWidgetList;
    NmUiEngine &mUiEngine;
    NmMailboxListModel &mMailboxListModel;
    NmMessageListModel *mMessageListModel;  // Not owned
    HbDocumentLoader *mDocumentLoader;      // Owned
    HbMenu *mItemContextMenu;               // Owned
    NmMessageListModelItem *mLongPressedItem;  // Not owned
    HbLabel *mNoMessagesLabel;              // Not owned
    HbGroupBox *mFolderLabel;               // Not owned
    QModelIndex mActivatedIndex;
    bool mViewReady;
    NmFolderType mCurrentFolderType;
    bool mSettingsLaunched;
    NmId mSelectedFolderId;
    NmId mSelectedMailboxId;
    int mPreviousModelCount;
    bool mIsFirstSyncInMessageList;
    QList<NmAction*> mOptionsMenulist;
};

#endif /* NMMESSAGELISTVIEW_H_ */
