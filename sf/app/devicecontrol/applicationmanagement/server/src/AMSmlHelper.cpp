/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#include <SyncMLClientDM.h>
#include <rconnmon.h> 
#include "amsmlhelper.h"
#include "debug.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock.h>
#else
#include <es_sock.h>
#include <es_enum_internal.h>
#include <es_sock_partner.h>
#endif
#include <es_enum.h>

const TUid KUidNSmlMediumTypeInternet =
    {
    0x101F99F0
    };
const TUid KSosServerUid =
    {
    0x101F99FB
    };

const TInt KBufSize32 = 32;

using namespace NApplicationManagement;

/*
 void CApplicationManagementServer::StartDMSessionL( )
 {
 RSyncMLSession ses;
 ses.OpenL() ;
 
 CleanupClosePushL( ses );
 TSmlJobId jid;
 TSmlUsageType jtype;
 ses.CurrentJobL(jid, jtype );
 RSyncMLDevManJob job;
 job.OpenL( ses, jid );
 CleanupClosePushL( job ) ;
 TSmlProfileId pid( job.Profile() );
 RSyncMLDevManProfile prof;
 prof.OpenL( ses, pid, ESmlOpenRead );
 
 TBuf<64> serverid;
 serverid.Copy( prof.ServerId()) ;
 prof.Close();
 
 CleanupStack::PopAndDestroy( &job )	;
 CleanupStack::PopAndDestroy( &ses )	;
 }
 */

TInt SmlHelper::GetCurrentProfileIdL(TSmlProfileId &aProfile)
    {
    RSyncMLSession ses;
    ses.OpenL() ;
    CleanupClosePushL(ses);
    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    CleanupClosePushL(job) ;
    TSmlProfileId pid(job.Profile() );
    aProfile = pid;

    CleanupStack::PopAndDestroy( &job) ;
    CleanupStack::PopAndDestroy( &ses) ;
    return 0;
    }

HBufC8* SmlHelper::GetCurrentServerIDL()
    {
    HBufC8 *serverid;
    //TBufC<256> serverid;
    RSyncMLSession ses;
    ses.OpenL() ;
    CleanupClosePushL(ses);
    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    CleanupClosePushL(job) ;
    TSmlProfileId pid(job.Profile() );
    //aProfile = pid;
    RSyncMLDevManProfile syncProfile;
    syncProfile.OpenL(ses, pid);
    RDEBUG("	5/8");
    CleanupClosePushL(syncProfile);

    serverid = syncProfile.ServerId().AllocLC();

    CleanupStack::Pop(); // serverid

    CleanupStack::PopAndDestroy( &syncProfile);
    CleanupStack::PopAndDestroy( &job) ;
    CleanupStack::PopAndDestroy( &ses) ;

    return serverid;
    }

TInt SmlHelper::LaunchSessionL(TSmlProfileId &aProfile)
    {
    if (aProfile > -1)
        {
        RSyncMLSession syncSession;
        RSyncMLConnection conn;
        RSyncMLDevManJob dmJob;
        RSyncMLDevManProfile dmProfile;

        //TSmlProfileId id ;
        TSmlConnectionId cid;
        syncSession.OpenL();
        CleanupClosePushL(syncSession);

        TRAPD( err, dmProfile.OpenL( syncSession, aProfile, ESmlOpenRead ) )
        if (err == KErrNone)
            {
            CleanupClosePushL(dmProfile);
            conn.OpenL(dmProfile, KUidNSmlMediumTypeInternet.iUid);
            CleanupStack::PopAndDestroy( &dmProfile) ;
#ifdef __TARM_SYMBIAN_CONVERGENCY
            cid = conn.Identifier();
#else
            cid = conn.ConnectionId();
#endif
            conn.Close();
            dmJob.CreateL(syncSession, aProfile, cid);
            dmJob.Close();
            }
        CleanupStack::PopAndDestroy( &syncSession) ; //
        return err;
        }
    else
        {
        return KErrArgument;
        }
    }

