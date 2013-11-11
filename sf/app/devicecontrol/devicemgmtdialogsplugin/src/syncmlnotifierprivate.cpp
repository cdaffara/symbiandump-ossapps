/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <e32property.h>
#include <SyncMLClientDM.h>
#include <SyncMLClient.h>
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include <devicedialogsymbian.h>
#include "syncmlnotifierprivate.h"
#include "nsmlconstantdefs.h"



syncmlnotifierprivate::syncmlnotifierprivate()
{
}

void syncmlnotifierprivate::setstatus(TInt value)
{

    qDebug("setstatus");

 
    
    enum TSyncmlHbNotifierKeys 
		{

     EHbSOSNotifierKeyStatus = 11, // status set will complete the client subscribe
     EHbSOSNotifierKeyStatusReturn = 12, // Return the content of actual status value accepted from UI
     
     EHbDMSyncNotifierKeyStatus = 13,
     EHbDMSyncNotifierKeyStatusReturn = 14
		};
  
    TInt err;
    RProperty intProperty;

    if(iNotifierorigin== EHbSOSNotifierKeyStatus)
    {
    TUid uid = {0x101F99FB};
    err = intProperty.Attach(uid , EHbSOSNotifierKeyStatus, EOwnerThread);
    if(err == KErrNone)
       {
        qDebug("EHbSOSNotifierKeyStatus entered no error"); 
        intProperty.Set(value);
        RProperty::Set(uid, EHbSOSNotifierKeyStatusReturn, value);
        qDebug("Set status of return value");
        qDebug()<<err;
        qDebug("Value user selected");
        qDebug()<<value;
       }
    
    intProperty.Close();
    }
    else
    {
    TUid uid = {0x101F6DE5};
    err = intProperty.Attach(uid , EHbDMSyncNotifierKeyStatus, EOwnerThread);
    if(err == KErrNone)
       {
        qDebug("EHbDMSyncNotifierKeyStatus entered no error"); 
        intProperty.Set(value);
        err = RProperty::Set(uid, EHbDMSyncNotifierKeyStatusReturn, value);
        qDebug("Set status of return value");
        qDebug()<<err;
        qDebug("Value user selected");
        qDebug()<<value;
       }
    
    intProperty.Close();
    }
}

void syncmlnotifierprivate::getdmprofileparameters(QString& servername, TInt profileid)
{
	 
    TBuf<256> serverdisplayname;
   
    RSyncMLSession syncSession;
    
    syncSession.OpenL();
    CleanupClosePushL( syncSession );
        
    RSyncMLDevManProfile syncProfile;
    syncProfile.OpenL( syncSession, profileid, ESmlOpenRead );
    CleanupClosePushL( syncProfile );
        
    serverdisplayname= syncProfile.DisplayName();
 

    //convert TBuf serverName to QString
    QString qString((QChar*)serverdisplayname.Ptr(),serverdisplayname.Length());
    servername = qString;

        
        
    syncProfile.Close();
    CleanupStack::Pop( &syncProfile );
     
    syncSession.Close();
    CleanupStack::Pop( &syncSession );
}


TInt syncmlnotifierprivate::serverinittype(TInt uimode)
    {

   
    TInt SanSupport = 0;
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    User::LeaveIfError( err );
    centrep->Get( KDevManSANUIBitVariation, SanSupport );
    delete centrep;
    
    //if SAN is specified then silent operation
    
    if(( SanSupport == 1 ))
        {
        if( uimode > ESANUserInteractive && uimode < ESANNotSpecified )
            {
            uimode = ESANNotSpecified;
            }
       
        if( uimode == ESANNotSpecified || uimode == ESANUserInteractive)
            {
            return ESANUserInteractive;
            }
        if( uimode == ESANUserInformative)
            {
            return ESANUserInformative;
            }
        }
   
      return ESANSilent;
        
      
    }

void syncmlnotifierprivate::setnotifierorigin(TInt notifierorig)
    {
    iNotifierorigin = notifierorig;
    }
    
