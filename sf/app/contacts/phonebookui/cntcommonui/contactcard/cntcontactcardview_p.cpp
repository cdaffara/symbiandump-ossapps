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
* Description:
*
*/

#include "cntcontactcardview_p.h"
#include "cntthumbnailmanager.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QStringList>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QDir>

#include <cntservicescontact.h>
#include <qtcontacts.h>
#include <hbapplication.h>
#include <hbscrollarea.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbview.h>
#include <hbtoolbar.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <hbmessagebox.h>
#include <hbicon.h>
#include <hbparameterlengthlimiter.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbselectiondialog.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <shareui.h>
#include <maptileservice.h>  //For maptile processing
#include <qversitcontactexporter.h>
#include <qversitwriter.h>
#include <xqservicerequest.h>
#include <QTimer>  //Progress indication icon
#include <QApplication>

#include <cntdebug.h>
#include "cntcontactcarddatacontainer.h"
#include "cntcontactcardheadingitem.h"
#include "cntcontactcarddataitem.h"
#include "cntcontactcarddetailitem.h"
#include "cntcontactcardcontextmenu.h"
#include "cntmainwindow.h"
#include "cntstringmapper.h"
#include "cntdocumentloader.h"
#include "cntimagelabel.h"
#include "cntimageutility.h"
#include "cntfavourite.h"
#include "cntactionlauncher.h"
#include "cntpresencelistener.h"
#include "cntactionpopup.h"
#include "cntvcarddetailhandler.h"
#include "cntsavemanager.h"
#include "cntextensionmanager.h"
#include <cntabstractengine.h>
#include <cntuiextensionfactory.h>
#include <cntuicontactcardextension.h>
#include <cntuisocialextension.h>

#define CNT_MAPTILE_PROGRESS_TIMER  100 // 100 msec
#define CNT_UNKNOWN_MAPTILE_STATUS   -1

// A few list items (CntInitialPopulation) will be populated before the first view;
// if there are more list items, they will be fetched and added one by one after the
// first view has been shown to the user
static const int CntListPopulationNotInProgress = -1;
static const QEvent::Type ProcessPopulateListEvent = QEvent::User;
static const int CntInitialPopulation = 8;

