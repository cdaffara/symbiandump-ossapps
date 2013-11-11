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

#include <hbaction.h>
#include <hblistview.h>
#include <hbscrollbar.h>

#include "irapplication.h"
#include "irviewmanager.h"
#include "ircategoryview.h"
#include "irstationsview.h"
#include "irqnetworkcontroller.h"
#include "ircategorymodel.h"
#include "iruidefines.h"

//                                    public functions             

/*
 * Description : constructor
 */
IRCategoryView::IRCategoryView(IRApplication* aApplication, TIRViewId aViewId) 
                               : IrAbstractListViewBase(aApplication, aViewId),
                                 iLastSelectItem(0)
{	
    setViewParameter(EIRViewPara_Genre);
    setFlag(EViewFlag_ClearStackWhenActivate|EViewFlag_StickyViewEnabled);
    
    iModel = new IRCategoryModel(this);
    iListView->setModel(iModel);
    
    connect(iModel, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
    
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)), this,
        SLOT(networkRequestNotified(IRQNetworkEvent)));

}

/*
 * Description : destructor
 */
IRCategoryView::~IRCategoryView()
{
}

/*
 * Description : virtual functions from base class IRBaseView. 
 *               handle view commands
 * 
 * see also    : IRBaseView::handleCommand
 */
TIRHandleResult IRCategoryView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{    
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {
        case EIR_ViewCommand_TOBEACTIVATED:
        {
            if (EIRViewPara_Genre == iViewParameter)
            {
                if (ABSTRACT_LIST_VIEW_BASE_WITH_TOOLBAR_SECTION != iLoadedSection)
                {
                    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_WITH_TOOLBAR_SECTION);
                    iLoadedSection = ABSTRACT_LIST_VIEW_BASE_WITH_TOOLBAR_SECTION;
                }
            }
            else
            {
                if (ABSTRACT_LIST_VIEW_BASE_NO_TOOLBAR_SECTION != iLoadedSection)
                {
                    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_NO_TOOLBAR_SECTION);
                    iLoadedSection = ABSTRACT_LIST_VIEW_BASE_NO_TOOLBAR_SECTION;
                    setToolBar(NULL);
                }
            }
        }
        break;

        case EIR_ViewCommand_ACTIVATED:
            if (EIRViewPara_Genre == getViewParameter())
            {
                getViewManager()->saveScreenShot();
            }
            break;
            
        default:
            break;
    }
    
    return ret;
}

void IRCategoryView::launchAction()
{    
    Q_ASSERT(EIRViewPara_Genre == iViewParameter);
    bool ret = iIsdsClient->isdsIsCategoryCached(IRQIsdsClient::EGenre);

    if ( !ret )
    {
        setUseNetworkReason(EIR_UseNetwork_StartingView);
        iApplication->verifyNetworkConnectivity();
    }   
    else
    {
        IRQIsdsClient::IRQIsdsClientInterfaceIDs request =
                IRQIsdsClient::EGenre;

        switch (iViewParameter)
        {
            case EIRViewPara_Genre:
                request = IRQIsdsClient::EGenre;
                break;

            case EIRViewPara_Language:
                request = IRQIsdsClient::ELanguages;
                break;

            case EIRViewPara_Country:
                request = IRQIsdsClient::ECountries;
                break;

            default:
                break;
        }

        connectToIsdsClient();
        bool cache = false;
        iIsdsClient->isdsCategoryRequest(request, cache);
    }
}

