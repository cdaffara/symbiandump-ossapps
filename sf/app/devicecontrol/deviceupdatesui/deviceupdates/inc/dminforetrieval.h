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
* Description:  Provides methods to retrieve device management
*               information. 
*
*/

#ifndef DMINFORETRIEVAL_H_
#define DMINFORETRIEVAL_H_
#include <qobject.h>
#include <featmgr.h>
#include <QGraphicsItem>
#include <e32base.h>
#include <badesca.h>
#include "nsmldmsyncappengine.h"
#include "nsmldmsyncprofile.h"
//#include "NSmlDMSyncProfileItem.h"
#include "nsmldmsyncutil.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsyncprofilelist.h"
#include "nsmldmdbnotifier.h"
class DmAdvancedView;
class TNSmlDMProfileItem;
//class CNSmlDMDbNotifier;
class DmInfo: public QObject, public MNSmlDMDbEventHandler
    {
    Q_OBJECT

   public:
    DmInfo(DmAdvancedView* serversview=NULL, QGraphicsItem *parent=0);
       virtual ~DmInfo();   
       void refreshProfileList(TBool aIncludeHidden = ETrue);
       int profilescount();
      int profileinfo(int profilelistnum, QStringList& itemtext, int& icon, bool& lock);       
       void setDefaultProfile(int listitem);      
       bool Isprofilelocked(int profilelistnum);
       bool Isdefaultprofile(int profilelistnum);
       bool isProfileDeleteAllowed(int profilelistnum);
       void profileTransport(int profilelistnum, int& transport);
       TInt DeleteProfile( TInt& aIndex );
       bool createNewprofile();       
       int saveProfile(QStringList& itemdata, bool& sessmode, QString& currap,unsigned int& portnum, bool& nauth );       
       bool getProfiledata(int& itemnum, QStringList& itemdata, bool& sessmode, QStringList& iaplist, int& curriap,  int& portnum, bool& nwauth);
       bool checksrvid(QString& serverid);
       void getIaplist(QStringList& iaplist, int& iapselected, int& listindex);
       void synchronize(int& itemnum);
       void sync(int aProfileId);
       void synccomplete(int);     
       /**
       * Handles DB events
       * @param aEvent DB event
       * @return None
       */
       void HandleDbEventL( TNSmlDMDbEvent aEvent );
       TInt DefaultFotaProfileIdL() const;           
       void DisableDbNotifications( TBool aEvent );
              
private:
       void GetProfiles( );
       TInt findProfile(TInt aProfileId);
       void OpenSyncSession();
       void SetDefaultFotaProfileIdL( const TInt aProfileId );
       
   public slots:
       
      
       


   private:
   //Profile list array
   CArrayFixFlat<TNSmlDMProfileItem>*  iProfileList;
   // Profile list
   CNSmlDMSyncProfileList*             iSyncProfileList;
   //Application engine
   CNSmlDMSyncAppEngine*               iAppEngine;
   // DB notifier
   CNSmlDMDbNotifier*                  iDbNotifier;
   TInt iProfileId;
   // Blocks db events
   TBool                               iDbEventsBlocked;
   CNSmlDMSyncProfile* iProfile;
   
   QMap<QString,int> iapmap;
   QStringList sortediaplist;
   DmAdvancedView* serversView;
   
    };

// CLASS DECLARATION


/**
*  TNSmlDMProfileItem helper class
*/
NONSHARABLE_CLASS (TNSmlDMProfileItem)
    {
    public:
        TBuf<KNSmlMaxProfileNameLength> iProfileName;   
        TInt     iProfileId;
        TBool    iSynced;
        TTime    iLastSync;
        TInt     iBearer;
        TInt     iActive;
        TTime    iLastSuccessSync;
        TBool    iDeleteAllowed;
        TBool    iProfileLocked; 
    };


#endif /* DMINFORETRIEVAL_H_ */
