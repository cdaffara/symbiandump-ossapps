// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "cactivewaiter.h"
#include "ctestimapservermtmbase.h"
#include <iapprefs.h>
#include <cemailaccounts.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT( KLoopbackAddress, "127.0.0.1" );
//_LIT( KLoopbackAddress, "10.22.64.6" );	// Used for online testing

const TInt KImapPort = 143;

//RTest test( _L( "IMPS.dll Unit Test" ) );

CTestImapServerMtmBase::CTestImapServerMtmBase( ) :
	iTestUtils( NULL ),
	iImapServerMtm( NULL ),
	iSpoofServer( NULL ),
	iAccounts( NULL ),
	iActiveWaiter( NULL ),
	iSelection( NULL ),
	iTest( _L( "IMPS.dll Unit Test" ) )
	{
	
	}
	
CTestImapServerMtmBase::~CTestImapServerMtmBase( )
	{
	delete iAccounts;
	delete iSpoofServer;
	delete iImapSettings;
	delete iTestUtils;
	delete iActiveWaiter;
	delete iSelection;
	
	}


/**
Set up environment
*/	
void CTestImapServerMtmBase::SetupL( )
	{
	DoLoadDriversL( );
	
	// Load Server MTM (Can this be in the TEFUnit server?)	
//	iTestUtils = CEmailTestUtils::NewL( test );
	iTestUtils = CEmailTestUtils::NewL( iTest );
	iTestUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );

// TODO: How to clean the message folders or close the messaging server without a PANIC ????

//	iTestUtils->CleanMessageFolderL( );
	iTestUtils->ClearEmailAccountsL( );
	
	iTestUtils->CreateAllTestDirectories( );
	iTestUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );
	
	iTestUtils->GoServerSideL( );
	iTestUtils->InstantiateImapServerMtmL( );
	iImapServerMtm = iTestUtils->iImapServerMtm;
	
	// Create an account
	iTestUtils->GoClientSideL( );
	iAccounts = CEmailAccounts::NewL( );	
	DoCreateImapAccountL( );
	
	// Delete iAccounts to make sure that CMsvServer is closed and 
	// we can go to the serverSide
	delete iAccounts;
	iAccounts = NULL;
	
	iTestUtils->GoServerSideL( );
	
	iEntry = iTestUtils->iServerEntry;

	iServiceId = iImapAccount.iImapService;
	iTestUtils->InstantiateImapServerMtmL( );
	iImapServerMtm = iTestUtils->iImapServerMtm;

	// Creata active waiter
	iActiveWaiter = new ( ELeave ) CActiveWaiter( );

	iEntry = iTestUtils->iServerEntry;		
	
	iImapSettings = CImapSettings::NewL(*iEntry);
	iImapSettings->LoadSettingsL(iServiceId);
	// and CMsvSelection
	iSelection = new ( ELeave ) CMsvEntrySelection( );
	}
	
/**
Tear down environment
*/
void CTestImapServerMtmBase::TearDownL( )
	{
	// tear Down
	delete iAccounts;
	iAccounts = NULL;
	
	delete iSpoofServer;
	iSpoofServer = NULL;

// TODO: cleanup
//	iTestUtils->GoClientSideL( );
//	iTestUtils->iMsvSession->CloseMessageServer( );
//	iTestUtils->Reset( );
	
	_LIT(KMsvServerPattern, "!MsvServer*");
	TFindProcess findprocess(KMsvServerPattern);
	TFullName name;

	// wait for the server to close before continuing
//	for(;;)
//		{   
//		TFindServer find(KMsvServerPattern);
//		if (find.Next(name) != KErrNone)
//			break;
//		User::After(100000);
//	}

	delete iTestUtils;
	iTestUtils = NULL;	
	
	delete iActiveWaiter;
	iActiveWaiter = NULL;

	delete iSelection;
	iSelection = NULL;
	
	}

void CTestImapServerMtmBase::DoConnectL( )
	{
	INFO_PRINTF1( _L("Connecting...") );
	iSelection->Reset( );
	iSelection->AppendL( iImapAccount.iImapService );
	
	iImapServerMtm->StartCommandL( *iSelection, 
					    		   KIMAP4MTMConnect, 
								   KNullDesC8, 
								   iActiveWaiter->iStatus );

	iActiveWaiter->WaitActive( );

	}

void CTestImapServerMtmBase::DoDisconnectL( )
	{
	INFO_PRINTF1( _L("Disconnecting...") );
	iSelection->Reset( );
	iImapServerMtm->StartCommandL( *iSelection,
								   KIMAP4MTMDisconnect,
								   KNullDesC8,
								   iActiveWaiter->iStatus );
								   
	iActiveWaiter->WaitActive( );
	
	}

void CTestImapServerMtmBase::DoCreateImapAccountL( )
	{
	INFO_PRINTF1( _L("Create IMAP account") );
	//create objects and initialise with the defaults
	CImImap4Settings* imap4Settings = new ( ELeave ) CImImap4Settings;
	CleanupStack::PushL( imap4Settings );
	
	CImSmtpSettings* smtpSettings = new ( ELeave ) CImSmtpSettings;
	CleanupStack::PushL( smtpSettings );
	
	CImIAPPreferences* imapIap = CImIAPPreferences::NewLC( );
	CImIAPPreferences* smtpIap = CImIAPPreferences::NewLC( );
	
	iAccounts->PopulateDefaultImapSettingsL( *imap4Settings, *imapIap );
	iAccounts->PopulateDefaultSmtpSettingsL( *smtpSettings, *smtpIap );
	
	//override some of the defaults
	imap4Settings->SetPasswordL( _L8( "prakash" ) );
	imap4Settings->SetLoginNameL( _L8( "prakash" ) );
	imap4Settings->SetServerAddressL( KLoopbackAddress );
	imap4Settings->SetFolderPathL( _L8( "" ) );
	imap4Settings->SetSynchronise( EUseLocal );
		
	//create the account
	iImapAccount = iAccounts->CreateImapAccountL( _L( "TestAccount" ), *imap4Settings, *imapIap, EFalse );
	//clean up
	CleanupStack::PopAndDestroy( 4,imap4Settings );
	
	INFO_PRINTF1( _L("IMAP account created") );
	}

void CTestImapServerMtmBase::DoLoadDriversL( )
	{
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	}
	

	
void CTestImapServerMtmBase::DoStartSpoofServerL( const TDesC& aScriptSection )
	{
	// Start Spoof Server
	INFO_PRINTF2( _L( "Loading spoofscript: " ), aScriptSection );
	TPtrC spoofScript;
	User::LeaveIfError( iConfig.GetString( aScriptSection, 
								  		   _L( "ScriptFile" ), 
								  		   spoofScript ) );
	
	INFO_PRINTF1( _L( "Starting spoofserver" ) );
	iSpoofServer = CSpoofServer::NewL( *this, spoofScript );
	iSpoofServer->StartL( KImapPort );
	INFO_PRINTF1( _L( "Spoofserver started" ) );
	
	}
	
void CTestImapServerMtmBase::TestComplete( TInt /*aErrorCode*/ )
	{
	// TODO
	}
