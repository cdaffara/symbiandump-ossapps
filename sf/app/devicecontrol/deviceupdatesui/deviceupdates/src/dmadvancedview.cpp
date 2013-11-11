
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
* Description:  Methods for DmAdvancedView.
*
*/
#include <hbnamespace.h>
#include "dmadvancedview.h"
#include "dmfotaview.h"
#include "serversettingsview.h"
#include "customviewitem.h"

DmAdvancedView::DmAdvancedView(HbMainWindow *mainWindow,DMFotaView *mainView, DmInfo *info, QGraphicsItem *parent):HbView(parent),
    bluetooth(":/icons/qgn_prop_sml_bt.svg"),
    internet(":/icons/qgn_prop_sml_http.svg"), 
    defaultprofileicon(":/icons/qtg_large_avatar.svg")                
    {
    qDebug("omadm DeviceManagerUi::DmAdvancedView >>");
    iMainWindow = mainWindow;
    serverSetView = NULL;
    iMainView = mainView;
    if(info==NULL)
        {
        dminfo = new DmInfo(this);
        }
    else 
        dminfo = info;
    currentdefaultprofile = -1;
    currentview = 0;
    connectionRequested = false;    
    backbehaviorset = false;
    currentselecteditem = -1;
    qDebug("omadm DeviceManagerUi::DmAdvancedView >>");
    }

DmAdvancedView::~DmAdvancedView()
    {
    qDebug("omadm DmAdvancedView::~DmAdvancedView");
    if(docmlLoader)
        delete docmlLoader;  
    if(dminfo)
        delete dminfo;
    qDebug("omadm DmAdvancedView::~DmAdvancedView end"); 
    }

void DmAdvancedView::handleLongPress(HbAbstractViewItem* item , QPointF coOrdinates)
    {
    qDebug("omadm DeviceManagerUi::handleLongPress >>");
    if(connectionRequested)
        return;
    if (item)
        {
        modelItem = model->itemFromIndex(item->modelIndex());
        currentselecteditem = modelItem->row();
        if(currentselecteditem == dminfo->profilescount())//Button item pressed
            return;
        HbMenu *csmenu = new HbMenu();
        csmenu->setAttribute( Qt::WA_DeleteOnClose);
        HbAction *defaultprofileAction = 0;
        HbAction *deleteaction = 0;
        HbAction *connectaction = 0;
        if (!dminfo->Isdefaultprofile(currentselecteditem))
            {
            defaultprofileAction = csmenu->addAction(hbTrId(
                    "txt_device_update_menu_set_as_default"));
            connect(defaultprofileAction, SIGNAL(triggered()), this,
                    SLOT(defaultMenuItemSelected()));
            }
        if (!dminfo->Isprofilelocked(currentselecteditem) && 
                     dminfo->isProfileDeleteAllowed(currentselecteditem) )
            {
            deleteaction = csmenu->addAction(hbTrId(
                    "txt_device_update_menu_delete"));
            connect(deleteaction, SIGNAL(triggered()), this,
                    SLOT(deleteMenuItemSelected()));
            }
        connectaction = csmenu->addAction(hbTrId(
                "txt_device_update_menu_connect"));
        connect(connectaction, SIGNAL(triggered()), this,
                SLOT(connectMenuItemSelected()));
        csmenu->setPreferredPos(coOrdinates);
        csmenu->open();
        }
    qDebug("omadm DeviceManagerUi::handleLongPress >>");
    }


void DmAdvancedView::handleClicked(QModelIndex index)
    {
    qDebug("omadm DeviceManagerUi::handleClicked >>");
    if(connectionRequested)
        return;
	int itemnum = 0;
    QStandardItem *selectedItem = model->itemFromIndex(index);
    itemnum = selectedItem->row();	        
     if(itemnum == dminfo->profilescount())//Button item pressed
        {
    qDebug("omadm create new server profile button clicked");
        return;
        }
  //Stop listening DB events for profile addition
    dminfo->DisableDbNotifications(true);                
    if(itemnum >= 0 && !dminfo->Isprofilelocked(itemnum))
        {        
        //read profile items
        QStringList itemdata;
        bool sessmode = 0;
        QStringList iaplist;
        int curriap = 0;
        int portnum = 0;
        bool nwauth = false ;
        dminfo->getProfiledata(itemnum, itemdata, sessmode,iaplist,curriap,portnum, nwauth);	
        QString editserverprof(itemdata[0]);
        if(!serverSetView)
            {
            serverSetView = new ServerSettingsView(iMainWindow, this , editserverprof);
            }
        else // view already created
            {
            //Just update the setting items as per this profile            
            serverSetView->makeviewItemsVisible(editserverprof);		  
            }        
        serverSetView->setProfileValues(itemdata,sessmode,iaplist,curriap,portnum,nwauth);
        iMainWindow->setCurrentView(serverSetView);
  
        }
    qDebug("omadm DeviceManagerUi::handleClicked >>");
    }

