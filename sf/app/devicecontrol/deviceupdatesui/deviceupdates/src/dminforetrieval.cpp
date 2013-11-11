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
* Description:  Provides methods for DmInfo class. 
*
*/

#include <centralrepository.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include "dmadvancedview.h"
#include "nsmldmsyncprivatecrkeys.h"
#include "dminforetrieval.h"

DmInfo::DmInfo(DmAdvancedView* serversview, QGraphicsItem* /*parent*/)
    {
    iAppEngine = CNSmlDMSyncAppEngine::NewL(this);
    iSyncProfileList = CNSmlDMSyncProfileList::NewL( iAppEngine );
    iProfileList = new (ELeave) CArrayFixFlat< TNSmlDMProfileItem >(1);
    iDbNotifier = CNSmlDMDbNotifier::NewL( iAppEngine->Session(), this );
    iDbNotifier->RequestL();
    serversView = serversview;
    iDbEventsBlocked = EFalse;
    }

DmInfo::~DmInfo()
    {
    delete iProfileList;
    delete iSyncProfileList;
    delete iAppEngine;
    delete iDbNotifier;
    }

void DmInfo::refreshProfileList(TBool aIncludeHidden )
    {    
    FLOG( "[OMADM] DmInfo::refreshProfileList" );
    iProfileList->Reset();
    TKeyArrayFix key(_FOFF( TNSmlDMProfileItem, iProfileName ), ECmpFolded16 );
    iProfileList->Sort(key);
    iSyncProfileList->Reset();
    TRAPD( error, iSyncProfileList->ReadProfileItemsL( aIncludeHidden ) );
    if ( error != KErrNone )
        {
        iSyncProfileList->Reset();
        }

    //read profile values
    for ( TInt index = 0; index < iSyncProfileList->Count(); index++ )
        {
        TNSmlDMProfileItem profile;
        profile.iProfileName = iSyncProfileList->Item( index ).iProfileName;
        if ( profile.iProfileName.Length() == 0 )
            {
            //StringLoader::Load( profile.iProfileName, R_QTN_APPS_EMPTYPROFILE );
            TBuf<20> noname(_L("No NAME"));
            profile.iProfileName = noname;
            }       
        profile.iProfileId   = iSyncProfileList->Item( index ).iProfileId;
        profile.iBearer      = iSyncProfileList->Item( index ).iBearer;
        profile.iSynced      = iSyncProfileList->Item( index ).iSynced;
        profile.iLastSync    = iSyncProfileList->Item( index ).iLastSync;
        profile.iLastSuccessSync     = iSyncProfileList->Item( index ).iLastSuccessSync;
        profile.iActive      = iSyncProfileList->Item( index ).iActive;
        profile.iDeleteAllowed =  iSyncProfileList->Item( index ).iDeleteAllowed;
        profile.iProfileLocked =  iSyncProfileList->Item( index ).iProfileLocked;    
        TInt start = 0;
        TBool found( EFalse );    
        if ( iProfileList->Count() == 0 )
            {
            iProfileList->AppendL ( profile );      
            }
        else
            {
            while ( ( start < iProfileList->Count() ) && ( !found ) )
                {
            if ( iAppEngine->Compare( profile.iProfileName,
                    iProfileList->At( start ).iProfileName ) < 1 )
                {
                iProfileList->InsertL( start, profile );
                found = ETrue;
                }
            start++;
                }
            if ( !found ) //insert to the end of the list
                {
                iProfileList->AppendL( profile );
                }
            }
        }
        FLOG( "[OMADM] CNSmlDMSyncDocument::ReadProfileListL() completed" );
     }
  
int DmInfo::profilescount()
    {
    return iProfileList->Count();
    }
    