const char *CNT_CONTACTCARDVIEW_XML = ":/xml/contacts_contactcard.docml";
const char *CNT_MAPTILE_INPROGRESS_ICON = "qtg_anim_small_loading_1";
const char *CNT_MAPTILE_SEARCH_STOP_ICON = "qtg_mono_search_stop";
/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntContactCardViewPrivate::CntContactCardViewPrivate(bool isTemporary) :
    QObject(),
    mContainerLayout(NULL),
    mContainerWidget(NULL),
    mContact(NULL),
    mDataContainer(NULL),
    mAvatar(NULL),
    mLoader(NULL),
    mVCardIcon(NULL),
    mShareUi(NULL),
    mAcceptSendKey(true),
    mPresenceListener(NULL),
	mIsTemporary(isTemporary),
	mIsExecutingAction(false),
    mMyCardId(0),
    mSaveManager(NULL),
    mListPopulationProgress(CntListPopulationNotInProgress),
    mStopListPopulation(false)
{
    bool ok;
    document()->load(CNT_CONTACTCARDVIEW_XML, &ok);
    if (!ok) 
    {
        qFatal("Unable to read :/xml/contacts_contactcard.docml");
    }
    
    mView = static_cast<HbView*>(document()->findWidget("view"));

    //back button
    mBackKey = new HbAction(Hb::BackNaviAction, mView);
    mView->setNavigationAction(mBackKey);  
    connect(mBackKey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    mProgressTimer = new QTimer(this);
    mProgressTimer->setSingleShot(true);
    connect(mProgressTimer, SIGNAL(timeout()),this, SLOT(updateSpinningIndicator()));

    mMaptile = new MapTileService;
    if( mMaptile->isLocationFeatureEnabled(MapTileService::AppTypeContacts) )
    {
        //Connect for maptile status evenet
        QObject::connect( mMaptile, SIGNAL(maptileFetchingStatusUpdate(int, 
            int,int)),this,SLOT(mapTileStatusReceived(int,int,int)));
    }
    
    mScrollArea = static_cast<HbScrollArea*>(document()->findWidget(QString("scrollArea")));
    mHeadingItem = static_cast<CntContactCardHeadingItem*>(document()->findWidget(QString("cnt_contactcard_heading")));
    mImageLabel = static_cast<CntImageLabel*>(document()->findWidget("cnt_contactcard_image"));
    
    // presence listener
    mPresenceListener = new CntPresenceListener();
    connect(mPresenceListener, SIGNAL(fullPresenceUpdated(bool)), mHeadingItem, SLOT(setOnlineStatus(bool)));
    connect(mPresenceListener, SIGNAL(accountPresenceUpdated(const QString&, bool)), 
            this, SLOT(updateItemPresence(const QString&, bool)));
    
    
    mSaveManager = new CntSaveManager();
}

/*!
Destructor
*/
CntContactCardViewPrivate::~CntContactCardViewPrivate()
{
    // stop the progress timer
    if ( mProgressTimer && mProgressTimer->isActive())
    {
        mProgressTimer->stop();
    }
    
    // Clear the container to avoid double deletion
    if (mContainerLayout != NULL)
    {
        int count = mContainerLayout->count();
        for (int i=0; i<count; i++) 
        {
            // do not delete items. They will be deleted automatically
            mContainerLayout->removeAt(i);
        }
    }
    
    // Delete all the detail pointers if any
    qDeleteAll(mDetailPtrs);
    mDetailPtrs.clear();
    
    mView->deleteLater();
    
    delete mContact;
    mContact = NULL;
    
    delete mDataContainer;
    mDataContainer = NULL;
    
    delete mAvatar;
    mAvatar = NULL;
    
    delete mLoader;
    mLoader = NULL;
    
    if (mVCardIcon) 
    {
        delete mVCardIcon;
        mVCardIcon = NULL;
    }
    
    if (mShareUi) 
    {
        delete mShareUi;
        mShareUi = NULL;
    }
    
    delete mPresenceListener;
    mPresenceListener = NULL;
    
    delete mMaptile;
    mMaptile = NULL;
        
    //delete maptile label memory
    for ( int index = 0; index < mMaptileLabelList.count(); index++ )
    {
        if( mMaptileLabelList[index] )
        {
           delete mMaptileLabelList[index];
           mMaptileLabelList[index] = NULL ;
        }
    }
    
    delete mSaveManager;
    mSaveManager = NULL;
}

/*!
Activates a previous view
*/
void CntContactCardViewPrivate::showPreviousView()
{
    // stop list population, if it is still in progress
    mStopListPopulation = true;

    int returnValue = KCntServicesReturnValueContactNotModified;

    //save the contact if avatar has been changed.
    QContact contact = contactManager()->contact(mContact->localId());
    if ( contact != *mContact && contactManager()->error() == QContactManager::NoError)
    {
        disconnect(contactManager(), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
                    this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
        CntSaveManager::CntSaveResult result = mSaveManager->saveContact(mContact, contactManager());
        
        if (result != CntSaveManager::ENothingDone)
            returnValue = KCntServicesReturnValueContactSaved;
        else
            returnValue = KCntServicesReturnValueContactNotModified;
    }
    
    emit backPressed(returnValue);
    
    mViewManager->back( mArgs );
}

/*!
Activates the root view
*/
void CntContactCardViewPrivate::showRootView()
{
    // stop list population, if it is still in progress
    mStopListPopulation = true;

    mViewManager->back( mArgs, true );
}

/*!
Handle extension action (tapping on activity stream button)
*/
void CntContactCardViewPrivate::handleExtensionAction()
{
    CNT_ENTRY
    CntExtensionManager& mgr = mEngine->extensionManager();
    for (int i = 0; i < mgr.pluginCount(); i++)
    {
        CntUiSocialExtension* socialExtension = mgr.pluginAt(i)->socialExtension();
        if (socialExtension)
        {
            CntViewParameters params;
            socialExtension->handleToolbarAction(params);
            if (params.count())
            {
                QList<int> keys = params.keys();
                for (int j = 0; j < keys.count(); j++)
                {
                    mArgs.insert(keys.at(j), params.value(keys.at(j)));
                }
                mViewManager->changeView(mArgs);
                break;
            }
        }
    }
    
    CNT_EXIT
}

/*
Activates a default view and setup name label texts
*/
void CntContactCardViewPrivate::activate(const CntViewParameters aArgs)
{   
    CNT_ENTRY
    mArgs = aArgs;

    mViewManager = &mEngine->viewManager();
    mThumbnailManager = &mEngine->thumbnailManager();
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
            this, SLOT(thumbnailReady(QPixmap, void*, int, int)));
       
    HbMainWindow* window = mView->mainWindow();
    if (window)
    {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        window->installEventFilter(this);
        
        setOrientation(window->orientation());
    }

    qApp->installEventFilter(this);
        
    QContact contact = aArgs.value(ESelectedContact).value<QContact>();
    mContact = new QContact( contact );
    
    //my card
    mMyCardId = contactManager()->selfContactId();
    
    populateHeadingItem();

    populateListItems();
    
    bool myCard = mContact->localId() == mMyCardId;
    if (myCard)
    {
        mView->menu()->clearActions();
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:history"))));
    }
    else
    {
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:sendMyCard"))));
    }
    if (mIsTemporary)
    {
        mView->menu()->clearActions();
        mView->toolBar()->clearActions();
        mView->toolBar()->addAction(static_cast<HbAction*>(document()->findObject(QString("cnt:addtocontact"))));
    }

    document()->findWidget("viewToolbar")->setParent(mView);
    document()->findWidget("viewMenu")->setParent(mView);
    
    // Menu items
    connectAction("cnt:sendbusinesscard", SLOT(sendBusinessCard()));
    connectAction("cnt:deletecontact", SLOT(deleteContact()));
    connectAction("cnt:setasfavorite", SLOT(setAsFavorite()));
    connectAction("cnt:removefromfavorite", SLOT(removeFromFavorite()));
    connectAction("cnt:placecontacttohs", SLOT(sendToHs()));
    connectAction("cnt:edit", SLOT(editContact()));
    connectAction("cnt:history", SLOT(viewHistory()));
    connectAction("cnt:sendMyCard", SLOT(sendBusinessCard()));
    connectAction("cnt:addtocontact", SLOT(onAddedToContacts()));
    connectAction("cnt:activityStream", SLOT(handleExtensionAction()));
    
    CntExtensionManager& mgr = mEngine->extensionManager();
    if (mgr.pluginCount())
    {
        for (int i = 0; i < mgr.pluginCount(); i++)
        {
            CntUiSocialExtension* socialExtension = mgr.pluginAt(i)->socialExtension();
            if (socialExtension)
            {
                static_cast<HbAction*> (document()->findObject("cnt:activityStream"))->setEnabled(true);
                break;
            }
            else
            {
                static_cast<HbAction*> (document()->findObject("cnt:activityStream"))->setEnabled(false);
            }
        }
    }
    else
    {
        static_cast<HbAction*> (document()->findObject("cnt:activityStream"))->setEnabled(false);
    }

        
    connect(contactManager(), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), 
        this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
    
    emit viewActivated( mViewManager, aArgs );

    CNT_EXIT
}

void CntContactCardViewPrivate::populateHeadingItem()
{
    CNT_ENTRY

    Q_ASSERT(mHeadingItem != NULL && mContact != NULL);
    
    mHeadingItem->setDetails(mContact);
    
    bool myCard = mContact->localId() == mMyCardId;
    
    if (!mIsTemporary)
    {
        connect(mHeadingItem, SIGNAL(passLongPressed(const QPointF&)), this, SLOT(drawMenu(const QPointF&)), Qt::UniqueConnection);
        connect(mHeadingItem, SIGNAL(passShortPressed(const QPointF&)), this, SLOT(doChangeImage()), Qt::UniqueConnection); 
        connect(mImageLabel, SIGNAL(iconClicked()), this, SLOT(doChangeImage()), Qt::UniqueConnection);
        connect(mImageLabel, SIGNAL(iconLongPressed(const QPointF&)), this, SLOT(drawMenu(const QPointF&)), Qt::UniqueConnection);
    }
    else
    {
        mHeadingItem->ungrabGesture(Qt::TapGesture);
        mImageLabel->ungrabGesture(Qt::TapGesture);
    }
    
    bool online;
    mInitiialPrecenceData = mPresenceListener->initialPresences(*mContact, online);
    mHeadingItem->setOnlineStatus(online);

    if (!myCard) {
        bool setAsFavorite = CntFavourite::isMemberOfFavouriteGroup(contactManager(), mContact);
        mHeadingItem->setFavoriteStatus(setAsFavorite); // if contact is part of favourites group
        static_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible( !setAsFavorite );
        static_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible( setAsFavorite );
    }
    
    // avatar
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
    for (int i = 0;i < details.count();i++)
    {
        if (details.at(i).imageUrl().isValid())
        {
            if (!mAvatar)
            {
                mAvatar = new QContactAvatar(details.at(i));
                mThumbnailManager->getThumbnail(ThumbnailManager::ThumbnailLarge, mAvatar->imageUrl().toString());
            }
            else if (*mAvatar != details.at(i))
            {
                delete mAvatar;
                mAvatar = new QContactAvatar(details.at(i));
                mThumbnailManager->getThumbnail(ThumbnailManager::ThumbnailLarge, mAvatar->imageUrl().toString());
            }
            break;
        }
    }

    CNT_EXIT
}

void CntContactCardViewPrivate::populateListItems()
{
    CNT_ENTRY

    Q_ASSERT(mContact != NULL && mScrollArea != NULL);

    if (mListPopulationProgress == CntListPopulationNotInProgress) {
        mListPopulationProgress = 0;

        if (!mDataContainer) {
            mDataContainer = new CntContactCardDataContainer(
                mMaptile,
                mEngine->extensionManager(),
                mView->mainWindow()->orientation());
        }

        // fill the data container with contact details
        mDataContainer->setContactData(mContact);
        
        // scroll area + container widget
        mContainerWidget = mScrollArea->contentWidget();
        if (!mContainerWidget) {
            // initialize
            mScrollArea->setScrollDirections(Qt::Vertical);
            
            mContainerWidget = new QGraphicsWidget();
            mScrollArea->setContentWidget(mContainerWidget); // takes ownership. Old widget is deleted
                
            mContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
            mContainerLayout->setContentsMargins(0, 0, 0, 0);
            mContainerLayout->setSpacing(0);
            mContainerLayout->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            
            mContainerWidget->setLayout(mContainerLayout);   // takes ownership. Old layout is deleted
        } else {
            // Already initialized
            Q_ASSERT(mContainerLayout != NULL);
            
            // Clear the container
            int count = mContainerLayout->count();
            for (int i = 0; i < count; i++) {
                // do not delete items. They will be deleted automatically
                mContainerLayout->removeAt(i);
            }
        }
    
        // Delete all the detail pointers if any
        qDeleteAll(mDetailPtrs);
        mDetailPtrs.clear();
    }


    do {
        if (mListPopulationProgress == mDataContainer->itemCount() || mStopListPopulation) {
            // population of the list has completed
            mListPopulationProgress = CntListPopulationNotInProgress;
            mStopListPopulation = false;
            disconnect(mView->mainWindow(), SIGNAL(viewReady()), this, SLOT(populateListItems()));
            return;
        }

        CntContactCardDataItem* dataItem = mDataContainer->dataItem(mListPopulationProgress);
        int pos = dataItem->position();

        // communication methods
        if (pos < CntContactCardDataItem::ESeparator && dataItem->isFocusable())
        { 
            CntContactCardDetailItem* item = new CntContactCardDetailItem(mListPopulationProgress, mContainerWidget);
            mDetailPtrs.append(item);

            connect(item, SIGNAL(clicked()), this, SLOT(onItemActivated()));
            
            if (!mIsTemporary)
            {
                connect(item, SIGNAL(longPressed(const QPointF&)), this, SLOT(onLongPressed(const QPointF&)));
            }
            
            if (mContact->isPreferredDetail(dataItem->action(), dataItem->detail()))
            {
                dataItem->setSecondaryIcon(HbIcon("qtg_mono_favourites"));
                mPreferredItems.insert(dataItem->action(), item);
            }
            
            if (dataItem->detail().definitionName() == QContactOnlineAccount::DefinitionName)
            {
                for (int i = 0;i < mInitiialPrecenceData.keys().count();i++)
                {
                    QString fullAccount = mInitiialPrecenceData.keys().at(i);
                    QContactOnlineAccount account = dataItem->detail();
                    QString currentFullAccount = account.serviceProvider() + ':' + account.accountUri();
                    if (fullAccount == currentFullAccount)
                    {
                        if (mInitiialPrecenceData.values().at(i))
                        {
                            dataItem->setSecondaryIcon(HbIcon("qtg_small_online"));
                        }
                        mPresenceItems.insert(fullAccount, item);
                        break;
                    }
                }
            }
         
            item->setDetails(dataItem);
            mContainerLayout->addItem(item);
        }

        // separator
        else if (pos == CntContactCardDataItem::ESeparator)
        {      
            HbFrameItem* frameItem = new HbFrameItem(QString("qtg_fr_list_separator"), HbFrameDrawer::NinePieces);
            HbLabel* label = static_cast<HbLabel*>(document()->findWidget(QString("separator")));
            label->setPlainText(dataItem->titleText());
            label->setBackgroundItem(frameItem); // takes ownership
            mContainerLayout->addItem(label);
        }

        // details
        else
        {
            //map support (image only)
            if (pos >= CntContactCardDataItem::EAddress && pos <= CntContactCardDataItem::EAddressWork && !dataItem->icon().isNull())
            {        
                int addressType = MapTileService::AddressPreference;
                
                if( pos == CntContactCardDataItem::EAddressHome  )
                {
                    addressType  = MapTileService::AddressHome;
                }
                else if( pos == CntContactCardDataItem::EAddressWork )
                {
                    addressType  = MapTileService::AddressWork;
                }
                  
                HbLabel* maptileLabel = loadMaptileLabel( addressType );
                setMaptileLabel( maptileLabel, dataItem->icon() );
                mContainerLayout->addItem(  maptileLabel );
                mMaptileLabelList.insert( addressType, maptileLabel );
            } 
            //other details
            else
            {
                CntContactCardDetailItem* item = new CntContactCardDetailItem(mListPopulationProgress, mContainerWidget, false);
                mDetailPtrs.append(item);
                
                //To check whether maptile status  icon is set with the address 
                if( ( dataItem->titleText() == hbTrId("txt_phob_formlabel_address") ||
                      dataItem->titleText() == hbTrId("txt_phob_formlabel_address_home")||  
                      dataItem->titleText() == hbTrId("txt_phob_formlabel_address_work") ) &&
                      dataItem->secondaryIcon().iconName() == QString(CNT_MAPTILE_INPROGRESS_ICON) )
                    
                {
                    //Information for displaying maptile fetching progress bar.
                    //Memory will be deleted from the queue.
                    CntContactCardMapTileDetail* detail = new CntContactCardMapTileDetail;
                    detail->mContactId = mContact->localId();
                        
                    if( dataItem->titleText() == hbTrId("txt_phob_formlabel_address") )
                    {
                               detail->mAddressType = MapTileService::AddressPreference;
                    }
                    else if ( dataItem->titleText() ==  hbTrId("txt_phob_formlabel_address_home") )
                    {
                               detail->mAddressType = MapTileService::AddressHome;
                    }
                    else if( dataItem->titleText() ==  hbTrId("txt_phob_formlabel_address_work") )
                        {
                               detail->mAddressType = MapTileService::AddressWork;
                    }
                    
                    detail->mProgressCount = 0;
                    detail->mDetailItem =  item;
                    detail->maptileStatus = CNT_UNKNOWN_MAPTILE_STATUS;
                    mAddressList.append( detail );  
                    
                    //Update the spinning indicator
                    updateSpinningIndicator();
                    
                }
                item->setDetails(dataItem);
                mContainerLayout->addItem(item);
            }
        }
    
        ++mListPopulationProgress;
    } while (mListPopulationProgress < CntInitialPopulation);

    if (mListPopulationProgress <= CntInitialPopulation) {
        connect(mView->mainWindow(), SIGNAL(viewReady()), this, SLOT(populateListItems()));
    } else {
        disconnect(mView->mainWindow(), SIGNAL(viewReady()), this, SLOT(populateListItems()));
        HbApplication::instance()->postEvent(this, new QEvent(ProcessPopulateListEvent));
    }

    CNT_EXIT
}

/*
    Connects the action with a slot and also sets the view as the parent for the action.
 */
void CntContactCardViewPrivate::connectAction(QString actionName, const char* slot)
{
    HbAction *action = qobject_cast<HbAction *>(document()->findObject(actionName));
    if (action) {
        action->setParent(mView);
        if (slot != NULL) {
            connect(action, SIGNAL(triggered()), this, slot);
        }
    }
}

/*
    Updates the maptile fetching spinning indicator icon
 */
void CntContactCardViewPrivate::updateSpinningIndicator()
{
    //Check all address details( Preferred, Home, Work )
    for( int index = 0 ; index < mAddressList.count(); )
    {
        //Maptile status not received update the rotating icon
        if( mAddressList[index]->maptileStatus == CNT_UNKNOWN_MAPTILE_STATUS )
        {
             QString iconName("qtg_anim_small_loading_");
             mAddressList[index]->mProgressCount = mAddressList[index]->mProgressCount % 10 + 1;
             iconName.append(QVariant(mAddressList[index]->mProgressCount).toString());
             
             HbIcon icon(iconName);
             mAddressList[index]->mDetailItem->setSecondaryIconItem( icon );
             mAddressList[index]->mDetailItem->update();   
             mProgressTimer->start(CNT_MAPTILE_PROGRESS_TIMER);
             index++;
        }
        else
        {
            //Maptile status received. Show the maptile image if available
            MapTileService::AddressType sourceAddressType =
                    static_cast <MapTileService::AddressType>( mAddressList[index]->mAddressType );
             
            QContactLocalId contactId = mContact->localId();
             
            if( mAddressList[index] != NULL )
            {
                 if( mAddressList[index]->maptileStatus == MapTileService::MapTileFetchingCompleted )
                 {
 
                     //Read the maptile path and update the image
                     QString imagePath;
                     mMaptile->getMapTileImage( 
                                contactId, sourceAddressType, imagePath, mView->mainWindow()->orientation() );
					 
                     if( !imagePath.isEmpty() )
                     {
                         //Empty icon. Clear the inprogress  icon
                         HbIcon emptyIcon;
                         mAddressList[index]->mDetailItem->setSecondaryIconItem( emptyIcon );
                         
                         HbIcon icon( imagePath );
                         
                         HbLabel* maptileLabel = loadMaptileLabel( sourceAddressType );
                         setMaptileLabel( maptileLabel, icon );
                         mMaptileLabelList.insert( sourceAddressType, maptileLabel );
                        
                         //find the index of the item and insert maptile in the next index 
                         for( int itemIndex = 0 ; itemIndex < mContainerLayout->count(); itemIndex++ )
                         {
                             if( mContainerLayout->itemAt(itemIndex) == mAddressList[index]->mDetailItem )
                             {
                                 mContainerLayout->insertItem( itemIndex+1, maptileLabel );
                                 break;
                             }
                         }
                         
                     }
                     else
                     {
                         //Maptile image not available. Show the search stop icon
                         setMaptileSearchStopIcon( index );
                     }
                 }
                 else
                 {
                     //Maptile fetching failed. Show the search stop icon
                     setMaptileSearchStopIcon( index );
                 }
                 
                 delete mAddressList[index];
                 mAddressList.removeAt(index);
            }
            else
            {
                 //increment the index now
                 index++;
            }
        }
    }
}

/*
* Sets the search stop icon to secondary icon item
*/
void CntContactCardViewPrivate::setMaptileSearchStopIcon( int index )
{
    if( index < mAddressList.count() )
    {
        QString iconName(CNT_MAPTILE_SEARCH_STOP_ICON);
        HbIcon icon(iconName);
        mAddressList[index]->mDetailItem->setSecondaryIconItem( icon );
        mAddressList[index]->mDetailItem->update();  
    }
}

/*
* Slot to receive the maptile status information
*/
void CntContactCardViewPrivate::mapTileStatusReceived( int contactid, int addressType, int status)
{
    //Update the maptile status information for all 3( Preferred, Work, Home ) address
    for( int index = 0 ; index < mAddressList.count(); index++  )
    {
        if( mAddressList[index]->mContactId == contactid &&  
                 mAddressList[index]->mAddressType == addressType )
        {
            mAddressList[index]->maptileStatus = status;
        }
    }
    
    updateSpinningIndicator();
}

/*
* Updates correct maptile image when screen orientation changes.
*/
void CntContactCardViewPrivate::updateMaptileImage()
{
    //If there is no maptile displayed, return immediately
    if( mMaptileLabelList.count() > 0 )
    {
        QContactLocalId contactId = mContact->localId();
        
        QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
        
        //address
        QString contextHome(QContactAddress::ContextHome.operator QString());
        QString contextWork(QContactAddress::ContextWork.operator QString());
        MapTileService::AddressType sourceAddressType 
                                             = MapTileService::AddressPreference;
        
        QString imagePath;
        
        for ( int i = 0; i < addressDetails.count(); i++ )
        {
            if ( !addressDetails[i].contexts().isEmpty() && 
                   addressDetails[i].contexts().at(0) == contextHome )
            {
                sourceAddressType = MapTileService::AddressHome;
            }
            else if ( !addressDetails[i].contexts().isEmpty() && 
                         addressDetails[i].contexts().at(0) == contextWork )
            {
                sourceAddressType = MapTileService::AddressWork;
            }
            
            int status = mMaptile->getMapTileImage( 
                                             contactId, 
                                             sourceAddressType, 
                                             imagePath, 
                                             mView->mainWindow()->orientation() );
            if( !imagePath.isEmpty() )
            {
                HbIcon icon( imagePath );
                HbLabel* label = mMaptileLabelList.value( sourceAddressType );
                if( label )
                {
                    setMaptileLabel( label, icon );
                }
            }
        }
    }
}


/*
* Asscoiate the maptile label widget with maptile image
*/
void CntContactCardViewPrivate::setMaptileLabel( HbLabel*& mapLabel, const HbIcon& icon )
{
    mapLabel->clear();
    mapLabel->setIcon( icon );
    
	qreal leftMarginSize;
	mapLabel->getContentsMargins( &leftMarginSize, 0 , 0 , 0 );
    mapLabel->setPreferredSize( 
          QSizeF(icon.width() + leftMarginSize, icon.height()));
   
}

/*
* Load the maptile label based on the address type
*/
HbLabel* CntContactCardViewPrivate::loadMaptileLabel( int addressType )
{
    HbLabel* maptileLabel = NULL;
    
    if( addressType == MapTileService::AddressPreference )
    {
        maptileLabel = static_cast<HbLabel*>(document()->findWidget(QString("maptilePreferenceWidget")));
    }
    else if( addressType == MapTileService::AddressHome  )
    {
        maptileLabel  = static_cast<HbLabel*>(document()->findWidget(QString("maptileHomeWidget")));
    }
    else if( addressType == MapTileService::AddressWork )
    {
        maptileLabel  = static_cast<HbLabel*>(document()->findWidget(QString("maptileWorkWidget")));
    }
                            
    return maptileLabel;
}

/*
* Update the presence status icon of action item with the given accountUri
*/
void CntContactCardViewPrivate::updateItemPresence(const QString& accountUri, bool online)
{
    CntContactCardDetailItem* item = mPresenceItems.value(accountUri);
    
    if (item)
    {
        if (online)
        {
            mDataContainer->dataItem(item->index())->setSecondaryIcon(HbIcon("qtg_small_online"));
        }
        else
        {
            mDataContainer->dataItem(item->index())->setSecondaryIcon(HbIcon());
        }
        item->setDetails(mDataContainer->dataItem(item->index()));
    }
}

void CntContactCardViewPrivate::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    CNT_ENTRY
    
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        HbIcon icon(pixmap);
        mHeadingItem->setIcon(icon);
        mVCardIcon = new HbIcon(pixmap);
        mImageLabel->clear();
        mImageLabel->setIcon(pixmap);
    }
    
    CNT_EXIT
}

