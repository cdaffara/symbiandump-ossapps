// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef IAPPrefs__
#define IAPPrefs__

 


#include <e32base.h>
#include <msvapi.h>
#include <msvuids.h>
#include <cdbcols.h>

// Note: Version 1 = EPOC6.1
//		 Version 2 = EPOC6.2 / Hurricane
const TInt KImIAPPreferencesVersion = 2;	// identify which version of this class has been stored

// for builds which don't define it.
/*
enum TCommDbDialogPref
	{
	ECommDbDialogPrefUnknown =0,
	ECommDbDialogPrefPrompt,
	ECommDbDialogPrefWarn,
	ECommDbDialogPrefDoNotPrompt
	};
*/

class TImIAPChoice
/** Sets the connection dialog preference to be used with a particular IAP used 
for an internet mail service.

The connection control component (GenConn) allows various options for whether 
or not the user should be should be prompted with a dialog at connect time. 
For example, a connection using the first choice GPRS IAP might not show a 
dialog, but a second choice connection using GSM might bring up a warning. 
@publishedAll
@released
*/
	{
	public:
	/** The IAP identifier, as specified in the CommDb record for the IAP. */
	TUint32	iIAP;
	/** Preference for what type of dialog is shown before a connection is made using 
	the specified IAP. */
	TCommDbDialogPref iDialogPref;
	};

class CImIAPPreferences : public CBase
/** Encapsulates preferences relating to IAPs for an email service.

An IAP defines all of the variable factors that determine how an Internet 
connection is made. These variable factors can include the bearer (CDMA, GSM 
or GPRS), dial-in number. network login names and passwords. 

The Comms Database is capable of storing details of several IAPs, which can 
then be used to initiate different types of connection. The database also 
stores the preference order of the IAPs: this defines which IAP should be 
used as the first choice, and also an optional second choice to be used if 
the first choice is not available.

The CImIAPPreferences class associates a first choice, and optionally a second 
choice IAP to use with a particular email service. The preference object is 
stored in the service entry's message store.

For SMTP, if no object has been stored in the service, then the SMTP client 
will attempt to use any default Internet settings in the Comms Database, or 
will use the existing Internet connection if one does already exist. Note 
that SMTP sessions will normally fail to send any email messages if the SMTP 
session is not created using an IAP which the SMTP server does not accept.

Note that in Symbian OS v6.0 a single IAP was associated with an email service 
entry through the iMtmData1 field. 
@publishedAll
@released
*/
	{
	public:
		IMPORT_C static CImIAPPreferences* NewLC();
		IMPORT_C TInt Version() const;
		IMPORT_C TInt NumberOfIAPs() const;
		IMPORT_C TImIAPChoice IAPPreference(TInt aPreference) const;
		IMPORT_C void AddIAPL(TImIAPChoice aIap, TInt aIndex=0);
		IMPORT_C void RemoveIAPL(TInt aPreferenceNumber);
		IMPORT_C TInt FindIAPL(TUint32 aIAP, TInt &aLocation) const;
		IMPORT_C void ReplaceIAPL(TInt aPreferenceNumber,TImIAPChoice aIap);
		IMPORT_C ~CImIAPPreferences();
		IMPORT_C TBool SNAPDefined() const;
		IMPORT_C TUint32 SNAPPreference() const;
		IMPORT_C void SetSNAPL(TUint32 aSnap);
		IMPORT_C void RemoveSNAP();
		
		void Reset();
		
		// class constants
		/**
		@deprecated 
		*/
		enum 
			{
			KSanityCheckOldWins = 0xEFBEADDE,
			KSanityCheck = 0xDEADBEEF
			};
		/** ID of the stream used to store IAP preference information in a 
		message store. */
		const static TUid KUidMsgFileInternetAccessPreferences;
	private:
		CArrayFixFlat<TImIAPChoice>* iChoices;
		TInt iVersion;
		TUint32 iSnapId;
	private:
		CImIAPPreferences();
		CImIAPPreferences(CImIAPPreferences& aOther); // not implemented
		int operator=(CImIAPPreferences& aOther); // not implemented
		void ConstructL();
		void Panic(int err) const;
	};

#endif
