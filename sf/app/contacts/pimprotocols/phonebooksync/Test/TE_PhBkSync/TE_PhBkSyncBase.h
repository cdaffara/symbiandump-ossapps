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
//

/**
 @file
 @internalComponent
*/
   
#ifndef __TE_PHBKSYNCBASE_H__
#define __TE_PHBKSYNCBASE_H__

#include <e32base.h>
#include <e32def.h>
#include <e32test.h>
#include <c32comm.h>
#include <e32keys.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <cntdb.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>
#include <connect/sbeclient.h>

#include "phbksync.h"
#include "simtsy.h"

using namespace conn;


_LIT(KPhbkSyncIniFile,"C:\\Private\\102030A1\\Phbksync.ini");

//
// Test macros
//
_LIT(KTestCheckFailText, "Failed: Got %d while expecting %d.");
_LIT(KTestCheckConditionFailText, "Failed: Condition was false.");
_LIT(KTestCheckStringFailText, "Failed: Got \"%S\" while expecting \"%S\".");

#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &(a), &(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckFailText, a, b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(KTestCheckStringFailText, &(a), &(b)); \
        SetTestStepResult(EFail); \
        } \
    }


_LIT16(KTel,"1234567890");

_LIT16(KName1, "Name1");
_LIT16(KName2, "Name2");
_LIT16(KName3, "Name3");
_LIT16(KName4, "Name4");
_LIT16(KName5, "Name5");

_LIT16(KAdditionalNumber1, "2345678901");
_LIT16(KAdditionalNumber2, "3456789012");
_LIT16(KAdditionalString1, "NumberString1");
_LIT16(KAdditionalString2, "NumberString2");

//static RMobilePhone::TMobileTON KAdditionalTON1 = RMobilePhone::EUnknownNumber;
//static RMobilePhone::TMobileTON KAdditionalTON2 = RMobilePhone::ENationalNumber;

_LIT16(KGroup1, "Group1");
_LIT16(KGroup2, "Group2");
_LIT16(KGroup3, "Group3");
_LIT16(KGroup4, "Group4");
_LIT16(KGroup5, "Group5");

_LIT16(KEmail1, "EmailName1@some.com");
_LIT16(KEmail2, "EmailName2@some.com");
_LIT16(KEmail3, "EmailName3@some.com");
_LIT16(KEmail4, "EmailName4@some.com");
_LIT16(KEmail5, "EamilName5@some.com");
//===========

_LIT(KInternationalPrefix,"+");

//static TUid KUidIccGlobalAdnPhonebookCallByRef = {KUidIccGlobalAdnPhonebookValue};
//static TUid KUidIccGlobalSdnPhonebookCallByRef = {KUidIccGlobalSdnPhonebookValue};
//static TUid KUidIccGlobalLndPhonebookCallByRef = {KUidIccGlobalLndPhonebookValue};
//static TUid KUidUsimAppAdnPhonebookCallByRef = {KUidUsimAppAdnPhonebookValue};
//static TUid KUidIccGlobalFdnPhonebookCallByRef = {KUidIccGlobalFdnPhonebookValue};

enum {KTotalNumberOfPhoneBook = 5};
enum {KOneSeconduS = 1000000};		// 1 second in uS

const TInt KFieldLength = 16; 
const TInt KSyncArrayGranularity=4;
const TInt KMaxPhBkNameSize=241;


// 
// This class is copied from phbksync server side since it is not supposed
// to be seen by client. This is defined here just for test purpose.
//
class MyCSyncContactICCEntry : public CBase
	{
public:
	typedef TBuf16<KMaxPhBkNameSize> TSyncEntryName;
	typedef TBuf16<RMobilePhone::KMaxMobileTelNumberSize> TSyncEntryNumber;

	class TSyncAdditionalNumber
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		TSyncEntryNumber iNumber;       // Additional number 
		TSyncEntryName iNumberString;	// Alpha string associated with 
		                                // additional number 
		RMobilePhone::TMobileTON iTON;  // TON associated with additional number
		};

	enum TSyncPhoneBook
		{
		EIccPhonebookNotSpecified,
		EIccGlobalAdnPhonebook,
		EIccGlobalSdnPhonebook,
		EIccGlobalLndPhonebook,
		EUsimAppAdnPhonebook,
		EIccGlobalFdnPhonebook
		};

public:
	static MyCSyncContactICCEntry* NewL();
	~MyCSyncContactICCEntry();
	//	CBufBase* StoreL();
	//	void StoreL(TDes8& aDes);
	//	void RestoreL(const TDesC8& aBuf);
	//	void InternalizeL(RReadStream& aStream);
	//	void ExternalizeL(RWriteStream& aStream) const;