/*!
Place contact to homescreen as widget
*/  
void CntContactCardViewPrivate::sendToHs()
{
    QVariantHash preferences;
    preferences["contactId"] = mContact->localId();
    
    XQServiceRequest snd("com.nokia.symbian.IHomeScreenClient",
                         "addWidget(QString,QVariantHash)"
                         ,false);
    snd << QString("contactwidgethsplugin");
    snd << preferences;
    snd.send();

    /* 
    if (mRequest)
    {
        delete mRequest;
        mRequest = 0;
    }
         
    mRequest = mAppManager.create("com.nokia.symbian.IHomeScreenClient", "addWidget(QString,QVariantHash)", false);
    
    if (mRequest)
    {
        QList<QVariant> args;
        QVariantHash preferences;
        preferences["contactId"] = mContact->localId();
        args << preferences;
        args << QString("contactwidgethsplugin");
        mRequest->setArguments(args);
        mRequest->send();
    }
    */
}

/*!
Launch contact editor 
*/
void CntContactCardViewPrivate::editContact()
{
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, editView);
    
    mViewManager->changeView( mArgs );
}

void CntContactCardViewPrivate::addToGroup()
{
}

void CntContactCardViewPrivate::setAsFavorite()
{
    QContactId id = mContact->id();
    CntFavourite::addContactToFavouriteGroup( contactManager(), id );
    
    QContact c = contactManager()->contact(mContact->localId());
    
    delete mContact;
    mContact = NULL;
    
    mContact = new QContact(c);
    
    qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible(true);
    qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible(false);
    mHeadingItem->setFavoriteStatus(true);  
}

