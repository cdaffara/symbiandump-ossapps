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

#ifndef T_SMCM2_H_
#define T_SMCM2_H_
#include <e32base.h>

#include <s32std.h>
#include <txtrich.h>    //CRichText
#include <e32test.h>
#include <msvuids.h>	// ServiceEntry, FolderEntry, etc.
#include <mtmuids.h>	// query capabilities
#include "smutset.h"	// sms settings
#include "smuthdr.h"	// sms header
#include "smsclnt.h"	// sms (a)sync commands
#include "smstestbase.h"
#include <csmsgetdetdescinterface.h>

#include <commsdattypesv1_1.h>
#include <commsdat.h>
using namespace CommsDat;

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif

class CSmsEmailFields;

GLDEF_D RTest				test(_L("SMCM Test Harness"));
GLDEF_D CTrapCleanup*		theCleanup;

const TUid KUidSmcmTestNonSense = {0x00011111};
const TUid KFileStreamId		= {0x01010101};

const TMsvPartList KMsvMessagePartBody			= 0x00000001;
const TMsvPartList KMsvMessagePartRecipient		= 0x00000002;
const TMsvPartList KMsvMessagePartOriginator	= 0x00000004;
const TMsvPartList KMsvMessagePartDescription	= 0x00000008;
const TMsvPartList KMsvMessagePartDate			= 0x00000010;
const TMsvPartList KMsvMessagePartAttachments	= 0x00000020;

GLDEF_C TInt E32Main(void);
LOCAL_C void doMainL(void);

class TCompare
	{
	public:
		TCompare(TBool aVal = ETrue) {iValue = aVal;}
		TCompare(const TCompare& aVal) {iValue = aVal.iValue;}
		void operator()(TBool aResult) {iValue = iValue && aResult;}
		operator TBool() const {return iValue;}
		TBool iValue;
	};

class CSmutTest : public CSmsTestBase
	{
public:
	static CSmutTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmutTest();

private:
	CSmutTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL();

	//Test Functions
	void TestSettingsL();
	void TestHeaderL();
	void TestSmsUtilitiesL();
	void TestSmsUtilities1L(CSmsHeader& aHeader);
	void TestSmsUtilities2L(CSmsHeader& aHeader);
	void TestSmsUtilities3L(CSmsHeader& aHeader);
	void TestSmsUtilitiesTimeStampsL();
	void TestInvalidTelephone(const TDesC& aTel, TDes& aDetails);
	void TestInvalidContact(const TDesC& aTel, TDes& aDetails);
	void TestDuplicateContact(const TDesC& aTel, TDes& aDetails);
	void TestValidTel(const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails);
	void TestSmsEmsExtensionsL();

	void TestSmsUtilitiesBL();
	void TestSmsUtilities4L(CSmsHeader& aHeader);
	void TestSmsUtilities5L(CSmsHeader& aHeader);
	void TestInvalidTelephone(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails);
	void TestInvalidContact(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails);
	void TestDuplicateContact(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails);
	void TestValidTel(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails);

	//CSmsSettings utilities/tests
	void TestSettingsStoreToCommDbL();
	TBool CompareSettingsAndCommDbL(const CSmsSettings& aSettings) const;
	TBool CompareSettingsAndSmsBearerL(const CSmsSettings& aSettings) const;
//	void OpenCommDbModemTableLCC(CCommsDatabase*& rCommDb, CCommsDbTableView*& rTable) const;
	void TestSettingsRestoreDefectL();
	void TestRevertSettingsL();
	TBool CompareSettings(const CSmsSettings& aLeft, const CSmsSettings& aRight, TBool aTestSC = ETrue);
	TBool CompareMessageSettings(const CSmsMessageSettings& aLeft, const CSmsMessageSettings& aRight);
	void TestSettingsMembersL(CSmsSettings& aSettings);
	void TestMessageSettingsMembersL(CSmsMessageSettings& aSettings);
	void StoreSettingsL(const CSmsSettings& aSettings);
	void RestoreSettingsL(CSmsSettings& aSettings);
	void StoreHeaderL(const CSmsHeader& aHeader);
	void RestoreHeaderL(CSmsHeader& aHeader);
	void TestStoreRestoreSettingsL(const CSmsSettings& aSettings);
	TBool TestSmsSettingsCopyL(const CSmsSettings& aSettings);
	TBool CompareSettingsFiles(
		RFs& aFs, const TDesC& aCompare1, const TDesC& aCompare2
		);

	//TMsvSmsEntry
	void TestSmsEntryL();
	TBool CheckSmsEntry(
					const TMsvSmsEntry& aEntry, 
					TBool aClassExpected, 
					TSmsDataCodingScheme::TSmsClass aClass, 
					TUint8 aPID, 
					TBool aUPI, 
					TMsvSmsEntry::TMsvSmsEntryAckSummary aDeliverySummary, 
					TBool aValidMessageId, 
					TInt32 aMessageId) const;

	TSmsProtocolIdentifier ConstructPID(TUint8 aVal) const;


	//CSmsNumber utilities
	void TestNumberL();
	void StoreNumberL(const CSmsNumber& aNumber, TUid aStreamId = KFileStreamId);
	void RestoreNumberL(CSmsNumber& rNumber, TUid aStreamId = KFileStreamId);
	TBool CompareNumbers(const CSmsNumber& aLeft, const CSmsNumber& aRight);
	TBool CompareServiceCenters(const CSmsServiceCenter& aLeft, const CSmsServiceCenter& aRight);

	HBufC8* ConvertToBinaryLC(const TDesC8& aHex) const;
	CSmsMessage* CreateSmsMessageLC(const TDesC8& aHexPdu) const;
	void CheckDescriptionL(RResourceFile& aResFile, TInt aId, TRefByValue<const TDesC> aDesc, ...);
	void TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageType, TUint8 aMessageCount, TInt aId);
	void TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TSmsUtilities::TSmsUtilitiesSpecialMessageType aMessageType, TInt aId);
	
	// Test CSmsEmailFields
	void TestEmailFieldsL();
	void StoreEmailFieldsL(const CSmsEmailFields& aEmailFields);
	void RestoreEmailFieldsL(CSmsEmailFields& aEmailFields);
	void TestEmailFieldsParsePassL(const TDesC& aMessage, const TDesC& aAddress, const TDesC& aSubject, const TDesC& aBody);
	void TestEmailFieldsParseFailL(const TDesC& aMessage);
	void TestEmailFieldsComposeL(const TDesC& aFields, const TDesC& aAddress1, const TDesC& aAddress2, const TDesC& aSubject);
	TBool CompareEmailFields(const CSmsEmailFields& aFields1, const CSmsEmailFields& aFields2);
	TBool CompareEmailFieldsAddresses(const MDesCArray& aAddresses1, const MDesCArray& aAddresses2);
	
	CSmsHeader*			iSmsHeader;
	CRichText*			iRichText;
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
	CMDBSession*		iDBSession;
	
	};


#endif