private:
	MyCSyncContactICCEntry();
	void ConstructL();
public:
	// Usually two representations of same name
	CArrayFixFlat<TSyncEntryName>* iNameList; 

	TSyncEntryNumber iNumber;       // Phone number 

	// List of additional numbers
	CArrayFixFlat<TSyncAdditionalNumber>* iNumberList; 

	// List of groups this entry belongs to
	CArrayFixFlat<TSyncEntryName>* iGroupList; 

	// List of e-mails associated with this entry
	CArrayFixFlat<TSyncEntryName>* iEmailList; 
	TInt iSlotNum;			// Slot number 
	RMobilePhone::TMobileTON iTON;	// TON associated with iNumber
	TContactItemId iContactUID;     // Contacts Unique ID
	TSyncPhoneBook iPhonebook; // Phonebook that this entry belongs to

	// Hidden entry flag associated with USIM App phonebooks 
	// only - used when user is creating a hidden entry 
	TBool iIsHidden;				

	// This is needed for new entries only, so that server can map this group
	TContactItemId iGroupId;		    
	                                          
	};


class CTestStepPhbkSync : public CTestStep 
	{
public:

	CTestStepPhbkSync();

	// common phone book synchronisation operations
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
	void DestroyDefaultIniFileL();
	void DestroyDefaultFilesL();

	void ChangeSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode, 
			     TUid aPhonebookUid);
	void CheckSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode, 
			    TUid aPhonebookUid);
	void CheckCacheStateValidL(TUid aPhonebookUid);
	void CheckCacheStateInvalidL(TUid aPhonebookUid);
	void CheckCacheStateErrorL(TUid aPhonebookUid);
 	
	void WaitForIccToBecomeLockedL();


	MyCSyncContactICCEntry::TSyncPhoneBook convertPhonebookUid2Enum(TUid aPhonebookUid);

	// This method is called by all WriteContact() related test steps.
	// This method accepts the internal format (see MyCSyncContactICCEntry
	// defined in this header file) and then converts it into the 
	// external/standard format before calling the iSession.WriteContact(). 
	// By doing that, only a simple ICC entry data structure is exposed 
	// to the tetsers. 
	void DoWriteContactToICCL(MyCSyncContactICCEntry* iccEntryInternal, 
				  TUid&                 aPhonebookUid, 
				  TRequestStatus&	aStatus,
				  RPhoneBookSession* aSession = NULL);

	void DoSyncL(TUid aPhonebookUid, TInt expectedErr, TBool aIgnoreErr=EFalse);
	void DoSyncAndNotificationL(TUid aPhonebookUid, TInt aExpectedErr,
		                        RPhoneBookSession::TSyncState aExpectedState,
								TRequestStatus& aNotifyStatus, TBool aIgnoreErr=EFalse);


	void SetSimTsyTestNumberL(TInt aTestNumber, TBool aIgnoreErr=EFalse);
	void SignalSimTsyToReduceTimersL();

	void AddTextFieldToIccContactL(TStorageType	 aType, 
				       TFieldType	 aFieldType, 
				       TUid	         aMapping, 
				       TDesC&		 aField, 
				       CContactICCEntry* aIccEntry,
					   TInt aCount);
	TContactItemId GetValidUIDFromContactsDbL(TUid aPhonebookUid, 
						 TBool aHiddenRequired=EFalse);
	TInt GetValidSlotFromContactsDbL(TUid aPhonebookUid, 
					TBool aHiddenRequired=EFalse);

	void InitialiseIccEntryInternalOldL(MyCSyncContactICCEntry& aIccEntryInternal);
	void InitialiseIccEntryInternalOldL(MyCSyncContactICCEntry* aIccEntryInternal);

	void InitialiseIccEntryInternalNewL(MyCSyncContactICCEntry& aIccEntryInternal);
	void InitialiseIccEntryInternalNewL(MyCSyncContactICCEntry* aIccEntryInternal);

	void ConfigurePhbkSyncToIdleL();
	void ConfigurePhbkSyncToFullL();
	
	RPhoneBookSession iSession;

public:
	TUid iPhoneBook[KTotalNumberOfPhoneBook]; 
	TUid iUidIccGlobalAdnPhonebookCallByRef;
	TUid iUidIccGlobalSdnPhonebookCallByRef;
	TUid iUidIccGlobalLndPhonebookCallByRef;
	TUid iUidUsimAppAdnPhonebookCallByRef;
	TUid iUidIccGlobalFdnPhonebookCallByRef;
	
private:
	CActiveScheduler*  iScheduler;
	CSBEClient*  iSecureBackupEngine;
	};

#endif // __TE_PHBKSYNCBASE_H__