bool DmAdvancedView::displayItems()
    {    	
    qDebug("omadm DeviceManagerUi::displayItems >>");
    docmlLoader = new HbDocumentLoader;
    bool ok ;    
    docmlLoader->load( DOCML_FILE_NAME,  &ok  );
    if(ok)
    	{
        if(iMainWindow->orientation() == Qt::Vertical)
            docmlLoader->load( DOCML_FILE_NAME,  PORTRAIT,&ok  );
        else
            docmlLoader->load( DOCML_FILE_NAME, LANDSCAPE,&ok  );            
    	}
    else
    	{
    	 qDebug("omadm docml loading failed");
    	 return ok;
    	}
    if(ok)
        {        	
        connect(iMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
                this,  SLOT(reLayout(Qt::Orientation)));    
        viewwidget = qobject_cast<HbView *>(docmlLoader->findWidget( VIEW_NAME ) );    
        Q_ASSERT_X(viewwidget != 0, "OMADM View not found","Not found");        
        viewspecificmenu = new HbMenu();    
        help = viewspecificmenu->addAction(hbTrId("txt_common_menu_help")); 
        exit = viewspecificmenu->addAction(hbTrId("txt_common_menu_exit"));
        connect(exit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));
        setMenu(viewspecificmenu);            
        serversListGroup = qobject_cast<HbGroupBox *>(docmlLoader->findWidget( GROUP ) );
        serversListGroup->setHeading(hbTrId("txt_device_update_setlabel_device_update_services"));
        serversListGroup->setCollapsable( true );        
        connect(serversListGroup, SIGNAL(toggled(bool)), this, SLOT(serversListGroupClicked(bool)));
        otherDetailsGroup = qobject_cast<HbGroupBox *>(docmlLoader->findWidget( OTHERDETAILSGROUP ) );
        connect(otherDetailsGroup, SIGNAL(toggled(bool)), this, SLOT(otherDetailsGroupClicked(bool)));        
        otherDetailsGroup->setHeading(hbTrId("txt_device_update_setlabel_other_details"));
        otherDetailsGroup->setCollapsable( true );
        otherDetailsGroup->setCollapsed( true );                            
        list = qobject_cast<HbListView *>(docmlLoader->findWidget( LIST_NAME ) );

        list->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
        list->listItemPrototype()->setSecondaryTextRowCount(1,2);
        list->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);

        dminfo->refreshProfileList();    
        int IndicatorCount =dminfo->profilescount();
        model = new QStandardItemModel();   
        QList<HbAbstractViewItem *> prototypes ;
        prototypes.append(list->listItemPrototype());
        CustomViewItem *prototype2 = new CustomViewItem(this,list);
        prototypes.append(prototype2);
        list->setItemPrototypes(prototypes);
        list->setModel(model);
        int i;
        for (i = 0; IndicatorCount > 0 && i < IndicatorCount; ++i) {        
        QStringList liststr;
        bool lock = false;
        int icon = 0;
        int ret = dminfo->profileinfo(i,liststr,icon, lock);
        if(ret <= 0)
            {
            break;
            }
        QStandardItem* item ;    
        item = new QStandardItem();    
        HbIcon icon1;
        icon1.setIconName(":/icons/qgn_prop_sml_http.svg");
        HbIcon icon2;
        icon2.setIconName(":/icons/qgn_prop_sml_bt.svg");
        HbIcon icon3;
        icon3.setIconName(":/icons/qgn_indi_sett_protected_add.svg");   
        HbIcon defaultprofile;
        defaultprofile.setIconName(":/icons/qtg_large_avatar.svg");            
        QList<QVariant> Iconl;    
        if(icon == 0)//Internet
            Iconl.insert(0,icon1);
        else if(icon == 1)//Bluetooth
            Iconl.insert(0,icon2); 
        else 
            {
            Iconl.insert(0,defaultprofile);
            currentdefaultprofile = i;
            }
        icon = 0;
        if(lock)
            {
            Iconl.insert(1,icon3);
            lock = false; 
            }
        QVariant iconlist(Iconl);
        iconlist.setValue(Iconl);
        item->setData(liststr , Qt::DisplayRole);
        item->setData(iconlist , Qt::DecorationRole);
        model->setItem(i, item);    
        }        
        list->setItemRecycling(false);
        QStandardItem* customitem = new QStandardItem();
        customitem->setData(Hb::ItemType_Last + 1001,Hb::ItemTypeRole);
        model->setItem(i, customitem);
        connect(list, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                this,  SLOT(handleLongPress(HbAbstractViewItem*,QPointF)));
    
        connect(list, SIGNAL(activated(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));            
        
        qDebug("omadm launching other details list");
        otherdetailslist = qobject_cast<HbListView *>(docmlLoader->findWidget( LISTOTHERDETAILS ) );    
        otherdetailslist->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
		otherdetailslist->setLongPressEnabled(EFalse);
        qDebug("omadm launching other details list done");    
        mainCalltoUpdateView();                             
        label = qobject_cast<HbLabel *>(docmlLoader->findWidget(LABEL));        
        label->setPlainText(hbTrId("txt_device_update_subhead_advanced_device_updates"));        
        
        setWidget(viewwidget);        
        }
    else 
        {
        qDebug("omadm docml section loading failed");
        }    
    qDebug("omadm DeviceManagerUi::displayItems >>");
    return ok;
    }

