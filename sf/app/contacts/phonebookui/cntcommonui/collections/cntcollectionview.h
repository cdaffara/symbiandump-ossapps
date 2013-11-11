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

#ifndef CNTCOLLECTIONVIEW_H
#define CNTCOLLECTIONVIEW_H

#include <QObject>
#include <QSet>
#include <hbdocumentloader.h>

#include <cntabstractview.h>
#include <cntextensiongroupcallback.h>

class HbAction;
class HbView;
class HbListView;
class CntCollectionListModel;
class QModelIndex;
class QActionGroup;
class HbAbstractViewItem;

QTM_USE_NAMESPACE

class CntCollectionView : public QObject, public CntAbstractView, public CntExtensionGroupCallback
{
    Q_OBJECT
    friend class TestCntCollectionView;
    
public:
    CntCollectionView();
    ~CntCollectionView();
    
public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return collectionView; }
    inline void setEngine( CntAbstractEngine& aEngine ){mEngine = &aEngine;}
    
public: // From CntExtensionGroupCallback
    void openView(CntViewParameters& viewParams);

private slots:
    void showPreviousView();
    void showNamesViewWithFinder();
    void openGroup(const QModelIndex &index);
    
    void showContextMenu(HbAbstractViewItem *item, const QPointF &coords);
    void handleMenu(HbAction* action);
    
    void handleExtensionAction();
    
    void newGroup();
    void handleNewGroup(HbAction* action);
    void handleNewGroupMembers( QSet<QContactLocalId> aMembers );
    void handleCancelGroupMembers();
    void notifyNewGroup();
    
    void refreshDataModel();
    
    void deleteGroup(QContact group);
    void handleDeleteGroup(int action);
    
    void deleteGroups();
    void handleDeleteGroups(HbAction* action);
    
    void groupCountChanged();
    
private:
    QContactManager* getContactManager();
    void saveNewGroup(QContact* aContact);
    
    
private:
    CntExtensionManager*    mExtensionManager;
    HbView*                 mView; // own
    HbAction*               mSoftkey; // owned by view
    CntAbstractViewManager* mViewManager;
    HbDocumentLoader        mDocumentLoader;
    CntCollectionListModel* mModel; // own
    HbListView*             mListView; // owned by layout
    HbAction*               mNamesAction; // owned by view
    HbAction*               mFindAction; // owned by view
    HbAction*               mExtensionAction; // owned by view
    HbAction*               mNewGroupAction; // owned by view
    HbAction*               mDeleteGroupsAction; // owned by view
    
    QContact*               mHandledContact; // own, needed for asynchronous popups
    QSet<QContactLocalId>   mSelectedContactsSet;
    
    QActionGroup*           mActionGroup;
    CntAbstractEngine* mEngine;
};

#endif // CNTCOLLECTIONVIEW_H

// EOF