int DmInfo::profileinfo(int profilelistnum, QStringList& itemtext, int& icon, bool& lock)
    {
    //TNSmlDMProfileItem* item = ProfileItemAt(profilelistnum);
    TBuf<KNSmlMaxTextLength256> text;
    TBuf<KNSmlMaxTextLength64>  timeString; 
    TBuf<KNSmlMaxTextLength64>  dateString; 
    HBufC* timeFormatted;

    TBool settingEnforcement = TUtil::SettingEnforcementState();
    TBool btObex = FeatureManager::FeatureSupported(KFeatureIdSyncMlDmObex);

    if(profilelistnum >= 0 && iProfileList->Count() > 0)
        {
        
        QString text = QString::fromUtf16(iProfileList->At( profilelistnum ).iProfileName.Ptr(), 
                iProfileList->At( profilelistnum ).iProfileName.Length());
        itemtext << text ;
        if(iProfileList->At( profilelistnum ).iSynced)
            {            
            timeFormatted = TUtil::SyncTimeLC( ( iProfileList->At( profilelistnum ).iLastSuccessSync ));
            QString time = QString::fromUtf16(timeFormatted->Ptr(), 
                    timeFormatted->Length());
            itemtext << time ;
            CleanupStack::PopAndDestroy(1);
            }
        else
            {
            itemtext << hbTrId("txt_device_update_dblist_server_view_l2"); 
            }
        if ( ( iProfileList->At( profilelistnum ).iBearer == EProfileInternet ))
                    {                    
                    icon = 0;
                    }
        else 
            {
        //Bluetooth 
        icon = 1;        
            }
        
        TInt id = DefaultFotaProfileIdL();
        if( id == iProfileList->At( profilelistnum ).iProfileId)
            {
        //Check default or not
        //If default icon = 3;
        icon = 2 ;
            }   
        
        if(settingEnforcement || ( iProfileList->At( profilelistnum ).iProfileLocked))
            {
            lock = true;
            }
        return 1;
        }
    else
        return 0;
    }

// -----------------------------------------------------------------------------
// DmInfo::DefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
TInt DmInfo::DefaultFotaProfileIdL() const
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL()" );
    TInt profileId( KErrNotFound );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );
    User::LeaveIfError( err );
    centrep->Get( KNSmlDMDefaultFotaProfileKey, profileId );
    delete centrep;    
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL() completed, profileId = %d" ),
        profileId ) );
    return profileId;
    }

// -----------------------------------------------------------------------------
// DmInfo::SetDefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
void DmInfo::SetDefaultFotaProfileIdL( const TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );

    User::LeaveIfError( err );
    centrep->Set( KNSmlDMDefaultFotaProfileKey, aProfileId );
    delete centrep;
    
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL() completed" );
    }

void DmInfo::setDefaultProfile(int listitem)
    {
    if(listitem >= 0&& listitem < profilescount())
        {
        SetDefaultFotaProfileIdL(iProfileList->At( listitem ).iProfileId);
        FLOG( "[OMADM] default profile set done" );
        }
    else
        {
        FLOG( "[OMADM] Invalid list item" );
        }
    }

bool DmInfo::Isprofilelocked(int profilelistnum)
    {
    bool lock = false;
    if(profilelistnum >= 0 && iProfileList->Count() > 0)
        {
        TBool settingEnforcement = TUtil::SettingEnforcementState();
        if(settingEnforcement || ( iProfileList->At( profilelistnum ).iProfileLocked))
            {
            lock = true;
            }                  
        }
    return lock;
    }


bool DmInfo::Isdefaultprofile(int profilelistnum)
    {
    bool defaultprofile = false;
    if(profilelistnum >= 0 && iProfileList->Count() > 0)
           {
            TInt id = DefaultFotaProfileIdL();
            if( id == iProfileList->At( profilelistnum ).iProfileId)
                {
                defaultprofile = true;            
                }  
           }
    return defaultprofile;
    }

bool DmInfo::isProfileDeleteAllowed(int profilelistnum)
    {
    bool deleteAllowed = false;
    if(profilelistnum >= 0 && iProfileList->Count() > 0)
        {        
        if(( iProfileList->At( profilelistnum ).iDeleteAllowed))
            {
        deleteAllowed = true;
            }                  
        }
    return deleteAllowed;
    }

void DmInfo::profileTransport(int profilelistnum, int& transport)
    {
    if(profilelistnum >= 0 && profilelistnum < profilescount())
            {
    if ( ( iProfileList->At( profilelistnum ).iBearer == EProfileInternet ))
                       {                    
    transport = 0;
    FLOG( "[OMADM] DmInfo::profileTransport Internet" );
                       }
    else
        {
    transport = 1;
    FLOG( "[OMADM] DmInfo::profileTransport Bluetooth" );
        }
            FLOG( "[OMADM] DmInfo::profileTransport set done" );
            }
        else
            {
            FLOG( "[OMADM] Invalid list item" );
            }
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::DeleteProfile
// -----------------------------------------------------------------------------
//
TInt DmInfo::DeleteProfile( TInt& aIndex )
    {
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::DeleteProfile(), index = %d"),
        aIndex ) );
        
    TInt retVal = KErrNotFound;
    TInt profcount = iProfileList->Count();
    if (  profcount <= 0 && aIndex >= profcount && aIndex < 0)
        {
        return retVal;
        }
    TRAP( retVal, iAppEngine->DeleteProfileL( 
                                (*iProfileList)[aIndex].iProfileId ) );
    if ( retVal == KErrNone )
        {
        //Update profile list
        refreshProfileList();    
        }

    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::DeleteProfile() completed, retval = %d"),
        retVal) );
    return retVal;
    }

