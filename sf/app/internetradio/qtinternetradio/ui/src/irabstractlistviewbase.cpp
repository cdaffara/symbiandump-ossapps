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

#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbscrollbar.h>
#include <HbGroupBox>
#include <HbMarqueeItem>
#include <HbLabel>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <HbColorScheme>

#include "irviewmanager.h"
#include "irabstractlistviewbase.h"
#include "irplaycontroller.h"
#include "irapplication.h"
#include "irqmetadata.h"
#include "irqisdsdatastructure.h"
#include "irqisdsclient.h"
#include "irqenums.h"
#include "iruidefines.h"
#include "irplayingbanner.h"
 
const int KAnimationLoopTimes = 2; // Animation loop times
#define NOW_PLAYING_BANNER_FRAME "qtg_fr_multimedia_trans"

IrAbstractListViewBase::IrAbstractListViewBase(IRApplication *aApplication, TIRViewId aViewId)
    : IRBaseView(aApplication, aViewId),
      iListView(NULL),
      iStationName(NULL),
      iArtistSongName(NULL),
      iHeadingLabel(NULL),
      iCollectionsAction(NULL), iFavoritesAction(NULL),
      iGenresAction(NULL), iSearchAction(NULL),
      iViewParameter(EIRViewPara_InvalidId)
{
    // Create widget hierarchy
    setObjectName( ABSTRACT_LIST_VIEW_BASE_OBJECT_VIEW );

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this ); // abstractlistviewbase is referred in the XML document
    iLoader.setObjectTree( roots );
    
    // Load the XML file
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME);
        
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION);
    
#ifdef SUBTITLE_STR_BY_LOCID
    setTitle(hbTrId("txt_irad_title_internet_radio"));
         
#else
    setTitle("Internet radio");      
#endif

    initMenu();
    initContentWidget();
    initScrollBar();
    initToolBar();
    
    connect(iListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(clickItem(const QModelIndex&)));
    connect(iListView, SIGNAL(longPressed(HbAbstractViewItem *,const QPointF&)), this, SLOT(listViewLongPressed(HbAbstractViewItem *,const QPointF&)));
    connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(metaDataAvailable(IRQMetaData*)));
    connect(iPlayController, SIGNAL(playingStopped()), this, SLOT(removeBanner()));
    connect(getViewManager(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(handleOrientationChanged(Qt::Orientation)));
}

IrAbstractListViewBase::~IrAbstractListViewBase()
{
}

void IrAbstractListViewBase::initMenu()
{
    HbMenu *viewMenu = menu();
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(prepareMenu()));

    QObject *openWebAddressAction = qobject_cast<HbAction *>(iLoader.findObject(GO_TO_STATION_ACTION));
    connect(openWebAddressAction, SIGNAL(triggered()), this, SLOT(openWebAddress()));

    QObject *settings = iLoader.findObject(SETTINGS_ACTION);
    connect(settings, SIGNAL(triggered()), this, SLOT(launchSettingsView())); 
    
    QObject *exitAction = iLoader.findObject(EXIT_ACTION);
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}

void IrAbstractListViewBase::initToolBar()
{
    QActionGroup *actionGroup = new QActionGroup(this);
    iCollectionsAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_COLLECTION_ACTION));
	iCollectionsAction->setActionGroup(actionGroup);
	iFavoritesAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_FAVORITE_ACTION));
	iFavoritesAction->setActionGroup(actionGroup);
	iGenresAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_GENRE_ACTION));
	iGenresAction->setActionGroup(actionGroup);	 
	iSearchAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_SEARCH_ACTION));
	iSearchAction->setActionGroup(actionGroup);

	connect(iCollectionsAction, SIGNAL(triggered()), this, SLOT(collectionsActionClicked()));
	connect(iFavoritesAction, SIGNAL(triggered()), this, SLOT(favoritesActionClicked()));
	connect(iSearchAction, SIGNAL(triggered()), this, SLOT(searchActionClicked()));
	connect(iGenresAction, SIGNAL(triggered()), iApplication, SLOT(loadGenre()));

	setCheckedAction();
}

void IrAbstractListViewBase::initContentWidget()
{
    iHeadingLabel = qobject_cast<HbGroupBox *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_HEADINGTEXTLABEL));
    QFont font;
    font.setBold(true);
    iHeadingLabel->setFont(font);
    
    IRPlayingBanner *playingBanner = qobject_cast<IRPlayingBanner *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_PLAYINGBANNER)); 
    iStationName = qobject_cast<HbLabel *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_STATIONNAME));
    iStationName->setFont(font);
    iArtistSongName = qobject_cast<HbMarqueeItem *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_ARTISTSONGNAME));
    iArtistSongName->setLoopCount(KAnimationLoopTimes);
    QColor color = HbColorScheme::color(KNowPlayingBannerColorNormal);
    iStationName->setTextColor(color);
    iArtistSongName->setTextColor(color);

    connect(playingBanner, SIGNAL(playingBannerTapFinished()), this, SLOT(playingBannerTapFinished()));

    iListView = qobject_cast<HbListView *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_LISTVIEW));
    iListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    
    // draw background for now playing banner
    HbFrameDrawer* drawer = new HbFrameDrawer(NOW_PLAYING_BANNER_FRAME, HbFrameDrawer::NinePieces);
    HbFrameItem* backgroundItem = new HbFrameItem(drawer, playingBanner);
    if (backgroundItem)
    {
        // set item to fill the whole widget
        backgroundItem->setGeometry(QRectF(QPointF(0, 0), playingBanner->size()));
        backgroundItem->setZValue(0);
        playingBanner->setBackgroundItem(backgroundItem);
    }
}

