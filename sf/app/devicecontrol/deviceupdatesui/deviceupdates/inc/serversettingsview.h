/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Launches server settings view to edit or create
*               device management server profile.
*
*/

#ifndef SERVERSETTINGSVIEW_H_
#define SERVERSETTINGSVIEW_H_
#include <hbinstance.h>
#include <hbview.h>
#include <hbaction.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <QStandardItemModel>
#include <qmobilityglobal.h> 

class HbDataFormViewItem;
class HbInputFilter;
class SettingsDataFormCustomItem;
class DmAdvancedView;
class ServerSettingsView : public HbView
{
    Q_OBJECT

public:
    explicit ServerSettingsView(HbMainWindow* mainWindow, DmAdvancedView* serversView ,
                QString& header, QGraphicsItem* parent = 0);
    virtual ~ServerSettingsView();       
    void constructview(QString& header);       
    int setProfileValues(QStringList& settingdatalist,bool& sessionmode, QStringList& apdata, int& currentap
            ,  int& port, bool& networkauth);    
    void setBackButtonBehavior();    
    void makeviewItemsVisible(QString& heading);
    
private:    
    void hideInternetitems(); 
    void showInternetitems(); 
    
public slots:
    void backButtonClicked();
    void checkServerIdvalue();    
    void accessPointItemChanged(int apindex);
    void portItemChanged(QString port);

public:    
    HbMainWindow *iMainWindow;        
    HbMenu* viewspecificmenu;
    HbAction* help;
    HbAction* exit;
    HbDataForm* form;
    HbDataFormModel* dataformmodel;
    HbDataFormModelItem* servername; 
    HbDataFormModelItem* serverid; 
    HbDataFormModelItem* sessionmode;
    HbDataFormModelItem *serverpwd; 
    HbDataFormModelItem* accesspoint;
    HbDataFormModelItem* hostaddress;
    HbDataFormModelItem* port; 
    HbDataFormModelItem* username;
    HbDataFormModelItem* userpwd;
    HbDataFormModelItem* networkauth;
    HbDataFormModelItem* networkusername;
    HbDataFormModelItem* networkpwd;
    HbAction* backaction;    
    DmAdvancedView* serversView;
    SettingsDataFormCustomItem* customPrototype;
    int currentiapindex;
    QStringList apnameslist;
    bool backbehaviorset; 
    unsigned int iPort;
};

#endif /* SERVERSETTINGSVIEW_H_ */
