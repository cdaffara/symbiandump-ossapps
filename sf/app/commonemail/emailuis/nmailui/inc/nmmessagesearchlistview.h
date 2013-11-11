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
* Description:
*
*/

#ifndef NMMESSAGESEARCHLISTVIEW_H_
#define NMMESSAGESEARCHLISTVIEW_H_

#include <QModelIndex>
#include <nmactionobserver.h>
#include "nmbaseview.h"


class HbAbstractViewItem;
class HbDocumentLoader;
class HbGroupBox;
class HbLabel;
class HbLineEdit;
class HbMenu;
class HbPushButton;
class HbTreeView;
class HbSearchPanel;
class HbIcon;
class HbShrinkingVkbHost;

class NmActionResponse;
class NmApplication;
class NmMessageListModel;
class NmMessageListModelItem;
class NmUiEngine;
class NmUiStartParam;


class NmMessageSearchListView : public NmBaseView, public NmActionObserver
{
    Q_OBJECT

public:

    NmMessageSearchListView(NmApplication &application,
                            NmUiStartParam *startParam,
                            NmUiEngine &uiEngine,
                            NmMessageListModel &msglistModel,
                            HbDocumentLoader *documentLoader,
                            QGraphicsItem *parent = 0);
    ~NmMessageSearchListView();


public: // From NmBaseView

    NmUiViewId nmailViewId() const;
    void aboutToExitView();
    void viewReady();

public: // From NmActionObserver

    void handleActionCommand(NmActionResponse &menuResponse);


private: // Data types

    enum NmSearchInputMode {
        NmNormalMode = 0,
        NmDimmedMode
    };

private:

    void loadViewLayout();
    void initTreeView();
    void setViewTitle();
    void noMessagesLabelVisibility(bool visible);
    void updateResultView();
    void setSearchInputMode(NmSearchInputMode mode);

public slots:

    void reloadViewContents(NmUiStartParam *startParam);


private slots:

    void showItemContextMenu(HbAbstractViewItem *index, const QPointF &coords);
    void itemActivated(const QModelIndex &index);
    void handleSelection();
    void itemsAdded(const QModelIndex &parent, int start, int end);
    void itemsRemoved();
    void refreshList();
    void toggleSearch();
    void handleSearchComplete();
    void textChanged(const QString &text);
    void exitClicked();
    void orientationAboutToChange();
    void orientationChanged();
    void sendSoftwareInputPanelRequest(QEvent::Type eventType = QEvent::RequestSoftwareInputPanel);
    void vkbOpened();
    void vkbClosed();
    void loadOrientationSection();

private: // Data

    NmApplication &mApplication;
    NmUiEngine &mUiEngine;
    NmMessageListModel &mMsgListModel;
    HbDocumentLoader *mDocumentLoader;          // Owned
    QObjectList mWidgetList;
    HbMenu *mItemContextMenu;                   // Owned
    HbTreeView *mMessageList;                   // Not owned
    HbLabel *mNoMessagesLabel;                  // Not owned
    HbSearchPanel *mSearchPanel;                // Not owned
    NmMessageListModelItem *mLongPressedItem;   // Not owned
    HbPushButton *mProgressButton;              // Not owned
    HbLineEdit *mLineEdit;                      // Not owned
	QModelIndex mActivatedIndex;
    bool mViewReady;
    bool mSearchInProgress;
    HbIcon *mSpinnerIcon;                       // Owned
    bool mAnimationAddedToManger;
    bool mSelectTextAfterOrientationChange;
    HbShrinkingVkbHost *mVkbHost;               // Owned
};


#endif /* NMMESSAGESEARCHLISTVIEW_H_ */

// End of file.
