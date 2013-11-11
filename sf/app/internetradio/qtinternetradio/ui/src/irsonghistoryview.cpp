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
#include <hbicon.h>
#include <QTimer> 
#include <QVariant>

#include "irviewmanager.h"
#include "irapplication.h" 
#include "irplaycontroller.h"
#include "irsonghistoryview.h" 
#include "irsonghistorymodel.h"
#include "irqsonghistoryinfo.h" 
#include "irqenums.h"
#include "irqstatisticsreporter.h"
#include "irqsettings.h"
#include "irqutility.h"
#include "iruidefines.h"
#include "irdbwrapper.h"
#include "irqisdsdatastructure.h"

const QString KActionSearchInMusicStoreName("SearchInMusicStore");
const QString KActionDeleteName("Delete");

#ifdef STATISTIC_REPORT_TEST_ENABLED
static const int KIRMusicStoreUid           = 0xE609761B;
#else
static const int KIRMusicStoreUid           = 0;
#endif

//                                         public functions

/*
 * Description : constructor
 */
IRSongHistoryView::IRSongHistoryView(IRApplication *aApplication, TIRViewId aViewId) :
    IrAbstractListViewBase(aApplication, aViewId), iClearSongHistoryAction(NULL),
    iShowPrompt(false)
{         
        
    iModel = new IRSongHistoryModel(this);
    iModel->setOrientation(getViewManager()->orientation());
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
#ifdef SUBTITLE_STR_BY_LOCID
    iClearSongHistoryAction = new HbAction(hbTrId("txt_irad_opt_clear_list"), this);
#else
    iClearSongHistoryAction = new HbAction(hbTrId("Clear list"), this);    
#endif
    iStatisticsReporter = IRQStatisticsReporter::openInstance();

    iShowPrompt = iSettings->getSongHistoryShow();    
    if( iShowPrompt )
    {
        iSettings->setSongHistoryShow(0);
    }  
    
    connect(iClearSongHistoryAction, SIGNAL(triggered()), this, SLOT(popupClearHistoryConfirmMessageBox()));    
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));        
}

/* 
 * Description : destructor
 */
IRSongHistoryView::~IRSongHistoryView()
{
    if (iStatisticsReporter)
    {
        iStatisticsReporter->closeInstance();
    }
}

/*
 * Description : virtual functions from base class IRBaseView. 
 *               handle view commands
 * 
 * see also    : IRBaseView::handleCommand
 */
TIRHandleResult IRSongHistoryView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{     
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
     

    switch (aCommand)
    {
    case EIR_ViewCommand_TOBEACTIVATED:       
        showSongHistory();
        ret = EIR_NoDefault;
        break;
                
    case EIR_ViewCommand_ACTIVATED:        
        connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(newMetadataAdded(IRQMetaData*)));
        if( iShowPrompt )
        {             
            QTimer::singleShot(5, this, SLOT(showPrompt()));
            iShowPrompt = false;
        }         
        ret = EIR_NoDefault;       
        break;

    case EIR_ViewCommand_DEACTIVATE:        
        disconnect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(newMetadataAdded(IRQMetaData*)));         
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
void IRSongHistoryView::handleItemSelected()
{     
    if (!IRQUtility::findAppByUid(KIRMusicStoreUid))  // if no music store, coming soon is shown.
    {
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_music_store_not_available"), HbMessageBox::MessageTypeInformation);
#else
        popupNote(hbTrId("Service Coming Soon"), HbMessageBox::MessageTypeInformation);    
#endif        
        return;
    }
    
    IRQSongInfo *hisInfo = iModel->getSongHistoryInfo(iListView->currentIndex().row()); 
    int presetType = 0; // user defined
    
    IRDBWrapper irDb;
    columnMap selectCriteriaSet;
    selectCriteriaSet.insert(channelId,hisInfo->getChannelID());
    
    QList<QVariant*>* dataSet = NULL;
    dataSet = irDb.getIRDB(&selectCriteriaSet);
    
    if (dataSet && (dataSet->size()>0))
    {
        presetType = (*(dataSet->at(0) + channelType)).toInt(); 
    }    
    
    if (dataSet)
    {
        while(false == dataSet->isEmpty())
        {
            delete []dataSet->takeFirst();
        }
        dataSet->clear();
        
        delete dataSet;
        dataSet = NULL;        
    }
    
    bool launchResult = false;
    if( hisInfo && ( 0 != hisInfo->getMusicshopStatus().compare("yes",Qt::CaseInsensitive) ) )
    {
        // TODO : launch music store with search result page
        launchResult = IRQUtility::launchAppByUid(KIRMusicStoreUid);
        if (launchResult)        
        {
            if (presetType)
            {
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsFind,iPlayController->getNowPlayingPreset()->presetId);
            }
            else
            {            
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsFind,0);
            }
        }
    }
    else
    {
        // TODO : lunch music store with homepage  
        launchResult = IRQUtility::launchAppByUid(KIRMusicStoreUid);
        if (launchResult)        
        {
            if (presetType)
            {
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsLaunch,iPlayController->getNowPlayingPreset()->presetId);
            }
            else
            {            
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsLaunch,0);
            }
        }          
    }
    
    if (!launchResult)
    {        
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_music_store_not_available"), HbMessageBox::MessageTypeInformation);
#else
        popupNote(hbTrId("Music store not ready"), HbMessageBox::MessageTypeInformation);    
#endif
    }    
}
   