void CntContactCardViewPrivate::removeFromFavorite()
{
    QContactId id = mContact->id();   
    CntFavourite::removeContactFromFavouriteGroup( contactManager(), id );
    
    QContact c = contactManager()->contact(mContact->localId());
    
    delete mContact;
    mContact = NULL;
    
    mContact = new QContact(c);

    qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible(false);
    qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible(true);
    mHeadingItem->setFavoriteStatus(false); 
}

/*!
Delete contact
*/
void CntContactCardViewPrivate::deleteContact()
{    
    QString name = contactManager()->synthesizedContactDisplayLabel(*mContact);
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(int)),
            HbMessageBox::Delete | HbMessageBox::Cancel);
}

/*!
Handle action for deleting a contact
*/
void CntContactCardViewPrivate::handleDeleteContact(int action)
{
    if (action == HbMessageBox::Delete)
    {
        disconnect(contactManager(), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
                this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
        
        bool success = contactManager()->removeContact(mContact->localId());
        
        int returnValue = success ? KCntServicesReturnValueContactDeleted : KCntServicesReturnValueContactNotModified;
        
        emit backPressed(returnValue);  
        
        mViewManager->back( mArgs, true );
    }
}

/*!
Launch history view 
*/
void CntContactCardViewPrivate::viewHistory()
{
    QVariant var;
    var.setValue(*mContact);
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, historyView);
    
    mViewManager->changeView( mArgs );
}

