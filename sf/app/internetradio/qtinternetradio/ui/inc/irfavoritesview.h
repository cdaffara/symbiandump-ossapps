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
#ifndef IRFAVORITESVIEW_H
#define IRFAVORITESVIEW_H

#include "irabstractlistviewbase.h"
#include "irqevent.h"

class HbSelectionDialog;
class IRFavoritesModel;
class IRStationShare;

class IRFavoritesView : public IrAbstractListViewBase 
{
    Q_OBJECT

public:
    ~IRFavoritesView();

protected:
    IRFavoritesView(IRApplication *aApplication, TIRViewId aViewId);
    
    //from base
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
private slots:
    void deleteFavorites();        
    void networkRequestNotified(IRQNetworkEvent aEvent);    
    void modelChanged();
    void actionClicked(HbAction *aAction);
    void renameConfirmed(HbAction *aAction);
    void deleteDialogClosed(HbAction *aAction);
    void deleteContext(HbAction *aAction);
    
private:
    //from IrAbstractListViewBase
#ifdef HS_WIDGET_ENABLED	
    void itemAboutToBeSelected(bool &aNeedNetwork);
#endif
	
    void setCheckedAction();
    void handleItemSelected();
    void prepareMenu();
    void shareContextAction();
    void deleteContextAction();
    void renameContextAction();
    void detailsContextAction();
    void popupDeleteContextConfirmMessageBox();
    
    //from base view 
    void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords);
    
private:
    
    IRStationShare    *iStationShare;
    IRFavoritesModel  *iModel;
    HbSelectionDialog *iMultiDeleteDialog;
    HbAction          *iMultiDeleteAction;
    
    friend class IRViewManager;
};

#endif //IRFAVORITESVIEW_H
