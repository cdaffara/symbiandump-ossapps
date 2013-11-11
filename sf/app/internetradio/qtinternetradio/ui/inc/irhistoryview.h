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
#ifndef IRHISTORYVIEW_H
#define IRHISTORYVIEW_H

#include "irabstractlistviewbase.h" 
#include "irqevent.h"


class IRHistoryModel;


class IRHistoryView : public IrAbstractListViewBase
{
    Q_OBJECT

public:
    ~IRHistoryView();
    
protected:
    IRHistoryView(IRApplication *aApplication, TIRViewId aViewId);
    
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
   
private slots:
    void clearAllList(HbAction *aAction);
    void networkRequestNotified(IRQNetworkEvent aEvent);
    
    void modelChanged();
    
    void actionClicked(HbAction *aAction);
    void popupClearHistoryConfirmMessageBox();    
private:
    void showHistory();
    void handleItemSelected();
    void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords);
    
    //from IrAbstractListViewBase
    void prepareMenu();
#ifdef HS_WIDGET_ENABLED	
    void itemAboutToBeSelected(bool &aNeedNetwork);
#endif
    void addContextAction();
    void deleteContextAction();
    void detailsContextAction();

private:  
    IRHistoryModel *iModel;
    HbAction       *iClearHistoryAction; 
    friend class IRViewManager;
};

#endif //IRHISTORYVIEW_H
