// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync API GetContactFormat() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestGetContactFormatSteps.h"

void CPhbkSyncGetContactFormatTest_01::CheckContactFieldsFormatV1(RPhoneBookSession::TContactFieldsV1& aContactFormatV1)
	{
	TESTCHECKCONDITION(aContactFormatV1.iNameField.iLength > 0);
	TESTCHECK(aContactFormatV1.iNameField.iCount, 1);
	TESTCHECKCONDITION(aContactFormatV1.iNameField.iDisplayed);
	TESTCHECKCONDITION(aContactFormatV1.iNumberField.iLength > 0);
	TESTCHECK(aContactFormatV1.iNumberField.iCount, 1);
	TESTCHECKCONDITION(aContactFormatV1.iNumberField.iDisplayed);
	TESTCHECK(aContactFormatV1.iIccSlotField.iLength, -1);
	TESTCHECK(aContactFormatV1.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(aContactFormatV1.iIccSlotField.iDisplayed);
	}

void CPhbkSyncGetContactFormatTest_01::CheckContactFieldsFormatV2(RPhoneBookSession::TContactFieldsV2& aContactFormatV2, TUid aPhonebook)
	{
	CheckContactFieldsFormatV1(aContactFormatV2);

	if ((aPhonebook == KUidIccGlobalSdnPhonebook) ||
		(aPhonebook == KUidIccGlobalLndPhonebook) ||
		(aPhonebook == KUidIccGlobalFdnPhonebook))
		{
		TESTCHECK(aContactFormatV2.iAdditionalNumString.iLength, 0);
		TESTCHECK(aContactFormatV2.iAdditionalNumString.iCount, 0);
		TESTCHECKCONDITION(!aContactFormatV2.iAdditionalNumString.iDisplayed);
		TESTCHECK(aContactFormatV2.iGroupField.iLength, 0);
		TESTCHECK(aContactFormatV2.iGroupField.iCount, 0);
		TESTCHECKCONDITION(!aContactFormatV2.iGroupField.iDisplayed);
		TESTCHECK(aContactFormatV2.iEmailField.iLength, 0);
		TESTCHECK(aContactFormatV2.iEmailField.iCount, 0);
		TESTCHECKCONDITION(!aContactFormatV2.iEmailField.iDisplayed);
		}
	else if ((aPhonebook == KUidIccGlobalAdnPhonebook) ||
		(aPhonebook == KUidUsimAppAdnPhonebook))
		{
		// Assuming TMobilePhoneBookInfoV5...
		TESTCHECK(aContactFormatV2.iAdditionalNumString.iLength, 41);
		TESTCHECK(aContactFormatV2.iAdditionalNumString.iCount, 3);
		TESTCHECKCONDITION(aContactFormatV2.iAdditionalNumString.iDisplayed);
		TESTCHECK(aContactFormatV2.iGroupField.iLength, 47);
		TESTCHECK(aContactFormatV2.iGroupField.iCount, 10);
		TESTCHECKCONDITION(aContactFormatV2.iGroupField.iDisplayed);
		TESTCHECK(aContactFormatV2.iEmailField.iLength, 47);
		TESTCHECK(aContactFormatV2.iEmailField.iCount, 3);
		TESTCHECKCONDITION(aContactFormatV2.iEmailField.iDisplayed);
		}

		TESTCHECK(aContactFormatV2.iPhonebook, aPhonebook);
	}

void CPhbkSyncGetContactFormatTest_01::CheckContactFieldsFormatV3(RPhoneBookSession::TContactFieldsV3& aContactFormatV3, TUid aPhonebook)
	{
	CheckContactFieldsFormatV2(aContactFormatV3, aPhonebook);

	if (aPhonebook == KUidIccGlobalSdnPhonebook  ||
		aPhonebook == KUidIccGlobalLndPhonebook  ||
		aPhonebook == KUidIccGlobalFdnPhonebook)
		{
		TESTCHECK(aContactFormatV3.iAdditionalNumAlphaString.iLength, 0);
		TESTCHECK(aContactFormatV3.iAdditionalNumAlphaString.iCount, 0);
		TESTCHECKCONDITION(!aContactFormatV3.iAdditionalNumAlphaString.iDisplayed);
		TESTCHECK(aContactFormatV3.iSecondNameField.iLength, 0);
		TESTCHECK(aContactFormatV3.iSecondNameField.iCount, 0);
		TESTCHECKCONDITION(!aContactFormatV3.iSecondNameField.iDisplayed);
		}
	else if (aPhonebook == KUidIccGlobalAdnPhonebook  ||
			 aPhonebook == KUidUsimAppAdnPhonebook)
		{
		// Assuming TMobilePhoneBookInfoV5...
		TESTCHECK(aContactFormatV3.iAdditionalNumAlphaString.iLength, 47);
		TESTCHECK(aContactFormatV3.iAdditionalNumAlphaString.iCount, 3);
		TESTCHECKCONDITION(aContactFormatV3.iAdditionalNumAlphaString.iDisplayed);
		TESTCHECK(aContactFormatV3.iSecondNameField.iLength, 47);
		TESTCHECK(aContactFormatV3.iSecondNameField.iCount, 1);
		TESTCHECKCONDITION(aContactFormatV3.iSecondNameField.iDisplayed);
		}
	}

void CPhbkSyncGetContactFormatTest_01::PrintGetContactFieldsFormatV1(RPhoneBookSession::TContactFieldsV1& aContactFormatV1)
	{
	INFO_PRINTF1(_L("RPhoneBookSession::TContactFieldsV1:"));
	INFO_PRINTF2(_L("ExtensionId                      =%d"), aContactFormatV1.ExtensionId());

	INFO_PRINTF2(_L("Format: iNameField.iLength       =%d"), aContactFormatV1.iNameField.iLength);
	INFO_PRINTF2(_L("Format: iNameField.iCount        =%d"), aContactFormatV1.iNameField.iCount);
	if (aContactFormatV1.iNameField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iNameField.iDisplayed    =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iNameField.iDisplayed    =False"));
		}

	INFO_PRINTF2(_L("Format: iNumberField.iLength     =%d"), aContactFormatV1.iNumberField.iLength);
	INFO_PRINTF2(_L("Format: iNumberField.iCount      =%d"), aContactFormatV1.iNumberField.iCount);
	if (aContactFormatV1.iNumberField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iNumberField.iDisplayed  =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iNumberField.iDisplayed  =False"));
		}

	INFO_PRINTF2(_L("Format: iIccSlotField.iLength    =%d"), aContactFormatV1.iIccSlotField.iLength);
	INFO_PRINTF2(_L("Format: iIccSlotField.iCount     =%d"), aContactFormatV1.iIccSlotField.iCount);
	if (aContactFormatV1.iIccSlotField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iIccSlotField.iDisplayed =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iIccSlotField.iDisplayed =False"));
		}
	}

void CPhbkSyncGetContactFormatTest_01::PrintGetContactFieldsFormatV2(RPhoneBookSession::TContactFieldsV2& aContactFormatV2)
	{
	PrintGetContactFieldsFormatV1(aContactFormatV2); // up cast

	INFO_PRINTF1(_L("RPhoneBookSession::TContactFieldsV2:"));
	INFO_PRINTF2(_L("ExtensionId                              =%d"), aContactFormatV2.ExtensionId());

	INFO_PRINTF2(_L("Format: iAdditionalNumString.iLength     =%d"), aContactFormatV2.iAdditionalNumString.iLength);
	INFO_PRINTF2(_L("Format: iAdditionalNumString.iCount      =%d"), aContactFormatV2.iAdditionalNumString.iCount);
	if (aContactFormatV2.iAdditionalNumString.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iAdditionalNumString.iDisplayed  =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iAdditionalNumString.iDisplayed  =False"));
		}

	INFO_PRINTF2(_L("Format: iGroupField.iLength              =%d"), aContactFormatV2.iGroupField.iLength);
	INFO_PRINTF2(_L("Format: iGroupField.iCount               =%d"), aContactFormatV2.iGroupField.iCount);
	if (aContactFormatV2.iGroupField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iGroupField.iDisplayed           =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iGroupField.iDisplayed           =False"));
		}

	INFO_PRINTF2(_L("Format: iEmailField.iLength              =%d"), aContactFormatV2.iEmailField.iLength);
	INFO_PRINTF2(_L("Format: iEmailField.iCount               =%d"), aContactFormatV2.iEmailField.iCount);
	if (aContactFormatV2.iEmailField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iEmailField.iDisplayed           =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iEmailField.iDisplayed           =False"));
		}

	INFO_PRINTF2(_L("Format: iPhonebookUid                    =%d"), aContactFormatV2.iPhonebook);
	}

void CPhbkSyncGetContactFormatTest_01::PrintGetContactFieldsFormatV3(RPhoneBookSession::TContactFieldsV3& aContactFormatV3)
	{
	PrintGetContactFieldsFormatV2(aContactFormatV3); // up cast

	INFO_PRINTF1(_L("RPhoneBookSession::TContactFieldsV3:"));
	INFO_PRINTF2(_L("ExtensionId                                  =%d"), aContactFormatV3.ExtensionId());

	INFO_PRINTF2(_L("Format: iAdditionalNumAlphaString.iLength    =%d"), aContactFormatV3.iAdditionalNumAlphaString.iLength);
	INFO_PRINTF2(_L("Format: iAdditionalNumAlphaString.iCount     =%d"), aContactFormatV3.iAdditionalNumAlphaString.iCount);

	if (aContactFormatV3.iAdditionalNumAlphaString.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iAdditionalNumAlphaString.iDisplayed =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iAdditionalNumAlphaString.iDisplayed =False"));
		}

	INFO_PRINTF2(_L("Format: iSecondNameField.iLength             =%d"), aContactFormatV3.iSecondNameField.iLength);
	INFO_PRINTF2(_L("Format: iSecondNameField.iCount              =%d"), aContactFormatV3.iSecondNameField.iCount);

	if (aContactFormatV3.iSecondNameField.iDisplayed)
		{
		INFO_PRINTF1(_L("Format: iSecondNameField.iDisplayed          =True"));
		}
	else
		{
		INFO_PRINTF1(_L("Format: iSecondNameField.iDisplayed          =False"));
		}
	}

/**
 * Test getting the format of a new contact and print this information
 * to screen. Repeat for all supported honebooks.
 */
//
//    Test step1 -- Test#53 in Unit Test Spec. Doc. //
//
CPhbkSyncGetContactFormatTest_01::CPhbkSyncGetContactFormatTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncGetContactFormatTest_01"));
	}