/*!
Deactivate the view
*/
void CntContactCardViewPrivate::deactivate()
{
}

/*!
Send the business card / my card 
*/
void CntContactCardViewPrivate::sendBusinessCard()
{
    CNT_ENTRY
    
    // Check if the contact has an image.
    QList<QContactAvatar> avatars = mContact->details<QContactAvatar>();
    bool imageExists( false );
    /*foreach(QContactAvatar a, mContact->details<QContactAvatar>())
    {
        if (!a.imageUrl().isEmpty())
        {
            imageExists = true;
            HbMessageBox *note = new HbMessageBox(
                    hbTrId("txt_phob_info_add_contact_card_image_to_business_c"),
                    HbMessageBox::MessageTypeQuestion);
            note->setIcon(*mVCardIcon);
            note->clearActions();
            
            HbAction* ok = new HbAction(hbTrId("txt_common_button_ok"), note);
            HbAction* cancel = new HbAction(hbTrId("txt_common_button_cancel"), note);
            
            ok->setObjectName( "ok" );
            cancel->setObjectName( "cancel" );
             
            note->addAction( ok );
            note->addAction( cancel );
            
            note->setModal( true );
            note->setAttribute(Qt::WA_DeleteOnClose, true );
            note->open( this, SLOT(handleSendBusinessCard(HbAction*)));
            break;
        }
    }*/
    
    if ( !imageExists )
    {
        CNT_LOG_ARGS("snd vCard without image")
        handleSendBusinessCard( NULL ); // no image
    }
    
    CNT_EXIT
}

