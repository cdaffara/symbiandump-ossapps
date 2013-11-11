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

#ifndef CNTEDITVIEW_P_H_
#define CNTEDITVIEW_P_H_

#include <QObject>

#include "cnteditview.h"
#include "cntglobal.h"
#include <qtcontacts.h>
#include <cnteditviewitemcallback.h>
#include <xqappmgr.h>
#include <qabstractitemmodel.h>

class CntAbstractViewManager;
class CntEditViewListModel;
class CntEditViewHeadingItem;
class CntEditViewItem;
class CntImageLabel;

class CntThumbnailManager;
class QModelIndex;
class HbDocumentLoader;
class HbAbstractViewItem;
class HbListView;
class HbView;
class HbAction;
class QAction;
class HbMenu;
class XQAiwRequest;
class CntSaveManager;
class CntAbstractEngine;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
class QContactId;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class CntEditViewPrivate : public QObject, public CntEditViewItemCallback
{
    Q_OBJECT    
    
public:
    CntEditViewPrivate();
    ~CntEditViewPrivate();
    
    void activate( const CntViewParameters aArgs );
    void deactivate();
    
public: // From CntEditViewItemCallback
    void openView(CntViewParameters& viewParams);
    void requestRefresh();
    
public:
    CntEditView* q_ptr;
    
private slots:
    void showPreviousView();
    void activated( const QModelIndex& aIndex );
    void longPressed( HbAbstractViewItem *item, const QPointF &coords );
    
    void addDetailItem();
    void handleAddDetailItem(HbAction *aAction);
    void deleteContact();
    void handleDeleteContact(int action);
    void discardChanges();
    bool saveChanges();
    void showRootView();
    
    void openNameEditor();
    void openImageEditor();
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    void setOrientation(Qt::Orientation aOrientation);
    void handleMenuAction( HbAction* aAction );
    void setScrollPosition();
    void ringToneFetchHandleError(int errorCode, const QString& errorMessage);
    void ringToneFetchHandleOk(const QVariant &result);
    void contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds);

private:
    void loadAvatar();
    HbMenu* createPopup( const QModelIndex aIndex, CntEditViewItem* aDetail );
    void addDetail( CntEditViewItem* aDetail );
    void editDetail( CntEditViewItem* aDetail );
    void removeDetail( CntEditViewItem* aDetail, const QModelIndex& aIndex );
    void setSelectedContact( QContact aContact );
    void fetchTone();
    void changeEditorView();
    
public:
    HbView* mView;
    HbListView* mListView;
    CntEditViewListModel* mModel;
    CntEditViewHeadingItem* mHeading;
    CntImageLabel *mImageLabel;
    CntViewParameters mArgs;
    HbDocumentLoader* mDocument;
    CntThumbnailManager* mThumbnailManager;
    QContact* mContact; // own
    CntAbstractViewManager* mMgr; // not owned
    CntAbstractEngine* mEngine; // now owned
    bool mIsMyCard;
    
    HbAction* mSoftkey;
    HbAction* mDiscard;
    HbAction* mDelete;
    HbAction* mSave;
    QModelIndex mIndex;
    
private:
    Q_DECLARE_PUBLIC(CntEditView)
    friend class TestCntEditView;
 
private:
    XQAiwRequest* mReq;
    XQApplicationManager mAppMgr;
    HbMenu *mMenu;
    CntSaveManager* mSaveManager; // own

};
#endif /* CNTEDITVIEW_P_H_ */