void IRCategoryView::loadCategory(IRQIsdsClient::IRQIsdsClientInterfaceIDs aCategory)
{
    connectToIsdsClient();
    
    bool cache = false;
    switch (aCategory)
    {
    case IRQIsdsClient::EGenre : 
        if (EIRViewPara_Genre != getViewParameter())
        {
            //the view is used for other categories other than Genre
            resetCurrentItem();
        }
        setViewParameter(EIRViewPara_Genre);
        setFlag(EViewFlag_ClearStackWhenActivate|EViewFlag_StickyViewEnabled);
        iIsdsClient->isdsCategoryRequest(IRQIsdsClient::EGenre, cache);
        break;
        
    case IRQIsdsClient::ELanguages : 
#ifdef SUBTITLE_STR_BY_LOCID
        setHeadingText(hbTrId("txt_irad_list_stations_by_language"));
#else
        setHeadingText(hbTrId("Stations by language"));        
#endif
        if (EIRViewPara_Language != getViewParameter())
        {
            //the view is used for other categories other than Language
            resetCurrentItem();
        }
        setViewParameter(EIRViewPara_Language);
        clearFlag(EViewFlag_ClearStackWhenActivate);
        clearFlag(EViewFlag_StickyViewEnabled);
        iIsdsClient->isdsCategoryRequest(IRQIsdsClient::ELanguages, cache);
        break;
        
    case IRQIsdsClient::ECountries :
#ifdef SUBTITLE_STR_BY_LOCID
        setHeadingText(hbTrId("txt_irad_list_stations_by_country_region"));
#else
        setHeadingText(hbTrId("Stations by country/region"));        
#endif
        if (EIRViewPara_Country != getViewParameter())
        {
            //the view is used for other categories other than Country
            resetCurrentItem();
        }
        setViewParameter(EIRViewPara_Country);
        clearFlag(EViewFlag_ClearStackWhenActivate);
        clearFlag(EViewFlag_StickyViewEnabled);
        iIsdsClient->isdsCategoryRequest(IRQIsdsClient::ECountries, cache);
        break;
        
    default:
        Q_ASSERT(false);
        break;
    }
    
    if (!cache)
    {
        iApplication->startLoadingAnimation(this, SLOT(cancelRequest()));
    }
}

void IRCategoryView::storeCurrentItem()
{
    iLastSelectItem = iListView->currentIndex().row();
}

void IRCategoryView::resetCurrentItem()
{
    iLastSelectItem = 0;
    IRBaseView *stationView = getViewManager()->getView(EIRView_StationsView);
    if( stationView )
    {
        static_cast<IrAbstractListViewBase*>(stationView)->resetCurrentItem();
    }
}

/*
 * Description : slot function for active network event
 * Parameters  : aEvent, see the definition of IRQNetworkEvent
 */
void IRCategoryView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    Q_ASSERT(iViewParameter != EIRViewPara_InvalidId);
    
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        if (EIR_UseNetwork_StartingView == getUseNetworkReason())
        {
            IRQIsdsClient::IRQIsdsClientInterfaceIDs request = IRQIsdsClient::EGenre;
        
            switch (iViewParameter)
            {
            case EIRViewPara_Genre:
                request = IRQIsdsClient::EGenre;
                break;

            case EIRViewPara_Language:
                request = IRQIsdsClient::ELanguages;
                break;

            case EIRViewPara_Country:
                request = IRQIsdsClient::ECountries;
                break;

            default:
                break;
            }

            connectToIsdsClient();
            bool cache = false;
            iIsdsClient->isdsCategoryRequest(request, cache);
            iApplication->startLoadingAnimation(this, SLOT(cancelRequest()));
        }
        //for there may be some cache, and when we click, we need to handle here        
        else if ( EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            handleItemSelected();               
        }        
        
        break;
       
    case EIRQConnectingCancelled:
    case EIRQDisplayNetworkMessageNoConnectivity:
        if (iListView->model()->rowCount() == 0)
        {
            getViewManager()->activateView(EIRView_MainView);
        }
        else
        {
            setCheckedAction();
        }
        
        break;
        
    default:
        setCheckedAction();
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}


//                                 slots functions 

void IRCategoryView::itemAboutToBeSelected(bool &aNeedNetwork)
{
 
    if (iIsdsClient && iListView)
    {
        int index = iListView->currentIndex().row();         
        if (iIsdsClient->isdsIsCategoryBanner())
        {
            ++index;
        }       
         
        aNeedNetwork = !iIsdsClient->isdsIsChannelCached(index);              
    }       
}
/*
 * Description  : slot function when an item in a list is clicked. call isds client to get 
 *                channel list under the clicked category.
 * Parameters   : aItem  : pointer to the clicked item
 * Return       : None
 */
void IRCategoryView::handleItemSelected()
{
    if (iIsdsClient && iListView)
    {
        int index = iListView->currentIndex().row();
        if ( iLastSelectItem != index )
        {
            IRBaseView *stationView = getViewManager()->getView(EIRView_StationsView);
            if( stationView )
            {
                static_cast<IrAbstractListViewBase*>(stationView)->resetCurrentItem();
            }
        }
        iLastSelectItem = index;
        if (iIsdsClient->isdsIsCategoryBanner())
        {
            ++index;
        }
        
        const QString& headingText = iModel->categoryName(iListView->currentIndex());
        IRStationsView *stationsView =
        static_cast<IRStationsView*>(getViewManager()->getView(EIRView_StationsView, true));
        stationsView->loadCategoryStations(index, headingText);
    }
}