void DmAdvancedView::updateEarlierdefaultProfileIcon()
    {
    qDebug("omadm DeviceManagerUi::updateEarlierdefaultProfileIcon >>");
    if(currentdefaultprofile >= 0)
        {
        QStandardItem *earlierDefaultProfile = model->item(currentdefaultprofile);
        //Find the transport type & set the icon
        int transporttype = 0;
        dminfo->profileTransport(currentdefaultprofile,transporttype);
        if(transporttype == 0)//Internet
            {
            earlierDefaultProfile->setIcon(internet);
            }
        else
            {
            earlierDefaultProfile->setIcon(bluetooth);
            }        
        }
    qDebug("omadm DeviceManagerUi::updateEarlierdefaultProfileIcon >>");
    }

void DmAdvancedView::createNewProfile()
    {
    qDebug("omadm DeviceManagerUi::createNewProfile >>");
    if(dminfo->createNewprofile())
        {
        //Open server settings view    
        QString newserverprof(hbTrId("txt_device_update_setlabel_new_server_profile"));    
        if(!serverSetView)
            {
            serverSetView = new ServerSettingsView(iMainWindow, this, newserverprof);        
            }
        else // view already created
            {
            //Just update the setting items as per this profile                
            serverSetView->makeviewItemsVisible(newserverprof);       
            }
        QStringList serveritems;    
        serveritems<< "" <<"" <<"" <<"" <<""<<""<<""<<"";
        bool sessmode = true;
        bool nauth = true;
        QStringList apdata;        
        int currap =-1;
        int destap = 0;
        dminfo->getIaplist(apdata,currap,destap);    
        int portnum = 8080;    
        serverSetView->setProfileValues(serveritems,sessmode,apdata,currap,portnum,nauth);
        iMainWindow->setCurrentView(serverSetView);        
        }
    else // profile not getting created
        {
        qDebug("OMADM New server profile creation failed");
        }
    qDebug("omadm DeviceManagerUi::createNewProfile >>");
    }

void DmAdvancedView::saveProfile(QStringList& itemdata, bool& sessmode, QString& currap,unsigned int& portnum, bool& nauth )
    {
    qDebug("omadm DeviceManagerUi::saveProfile >>");
    dminfo->DisableDbNotifications(true);
    dminfo->saveProfile(itemdata,sessmode,currap,portnum,nauth);
    updateListview();
    dminfo->DisableDbNotifications(false);
    qDebug("omadm DeviceManagerUi::saveProfile >>");
    }