void SmlHelper::GetDefaultIAPFromDMProfileL(TInt& aDefaultIAP)
    {
    RDEBUG("ApplicationManagementServer::GetDefaultIAPFromDMProfileL() - start");

    // first try to obtain the current profile id
    RSyncMLSession ses;
    ses.OpenL();
    RDEBUG("	1/8");
    CleanupClosePushL(ses);

    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    RDEBUG("	2/8");
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    RDEBUG("	3/8");
    CleanupClosePushL(job);
    // get profile id
    TSmlProfileId pid(job.Profile() );
    CleanupStack::PopAndDestroy( &job);

    TBuf8<KBufSize32> key;
    TBuf<KBufSize32> value;
    TInt num = KErrNotFound;

    // get connection property name first
    GetConnectionPropertyNameL(ses, key, EPropertyIntenetAccessPoint);
    RDEBUG("	4/8");
    // we need to open sync profile in ordere to open the RSyncMLConnection
    RSyncMLDevManProfile syncProfile;
    syncProfile.OpenL(ses, pid);
    RDEBUG("	5/8");
    CleanupClosePushL(syncProfile);

    TSmlTransportId transport;
    RArray<TSmlTransportId> connections;
    _LIT8( KNSmlIAPId, "NSmlIapId" );

    // now open the syncML connection
    RSyncMLConnection syncConnection;
    syncProfile.ListConnectionsL(connections);

    transport = connections[0];
    syncConnection.OpenL(syncProfile, transport);
    RDEBUG("	6/8");
    CleanupClosePushL(syncConnection);

    const TDesC8& source(syncConnection.GetPropertyL(KNSmlIAPId) );
    RDEBUG("	7/8");
    value.Copy(source.Left(value.MaxLength() ) );
    User::LeaveIfError(StrToInt(value, num) );
    RDEBUG("	8/8");

    aDefaultIAP = num;

    // If there is no default Access point in DM profile then use
    // currently used access point by DM session

    if (aDefaultIAP == -2)
        {

        TInt sockIapid = -1;

        RSocketServ serv;
        CleanupClosePushL(serv);
        User::LeaveIfError(serv.Connect() );

        RConnection conn;
        CleanupClosePushL(conn);
        User::LeaveIfError(conn.Open(serv) );

        TUint count( 0);
        User::LeaveIfError(conn.EnumerateConnections(count) );
        // enumerate connections
        for (TUint idx=1; idx<=count; ++idx)
            {
            TConnectionInfo connectionInfo;
            TConnectionInfoBuf connInfo(connectionInfo);

            TInt err = conn.GetConnectionInfo(idx, connInfo); // iapid
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy( 2); // conn, serv
                User::Leave(err);
                }
            // enumerate connectionclients
            TConnectionEnumArg conArg;
            conArg.iIndex = idx;
            TConnEnumArgBuf conArgBuf(conArg);
            err=conn.Control(KCOLConnection, KCoEnumerateConnectionClients,
                    conArgBuf);
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy( 2); // conn, serv
                User::Leave(err);
                }
            TInt cliCount = conArgBuf().iCount;
            for (TUint j=1; j<=cliCount; ++j)
                {
                TConnectionGetClientInfoArg conCliInfo;
                conCliInfo.iIndex = j;
                TConnGetClientInfoArgBuf conCliInfoBuf(conCliInfo);
                err=conn.Control(KCOLConnection, KCoGetConnectionClientInfo,
                        conCliInfoBuf);

                if (err != KErrNone)
                    {
                    CleanupStack::PopAndDestroy( 2); // conn, serv
                    User::Leave(err);
                    }
                TConnectionClientInfo conCliInf = conCliInfoBuf().iClientInfo;
                TUid uid = conCliInf.iUid;
                if (uid == KSosServerUid)
                    {
                    sockIapid = connInfo().iIapId;

                    }

                }
            }
        CleanupStack::PopAndDestroy( 2); // conn, serv		

        aDefaultIAP = sockIapid;

        }
    // get profile's server id
    //aServerId = syncProfile.ServerId().AllocL();

    CleanupStack::PopAndDestroy( &syncConnection);
    CleanupStack::PopAndDestroy( &syncProfile);

    CleanupStack::PopAndDestroy( &ses);
    connections.Close();
    RDEBUG("ApplicationManagementServer::GetDefaultIAPFromDMProfileL() - end");
    }

void SmlHelper::GetConnectionPropertyNameL(RSyncMLSession& aSyncSession,
        TDes8& aText, TInt aPropertyPos)
    {
    RDEBUG("ApplicationManagementServer::GetConnectionPropertyNameL() - start");

    // at the moment RSyncMLTransport is only needed
    // for internet connection settings
    RSyncMLTransport transport;
    CleanupClosePushL(transport);
    transport.OpenL(aSyncSession, KUidNSmlMediumTypeInternet.iUid);

    const CSyncMLTransportPropertiesArray& arr = transport.Properties();

    __ASSERT_DEBUG( arr.Count()> aPropertyPos, User::Panic(_L("invalid count"), KErrGeneral) );

    const TSyncMLTransportPropertyInfo& info = arr.At(aPropertyPos);
    aText = info.iName;
    CleanupStack::PopAndDestroy( &transport);

    RDEBUG("ApplicationManagementServer::GetConnectionPropertyNameL() - end");
    }

TInt SmlHelper::StrToInt(const TDesC& aText, TInt& aNum)
    {
    TLex lex(aText);
    TInt err = lex.Val(aNum);
    return err;
    }

// End of File
