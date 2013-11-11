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
#ifndef IRSEARCHCHANNELSVIEW_H
#define IRSEARCHCHANNELSVIEW_H

#include "irqevent.h"
#include "irbaseview.h"
#include "irqenums.h"

class HbGroupBox;
class HbListView;
class HbSearchPanel;
class HbAction;
class IrChannelModel; 
class IRQPreset;

class IRSearchChannelsView : public IRBaseView
{
    Q_OBJECT
    
public:
    
    ~IRSearchChannelsView();       
    
protected:
    IRSearchChannelsView(IRApplication* aApplication, TIRViewId aViewId);    
    //from base view
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
private: //functions
    
    void connectWidget();     
    //for connecting and disconnecting isds client's signals
    void connectIsdsClient();
    void disconnectIsdsClient();
    
    void switch2LoadingState();
    void switch2InitState();  
    void loadLayout();
    void handleItemSelected();    
    void initMenu();
    void resetHeadingText();
    
    bool eventFilter(QObject *object, QEvent *event);
    
private slots: //slots
     
    void cancelRequest();
    void networkRequestNotified(IRQNetworkEvent aEvent); 
    void searchTextAlready(const QString& aSearchCriteria);
    void dataChanged();     
    void operationException(IRQError aError);    
    void clickItem(const QModelIndex&);    
    //for playing a channel in the search result list
    void presetResponse(IRQPreset *aPreset);  
    void hideVkb();
    void showVkb();

public slots:    
    //this slot is used to back to previouse view or minimize the search widget
    void minimizeSearchPanel();   
    
private: // members       
    
    enum IRQSearchState
    {
        ESearch_init = 0,
        ESearch_Loading //loading means searching or buffering.
    };    
    
    HbGroupBox          *iHeadingLabel;
    HbListView          *iListView;
    QString              iKeyText;
    
    //default searchpanel is not progressive
    HbSearchPanel       *iSearchPanelWidget;
    IRQSearchState       iSearchState;
    IrChannelModel      *iChannelModel;
    //the object is created by IsdsClient, but application is responsible for free
    IRQPreset           *iPreset;
    IRQSettings         *iIRSettings;
    bool                 iFirstTime;
    friend class IRViewManager;
};
#endif //IRSEARCHCHANNELSVIEW_H
