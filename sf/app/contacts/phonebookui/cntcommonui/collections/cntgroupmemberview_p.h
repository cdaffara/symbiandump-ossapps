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

#ifndef CNTGROUPMEMBERVIEW_P_H
#define CNTGROUPMEMBERVIEW_P_H

#include "cntgroupmemberview.h"
#include "cntglobal.h"

#include <QObject>
#include <QList>
#include <hbdocumentloader.h>
#include <QSet>
#include <cntabstractview.h>

class CntListModel;
class CntContactCardHeadingItem;
class HbView;
class HbAction;
class HbListView;
class HbAbstractViewItem;
class QModelIndex;
class CntImageLabel;
class HbDocumentLoader;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupMemberViewPrivate : public QObject
{
    Q_OBJECT

public:
    CntGroupMemberViewPrivate();
    ~CntGroupMemberViewPrivate();
    
    void activate( const CntViewParameters aArgs );
    void deactivate();
    
public:
    CntGroupMemberView* q_ptr;
    
private slots:
    void showPreviousView();
    void openGroupActions();
    void manageMembers();
    void handleManageMembers(QSet<QContactLocalId> aIds);
    void editGroup();
    void deleteGroup();
    void handleDeleteGroup(int action); // Delete action
    
    void showContextMenu(HbAbstractViewItem *item, const QPointF &coords); // item owned by HbListView
    void handleMenu(HbAction* action);

    void showContactView(const QModelIndex &index);
    void removeFromGroup(const QModelIndex &index);
    void editContact(const QModelIndex &index);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void openImageEditor();
    void setOrientation(Qt::Orientation orientation);
    
    void drawImageMenu(const QPointF &aCoords);
    void createModel();
  
    void removeImage();
    void sendToHs(const QModelIndex &index);
   
private:
    QContactManager* getContactManager();
    void setRelationship(QSet<QContactLocalId>        &aLocalId,
                         QList<QContactRelationship>  &aRelationshipList);

    void decideActionButtonContext();
    
private:
    Q_DECLARE_PUBLIC(CntGroupMemberView)
    friend class TestCntGroupMemberView;
    
public:
    CntAbstractEngine*          mEngine;
private:
    QContact*                   mGroupContact; // own
    CntAbstractViewManager*     mViewManager;
    HbDocumentLoader            mDocumentLoader;
    HbView*                     mView; // own
    HbAction*                   mSoftkey; // owned by view
    CntContactCardHeadingItem*  mHeadingItem; // owned by layout
    CntThumbnailManager*        mThumbnailManager; // own
    HbAction*                   mManageAction; // owned by view
    HbAction*                   mDeleteAction; // owned by view
    HbAction*                   mShowActionsAction; // owned by view
    HbAction*                   mEditGroupAction; // owned by view
    CntListModel*               mModel; // own
    CntImageLabel*              mImageLabel;
    HbListView*                 mListView; // owned by layout
    HbDocumentLoader*           mDocument;
    QList<QContactLocalId>      mOriginalGroupMembers;
    QContactAvatar*             mAvatar;
    CntViewParameters           mArgs;
};

#endif // CNTGROUPMEMBERVIEW_P_H

// EOF
