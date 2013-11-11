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

#ifndef CNTCOMMLAUNCHERVIEW_H
#define CNTCOMMLAUNCHERVIEW_H

#include "cntcontactcardview.h"

#include <QObject>
#include <QKeyEvent>
#include <QGraphicsSceneResizeEvent>
#include "cntglobal.h"
#include <maptileservice.h>

class HbView;
class HbScrollArea;
class CntDocumentLoader;
class HbAction;
class HbIcon;
class QGraphicsWidget;
class QGraphicsLinearLayout;
class CntThumbnailManager;
class CntContactCardDataContainer;
class CntContactCardHeadingItem;
class CntContactCardDetailItem;
class CntContactCardDataItem;
class CntImageLabel;
class CntActionLauncher;
class XQServiceRequest;
class ShareUi;
class QStandardItemModel;
class QModelIndex;
class HbSelectionDialog;
class CntPresenceListener;
class HbLabel;
class CntSaveManager;
class QTimer;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactManager;
class QContactDetail;
class QContactAction;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

//To store the maptile request information
class CntContactCardMapTileDetail 
{
public:
    //Contact id 
    int mContactId;
    //Address type( preferred, home, work )
    int mAddressType;
    //Maptile status
    int maptileStatus;
    //Maptile progress icon animation count
    int mProgressCount;
    //Detial item containg contact address details
    CntContactCardDetailItem* mDetailItem;
};

class CntContactCardViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntContactCardView)

public:
    CntContactCardViewPrivate(bool isTemporary);
    virtual ~CntContactCardViewPrivate();

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void onItemActivated();
    void editContact();
	void viewHistory();
    void onLongPressed(const QPointF &aCoords);
    void setPreferredAction(const QString &aAction, const QContactDetail &aDetail, CntContactCardDetailItem *aDetailItem);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void drawMenu(const QPointF &aCoords);
    void sendToHs();
    void onAddedToContacts();
    void mapTileStatusReceived(int contactid, int addressType, int status);

private slots:
    void sendBusinessCard();
    void addToGroup();
    void deleteContact();
    void handleDeleteContact(int action);
    void setAsFavorite();
    void removeFromFavorite();
    void actionExecuted(CntActionLauncher* aAction);
    void setOrientation(Qt::Orientation orientation);
    void showPreviousView();
    void doChangeImage();
    void doRemoveImage();
    void showRootView();
    void handleExtensionAction();
    
    void handleMenuAction(HbAction* aAction);
    void handleSendBusinessCard( HbAction* aAction );
    void executeAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction);
    void sendKeyCancelSlot();
    void contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds);
    void contactUpdatedFromOtherSource(const QList<QContactLocalId>& contactIds);
    
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif    
    // Maptile related functions
	void updateSpinningIndicator();
	void setMaptileSearchStopIcon( int index );
	void setMaptileLabel( HbLabel*& mapLabel, const HbIcon& icon );
	void updateMaptileImage();
	HbLabel* loadMaptileLabel( int addressType );
    void populateListItems();
	
    // Presence related function
    void updateItemPresence(const QString& accountUri, bool online);
	    
public:
    CntContactCardView* q_ptr;    
    void activate(const CntViewParameters aArgs);
    void deactivate();
    CntDocumentLoader* document();
    QContactManager* contactManager();
    
signals:
    void preferredUpdated();
    void backPressed(int value);
    void viewActivated(CntAbstractViewManager* aMgr, const CntViewParameters aArgs);
    void addToContacts();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void connectAction(QString actionName, const char* slot);
    void executeAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction, CntContactCardDetailItem* aItem);
    void executeDynamicAction(QContact& aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor);
    bool sendKeyPressed();
    void populateHeadingItem();
    
public:
    CntAbstractViewManager*     mViewManager;   // not own
    HbView*                     mView;
    HbScrollArea                *mScrollArea;
    QGraphicsLinearLayout       *mContainerLayout;
    QGraphicsWidget*            mContainerWidget;
    QContact                    *mContact;
    CntContactCardDataContainer *mDataContainer;
    CntContactCardHeadingItem   *mHeadingItem;
    CntThumbnailManager         *mThumbnailManager;
    QContactAvatar              *mAvatar;
    QMap<QString, CntContactCardDetailItem*> mPreferredItems;
    
    /// maps a QContactOnlineAccount (for example "sip:sip@sip.com") to a detail item
    QMap<QString, CntContactCardDetailItem*> mPresenceItems;
    CntDocumentLoader           *mLoader;
    HbAction                    *mBackKey;
    CntImageLabel               *mImageLabel;
    HbIcon                      *mVCardIcon;
    CntViewParameters           mArgs;
    ShareUi                     *mShareUi;
    bool                        mAcceptSendKey;
    CntPresenceListener*        mPresenceListener; // own
    MapTileService              *mMaptile; //own
    QList <CntContactCardMapTileDetail*> mAddressList;
    QMap <int, HbLabel*>        mMaptileLabelList;
    bool                        mIsTemporary;
    bool                        mIsExecutingAction;
    QMap<QString, bool>         mInitiialPrecenceData;
    QContactLocalId             mMyCardId;
    CntSaveManager* mSaveManager; // own
    CntAbstractEngine* mEngine;
    QTimer* mProgressTimer; // own
    QList<CntContactCardDetailItem*>    mDetailPtrs;
    int                         mListPopulationProgress;
    bool                        mStopListPopulation;
};

#endif // CNTCOMMLAUNCHERVIEW_H

// EOF