void IrAbstractListViewBase::initScrollBar()
{
    HbScrollBar *scrollbar = iListView->verticalScrollBar();
    scrollbar->setVisible(true);
    scrollbar->setInteractive(true);
    iListView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
}


void IrAbstractListViewBase::setViewParameter(TIRViewParameter aParameter)
{
    iViewParameter = aParameter;
}

TIRViewParameter IrAbstractListViewBase::getViewParameter() const
{
    return iViewParameter;
}

void IrAbstractListViewBase::setCheckedAction()
{       
    //default implementation
    QList<QAction*> actionList = iCollectionsAction->actionGroup()->actions();
    for (QList<QAction*>::const_iterator it = actionList.begin(); it != actionList.end(); ++it)
    {
        (*it)->setChecked(false);
    }
}

void IrAbstractListViewBase::setHeadingText(const QString &aText)
{
    if (iHeadingLabel)
    {
        iHeadingLabel->setHeading(aText);
    }
}

void IrAbstractListViewBase::resetCurrentItem()
{
    
}

void IrAbstractListViewBase::storeCurrentItem()
{
}

/*
 * handle the signal of one item being selected.
 * Default implementation is doing nothing.
 */
void IrAbstractListViewBase::handleItemSelected()
{
}

/*
 * we call the functin when we handle the handleitemselect.
 * The return value shows wether we need to connect to the
 * network, true means we connect and false means do not
 */
void IrAbstractListViewBase::itemAboutToBeSelected(bool &aNeedNetwork)
{    
    aNeedNetwork =  true;
}

TIRHandleResult IrAbstractListViewBase::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;
    
    switch (aCommand)
    {
    case EIR_ViewCommand_TOBEACTIVATED:
        updateView();
        break;
        
    case EIR_ViewCommand_ACTIVATED:
        iArtistSongName->stopAnimation();
        iArtistSongName->startAnimation();
        break;

    case EIR_ViewCommand_DEACTIVATE:
        storeCurrentItem();
        iArtistSongName->stopAnimation();
        ret = EIR_NoDefault;
        break;

    default:
        break;
    }

    return ret;
}

/*
 * When a view is shown, update the view.
 */
void IrAbstractListViewBase::updateView()
{
    updateBanner(getViewManager()->orientation());
  
    setCheckedAction();
}

void IrAbstractListViewBase::handleOrientationChanged(Qt::Orientation aOrientation)
{
    updateBanner(aOrientation);
}

void IrAbstractListViewBase::collectionsActionClicked()
{
    getViewManager()->activateView(EIRView_MainView);
}

void IrAbstractListViewBase::favoritesActionClicked()
{
    getViewManager()->activateView(EIRView_FavoritesView);
}

void IrAbstractListViewBase::searchActionClicked()
{
    getViewManager()->activateView(EIRView_SearchView);
}

void IrAbstractListViewBase::openWebAddress()
{
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

/*
 * dynamically set the text of menu item or remove menu item.
 * Concrete classes can override this function if needed
 */
void IrAbstractListViewBase::prepareMenu()
{
}

void IrAbstractListViewBase::updateBanner(Qt::Orientation aOrientation)
{
    if ((aOrientation == Qt::Vertical) && iPlayController->isPlaying())
    {
        IRQMetaData *metaData = iPlayController->getMetaData();
        if (metaData && "" != metaData->getArtistSongName().trimmed() &&
            "-" != metaData->getArtistSongName().trimmed())
        {
            //set the banner text as song name
            addBanner(metaData->getArtistSongName(), true);
        }
        else
        {
            //set the banner text as station name
            IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
            Q_ASSERT(nowPlayingPreset);
            addBanner(nowPlayingPreset->nickName, false);
        }
    }
    else
    {
        removeBanner();
    }
}

void IrAbstractListViewBase::addBanner(const QString &aText, const bool &aMetaDataFlag)
{
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_WITH_PLAYINGBANNER_SECTION);
    iStationName->setPlainText(iPlayController->getNowPlayingPreset()->nickName);
    if (aMetaDataFlag)
    {
        iArtistSongName->setText(aText);
        // As the metadata is changed, the animation should be started again.
        iArtistSongName->stopAnimation();
        iArtistSongName->startAnimation();
    }
    else
    {
        iArtistSongName->setText(" ");
    }

}

void IrAbstractListViewBase::launchSettingsView()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

void IrAbstractListViewBase::metaDataAvailable(IRQMetaData* aMetaData)
{
    Q_UNUSED(aMetaData);

    if (getViewManager()->currentView() != this)
    {
        return;
    }

    updateBanner(getViewManager()->orientation());
}

void IrAbstractListViewBase::removeBanner()
{
    iArtistSongName->stopAnimation();
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION);
}

void IrAbstractListViewBase::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF &aCoords)
{    
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);
}

void IrAbstractListViewBase::playingBannerTapFinished()
{
    getViewManager()->activateView(EIRView_PlayingView);
}

void IrAbstractListViewBase::clickItem(const QModelIndex &aIndex)
{   
    Q_UNUSED(aIndex);
    bool needNetwork;
    itemAboutToBeSelected(needNetwork);     
    if (needNetwork)
    {
        setUseNetworkReason(EIR_UseNetwork_SelectItem);
        if (false == iApplication->verifyNetworkConnectivity())
        {
            return;
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
    }

    handleItemSelected();          
}
 
