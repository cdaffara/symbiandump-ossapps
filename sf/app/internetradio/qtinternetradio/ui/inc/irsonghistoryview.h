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
#ifndef IRSONGHISTORYVIEW_H_
#define IRSONGHISTORYVIEW_H_

#include "irabstractlistviewbase.h" 
#include "irqevent.h"

class IRQPreset;
class IRSongHistoryModel;
class IRQStatisticsReporter;
class IRQMetaData; 
class HbIcon;
class QTimer;

class IRSongHistoryView : public IrAbstractListViewBase 
{
    Q_OBJECT

public:
    ~IRSongHistoryView();
    
protected:
    IRSongHistoryView(IRApplication *aApplication, TIRViewId aViewId);
    
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
     
private slots:
    void modelChanged();
    void newMetadataAdded(IRQMetaData * aMetadata);
    void popupClearHistoryConfirmMessageBox();   
    void clearList(HbAction *aAction);   
    void showPrompt();
    void gotoStationHistory();
    void handleOrientationChanged(Qt::Orientation aOrientation);
    void actionClicked(HbAction *aAction);
    
private:
    void showSongHistory();
    void handleItemSelected();         
    //from IrAbstractListViewBase
    void prepareMenu(); 
    void itemAboutToBeSelected(bool& needNetwork);
    void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords);
    void searchInMusicStoreContextAction();
    void deleteContextAction();
	
private:  
    IRSongHistoryModel *iModel;
    HbAction           *iClearSongHistoryAction;   
    HbAction           *iStationHistoryAction;
    IRQStatisticsReporter *iStatisticsReporter;     
    int           iShowPrompt;
     
     
    friend class IRViewManager;
};

#endif  
