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
#ifndef IRCATEGORYVIEW_H
#define IRCATEGORYVIEW_H

#include "irabstractlistviewbase.h"
#include "irqevent.h"
#include "irqisdsclient.h"

class IRCategoryModel;

class IRCategoryView : public IrAbstractListViewBase
{
	Q_OBJECT
public:
	~IRCategoryView();
	 
    void loadCategory(IRQIsdsClient::IRQIsdsClientInterfaceIDs aCategory);
        
protected:
    IRCategoryView(IRApplication* aApplication, TIRViewId aViewId);
       
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
    //from base class IRBaseView
    void launchAction();
    void itemAboutToBeSelected(bool &aNeedNetwork);
    
private slots:       
    void cancelRequest();    
    
    void networkRequestNotified(IRQNetworkEvent aEvent);
    
    void dataChanged();
    
    //used for connect signal from isds client
    void operationException(IRQError aError);
    
    
private:
    //from base class IrAbstractListViewBase
    void setCheckedAction();
    
    void resetCurrentItem();
    void storeCurrentItem();
    void handleItemSelected();
    
    void connectToIsdsClient();
    
    void disconnectIsdsClient();
    
private:
    int iLastSelectItem;
    IRCategoryModel *iModel;
    QString iLoadedSection;
    
    friend class IRViewManager;
};

#endif // IRCATEGORYVIEW_H