void DmAdvancedView::updateListview()
    {
    qDebug("omadm DeviceManagerUi::updateListview >>");
    model->clear();
    int IndicatorCount =dminfo->profilescount();    
    int i;
    for ( i = 0; IndicatorCount > 0 && i < IndicatorCount; ++i) {    
    QStringList liststr;
    bool lock = false;
    int icon = 0;
    int ret = dminfo->profileinfo(i,liststr,icon, lock);
    if(ret <= 0)
        {
        break;
        }    
    QStandardItem* item ;
    item = new QStandardItem();
    HbIcon icon1;
    icon1.setIconName(":/icons/qgn_prop_sml_http.svg");
    HbIcon icon2;
    icon2.setIconName(":/icons/qgn_prop_sml_bt.svg");
    HbIcon icon3;
    icon3.setIconName(":/icons/qgn_indi_sett_protected_add.svg");   
    HbIcon defaultprofile;
    defaultprofile.setIconName(":/icons/qtg_large_avatar.svg");    
    QList<QVariant> Iconl;
    if(icon == 0)//Internet
        Iconl.insert(0,icon1);
    else if(icon == 1)//Bluetooth
        Iconl.insert(0,icon2); 
    else //Default profile
        {
        Iconl.insert(0,defaultprofile);
        currentdefaultprofile = i;
        }
    icon = 0;
    if(lock)
        {
        Iconl.insert(1,icon3);
        lock = false; 
        }
    QVariant iconlist(Iconl);
    iconlist.setValue(Iconl);
    item->setData(liststr , Qt::DisplayRole);
    item->setData(iconlist , Qt::DecorationRole);
    model->setItem(i, item);    
    }    
    QStandardItem* item = new QStandardItem();
    item->setData(Hb::ItemType_Last + 1001,Hb::ItemTypeRole);
    model->setItem(i, item);
    }

void DmAdvancedView::serversListGroupClicked(bool state)
    {
    qDebug("omadm DeviceManagerUi::serversListGroupClicked >>");
    Q_UNUSED(state);
    if(serversListGroup->isCollapsed())
        otherDetailsGroup->setCollapsed(false);
    else
        otherDetailsGroup->setCollapsed(true);
    qDebug("omadm DeviceManagerUi::serversListGroupClicked >>");
    }

void DmAdvancedView::otherDetailsGroupClicked(bool state)
    {
    qDebug("omadm DeviceManagerUi::otherDetailsGroupClicked >>");
    Q_UNUSED(state);
    if(otherDetailsGroup->isCollapsed())
        serversListGroup->setCollapsed(false);
    else
        serversListGroup->setCollapsed(true);
    qDebug("omadm DeviceManagerUi::otherDetailsGroupClicked >>");
    }

bool DmAdvancedView::checkServerId(QString& serverid)
    {
    qDebug("omadm DeviceManagerUi::CheckforUpdate >>");
    return dminfo->checksrvid(serverid);
    qDebug("omadm DeviceManagerUi::CheckforUpdate >>");
    }

void DmAdvancedView::reLayout(Qt::Orientation orientation)
    {   
    qDebug("OMADM servers view DmAdvancedView::reLayout <<");
    if(orientation == Qt::Horizontal)
        {
        qDebug("OMADM servers view landscape");
        docmlLoader->load(DOCML_FILE_NAME,LANDSCAPE); 
        }
    else
        {
        qDebug("OMADM servers view portrait");	        
        bool ok;
        docmlLoader->load( DOCML_FILE_NAME,  PORTRAIT, &ok  );
        }
    qDebug("OMADM servers view DmAdvancedView::reLayout >>");
    }

void DmAdvancedView::setBackBehavior()
    {
    qDebug("OMADM servers view DmAdvancedView::setBackBehavior <<");
    if (!backbehaviorset)
        {
        qDebug("OMADM servers view back behavior setting");
        backaction = new HbAction(Hb::BackNaviAction, this);
        connect(backaction, SIGNAL(triggered()), this,
                SLOT(backButtonClicked()));
        setNavigationAction(backaction);
        backbehaviorset = true;
        }
    qDebug("OMADM servers view back behavior setting done setBackBehavior >>");
    }

void DmAdvancedView::backButtonClicked()
    {    
    qDebug("OMADM servers view DmAdvancedView::backButtonClicked <<");
    QList <HbView*> views = iMainWindow->views();    
    if(iMainWindow->orientation()==Qt::Vertical)
            {
            iMainWindow->setCurrentView(views[0]);
            }
        else
            {
            iMainWindow->setCurrentView(views[1]);
            }    
    qDebug("OMADM servers view DmAdvancedView::backButtonClicked >>");
    }