bool DmInfo::createNewprofile()
    {
    iProfileId = iAppEngine->CreateProfileL();
    if(iProfileId > 0)
        return true;
    else
        return false;
    }


int DmInfo::saveProfile(QStringList& itemdata, bool& sessmode, QString& currap,unsigned int& portnum, bool& nauth )
    {
    Q_UNUSED(sessmode);
    Q_UNUSED(nauth);	
    FLOG( "[OMADM] void CNSmlDMDlgProfileView::SaveProfileL:" );    
    TRAPD( err, iProfile = iAppEngine->OpenProfileL( iProfileId, ESmlOpenReadWrite ) );
    if( err != KErrNone )
        {
        FLOG( "[OMADM] err != KErrNone--problem in opening the profile" );
        return false;
        }
    TPtrC srvname(reinterpret_cast<const TText*>(itemdata[0].constData()));
    // Server name
    iProfile->SetNameL( srvname );
    // Server Id
    TPtrC srvid(reinterpret_cast<const TText*>(itemdata[1].constData()));
    iProfile->SetServerIdL(srvid );
    // Server password
    TPtrC srvpwd(reinterpret_cast<const TText*>(itemdata[2].constData()));
    iProfile->SetServerPasswordL(srvpwd );
    // connection type
    iProfile->SetBearerTypeL( EAspBearerInternet );
    // Accesspoint
    if(currap.length())
        iProfile->SetAccessPointL( iapmap[currap] );
    else
        iProfile->SetAccessPointL( KErrGeneral ); //Default access point
    // Host address
    TInt num = portnum;
    TPtrC hostaddr(reinterpret_cast<const TText*>(itemdata[3].constData()));
    iProfile->SetHostAddressL( hostaddr, num );
    // User name
    TPtrC usrname(reinterpret_cast<const TText*>(itemdata[4].constData()));
    iProfile->SetUserNameL( usrname );
    // User password
    TPtrC usrpwd(reinterpret_cast<const TText*>(itemdata[5].constData()));
    iProfile->SetPasswordL( usrpwd );                                 
    // Http authentication  
    iProfile->SetHttpUsedL( nauth );
    // http username
    TPtrC httpusrname(reinterpret_cast<const TText*>(itemdata[6].constData()));
    iProfile->SetHttpUserNameL( httpusrname );
    // http password
    TPtrC httpusrpwd(reinterpret_cast<const TText*>(itemdata[7].constData()));
    iProfile->SetHttpPasswordL( httpusrpwd );
    // iProfile->SetCreatorId( KUidSmlSyncApp.iUid );
    iProfileId = 0;
    iProfile->SaveL();
    iAppEngine->CloseProfile();    
    refreshProfileList();//updates profile list    
    return 1;
    }
              
