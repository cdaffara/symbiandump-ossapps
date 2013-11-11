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
* Description:  Methods for ServerSettingsView
*
*/

#include <hblineedit.h>
#include <hbmessagebox.h>
#include <hbnamespace.h>
#include "serversettingsview.h"
#include "dmadvancedview.h"
#include "settingsdataformcustomitem.h"

ServerSettingsView::ServerSettingsView(HbMainWindow* mainWindow, DmAdvancedView* serversview, QString& header, QGraphicsItem* parent): HbView(parent), iMainWindow(mainWindow),
 form(NULL),dataformmodel(NULL),serversView(serversview),backbehaviorset(false)
    {    
    constructview(header);
    viewspecificmenu = new HbMenu();
    help = viewspecificmenu->addAction(hbTrId("txt_common_menu_help"));
    exit = viewspecificmenu->addAction(hbTrId("txt_common_menu_exit"));
    connect(exit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));    
    setMenu(viewspecificmenu);
    iPort = 0;
    }
ServerSettingsView::~ServerSettingsView()
    {
    
    }  

void ServerSettingsView::constructview(QString& header)
    {
    if(!form)
        {
        //custom prototype    
        form = new HbDataForm();
        QList <HbAbstractViewItem*> protos = form->itemPrototypes();
        customPrototype = new SettingsDataFormCustomItem(form);
        protos.append(customPrototype);
        form->setItemPrototypes(protos);
        dataformmodel =  new HbDataFormModel();    
        makeviewItemsVisible(header);
        form->setModel(dataformmodel); 
        setWidget(form);    
        iMainWindow->addView(this);    
        }    
    else //form already created
        {   
        }        
    }

int ServerSettingsView::setProfileValues(QStringList& settingdatalist,
				bool& sessiontype, QStringList& apdata, int& currentap
        					   ,  int& portnum, bool& networkauthval)
    {
    Q_UNUSED(sessiontype);
    Q_UNUSED(networkauthval);	
    int count =  settingdatalist.count();
    apnameslist = apdata;
    if(count > 0)
        {
        servername->setContentWidgetData(QString("text"), settingdatalist.first());
        serverid->setContentWidgetData(QString("text"), settingdatalist[1]);         
        serverpwd->setContentWidgetData(QString("echoMode"),HbLineEdit::Password );
        serverpwd->setContentWidgetData(QString("text"), settingdatalist[2]);                
        QStringList sList;
        sList <<hbTrId("txt_deviceupdate_setlabel_to_update_your_val_inter");
        sList<<hbTrId("txt_deviceupdate_setlabel_to_update_your_val_bluet");        
        sessionmode->setContentWidgetData(QString("text"), sList[0]);//Internet selected
        sessionmode->setContentWidgetData(QString("additionalText"), sList[1]);
        
        apdata.append(hbTrId("txt_deviceupdate_setlabel_access_point_val_default"));
        accesspoint->setContentWidgetData(QString("items"), apdata);
        currentiapindex = currentap;
        if(currentap >= 0)
        accesspoint->setContentWidgetData(QString("currentIndex"), currentap); 
        else
            accesspoint->setContentWidgetData(QString("currentIndex"), apdata.count()-1); 
        form->addConnection(accesspoint, SIGNAL(currentIndexChanged  (int  ) ),this, SLOT(accessPointItemChanged(int)));                
        hostaddress->setContentWidgetData(QString("text"),settingdatalist[3]);  
        iPort = portnum;
        port->setContentWidgetData(QString("text"),iPort);//Integer only setting needs to be done    
        form->addConnection(port, SIGNAL(textChanged  (QString) ),this, SLOT(portItemChanged(QString)));
        username->setContentWidgetData(QString("text"),settingdatalist[4]);        
        userpwd->setContentWidgetData(QString("echoMode"),HbLineEdit::Password );
        userpwd->setContentWidgetData(QString("text"),settingdatalist[5]);                
        QStringList yesnoList;
        yesnoList <<hbTrId("txt_deviceupdate_setlabel_network_auth_val_yes");
        yesnoList<<hbTrId("txt_deviceupdate_setlabel_network_auth_val_no");
        networkauth->setContentWidgetData(QString("text"), yesnoList[0]);
        networkauth->setContentWidgetData(QString("additionalText"), yesnoList[1]);        
        networkusername->setContentWidgetData(QString("text"),settingdatalist[6]);
        networkpwd->setContentWidgetData(QString("echoMode"),HbLineEdit::Password );
        networkpwd->setContentWidgetData(QString("text"),settingdatalist[7]);                
        return 0;
        }
    else 
        return -1;
    }