/*!
Set orientation of the view
*/
void CntContactCardViewPrivate::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        document()->load(CNT_CONTACTCARDVIEW_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        document()->load(CNT_CONTACTCARDVIEW_XML, "landscape");
    }
	//Update the maptile image
	updateMaptileImage();
}

/*!
Called after user clicked on the listview.
*/
void CntContactCardViewPrivate::onItemActivated()
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    CntContactCardDataItem& dataItem = *mDataContainer->dataItem(index);
    QString action = mDataContainer->dataItem(index)->action();
    CntUiContactCardExtension* extension = dataItem.contactCardExtension();
    if (!extension)
    {
        //It's not an extension action.
        // Check if action is internal
        QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(action, "symbian", 1);
        if (0 < actionDescriptors.count())
        {
            // These actions are considered internal(vendor=symbian and version=1)
            executeAction(*mContact, dataItem.detail(), action, item);
        }
        else
        {
            //Handle dynamic actions differently
            executeDynamicAction(*mContact, dataItem.detail(), dataItem.actionDescriptor());
        }
    } else {
        // The action is from an extension
        int indexWithinPlugin = dataItem.contactCardExtensionActionIndex();
        extension->handleShortPress(indexWithinPlugin);
    }
}

/*!
Execute the call / message / email action
*/
void CntContactCardViewPrivate::executeAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction, CntContactCardDetailItem* aItem)
{
    if (mIsExecutingAction)
    {
        return;
    }
    else
    {
        mIsExecutingAction = true;
    }
    
    CntActionLauncher* other = new CntActionLauncher(*contactManager(), aAction);
    connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
    if (aItem && aContact.preferredDetail(aAction).isEmpty())
    {
        setPreferredAction(aAction, aDetail, aItem);
    }
    other->execute(aContact, aDetail);  
}


/*!
Execute the call / message / email action
*/
void CntContactCardViewPrivate::executeAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction)
{ 
    //TODO: need refactoring
    CntContactCardDetailItem* detailItem = NULL;
    for (int index = 0; index < mDataContainer->itemCount(); index++)
    {
        QContactDetail detail = mDataContainer->dataItem(index)->detail();
        QString action = mDataContainer->dataItem(index)->action();
        if (detail == aDetail && action == aAction)
        {
            detailItem = static_cast<CntContactCardDetailItem*>(mContainerLayout->itemAt(index));
            break;
        }
    }
    executeAction(aContact, aDetail, aAction, detailItem);
    mAcceptSendKey=true;
}

/*!
Execute dynamic action
*/
void CntContactCardViewPrivate::executeDynamicAction(QContact& aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor)
{
    if (mIsExecutingAction)
    {
        return;
    }
    else
    {
        mIsExecutingAction = true;
    }
    
    CntActionLauncher* other = new CntActionLauncher(*contactManager());
    connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
    other->execute(aContact, aDetail, aActionDescriptor);
}

void CntContactCardViewPrivate::actionExecuted(CntActionLauncher* aAction)
{
    aAction->deleteLater();
    mIsExecutingAction = false;
}

/*!
Item specific menu
*/
void CntContactCardViewPrivate::onLongPressed(const QPointF &aCoords)
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    
    CntContactCardContextMenu *menu = new CntContactCardContextMenu( item );
    HbAction *communicationAction = 0;
    HbAction *dynamicAction = 0;
    HbAction *videoCommunicationAction = 0;
    HbAction *preferredAction = 0;
    CntStringMapper stringMapper;
    
    QString action = mDataContainer->dataItem(index)->action();
    QContactDetail detail = mDataContainer->dataItem(index)->detail();
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {       
        QContactDetail detail = mDataContainer->dataItem(index)->detail();
        if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber number = static_cast<QContactPhoneNumber>(detail);
            QString context = number.contexts().isEmpty() ? QString() : number.contexts().first();
            QString subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();

            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(subtype, context));
            videoCommunicationAction = menu->addAction(hbTrId("txt_phob_menu_call_video_number"));
        }        
    }
    else if (action.compare("message", Qt::CaseInsensitive) == 0)
    {
        communicationAction = menu->addAction(hbTrId("txt_phob_menu_send_message"));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        if (!detail.contexts().isEmpty())
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), QString()));
        }
    }
    else if (action.compare("url", Qt::CaseInsensitive) == 0)
    {
        if (!detail.contexts().isEmpty())
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), QString()));
        }
    }
    else if (1 > QContactAction::actionDescriptors(action, "symbian", 1).count())
    {
        // Do verification that action is not internal type(vendor=symbian and version=1)
        // If hard coded action is not found, fetch localization from dynamic action
        // and create dynamic action item.
        dynamicAction = menu->addAction(mDataContainer->dataItem(index)->longPressText());
    }
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_number"));
    }
    else if (action.compare("message", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_number"));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_email"));
    }
      
    if (mContact->isPreferredDetail(action, detail) && preferredAction)
    {
        preferredAction->setEnabled(false);
    }
   
    if ( communicationAction )
    {
        communicationAction->setObjectName( "communicationAction" );
    }
    if ( videoCommunicationAction )
    {
        videoCommunicationAction->setObjectName( "videoCommunicationAction" );
    }
   
    if ( dynamicAction )
    {
        dynamicAction->setObjectName( "dynamicAction" );
    }
    
    if ( preferredAction )
    {
        preferredAction->setObjectName( "preferredAction" );
        menu->insertSeparator(preferredAction);
    }
   
    menu->setPreferredPos( aCoords );
    menu->setAttribute( Qt::WA_DeleteOnClose, true );
    menu->open( this, SLOT(handleMenuAction(HbAction*)) );
}