CPhbkSyncGetContactFormatTest_01::~CPhbkSyncGetContactFormatTest_01()
	{
	}

/** 
 * Test API GetContactFormat operation
 */
TVerdict CPhbkSyncGetContactFormatTest_01::doTestStepL( )
	{
	// 1. Load phonebook
	SetSimTsyTestNumberL(0);

	// 2. Loop for all phonebooks to produce contact format info
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++)
		{ 

		// get the contact format (both V1, V2 and V3) and print it out
		RPhoneBookSession::TContactFieldsV3 contactFormatV3;
		contactFormatV3.iPhonebook = iPhoneBook[i];
		RPhoneBookSession::TContactFieldsV3Pckg contactFormatV3Pckg(contactFormatV3);
		TInt returnCode = KErrNone;
		returnCode = iSession.GetContactFormat(contactFormatV3Pckg); // Get contact format

		//If GetContactFormat returns KErrNotReady (because Subscriber ID has not been obtained)
		//Try again up to five times with a 1sec delay before each retry
		if (returnCode == KErrNotReady)
			{
			TInt jj = 0;
			while(jj < 5)
				{
				User::After(1000000);
				returnCode = iSession.GetContactFormat(contactFormatV3Pckg);
				if (returnCode != KErrNotReady)
					{	
					break;
					}
				jj++;
				}
			}

		TESTCHECKL(returnCode, KErrNone);

		//INFO_PRINTF2(_L("Combined V1, V2 & V3 Contact format Package for phonebook %d"), iPhoneBook[i]);
		//PrintGetContactFieldsFormatV3(contactFormatV3);  

		// Check results
		CheckContactFieldsFormatV3(contactFormatV3, iPhoneBook[i]);

		//
		// Get the contact format (V1 and V2 only) and print it out...
		//
		RPhoneBookSession::TContactFieldsV2 contactFormatV2;
		contactFormatV2.iPhonebook = iPhoneBook[i];
		RPhoneBookSession::TContactFieldsV2Pckg contactFormatV2Pckg(contactFormatV2);
		iSession.GetContactFormat(contactFormatV2Pckg); // Get contact format

		//INFO_PRINTF2(_L("V1 & V2 Contact format Package Only for phonebook %d"),iPhoneBook[i]);
		//PrintGetContactFieldsFormatV1(contactFormatV2);  

		CheckContactFieldsFormatV1(contactFormatV2);

		//
		// Get the contact format (V1 only) and print it out...
		//
		RPhoneBookSession::TContactFieldsV1 contactFormatV1;
		RPhoneBookSession::TContactFieldsV1Pckg contactFormatV1Pckg(contactFormatV1);
		iSession.GetContactFormat(contactFormatV1Pckg); // Get contact format

		//INFO_PRINTF2(_L("V1 Contact format Package Only for phonebook %d"),iPhoneBook[i]);
		//PrintGetContactFieldsFormatV1(contactFormatV1);  

		CheckContactFieldsFormatV1(contactFormatV1);
		}

	return TestStepResult();
	}
