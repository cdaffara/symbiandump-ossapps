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

#include <iapprefs.h>
#include <e32panic.h>


EXPORT_C CImIAPPreferences* CImIAPPreferences::NewLC()
/** Allocates and creates a new CImIAPPreferences object, leaving the object on 
the cleanup stack.

@return New CImIAPPreferences object */
	{
	CImIAPPreferences* self = new (ELeave) CImIAPPreferences();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C TInt CImIAPPreferences::Version() const
/** Gets the version number of the preferences structure.

@return The version number of the preferences structure */
	{
	return iVersion;
	}

void CImIAPPreferences::Reset()
	{
	iChoices->Reset();	
	}


EXPORT_C TInt CImIAPPreferences::NumberOfIAPs() const
/** Gets the number of IAPs that are assigned to the current service entry.

@return Number of IAPs */
	{
	return iChoices->Count();
	}

EXPORT_C TImIAPChoice CImIAPPreferences::IAPPreference(TInt aPreference) const
/** Gets the IAP choice of the given preference value.

Note that preference values run from 0 to n, with 0 being the first choice. 
Currently the number of IAPs per service is limited to two. 

@param aPreference Preference value
@return IAP choice for the given preference value */
	{
	return (*iChoices)[aPreference];
	}

EXPORT_C void CImIAPPreferences::AddIAPL(TImIAPChoice aIap, TInt aIndex)
/** Adds the given IAP with the given preference value. 

Any existing IAPs of equal or lower preference will have their preference 
lowered further to make way for the new IAP. For example, if an IAP of preference 
value 0 is already associated with the current service and another IAP of 
preference 0 is added, then the existing IAP will be relegated to preference 
value 1. Currently the number of IAPs per service is limited to two.

@param aIap IAP
@param aIndex Preference value for this IAP 
@leave Leaves with KErrNotSupported if a SNAP is defined*/
	{
	if (iSnapId)
		{
		User::Leave(KErrNotSupported);
		}
	iChoices->InsertL(aIndex, aIap);
	}

EXPORT_C void CImIAPPreferences::RemoveIAPL(TInt aPreferenceNumber)
/** Removes the IAP of the given preference.

Any existing IAPs of lower preference will have their preference increased 
to fill the gap created by the IAP that has been removed. For example, if 
two IAPs are associated with the current service with the preference values 
0 and 1. If the IAP of preference value 0 is later removed, then the other 
IAP is promoted to preference value 0. It should be noted that adding or removing 
an IAP from a CImIAPPreferences object does not change any global IAP settings, 
it only changes the behaviour associated with a particular with a particular 
email service. 

@param aPreferenceNumber Preference value */
	{
	iChoices->Delete(aPreferenceNumber);
	}

EXPORT_C TInt CImIAPPreferences::FindIAPL(TUint32 aIAP, TInt &aLocation) const
/** Finds the preference value for a specified IAP.
 
@param aIAP The identifier of the IAP to find, as specified in the CommDb record for the IAP
@param aLocation On return, if the specified IAP was found, its preference value
@return KErrNone if the specified IAP was found; KErrNotFound if not.
*/
	{
	TInt count=iChoices->Count();
	for(TInt i=0;i<count;i++)
		{
		if(aIAP==(*iChoices)[i].iIAP)
			{
			aLocation=i;
			return(KErrNone);
			}
		}
	return(KErrNotFound);
	}

EXPORT_C void CImIAPPreferences::ReplaceIAPL(TInt aPreferenceNumber,TImIAPChoice aIap)
/** Replaces the current IAP at a specified preference value.
 
A panic occurs if an IAP is not already set for the preference value. 
@param aPreferenceNumber The preference value for which to set the IAP
@param aIap The new IAP value for the preference value
*/	
	{
	(*iChoices)[aPreferenceNumber]=aIap;
	}

/**
Used to check if SNAP settings are defined for this Email account.

@param None

@return A boolean is returned set as ETrue if a SNAP has been provisioned for the email account.
*/
EXPORT_C TBool CImIAPPreferences::SNAPDefined() const
	{
	if (iSnapId)
		{
		return ETrue;
		}
	return EFalse;
	}

/**
Returns the current SNAP Id for the email account.

@param None

@return The current SNAP id for the email account.
*/	
EXPORT_C TUint32 CImIAPPreferences::SNAPPreference() const
	{
	return iSnapId;
	}

/**
Sets the SNAP id for the email account, to the value specified in aSnap.

@param aSnap
Specifies the new SNAP id to be used by this email account.
@leave Leaves with KErrNotSupported if IAP list already defined
@return A boolean is returned set as ETrue if a SNAP has been provisioned for the email account.
*/
EXPORT_C void CImIAPPreferences::SetSNAPL(TUint32 aSnap)
	{
	// Check that an IAP does not already exist
	if (iChoices->Count())
		{
		User::Leave(KErrNotSupported);
		}
	iSnapId = aSnap;
	}

/**
Removes the SNAP id for this Email account.

@param None

@return A boolean is returned set as ETrue if a SNAP has been provisioned for the email account.
*/	
EXPORT_C void CImIAPPreferences::RemoveSNAP()
	{
	iSnapId = 0;
	}

EXPORT_C CImIAPPreferences::~CImIAPPreferences()
/** Destructor. */
	{
	delete iChoices;
	}

CImIAPPreferences::CImIAPPreferences()
	{
	iVersion = KImIAPPreferencesVersion;
	}

void CImIAPPreferences::ConstructL()
	{
	iChoices = new(ELeave) CArrayFixFlat<TImIAPChoice>(5);
	}

void CImIAPPreferences::Panic(int err) const
	{
	User::Panic(_L("IAPPrefs"), err);
	}