void CntContactCardViewPrivate::handleMenuAction(HbAction* aAction)
{
    CntContactCardContextMenu* menu = static_cast<CntContactCardContextMenu*>(sender());
    CntContactCardDetailItem *item = menu->item();
    int index = item->index();
       
    QString action = mDataContainer->dataItem(index)->action();
    QContactDetail detail = mDataContainer->dataItem(index)->detail();
        
    QString name = aAction->objectName();
    
    if ( name == "communicationAction" )
    {
        executeAction(*mContact, detail, action, item);
    }

    if ( name == "videoCommunicationAction" )
    {
        //service name latter on should come from service table.
        executeAction(*mContact, detail, "videocall", item);
    }
    if ( name == "dynamicAction" )
    {             
        executeDynamicAction(*mContact, detail, mDataContainer->dataItem(index)->actionDescriptor());
    }
    
    if ( name == "preferredAction" )
    {
        setPreferredAction(action, detail, item);
    }
}

/*!
Set selected detail as preferred for selected action in detail item
*/
void CntContactCardViewPrivate::setPreferredAction(const QString &aAction, const QContactDetail &aDetail, CntContactCardDetailItem *aDetailItem)
{
    if (aAction == "call" || aAction == "message" || aAction == "email")
    {
        mContact->setPreferredDetail(aAction, aDetail);
        contactManager()->saveContact(mContact);
        if (mPreferredItems.contains(aAction))
        {
           CntContactCardDetailItem *oldItem = mPreferredItems.value(aAction);
           mDataContainer->dataItem(oldItem->index())->setSecondaryIcon(HbIcon());
           oldItem->setDetails(mDataContainer->dataItem(oldItem->index()));
        }
        
        mDataContainer->dataItem(aDetailItem->index())->setSecondaryIcon(HbIcon("qtg_mono_favourites"));
        aDetailItem->setDetails(mDataContainer->dataItem(aDetailItem->index()));
        
        mPreferredItems.insert(aAction, aDetailItem);
    }
}


void CntContactCardViewPrivate::handleSendBusinessCard( HbAction* aAction )
{
    CNT_ENTRY
    Q_UNUSED(aAction);
    
    QList<QContact> list;
    /*if ( aAction && aAction->objectName() == "cancel" )
    {
        QContact tmpContact( *mContact );
        foreach ( QContactAvatar a, tmpContact.details<QContactAvatar>() )
        {
            tmpContact.removeDetail( &a );
        }
        list.append( tmpContact );
    }
    else
    {
        list.append( *mContact );
    }*/
    QContact tmpContact( *mContact );
    foreach ( QContactAvatar a, tmpContact.details<QContactAvatar>() )
    {
        tmpContact.removeDetail( &a );
    }
    
    //temproral solution for a contact without name - add an empty name,
    //so vcard construction doesn't fail
    //TODO: use custom handler again when new QVersitContactExporterDetailHandlerV2
    //(part of QtVersit) is available in the platform
    QList<QContactName> names = tmpContact.details<QContactName>();
    if (names.count() > 0)
    {
        QContactName name = names.at(0);
        if (name.firstName().isEmpty() && name.lastName().isEmpty())
        {
            //empty first and last names - update with empty string
            name.setFirstName(QString(""));
            tmpContact.saveDetail(&name);
        }
    }
    else
    {
        //no name detail - create one
        QContactName name;
        name.setFirstName(QString(""));
        tmpContact.saveDetail(&name);
    } //temproral solution for a contact without name - add an empty name,
      //so vcard construction doesn't fail - end
    
    list.append( tmpContact );
    
    QString tempDir = QDir::tempPath().append("/tempcntvcard");
    QDir dir(tempDir);
           
    // Temporary directory to store the vCard file
    if (!dir.exists()) 
    {
        // Create a temp directory
        if (!QDir::temp().mkdir("tempcntvcard"))
        {
            return;
        }
    } 
    else 
    {    
        // Empty the temp directory since the other vCards are not required
        QStringList l = dir.entryList();
        foreach(QString s, l) 
        {
            if (dir.exists(s))
            {
                dir.remove(s);
            }    
        }   
    }
           
    QString vCardName;
    if ( mContact->displayLabel().isEmpty() ) {
        vCardName = hbTrId("txt_phob_list_unnamed").append(".vcf");
    } 
    else {
        // Remove invalid characters from file name
        QString filename = mContact->displayLabel().trimmed();
        QString invalidChars("?*<>/\"|\\:");
        QChar replaceChar('_');
        QChar *data = filename.data();
        int count(0);
        while ( !data->isNull() ) {
            if ( invalidChars.contains(*data) ) {
                *data = replaceChar;
                count++;
            }
            ++data;
        }
        
        // Check if filename contains only special characters
        if ( count == filename.count() )
            filename = "_";
        vCardName = filename.append(".vcf");
    }
        
    
    QString vCardPath = dir.absolutePath().append(QDir::separator());
    vCardPath.append(vCardName);
    vCardPath = QDir::toNativeSeparators(vCardPath);
    
    QVersitContactExporter exporter;
    //TODO: use custom handler again when new QVersitContactExporterDetailHandlerV2
    //(part of QtVersit) is available in the platform
    //CntVCardDetailHandler hanlder;
    //exporter.setDetailHandler(&hanlder);
    // The vCard version needs to be 2.1 due to backward compatiblity when sending 
    if (exporter.exportContacts(list, QVersitDocument::VCard21Type))
    {
        CNT_LOG_ARGS("VCard21Type");
        QList<QVersitDocument> docs = exporter.documents();
        QFile f(vCardPath);
        if ( f.open(QIODevice::WriteOnly) ) 
        {
            CNT_LOG_ARGS("write VCard");
            // Start creating the vCard
            QVersitWriter writer;
            writer.setDevice(&f);
        
            bool ret = writer.startWriting(docs);
            ret = writer.waitForFinished();
        
            // Create the vCard and send it to messaging service
            if (!mShareUi) {
                mShareUi = new ShareUi();
            }
            QStringList l;
            l << vCardPath;
            mShareUi->send(l, true);
        }
    }
    
    CNT_EXIT
}