void DmAdvancedView::addVersionInfo()
    {
    qDebug("OMADM servers view DmAdvancedView::addVersionInfo <<");
    QStringList liststr;
    QStandardItem* item;
    QString val;
    QString str;
    TInt runtimesupport(0);
        CRepository* cenrep = NULL;
        TRAPD( error, cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );  
        if(error)
           {     
           FLOG( "[OMADM] CNSmlDMFotaContainer::FormatListboxL() cenrep problem" );
           }    
        if ( cenrep )
           {
           cenrep->Get( KNsmlDmRuntimeVerSupport, runtimesupport );
           delete cenrep; cenrep = NULL;
           }
        //Browser version
        Swi::RSisRegistrySession sisses ;
        TInt r( sisses.Connect() );
        CleanupClosePushL( sisses );
        if (r== KErrNone && runtimesupport)
            {           
             Swi::RSisRegistryEntry sientry;
             TInt oerr = sientry.Open(sisses, browseruid);
             TVersion bversion;
             TBuf <255> browserversion;
             if(oerr == KErrNone)
                 {
                 TRAPD(err2,bversion= sientry.VersionL());     
                 if (err2 == KErrNone)
                     {
                     browserversion.AppendNum(bversion.iMajor);
                     browserversion.Append(_L("."));
                     browserversion.AppendNum(bversion.iMinor);
                     if( browserversion.Length()>0 )
                         {                                
                         str = QString::fromUtf16(browserversion.Ptr(), browserversion.Length());
                         item = new QStandardItem();
                         val = hbTrId("txt_device_update_dblist_browser_version");
                         liststr.clear();
                         liststr << val;
                         liststr << str;
                         item->setData(liststr , Qt::DisplayRole);
                         //m_otherdetailsmodel->setItem(i++, item);
                         m_otherdetailsmodel->appendRow(item);
                         }
                     }
                 }
            }
        
        //flash version
        if (r== KErrNone&& runtimesupport)
            {           
              Swi::RSisRegistryEntry sientry;
              TInt oerr = sientry.Open(sisses, flashuid);
              TVersion fversion;
              TBuf <255> flashversion;
              if(oerr == KErrNone)
                  {
                  TRAPD(err2,fversion= sientry.VersionL());         
                  if (err2 == KErrNone)
                      {
                      flashversion.AppendNum(fversion.iMajor);
                      flashversion.Append(_L("."));
                      flashversion.AppendNum(fversion.iMinor); 
                      if( flashversion.Length()>0 )
                          {                               
                          str = QString::fromUtf16(flashversion.Ptr(), flashversion.Length());
                          item = new QStandardItem();
                          val = hbTrId("txt_device_update_dblist_flash_version");
                          liststr.clear();
                          liststr << val;
                          liststr << str;
                          item->setData(liststr , Qt::DisplayRole);
                          //m_otherdetailsmodel->setItem(i++, item);
                          m_otherdetailsmodel->appendRow(item);
                          }                                   
                      }      
                  }
            }
               
         if (r== KErrNone&& runtimesupport)
             {            
              Swi::RSisRegistryEntry sientry;
              TInt oerr = sientry.Open(sisses, javauid);
              TVersion jversion;
              TBuf <255> javaversion;
              if(oerr == KErrNone)
                  {       
                  TRAPD(err2,jversion= sientry.VersionL());     
                  if (err2 == KErrNone)
                      {
                      javaversion.AppendNum(jversion.iMajor);
                      javaversion.Append(_L("."));
                      javaversion.AppendNum(jversion.iMinor);  
                      if( javaversion.Length()>0 )
                          { 
                          str = QString::fromUtf16(javaversion.Ptr(), javaversion.Length());
                          item = new QStandardItem();
                          val = hbTrId("txt_device_update_dblist_java_version");
                          liststr.clear();
                          liststr << val;
                          liststr << str;
                          item->setData(liststr , Qt::DisplayRole);
                          m_otherdetailsmodel->appendRow(item);
                          } 
                      }            
                  }            
             } 
        CleanupStack::PopAndDestroy(1);
        qDebug("OMADM servers view DmAdvancedView::addVersionInfo >>");
    }