bool DmInfo::getProfiledata(int& itemnum, QStringList& itemdata,
        bool& sessmode, QStringList& iaplist, int& curriap, int& portnum,
        bool& nwauth)
    {
    if (itemnum >= 0 && iProfileList->Count() > 0)
        {
        iProfileId = iProfileList->At(itemnum).iProfileId;
        }
    TRAPD( err, iProfile = iAppEngine->OpenProfileL( iProfileId, ESmlOpenRead ) );    
    if (err == 0)
        {
        //srvname
        TBuf<KBufSize256> bufdata;
        iProfile->GetName(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //srvid
        iProfile->GetServerId(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //srvpwd
        iProfile->GetServerPassword(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //hostaddress

        iProfile->GetHostAddress(bufdata, portnum);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //username
        iProfile->GetUserName(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //usrpwd
        iProfile->GetPassword(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //nwusrname
        iProfile->GetHttpUserNameL(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //nwpwd
        iProfile->GetHttpPasswordL(bufdata);
        itemdata << QString::fromUtf16(bufdata.Ptr(), bufdata.Length());

        //connection type 
        int transporttype = iProfile->BearerType();
        if (((transporttype != EProfileObexBlueTooth) && (transporttype
                != EProfileInternet)) || transporttype == EProfileInternet)
            {
            //default to internet
            sessmode = true;           
            }
        else
            sessmode = false;       
        int apid = iProfile->AccessPointL();
        curriap = apid;
        getIaplist(iaplist, apid, curriap); //append default connection to list
        TBuf<20> apnum;
        RDebug::RawPrint(_L("omadm access point is"));
        apnum.AppendNum(curriap);
        RDebug::RawPrint(apnum);
        //check iap id exists or not 
        nwauth = iProfile->HttpUsedL();
        return true;
        }
    else
        {
        return false;
        }
    }

bool DmInfo::checksrvid(QString& serverid)
    {
    TPtrC srvid(reinterpret_cast<const TText*> (serverid.constData()));
    return iAppEngine->ServerIdFoundL(srvid, iProfileId);
    }

void DmInfo::getIaplist(QStringList& iaplist, int& iapselected,
        int& listindex)
    {
    iapmap.clear();
    RCmManager cmManager;
    cmManager.OpenLC();
    RArray<TUint32> iapidlist;
    cmManager.ConnectionMethodL(iapidlist, EFalse, EFalse, ETrue);
    RCmConnectionMethod conn;
    for (TInt i = 0; i < iapidlist.Count(); i++)
        {
        TRAPD(err, conn = cmManager.ConnectionMethodL( iapidlist[i] ));

        if (err == KErrNone)//connection method exists
            {
            CleanupClosePushL(conn);
            HBufC* name = conn.GetStringAttributeL(CMManager::ECmName);
            QString iapname = QString::fromUtf16(name->Ptr(), name->Length());
            iaplist << iapname;
            iapmap[iapname] = iapidlist[i];
            if (iapselected == iapidlist[i])
                {
                RDebug::RawPrint(_L("omadm iap found"));
                listindex = iapmap.count() - 1;
                }
            delete name;

            CleanupStack::PopAndDestroy(1); // conn
            }
        }
    CleanupStack::PopAndDestroy(1); // cmManager
    }

void DmInfo::synchronize(int& itemnum)
    {
    TBool fotanote = EFalse;
    DisableDbNotifications(ETrue);
    TRAPD( error, iAppEngine->SynchronizeL( iProfileList->At( itemnum ).iProfileName,
                    iProfileList->At( itemnum ).iProfileId,
                    iProfileList->At( itemnum ).iBearer,fotanote ) );
    if(error == KErrNone)   {}             
    }

void DmInfo::sync(int aProfileId)
    {
    int itemNum = findProfile(aProfileId);
    if(itemNum!=KErrNotFound)
        {
        synchronize(itemNum);
        }
    }

void DmInfo::synccomplete(int jobstatus)
    {
    if (jobstatus == ENSmlSyncComplete)
        {
        //iSyncProfileList->ReadProfileItemsL();
        //Update profile list
        refreshProfileList();
        }
    if(serversView!=NULL)
        {
        serversView->syncCompleted(jobstatus);
        }
    iDbEventsBlocked = EFalse;
    }

// -----------------------------------------------------------------------------
// HandleDbEventL
// -----------------------------------------------------------------------------
//
void DmInfo::HandleDbEventL(TNSmlDMDbEvent aEvent)
    {
    FLOG( "[OMADM] DmInfo::HandleDbEventL:" );

    if (!iDbEventsBlocked)
        {
        switch (aEvent.iType)
            {
            case CNSmlDMDbNotifier::EClose:
                iAppEngine->SyncCompleted(ENSmlRefreshMainView);
                break;

            case CNSmlDMDbNotifier::EUpdate:
                iSyncProfileList->ReadProfileItemL(aEvent.iProfileId);
                iAppEngine->SyncCompleted(ENSmlRefreshMainView);
                break;

            case CNSmlDMDbNotifier::EDelete:
                iSyncProfileList->Remove(aEvent.iProfileId);
                iAppEngine->SyncCompleted(ENSmlRefreshMainView);
                break;

            case CNSmlDMDbNotifier::EUpdateAll:
                iSyncProfileList->ReadProfileItemsL();
                iAppEngine->SyncCompleted(ENSmlRefreshMainView);
                break;

            default:
                break;
            }
        }
    FLOG( "[OMADM] DmInfo::HandleDbEventL() completed" );
    }

// -----------------------------------------------------------------------------
// DisableDbNotifications
// -----------------------------------------------------------------------------
//
void DmInfo::DisableDbNotifications(TBool aEvent)
    {
    iDbEventsBlocked = aEvent;
    FLOG( "[OMADM] DmInfo::DisableDbNotifications() completed" );
    }

TInt DmInfo::findProfile(TInt aProfileId)
    {
    for(TInt i=0;i<iProfileList->Count();i++)
        {
        if(iProfileList->At(i).iProfileId == aProfileId)
            {
            return i;
            }
        }
    return KErrNotFound;
    }