/*!
Called after the user clicked "Change Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doChangeImage()
{
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, imageEditorView);
        
    mViewManager->changeView( mArgs );
}

/*!
Called after the user clicked "Remove Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doRemoveImage()
{
    if (mAvatar) 
    {
        CntImageUtility imageUtility;
        QString filePath=mAvatar->imageUrl().toString();
        
        bool success = mContact->removeDetail(mAvatar);
        if (success) 
        { 
            if (!filePath.isEmpty())
            {
                // Check if image removable.
                CntImageUtility imageUtility;
                if(imageUtility.isImageRemovable(filePath))
                {
                    imageUtility.removeImage(filePath);
                }
            }
            mAvatar->setImageUrl(QUrl());
            mImageLabel->clear();
            mImageLabel->setAvatarIcon(HbIcon("qtg_large_add_contact_picture"));
            mHeadingItem->setIcon(HbIcon("qtg_large_add_contact_picture"));
            
            disconnect(contactManager(), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
                                this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
            CntSaveManager::CntSaveResult result = mSaveManager->saveContact(mContact, contactManager());
        }
    }
}

/*!
Draw the image specific content menu
*/
void CntContactCardViewPrivate::drawMenu(const QPointF &aCoords)
{
    HbMenu *menu = new HbMenu();
    menu->addAction(hbTrId("txt_phob_menu_change_picture"), this, SLOT(doChangeImage()) );
    if (mAvatar && !mAvatar->imageUrl().isEmpty())
    {
        menu->addAction(hbTrId("txt_phob_menu_remove_image"), this, SLOT(doRemoveImage()) );
    }
    menu->setAttribute( Qt::WA_DeleteOnClose );
    menu->setPreferredPos( aCoords );
    menu->open();
}

/*!
Return the pointer to the document loader
*/
CntDocumentLoader* CntContactCardViewPrivate::document()
{
    if (!mLoader) 
    {
        mLoader = new CntDocumentLoader();
    }
    return mLoader;
}

/*!
Return pointer to the contact manager
*/
QContactManager* CntContactCardViewPrivate::contactManager()
{
    return &mEngine->contactManager(SYMBIAN_BACKEND);
}

bool CntContactCardViewPrivate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == ProcessPopulateListEvent && obj == this) {
        populateListItems();
        return true;
    }

    if (event->type() == QEvent::KeyPress && obj == mView->mainWindow())
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Yes)
        {
            return sendKeyPressed();
        }
    }
    
    // Update the contact only when the application is in the backgound
    // This avoids flickering when the contact card is in focus
    if ( event->type() == QEvent::ApplicationActivate ) 
    {
        CNT_LOG_ARGS("app activated")

        disconnect(contactManager(), SIGNAL(contactsChanged(const QList<QContactLocalId>&)),
                this, SLOT(contactUpdatedFromOtherSource(const QList<QContactLocalId>&)));
        
        QContact c = contactManager()->contact(mContact->localId());
        
        QContactTimestamp latestTimeStamp = c.detail<QContactTimestamp>();
        QContactTimestamp localTimeStamp = mContact->detail<QContactTimestamp>();
        
        // Once we regain focus check if the contact was modified in the background
        if (contactManager()->error() == QContactManager::NoError 
            && latestTimeStamp.lastModified() > localTimeStamp.lastModified())
        {
            if (mContact)
                delete mContact;
            mContact = new QContact(c);
                
            populateHeadingItem();
            populateListItems();
        }
    }
    
    if ( event->type() == QEvent::ApplicationDeactivate )
    {
        CNT_LOG_ARGS("app deactivated")

        connect(contactManager(), SIGNAL(contactsChanged(const QList<QContactLocalId>&)), 
            this, SLOT(contactUpdatedFromOtherSource(const QList<QContactLocalId>&)), Qt::UniqueConnection);
    }
    
    return false;
}

bool CntContactCardViewPrivate::sendKeyPressed()
{   
    int count = 0;
    bool keyConsumed = false;
    
    for (int index = 0; index < mDataContainer->itemCount(); index++)
    {
        CntContactCardDataItem* dataItem = mDataContainer->dataItem(index);
        if (dataItem->action().compare("call", Qt::CaseInsensitive) == 0)
        {
            count++;
        }
    }

    if (count)
    {
        QContactDetail preferredDetail = mContact->preferredDetail("call");
        if (!preferredDetail.isEmpty())
        {
            keyConsumed = true;
            executeAction(*mContact, preferredDetail, "call");
        }
        else if (count == 1 )
        {
            keyConsumed = true;
            executeAction( *mContact, mContact->details<QContactPhoneNumber>().first(), "call"); 
        }
        else if(count >= 2 && mAcceptSendKey)
        {
            keyConsumed = true;
            mAcceptSendKey = false;
            CntActionPopup *actionPopup = new CntActionPopup(mContact);
            actionPopup->showActionPopup("call");
            connect( actionPopup, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, 
                    SLOT(executeAction(QContact&, QContactDetail, QString)));   
            connect( actionPopup, SIGNAL(actionPopupCancelPressed()), this, 
                    SLOT(sendKeyCancelSlot()));
        }
        else
        {
            //ignore
        }
    }
    
    return keyConsumed;
}

void CntContactCardViewPrivate::sendKeyCancelSlot()
{
    mAcceptSendKey = true;
}

void CntContactCardViewPrivate::onAddedToContacts()
{
    emit addToContacts();
}

void CntContactCardViewPrivate::contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds)
{
    if ( contactIds.contains(mContact->localId()) )
    {
        // Do not switch to the previous view immediately. List views are
        // not updated properly if this is not done in the event loop
        QTimer::singleShot(0, this, SLOT(showRootView()));
    }
}

void CntContactCardViewPrivate::contactUpdatedFromOtherSource(const QList<QContactLocalId>& contactIds)
{
    if ( contactIds.contains(mContact->localId()) )
    {
        QContact c = contactManager()->contact(mContact->localId());
        if (contactManager()->error() == QContactManager::NoError)
        {
            if (mContact)
                delete mContact;
            mContact = new QContact(c);
            
            populateHeadingItem();
            populateListItems();
        }
    }
}

// end of file
