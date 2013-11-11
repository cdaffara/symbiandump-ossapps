/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RCntParserServer implementation.
*
*/


// needed for client interface
#include "clientserver.h"
#include "cntparserserver.h"
#include "cntparserclient.h"

#include "debug.h"

const TUint KDefaultMessageSlots=4;


//------------------------------------------------------------
// RCntParserServer::RCntParserServer()
//------------------------------------------------------------
EXPORT_C RCntParserServer::RCntParserServer():iConnected(ECntParserNotConnected)
	{
	}
//------------------------------------------------------------
// RCntParserServer::Connect()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::Connect()
	{
	LOGGER_ENTERFN( "RCntParserServer::Connect()" );

	TInt r = KErrNone;

	if( iConnected == ECntParserNotConnected )
		{
		r = CreateSession( KCntParserName, Version(), KDefaultMessageSlots );
		if(r==KErrNone) 
			{
			iConnected=ECntParserConnected;	
			}
		else
			{
			RProcess p;
			TInt err = p.Create(KTxtServer, KNullDesC);
	   		if ( err != KErrNone )
           		{
           		// Loading failed.
           		return err;
           		}
			TRequestStatus status;
			p.Rendezvous( status );
			if( status != KRequestPending )
				{
				p.Kill(0);		// abort startup
				p.Close();
				return KErrGeneral;   // status can be KErrNone: don't return status.Int()
				}
			else
				{
				p.Resume();	// logon OK - start the server
				}
			
			User::WaitForRequest( status );

			if( status != KErrNone )
				{
				p.Close();
				return status.Int();
				}

			r = CreateSession( KCntParserName, Version() );
			if( r == KErrNone )
				{
				iConnected = ECntParserConnected;	
				}
			p.Close();
			}
		}
	else
		{
		r = KErrCouldNotConnect;
		}
	LOGGER_WRITE_1( "RCntParserServer::Connect() : return %d", r );
	return r; 
	}
//------------------------------------------------------------
// RCntParserServer::Disconnect()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::Disconnect()
	{
	LOGGER_ENTERFN( "RCntParserServer::Disconnect()" );

	TInt res = KErrNone;
	if(iConnected==ECntParserConnected)
		{
		res = SendReceive( ECntParserServerDisconnect, TIpcArgs(NULL) );
		if( res == KErrNone )
			{
			iConnected = ECntParserNotConnected;
			RSessionBase::Close();
			}
		}
	else
		{
		res = KErrCouldNotDisconnect;
		}
	LOGGER_WRITE_1( "RCntParserServer::CreateIrMCL2PhoneBook() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::Version(void) const
//------------------------------------------------------------
EXPORT_C TVersion RCntParserServer::Version(void) const
	{
	return(TVersion(KCntParserMajorVersionNumber,KCntParserMinorVersionNumber,KCntParserBuildVersionNumber));
	}
//------------------------------------------------------------
// RCntParserServer::CreateIrMCL2PhoneBook(TInt aMaxNumberOfContacts, TBool aForced)
//------------------------------------------------------------
EXPORT_C int RCntParserServer::CreateIrMCL2PhoneBook(TInt aMaxNumberOfContacts, TBool aForced)
	{
	LOGGER_ENTERFN( "RCntParserServer::CreateIrMCL2PhoneBook()" );
    TInt res(KErrNone);
	if( iConnected == ECntParserConnected )
		{
		res = SendReceive(ECntParserServerCreateIrMCL2,TIpcArgs(aMaxNumberOfContacts,aForced));
		}
	else
		{
		res = KErrNotReady;
		}
	LOGGER_WRITE_1( "RCntParserServer::CreateIrMCL2PhoneBook() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::ContactsParsed()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::ContactsParsed()
	{
	LOGGER_ENTERFN( "RCntParserServer::ContactsParsed()" );

	TInt res=KErrNone;
	if( iConnected == ECntParserConnected )
		{
		TPckgBuf<TInt> pckg;
		res = SendReceive(ECntParserServerCntParsed,TIpcArgs(&pckg));
		if( res == KErrNone )
            {
            res = pckg();
            }
		}
	else
		{
		res = KErrNotReady;
		}
	LOGGER_WRITE_1( "RCntParserServer::ContactsParsed() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::TotalNumberOfContacts()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::TotalNumberOfContacts()
	{
	LOGGER_ENTERFN( "RCntParserServer::TotalNumberOfContacts()" );

	TInt res = KErrNone;
	if( iConnected == ECntParserConnected )
		{
		TPckgBuf<TInt> pckg;
		res = SendReceive(ECntParserServerTotalNumOfContacts,TIpcArgs(&pckg));
		if( res == KErrNone)
            {
            res = pckg();
            }
		}
	else
		{
		res = KErrNotReady;
		}
	LOGGER_WRITE_1( "RCntParserServer::TotalNumberOfContacts() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::Cancel()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::Cancel()
	{
	LOGGER_ENTERFN( "RCntParserServer::Cancel()" );

	TInt res = KErrNone;
	if( iConnected == ECntParserConnected )
		{
		res = SendReceive(ECntParserServerCancel,TIpcArgs(NULL));
		}
	else
		{
		res = KErrCancel;
		}
	LOGGER_WRITE_1( "RCntParserServer::Cancel() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::CancelAll()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::CancelAll()
	{
	LOGGER_ENTERFN( "RCntParserServer::CancelAll()" );

	TInt res = KErrNone;
	if(iConnected==ECntParserConnected)
		{
		res = SendReceive(ECntParserServerCancelAll,TIpcArgs(NULL));
		}
	else
		{
		res = KErrCancel;
		}
    LOGGER_WRITE_1( "RCntParserServer::CancelAll() : return %d", res );
	return res;
	}
//------------------------------------------------------------
// RCntParserServer::ContactsSaved()
//------------------------------------------------------------
EXPORT_C TInt RCntParserServer::ContactsSaved()
	{
	LOGGER_ENTERFN( "RCntParserServer::ContactsSaved()" );

	TInt res = KErrNone;
	if( iConnected == ECntParserConnected )
		{
		TPckgBuf<TInt> pckg;
		SendReceive(ECntParserServerCntSaved,TIpcArgs(&pckg));
		res = pckg();
		}
	else
		{
		res = KErrNotReady;
		}
    LOGGER_WRITE_1( "RCntParserServer::ContactsSaved() : return %d", res );
	return res;
	}

