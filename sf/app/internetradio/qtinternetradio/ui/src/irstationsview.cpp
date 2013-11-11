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
#include <QPixmap>
#include <hblistview.h>
#include <hbaction.h>
#include <QTimer>
#include <hbscrollbar.h>
#include <hgwidgets/hgcacheproxymodel.h>

#include "irviewmanager.h"
#include "irstationsview.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irqisdsclient.h"
#include "irqenums.h"
#include "irqutility.h"
#include "irchanneldataprovider.h"
#include "irqisdsdatastructure.h"
#include "irqnetworkcontroller.h"

 
const uint KConnectTimeOut = 15000; //if the connecting take more than 15 seconds, we will show a pop up

//                                      public functions

/*
 * Description : constructor
 */
IRStationsView::IRStationsView(IRApplication* aApplication, TIRViewId aViewId) 
                               : IrAbstractListViewBase(aApplication, aViewId),
                               iPreset(NULL), 
                               iLastSelectitem(0)                        
{       
    
    iDataProvider = new IRChannelDataProvider(this);
    HgCacheProxyModel *model = new HgCacheProxyModel(this);
    model->setDataProvider(iDataProvider, KCacheSize, KCacheThreshold);
    iListView->setModel(model);
    
    iConnectTimer = new QTimer(this);
    iConnectTimer->setInterval(KConnectTimeOut);     
     
    connect(iDataProvider, SIGNAL(dataAvailable()), this, SLOT(dataChanged()));    
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));    
    connect(iConnectTimer, SIGNAL(timeout()), this, SLOT(connectTimeOut()));
}

/*
 * Description : destructor
 */
IRStationsView::~IRStationsView()
{
    delete iPreset;
    iPreset = NULL;
}

void IRStationsView::loadCategoryStations(int aIndex, const QString &aHeadingText)
{
    connectToIsdsClient();
    setHeadingText(aHeadingText);
    
    bool cache = false;
    iIsdsClient->isdsChannelRequest(aIndex, cache);
    
    if (!cache)
    {
        iApplication->startLoadingAnimation(this, SLOT(cancelRequest()));
    }
}

void IRStationsView::storeCurrentItem()
{
    iLastSelectitem = iListView->currentIndex().row();
}

void IRStationsView::resetCurrentItem()
{
    iLastSelectitem = 0;
}

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRStationsView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {   
    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                this, SLOT(presetResponse(IRQPreset *)));
        
        iDataProvider->activate();
        
        ret = EIR_NoDefault;
        break;
        
    case EIR_ViewCommand_DEACTIVATE:
        iConnectTimer->stop();       
        iDataProvider->deactivate();
        
        disconnect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                   this, SLOT(presetResponse(IRQPreset *)));       
        ret = EIR_NoDefault;
        break;

    default:
        break;
    }
    
    return ret;
}

#ifdef HS_WIDGET_ENABLED
void IRStationsView::itemAboutToBeSelected(bool &aNeedNetwork)
{
    aNeedNetwork =  true;
    
    int index = iListView->currentIndex().row();
    iPlayController->setConnectingStationName(iDataProvider->getChannelItemByIndex(index)->channelName,true);
}
#endif

//                                      slots functions

/*
 * Description : slot function when an item in a list is clicked.
 *               issue a listen request to isds client
 * Parameters  : aItem : pointer to the clicked item.
 */
void IRStationsView::handleItemSelected()
{
    if (iListView)
    {
        int index = iListView->currentIndex().row();
        if (index != -1)  
        {
            if (iIsdsClient)
            {
                connect(iIsdsClient, SIGNAL(operationException(IRQError)),
                        this, SLOT(operationException(IRQError)));
                
                //once an item is selected, we show a dialog to prevent user from clicking the
                //item again
                iApplication->startLoadingAnimation(this, SLOT(cancelRequest()));
                
                if (iIsdsClient->isdsIsChannelBanner())
                {
                    iIsdsClient->isdsListenRequest(index + 1);
                }
                else
                {
                    iIsdsClient->isdsListenRequest(index);
                }
            }
        }
    }
}

/*
 * Description : data is received from isds client. Load the data to the list widget and display
 *               the stations view
 */
void IRStationsView::dataChanged()
{
    iConnectTimer->stop();
    
    disconnectIsdsClient();

    iListView->reset();
    iListView->setCurrentIndex(iDataProvider->index(iLastSelectitem, 0));
    iListView->scrollTo(iDataProvider->index(iLastSelectitem, 0));
    qreal value = 0.0;
    if (iListView->model()->rowCount() > 0)
    {
        value = iLastSelectitem / iListView->model()->rowCount();
    }
    iListView->verticalScrollBar()->setValue(value);

    getViewManager()->activateView(this);
    iApplication->stopLoadingAnimation();
}

/*
 * Description : the preset of the selected station is received from isds client.
 *               application is responsible for deleting old preset.
 *               call player controler to play the station
 * Parameters  : aPreset : the preset of the selected station            
 */
void IRStationsView::presetResponse(IRQPreset *aPreset)
{
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
               this, SLOT(operationException(IRQError)));
    
    delete iPreset;
    iPreset = aPreset;
    
    if (iPreset)
    {
        iPlayController->connectToChannel(iPreset,EIRQIsds);
    }
}


/*
 * Description  : slot function for operation failure. The signal is emitted by isds client
 * Parameters   : aError : see the definition of IRQError
 */
void IRStationsView::operationException(IRQError aError)
{
    iApplication->stopLoadingAnimation();

    disconnectIsdsClient();
#ifdef SUBTITLE_STR_BY_LOCID
    QString errorString = hbTrId("txt_irad_info_failed_to_connect");
#else
    QString errorString = hbTrId("Connecting failed");    
#endif
    
    switch (aError)
    {   
    case EIRQErrorNotFound:
#ifdef SUBTITLE_STR_BY_LOCID
        errorString = hbTrId("txt_irad_info_no_matching_station_found");
#else
        errorString = hbTrId("No matching station found");        
#endif
        if (iConnectTimer->isActive())
        {
            iConnectTimer->stop();
        }
        break;
        default:
            break;
    }
    
    popupNote(errorString, HbMessageBox::MessageTypeWarning);
}

void IRStationsView::cancelRequest()
{
    if( iConnectTimer->isActive())
    {
        iConnectTimer->stop();
    }    
    iIsdsClient->isdsCancelRequest();
    disconnectIsdsClient();
    iApplication->stopLoadingAnimation();
}

void IRStationsView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            handleItemSelected();
        }
        
        break;
        
    default:
        setCheckedAction();       
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

void IRStationsView::connectToIsdsClient()
{
    connect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
            iDataProvider, SLOT(updateData(QList<IRQChannelItem *> *)));
    
    connect(iIsdsClient, SIGNAL(operationException(IRQError)),
            this, SLOT(operationException(IRQError)));
}

void IRStationsView::disconnectIsdsClient()
{
    disconnect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
               iDataProvider, SLOT(updateData(QList<IRQChannelItem *> *)));
    
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
               this, SLOT(operationException(IRQError)));
}

void IRStationsView::connectTimeOut()
{    
    iConnectTimer->stop();
    cancelRequest();   
#ifdef SUBTITLE_STR_BY_LOCID
    popupNote(hbTrId("txt_irad_info_connecting_timout"), HbMessageBox::MessageTypeWarning);
#else
    popupNote(hbTrId("Connecting timeout"), HbMessageBox::MessageTypeWarning);    
#endif
}
