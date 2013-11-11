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

#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>

#include "irplsview.h"
#include "irapplication.h"
#include "irviewmanager.h"
#include "irqnetworkcontroller.h"
#include "irplsmodel.h"
#include "irqisdsdatastructure.h"
#include "irplaycontroller.h"
#include "irplaylist.h"
#include "iruidefines.h"

IRPlsView::IRPlsView(IRApplication* aApplication, TIRViewId aViewId) : 
                     IrAbstractListViewBase(aApplication, aViewId), iPlsModel(NULL)
{
    setFlag(EViewFlag_ClearStackWhenActivate);
    
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    
    IRPlayList *playList = iApplication->getPlayList();
    setHeadingText(playList->getFileName());
        
    iPlsModel = new IRPlsModel(playList, this);
    iListView->setModel(iPlsModel);
    iListView->setCurrentIndex(iPlsModel->index(0));
}

IRPlsView::~IRPlsView()
{
}

void IRPlsView::networkRequestNotified(IRQNetworkEvent aEvent)
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
            break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

#ifdef HS_WIDGET_ENABLED
void IRPlsView::itemAboutToBeSelected(bool &aNeedNetwork)
{
    aNeedNetwork =  true;
    
    int currentRow = iListView->currentIndex().row();
    iPlayController->setConnectingStationName(iPlsModel->getTitle(currentRow));
}
#endif

void IRPlsView::handleItemSelected()
{
    int currentRow = iListView->currentIndex().row();
    IRQChannelServerURL server;
    server.bitrate = 0;
    server.url = iPlsModel->getFile(currentRow);
    server.serverName = iPlsModel->getTitle(currentRow);
    IRQPreset preset;
    preset.insertChannelServer(server);
    preset.name = iPlsModel->getTitle(currentRow);
    preset.nickName = iPlsModel->getTitle(currentRow);
    preset.description = "";
    preset.shortDesc = "";
    preset.type = 0;
    preset.uniqID = 0;
    preset.presetId = 0;

    iPlayController->connectToChannel(&preset, EIRQAdhocExternal);
}

void IRPlsView::prepareMenu()
{
    HbMenu *viewMenu = menu();
    
    HbAction *openWebAddressAction = qobject_cast<HbAction *>(iLoader.findObject(GO_TO_STATION_ACTION));
    HbAction *settings = qobject_cast<HbAction *>(iLoader.findObject(SETTINGS_ACTION));
    viewMenu->removeAction(openWebAddressAction);
    viewMenu->removeAction(settings);
}