/*
 * Description : data is received from isds client. Load the data to the list widget and display
 *               the category view
 * 
 */
void IRCategoryView::dataChanged()
{
    disconnectIsdsClient();
    iApplication->stopLoadingAnimation();

    iListView->reset();
    iListView->setCurrentIndex(iModel->index(iLastSelectItem));
    iListView->scrollTo(iModel->index(iLastSelectItem));
    qreal value = 0.0;
    if (iListView->model()->rowCount() > 0)
    {
        value = iLastSelectItem / iListView->model()->rowCount();
    }
    iListView->verticalScrollBar()->setValue(value);
    
    getViewManager()->activateView(this);
}

/*
 * Description : slot function for canceling request.
 *               Before data is received, cancel requests through isds client
 */
void IRCategoryView::cancelRequest()
{
    iIsdsClient->isdsCancelRequest();
    disconnectIsdsClient();
    iApplication->stopLoadingAnimation();
    
    //if this function is called and this view is current view, it indicates that this view is starting view and 
    //data has not been loaded yet, so we need to back to collections view
    if (getViewManager()->currentView() == this)
    {
        getViewManager()->activateView(EIRView_MainView);
    }
    if((getViewManager()->currentViewId() == EIRView_MainView) || (getViewManager()->currentViewId() == EIRView_FavoritesView))
    {
        IRBaseView *baseView = static_cast<IRBaseView*>(getViewManager()->currentView());
        baseView->updateView();
    }
}

/*
 * Description  : slot function for operation failure. The signal is emitted by isds client
 * Parameters   : aError : see the definition of IRQError
 */
void IRCategoryView::operationException(IRQError aError)
{
    Q_UNUSED(aError);
    iApplication->stopLoadingAnimation();

    disconnectIsdsClient();
    
#ifdef SUBTITLE_STR_BY_LOCID
    popupNote(hbTrId("txt_irad_info_failed_to_connect"), HbMessageBox::MessageTypeWarning);
#else
    popupNote(hbTrId("Connecting failed"), HbMessageBox::MessageTypeWarning);
#endif
    
    //if category view is starting view and failed to get data from ISDS server, back to main view
    if (getViewManager()->currentView() == this &&
        iListView->model()->rowCount() == 0)
    {
        getViewManager()->activateView(EIRView_MainView);
    }
    if((getViewManager()->currentViewId() == EIRView_MainView) || (getViewManager()->currentViewId() == EIRView_FavoritesView))
    {
        IRBaseView *baseView = static_cast<IRBaseView*>(getViewManager()->currentView());
        baseView->updateView();
    }
}

//                                        private functions

void IRCategoryView::connectToIsdsClient()
{
    connect(iIsdsClient, SIGNAL(categoryItemsChanged(QList<IRQBrowseCategoryItem *> *)),
            iModel, SLOT(categoryReceived(QList<IRQBrowseCategoryItem *> *)));
    connect(iIsdsClient, SIGNAL(operationException(IRQError)),
            this, SLOT(operationException(IRQError)));
}

void IRCategoryView::disconnectIsdsClient()
{
    disconnect(iIsdsClient, SIGNAL(categoryItemsChanged(QList<IRQBrowseCategoryItem *> *)),
               iModel, SLOT(categoryReceived(QList<IRQBrowseCategoryItem *> *)));
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
               this, SLOT(operationException(IRQError)));
}

/*
 * Description : set the checked action in toolbar. For categoryview, iGenresAction should be
 *               checked if the view is for genre display. For other categories, iGenresAction
 *               should not be checked
 */
void IRCategoryView::setCheckedAction()
{
    if (EIRViewPara_Genre == getViewParameter())
    {
        iGenresAction->setChecked(true);
    }
    else
    {
        QList<QAction*> actionList = iCollectionsAction->actionGroup()->actions();
        for (QList<QAction*>::const_iterator it = actionList.begin(); it != actionList.end(); ++it)
        {
            (*it)->setChecked(false);
        }
    }
}