void DmAdvancedView::addOtherDetails()
    {
        qDebug("OMADM servers view DmAdvancedView::addOtherDetails >>");
        TRequestStatus status;
        QStringList liststr;
        QStandardItem* item;
        QString val;
        QString str;
        TInt error(iServer.Connect());
        if ( error != KErrNone )
        	return;
        iServer.GetPhoneInfo(0, info);
        error = imobPhone.Open(iServer, info.iName);
        if ( error != KErrNone )
        {
        	iServer.Close();
        	return;
        }
        //Bands supported
        
        RMobilePhone::TMobilePhoneNetworkInfoV1 nwInfov1;
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg nwInfov1Pckg(nwInfov1);                     
        imobPhone.GetCurrentNetwork(status, nwInfov1Pckg);
        User::WaitForRequest( status );
        status = KRequestPending;
        nwInfov1 = nwInfov1Pckg();
        NetworkBand(nwInfov1.iBandInfo, str);
        item = new QStandardItem();
        val = hbTrId("txt_deviceupdate_dblist_network_band_supported");
        liststr.clear();
        liststr << val;
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
        
        //Packet Service
        
        TInt packetsrvc =0;
        RMobilePhone::TMobilePhoneNetworkInfoV5 nwInfov5;
        RMobilePhone::TMobilePhoneNetworkInfoV5Pckg nwInfov5Pckg(nwInfov5);
        imobPhone.GetCurrentNetwork( status, nwInfov5Pckg );
        User::WaitForRequest( status );
        status = KRequestPending;
        nwInfov5 = nwInfov5Pckg();
        
        if(nwInfov5.iHsdpaAvailableIndicator) packetsrvc =1;
        
        if(nwInfov5.iEgprsAvailableIndicator) packetsrvc =2;
        
        if(!packetsrvc)
        {
            RMobilePhone::TMobilePhoneNetworkInfoV8 nwInfov8;
            RMobilePhone::TMobilePhoneNetworkInfoV8Pckg nwInfov8Pckg(nwInfov8);
            imobPhone.GetCurrentNetwork( status, nwInfov8Pckg );
            User::WaitForRequest( status );
            status = KRequestPending;
            nwInfov8 = nwInfov8Pckg();
            if(nwInfov8.iHsupaAvailableIndicator) packetsrvc =3;
        }     
        liststr.clear();
        val = hbTrId("txt_deviceupdate_dblist_data_service_supported");
        liststr << val;
        item = new QStandardItem();
        PacketService(packetsrvc, str);
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
                    
        // Ciphering
        
        liststr.clear();
        item = new QStandardItem();
        val = hbTrId("txt_deviceupdate_dblist_network_security_level");
        liststr << val;
        imobPhone.GetNetworkSecurityLevel(status,idispSecurity);
        User::WaitForRequest( status );
        NetworkCiphering(idispSecurity, str);
        liststr << str;
        item->setData(liststr, Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
        
        // WLAN MAC
        
        TUint KPhoneWlanSeparator (':');
        _LIT( KWLanMACDataFormat, "%02x");  
        // Fetch WLAN MAC address
        TBuf<KWlanMacAddrLength> address;
        RProperty::Get( KPSUidWlan, KPSWlanMacAddress, address );   
        // Format fetched address
        TBuf<KWlanMacAddrLength> wlanMACAddress;        
        for ( TInt i( 0 ); i < address.Length(); i++ )
            {
            // Set separator
            if( i > 0 )
                {
                wlanMACAddress.Append( KPhoneWlanSeparator );
                }
            // Set data
            TBuf<50> tmp;
            tmp.Format( KWLanMACDataFormat, address[i] );
            wlanMACAddress.Append( tmp );
            } 
        liststr.clear();
        item = new QStandardItem();
        val = hbTrId("txt_device_update_dblist_wlan_mac_address");
        liststr << val;
        str = QString::fromUtf16(wlanMACAddress.Ptr(), wlanMACAddress.Length());
        liststr << str;
        item->setData(liststr, Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
          
        // BT MAC
          
        TBuf<KBTAddrLength> addressBuffer;
        // Fetch from Cenrep
        CRepository* repository = NULL;
        TRAPD( err, repository = CRepository::NewL( KCRUidBluetoothLocalDeviceAddress ) );
        if ( err == KErrNone )
            {
            qDebug("KCRUidBluetoothLocalDeviceAddress errnone");
            err = repository->Get( KBTLocalDeviceAddress, addressBuffer );
            
            if (err == KErrNone)
                qDebug("KBTLocalDeviceAddress errnone");
            else
                qDebug("KBTLocalDeviceAddress Error");
                
            delete repository;
            }
        else
            {
                qDebug("KCRUidBluetoothLocalDeviceAddress Error openin cenrep");
            }
        liststr.clear();
        val = hbTrId("txt_device_update_dblist_bt_mac_address");
        liststr << val;
        item = new QStandardItem();
        str = QString::fromUtf16(addressBuffer.Ptr(), addressBuffer.Length());
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
        
        imobPhone.Close();
        iServer.Close();

    }
void DmAdvancedView::mainCalltoUpdateView()
{   
    qDebug("OMADM servers view DmAdvancedView::mainCalltoUpdateView >>");
    m_otherdetailsmodel = new QStandardItemModel();
    TInt i=0;
    
    QStringList liststr;
    QStandardItem* item;
    QString val;
    QString str;
    //1. Device Updated

    
    addVersionInfo();
    
    addOtherDetails();

    //sw version date
    TBuf< KSysUtilVersionTextLength > swversiondate;
    TBuf< KSysUtilVersionTextLength > version;
    version.Zero();
    swversiondate.Zero();
    if( SysUtil::GetSWVersion(version)==KErrNone)
        {
        TInt len= version.Length();
        TInt pos1 = version.Find(KSmlEOL);
             if( pos1 != KErrNotFound && len > pos1 )
                {
                 TBuf<KSysUtilVersionTextLength> version1;
                 version1.Zero();
                 version1.Append( version.Right( len-pos1-1 ));
                 len= version1.Length();
                 pos1 = version1.Find(KSmlEOL);
                 if( pos1 != KErrNotFound  && len > pos1 )
                     {
                     swversiondate.Append(version1.Left( pos1 ));
                     }
                }
            val = hbTrId("txt_device_update_dblist_sw_version_date");
            item = new QStandardItem();
            str = QString::fromUtf16(swversiondate.Ptr(), swversiondate.Length());
            liststr.clear();
            liststr << val;
            liststr << str;
            item->setData(liststr , Qt::DisplayRole);
            m_otherdetailsmodel->appendRow(item);
            }
            
    //Custom version and date
    //SysVersionInfo::TVersionInfoType x = SysVersionInfo::EOPVersion;
    TBuf< KNSmlMaxTextLength64 >  customsw;
    TBuf< KNSmlMaxTextLength64 >  customswdate;
    version.Zero();
    if( SysVersionInfo::GetVersionInfo(SysVersionInfo::EOPVersion,version) == KErrNone )
      {
         TInt len= version.Length();
         TInt pos1 = version.Find(KSmlEOL);
         if( pos1 != KErrNotFound  && len > pos1 )
          {        
          customsw.Append( version.Left(pos1));
          customswdate.Append( version.Right( len-pos1-1 ));         
          }
      } 
    
    if(customsw.Length()>0)
        {
        str = QString::fromUtf16(customsw.Ptr(), customsw.Length());
        item = new QStandardItem();
        val = hbTrId("txt_device_update_dblist_custom_version");
        liststr.clear();
        liststr << val;
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
        }
    if(customswdate.Length()>0)
        {
        str = QString::fromUtf16(customswdate.Ptr(), customswdate.Length());
        item = new QStandardItem();
        val = hbTrId("txt_device_update_dblist_custom_version_date");
        liststr.clear();
        liststr << val;
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
        }
    
    TBuf<KSysUtilVersionTextLength> Langversion; 
    TBuf<KSysUtilVersionTextLength> lversion;
    if( SysUtil::GetLangVersion(Langversion ) == KErrNone )
      {
        str = QString::fromUtf16(Langversion.Ptr(), Langversion.Length());
        item = new QStandardItem();
        val = hbTrId("txt_device_update_dblist_language_set");
        liststr.clear();
        liststr << val;
        liststr << str;
        item->setData(liststr , Qt::DisplayRole);
        m_otherdetailsmodel->appendRow(item);
      }
    

    //lang variant version
    Langversion.Zero();
    if( SysUtil::GetLangSWVersion(Langversion ) == KErrNone )
        {
        int len = Langversion.Length();
        TInt pos1 = Langversion.Find( KSmlEOL );
        if( pos1 != KErrNotFound && len > pos1 )
            {
            lversion.Zero();
            lversion.Append( Langversion.Left( pos1 ) );
            }
         str = QString::fromUtf16(lversion.Ptr(), lversion.Length());
         item = new QStandardItem();
         val = hbTrId("txt_device_update_dblist_language_variant_version");
         liststr.clear();
         liststr << val;
         liststr << str;
         item->setData(liststr , Qt::DisplayRole);
         m_otherdetailsmodel->appendRow(item);
        }
     
    otherdetailslist->setModel(m_otherdetailsmodel);
    qDebug("OMADM servers view DmAdvancedView::mainCalltoUpdateView <<");
		
}

void DmAdvancedView::PacketService(TInt val, QString& string)
    {
    qDebug("OMADM servers view DmAdvancedView::PacketService >>");
     switch (val)
         {
         case (0):
                 string = hbTrId("txt_deviceupdate_dblist_data_srvce_suppo_val_GPRS");
                 break;
         case (1):
                 string = hbTrId("txt_deviceupdate_dblist_data_srvce_suppo_val_HSDPA");
                 break;
         case (2):
                 string = hbTrId("txt_deviceupdate_dblist_data_srvce_suppo_val_HSUPA");
                 break;
         case (3):
                 string = hbTrId("txt_deviceupdate_dblist_data_srvce_suppo_val_EGPRS");
                 break;
         case (4):
                 string = hbTrId("txt_deviceupdate_dblist_ntwrk_secu_lvl_val_CDMA");
                 break;
         case (5):
         default :
                 string = hbTrId("txt_deviceupdate_dblist_data_srvce_suppo_val_Unknow");
                 break;
         }
     qDebug("OMADM servers view DmAdvancedView::PacketService <<");
     }


void DmAdvancedView::NetworkCiphering(RMobilePhone::TMobilePhoneNetworkSecurity val, QString& string)
    {
    qDebug("OMADM servers view DmAdvancedView::NetworkCiphering >>");
     switch (val)
         {
         case (RMobilePhone::ECipheringGSM):
                 string = hbTrId("txt_deviceupdate_dblist_ntwrk_secu_lvl_val_GSM");
                 break;
         case (RMobilePhone::ECipheringWCDMA):
                 string = hbTrId("txt_deviceupdate_dblist_ntwrk_secu_lvl_val_WCDMA");
                 break;
         case (RMobilePhone::ECipheringCDMA):
                 string = hbTrId("txt_deviceupdate_dblist_ntwrk_secu_lvl_val_CDMA");
                 break;
         case (RMobilePhone::ECipheringOff):
         default :
                 string = hbTrId("txt_deviceupdate_dblist_ntwrk_secu_lvl_val_none");
                 break;
         }
     qDebug("OMADM servers view DmAdvancedView::NetworkCiphering <<");
    }
    
void DmAdvancedView::NetworkBand(RMobilePhone::TMobilePhoneNetworkBandInfo val, QString& string)
    {
    qDebug("OMADM servers view DmAdvancedView::NetworkBand >>");
    switch (val)
        {
        case (RMobilePhone::E800BandA):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__800BA");
                break;
        case (RMobilePhone::E800BandB):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__800BB");
                break;
        case (RMobilePhone::E800BandC):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__800BC");
                break;
        case (RMobilePhone::E1900BandA):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BA");
                break;
        case (RMobilePhone::E1900BandB):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BB");
                break;
        case (RMobilePhone::E1900BandC):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BC");
                break;
        case (RMobilePhone::E1900BandD):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BD");
                break;
        case (RMobilePhone::E1900BandE):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BE");
                break;
        case (RMobilePhone::E1900BandF):
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val__1900BF");
                break;
        case (RMobilePhone::EBandUnknown):
        default :
                string = hbTrId("txt_deviceupdate_dblist_netwrk_bnd_spp_val_unknown");
                break;
        }   
    qDebug("OMADM servers view DmAdvancedView::NetworkBand <<");
    }

void DmAdvancedView::syncCompleted(int jobstatus)
    {
    qDebug("OMADM servers view DmAdvancedView::syncCompleted >>");
    Q_UNUSED(jobstatus);
    connectionRequested = false;    
    updateListview();
    iMainView->enableButtons();
    qDebug("OMADM servers view DmAdvancedView::syncCompleted <<");
    }
	
void DmAdvancedView::defaultMenuItemSelected()
    {
    qDebug("OMADM servers view DmAdvancedView::defaultMenuItemSelected >>");
    dminfo->setDefaultProfile(currentselecteditem);
    updateEarlierdefaultProfileIcon();
    modelItem->setIcon(defaultprofileicon);
    currentdefaultprofile = currentselecteditem;
    qDebug("OMADM servers view DmAdvancedView::defaultMenuItemSelected <<");
    }

void DmAdvancedView::deleteMenuItemSelected()
    {
    qDebug("OMADM servers view DmAdvancedView::deleteMenuItemSelected >>");
    dminfo->DisableDbNotifications(true);
    //check currentdefaultprofile is current item
    if(currentselecteditem == currentdefaultprofile) // deleting default profile
        currentdefaultprofile = -1;
    if(dminfo->DeleteProfile(currentselecteditem) >=0 )
        {
    //Update the profile list
    model->removeRow(currentselecteditem);
        }
    dminfo->DisableDbNotifications(false);
    qDebug("OMADM servers view DmAdvancedView::deleteMenuItemSelected <<");
    }

void DmAdvancedView::connectMenuItemSelected()
    {
    qDebug("OMADM servers view DmAdvancedView::connectMenuItemSelected >>");
    iMainView->displayNoteAndDisableButtons();
    backButtonClicked();
    dminfo->synchronize(currentselecteditem);    
    connectionRequested = true;
    qDebug("OMADM servers view DmAdvancedView::connectMenuItemSelected <<");
    }
	