void ServerSettingsView::backButtonClicked()
    {   
    qDebug("OMADM server settings back button clicked");
    //Trigger main view
    //Send data back to servers view and save the profile
    QStringList itemlist;
    itemlist << servername->contentWidgetData(QString("text")).toString();
    itemlist << serverid->contentWidgetData(QString("text")).toString();
    itemlist << serverpwd->contentWidgetData(QString("text")).toString();

    itemlist << hostaddress->contentWidgetData(QString("text")).toString();

    itemlist << username->contentWidgetData(QString("text")).toString();

    itemlist << userpwd->contentWidgetData(QString("text")).toString();

    itemlist
            << networkusername->contentWidgetData(QString("text")).toString();

    itemlist << networkpwd->contentWidgetData(QString("text")).toString();
    QString sesstype =
            sessionmode->contentWidgetData(QString("text")).toString();
    bool sessmode = false;
    if (sesstype.compare("Internet"))
        sessmode = true;
    //Map this current iap with iap array
    QString curriap;
    if (currentiapindex >= 0 && currentiapindex < apnameslist.count())
        curriap = apnameslist[currentiapindex];//accesspoint->contentWidgetData(QString("currentText")).toString();
    else
        curriap.clear();
    unsigned int portval = port->contentWidgetData(QString("text")).toInt();
    QString netauthdata =
            sessionmode->contentWidgetData(QString("text")).toString();
    bool netauthval = false;
    if (netauthdata.compare(hbTrId("txt_deviceupdate_setlabel_network_auth_val_yes")))
        netauthval = true;
    //check server name , server id, host addr & usrname        
    if ((itemlist[0].length() > 0) && (itemlist[1].length() > 0)
            && (itemlist[3].length() > 0) && (itemlist[4].length() > 0))
        {
        serversView->saveProfile(itemlist, sessmode, curriap, iPort,
                netauthval);        
        form->removeConnection(accesspoint,
                SIGNAL(currentIndexChanged (int ) ), this,
                SLOT(accessPointItemChanged(int)));        
        iMainWindow->setCurrentView(serversView);
        //Form wk08 soft key owned by view
        //iMainWindow->removeSoftKeyAction(Hb::SecondarySoftKey,backaction);
        dataformmodel->clear();
        }
    else
        {
        TInt x = itemlist.count();
        TInt valSet=0;
        int i=0;
        for(i=0;i<x;i++)
            {
            if(itemlist[i].length()>0)
                {
                valSet=1;
                break;
                }
            }
        if(valSet==1)
            {
        HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation);
        note->setText(hbTrId(
                "txt_device_update_dialog_enter_mandatory_field_values"));
        note->setAttribute( Qt::WA_DeleteOnClose);         
        note->setTimeout(HbPopup::NoTimeout);
        note->open();
            }
        else
            {
            form->removeConnection(accesspoint,SIGNAL(currentIndexChanged (int ) ), this,SLOT(accessPointItemChanged(int)));
            iMainWindow->setCurrentView(serversView);
        //Form wk08 soft key owned by view
        //iMainWindow->removeSoftKeyAction(Hb::SecondarySoftKey,backaction);
            dataformmodel->clear();   
            }
        
        }        
    }

void ServerSettingsView::checkServerIdvalue()
    {
    QString srvid = serverid->contentWidgetData(QString("text")).toString();
    if(serversView->checkServerId(srvid))
        {
    serverid->setContentWidgetData(QString("text"), QString("")); 
    HbMessageBox *note= new HbMessageBox(HbMessageBox::MessageTypeInformation);
           note->setText(hbTrId("txt_device_update_dialog_invalid_server_id")); 
           note->setAttribute( Qt::WA_DeleteOnClose);                   
           note->setTimeout(HbPopup::NoTimeout);
           note->open();         
        }          
    }

void ServerSettingsView::setBackButtonBehavior()
    {    
    if (!backbehaviorset)
        {
        qDebug("OMADM server settings view back behavior setting");
        backaction = new HbAction(Hb::BackNaviAction, this);        
        connect(backaction, SIGNAL(triggered()), this,
                SLOT(backButtonClicked()));                
        setNavigationAction(backaction);        
        backbehaviorset = true;
        }
    qDebug("OMADM server settings view back behavior setting done");
    }

void ServerSettingsView::makeviewItemsVisible(QString& formtitle)
    {    
    setBackButtonBehavior();
    form->setHeading(hbTrId("txt_device_update_subhead_advanced_device_updates"));
    form->setDescription(formtitle);    
    if(dataformmodel->rowCount() <= 0)
        {
        QString itemname(hbTrId("txt_device_update_formlabel_server_name"));
        servername = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem , itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_server_id");
        serverid = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem , itemname);
    
        form->addConnection(serverid, SIGNAL(editingFinished ()), 
                this, SLOT(checkServerIdvalue()));
        itemname = hbTrId("txt_device_update_formlabel_server_password");
        serverpwd = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        
        itemname = hbTrId("txt_device_update_setlabel_session_mode");
        sessionmode = dataformmodel->appendDataFormItem(HbDataFormModelItem::ToggleValueItem, itemname);
        
        itemname = hbTrId("txt_device_update_setlabel_access_point");
        accesspoint = dataformmodel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem , itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_host_address");
        hostaddress = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_port");
        port = dataformmodel->appendDataFormItem(HbDataFormModelItem::CustomItemBase, itemname);
    
        itemname = hbTrId("txt_device_update_formlabel_user_name");
        username = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_password");
        userpwd = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_network_authentication");
        networkauth = dataformmodel->appendDataFormItem(HbDataFormModelItem::ToggleValueItem, itemname);
        
        itemname = hbTrId("txt_device_update_formlabel_network_username");
        networkusername = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        
        itemname = hbTrId("txt_device_update_setlabel_network_password");
        networkpwd = dataformmodel->appendDataFormItem(HbDataFormModelItem::TextItem, itemname);
        }
    }

void ServerSettingsView::accessPointItemChanged(int apindex)
    {
    currentiapindex = apindex;
    qDebug("omadm iap changed");        
    }

void ServerSettingsView::portItemChanged(QString port)
    {    
    iPort = port.toInt();
    qDebug("omadm new port value is %d ",iPort);
    }
