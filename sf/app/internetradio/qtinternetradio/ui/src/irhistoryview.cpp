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
#include <hbnotificationdialog.h>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irqisdsclient.h"
#include "irplaycontroller.h"
#include "irhistoryview.h"
#include "irqisdsdatastructure.h"
#include "irhistorymodel.h"
#include "irqnetworkcontroller.h" 
#include "irqenums.h"
#include "irqfavoritesdb.h"
#include "irstationdetailsview.h"
#include "iruidefines.h"


const QString KActionAddName("Add");
const QString KActionDeleteName("Delete");
const QString KActionDetailsName("Details");

//                                         public functions

/*
 * Description : constructor
 */
IRHistoryView::IRHistoryView(IRApplication *aApplication, TIRViewId aViewId) :
    IrAbstractListViewBase(aApplication, aViewId), iClearHistoryAction(NULL)
{   
    iModel = new IRHistoryModel(this);
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
#ifdef SUBTITLE_STR_BY_LOCID
    iClearHistoryAction = new HbAction(hbTrId("txt_irad_opt_clear_list"), menu());
#else
    iClearHistoryAction = new HbAction(hbTrId("Clear list"), menu());    
#endif
    
    connect(iClearHistoryAction, SIGNAL(triggered()), this, SLOT(popupClearHistoryConfirmMessageBox()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
    this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
}
void IRHistoryView::popupClearHistoryConfirmMessageBox()
{
#ifdef SUBTITLE_STR_BY_LOCID
    HbMessageBox::question(hbTrId("txt_irad_info_clear_station_list"), this, SLOT(clearAllList(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);
#else
    HbMessageBox::question(hbTrId("Clear station list?"), this, SLOT(clearAllList(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);    
#endif
}
/* 
 * Description : destructor
 */
IRHistoryView::~IRHistoryView()
{
}

/*
 * Description : virtual functions from base class IRBaseView. 
 *               handle view commands
 * 
 * see also    : IRBaseView::handleCommand
 */
TIRHandleResult IRHistoryView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {
        
    case EIR_ViewCommand_TOBEACTIVATED:       
        showHistory();
        ret = EIR_NoDefault;
        break;
                
    case EIR_ViewCommand_ACTIVATED:
        iModel->startDownloadingLogo();
        ret = EIR_NoDefault;
        break;

    case EIR_ViewCommand_DEACTIVATE:
        iModel->stopDownloadingLogo();
        ret = EIR_NoDefault;
        break;

    default:
        break;
    }

    return ret;
}

//                                 slots functions 

/*
 * Description : slot function when an item in a list is clicked.
 *               issue a listen request to isds client
 * Parameters  : aItem : pointer to the clicked item.
 */
void IRHistoryView::handleItemSelected()
{
    int index = iListView->currentIndex().row();
    IRQPreset *preset = iModel->getHistoryInfo(index);
    if (NULL == preset)
    {
        return;
    }

    if (preset->type)
    {
        // channel from isds server
        iPlayController->connectToChannel(preset, EIRQHistoryIsds);
    }
    else
    {
        // user defined channel
        iPlayController->connectToChannel(preset,EIRQHistoryAdhoc);
    }
}

#ifdef HS_WIDGET_ENABLED
void IRHistoryView::itemAboutToBeSelected(bool &aNeedNetwork)
{
    aNeedNetwork =  true;
    
    int index = iListView->currentIndex().row();
    iPlayController->setConnectingStationName(iModel->getHistoryInfo(index)->nickName); 
}
#endif

void IRHistoryView::networkRequestNotified(IRQNetworkEvent aEvent)
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

// ---------------------------------------------------------------------------
// IRHistoryView::showHistory()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRHistoryView::showHistory()
{
    if (iModel->checkHistoryUpdate())
    {
        iListView->reset();
        iListView->setCurrentIndex(iModel->index(0)); 
    }
}

// ---------------------------------------------------------------------------
// IRHistoryView::clearAllList()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRHistoryView::clearAllList(HbAction *aAction)
{
    HbMessageBox *dialog = static_cast<HbMessageBox*>(sender());
    if (dialog)
    {
        if (aAction == dialog->actions().at(0))
        {
            iModel->clearAllHistory();
            iListView->reset();
        }
    }    
}

void IRHistoryView::prepareMenu()
{
    HbMenu *viewMenu = menu();
    
    viewMenu->removeAction(iClearHistoryAction);

    HbAction * settingAction = qobject_cast<HbAction *>(iLoader.findObject(SETTINGS_ACTION));

    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(settingAction, iClearHistoryAction);
    }
} 

void IRHistoryView::modelChanged()
{
#ifdef SUBTITLE_STR_BY_LOCID
    QString headingStr = hbTrId("txt_irad_subtitle_recently_played_stations") + " (" + QString::number(iModel->rowCount()) + ")"; 
#else
    QString headingStr = hbTrId("Recently played stations") + " (" + QString::number(iModel->rowCount()) + ")";
#endif
  
    setHeadingText(headingStr);
}
 
void IRHistoryView::actionClicked(HbAction *aAction)
{
    if ( aAction )
    {
        QString objectName = aAction->objectName();
        if ( objectName == KActionAddName )
        {
            addContextAction();
        }
        else if( objectName == KActionDeleteName)
        {
            deleteContextAction();
        }
        else if( objectName == KActionDetailsName)
        {
            detailsContextAction();
        }
    }
}

void IRHistoryView::addContextAction()
{        
    QModelIndex current = iListView->currentIndex();     
    IRQPreset * preset = iModel->getHistoryInfo(current.row());
    int retValue = iFavorites->addPreset(*preset);

    HbNotificationDialog *add2FavNote = new HbNotificationDialog();
    add2FavNote->setModal(true);
    add2FavNote->setTimeout(HbPopup::ConfirmationNoteTimeout);
    add2FavNote->setAttribute(Qt::WA_DeleteOnClose);
        
    switch (retValue)
    {
    case EIRQErrorNone:
#ifdef SUBTITLE_STR_BY_LOCID
        add2FavNote->setTitle(hbTrId("txt_irad_info_added_to_favorites"));
#else
        add2FavNote->setTitle(hbTrId("Added to Favorites"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();
        break;

    case EIRQErrorOutOfMemory:
#ifdef SUBTITLE_STR_BY_LOCID
        add2FavNote->setTitle(hbTrId("txt_irad_info_can_not_add_more"));
#else
        add2FavNote->setTitle(hbTrId("Can't add more"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();        
        break;

    case EIRQErrorAlreadyExist:
#ifdef SUBTITLE_STR_BY_LOCID
        add2FavNote->setTitle(hbTrId("txt_irad_info_favorite_updated"));
#else
        add2FavNote->setTitle(hbTrId("Favorite updated"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();           
        break;
 
    default:
        Q_ASSERT(false);         
        break;
    }    
} 

void IRHistoryView::deleteContextAction()
{
    int current = iListView->currentIndex().row();     
    bool ret = iModel->deleteHistory(current);     
    if( !ret )
	  {
#ifdef SUBTITLE_STR_BY_LOCID
	    popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
#else
	    popupNote(hbTrId("Operation failed"), HbMessageBox::MessageTypeWarning);	    
#endif
	  }
}
void IRHistoryView::detailsContextAction()
{   
    int selectedItemIndex = iListView->currentIndex().row();
    IRQPreset *channelPreset = iModel->getHistoryInfo(selectedItemIndex);

    IRStationDetailsView *stationDetailsView = static_cast<IRStationDetailsView*>(getViewManager()->getView(EIRView_StationDetailsView, true));    
    stationDetailsView->setDetails(channelPreset);

    getViewManager()->activateView(EIRView_StationDetailsView);
}

void IRHistoryView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords)
{
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);
    
    HbAction *action = NULL;
    HbMenu *contextMenu = new HbMenu;
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));
    
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_irad_menu_add_to_favorite"));
#else
    action = contextMenu->addAction(hbTrId("Add to favorites"));    
#endif
    action->setObjectName(KActionAddName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
#else
    action = contextMenu->addAction(hbTrId("Delete"));    
#endif
    action->setObjectName(KActionDeleteName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_details"));
#else
    action = contextMenu->addAction(hbTrId("Details"));    
#endif
    action->setObjectName(KActionDetailsName);
    
    contextMenu->setPreferredPos(aCoords);
    contextMenu->open();
}

 
 
 

 