// ---------------------------------------------------------------------------
// IRSongHistoryView::showSongHistory()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRSongHistoryView::showSongHistory()
{
    if (iModel->checkSongHistoryUpdate())
    {
        iListView->reset();
        iListView->setCurrentIndex(iModel->index(0));        
    }
}

void IRSongHistoryView::prepareMenu()
{
    HbMenu *viewMenu = menu();

    viewMenu->removeAction(iClearSongHistoryAction);

    HbAction * settingAction = qobject_cast<HbAction *>(iLoader.findObject(SETTINGS_ACTION));
    
    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(settingAction, iClearSongHistoryAction);
    }
}
    
void IRSongHistoryView::modelChanged()
{    
    iListView->reset();
    
#ifdef SUBTITLE_STR_BY_LOCID
    QString headingStr = hbTrId("txt_irad_subtitle_recently_played_songs") + " (" + QString::number(iModel->rowCount()) + ")";   
#else
    QString headingStr = hbTrId("Recently played songs") + " (" + QString::number(iModel->rowCount()) + ")";
#endif    
 
    setHeadingText(headingStr);   
}

void IRSongHistoryView::newMetadataAdded(IRQMetaData *aMetadata)
{
    Q_UNUSED(aMetadata);     
    iModel->checkSongHistoryUpdate();
     
}

void IRSongHistoryView::popupClearHistoryConfirmMessageBox()
{
#ifdef SUBTITLE_STR_BY_LOCID
    HbMessageBox::question(hbTrId("txt_irad_info_clear_song_list"), this, SLOT(clearList(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);
#else
    HbMessageBox::question(hbTrId("Clear song list?"), this, SLOT(clearList(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);    
#endif
}
void IRSongHistoryView::clearList(HbAction *aAction)
{
    HbMessageBox *dialog = static_cast<HbMessageBox*>(sender());
    if (dialog)
    {
        if (aAction == dialog->actions().at(0))
        {
            iModel->clearList();
        }
    }
}

void IRSongHistoryView::showPrompt()
{
#ifdef SUBTITLE_STR_BY_LOCID
    QString str = hbTrId("txt_irad_info_click_the_song_and_find_it_in_nokia_music_store");
#else
    QString str = hbTrId("Click the song and find it in nokia music store");    
#endif
    popupNote(str, HbMessageBox::MessageTypeInformation);
}

void IRSongHistoryView::itemAboutToBeSelected(bool& needNetwork)
{
    /* for in song history view, the data will retrived from the web browser*/
    needNetwork = false;
}

void IRSongHistoryView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords)
{
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);

    HbAction *action = NULL;
    HbMenu *contextMenu = new HbMenu;
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));

#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_irad_menu_search_in_music_store"));
#else
    action = contextMenu->addAction(hbTrId("Search in music store"));    
#endif
    action->setObjectName(KActionSearchInMusicStoreName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
#else
    action = contextMenu->addAction(hbTrId("Delete"));    
#endif
    action->setObjectName(KActionDeleteName);
    contextMenu->setPreferredPos(aCoords);
    contextMenu->open();
}

void IRSongHistoryView::searchInMusicStoreContextAction()
{
    handleItemSelected();
}
void IRSongHistoryView::deleteContextAction()
{
    int current = iListView->currentIndex().row();     
    bool ret = iModel->deleteOneItem(current);     
    if( !ret )
    {
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
#else
        popupNote(hbTrId("Operation failed"), HbMessageBox::MessageTypeWarning);        
#endif
    }
}

void IRSongHistoryView::gotoStationHistory()
{
	  getViewManager()->activateView(EIRView_HistoryView);
}

void IRSongHistoryView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    IrAbstractListViewBase::handleOrientationChanged(aOrientation);
    iModel->setOrientation(aOrientation);
    iListView->reset();
}

void IRSongHistoryView::actionClicked(HbAction *aAction)
{
    if ( aAction )
    {
        QString objectName = aAction->objectName();
        if ( objectName == KActionSearchInMusicStoreName )
        {
            searchInMusicStoreContextAction();
        }
        else if( objectName == KActionDeleteName)
        {
            deleteContextAction();
        }
    }
}
