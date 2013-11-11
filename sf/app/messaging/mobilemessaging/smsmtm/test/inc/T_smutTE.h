// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef T_SMUTTE_H_
#define T_SMUTTE_H_

#include <txtrich.h>    
#include "smstestbase.h"
#include <test/testexecutestepbase.h>
#include <commsdattypesv1_1.h>
#include "smuthdr.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif	
using namespace CommsDat;

const TUid KUidSmcmTestNonSense = {0x00011111};
const TUid KFileStreamId		= {0x01010101};

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
	static CSmutTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest,CTestStep& aTestStep);
	~CSmutTest();
	TVerdict TestSmsEntryL();
	TVerdict TestSettingsL();
	TVerdict TestNumberL();
	TVerdict TestEmailFieldsL();
	TVerdict TestHeaderL();
	TVerdict TestSmsUtilitiesL();
	TVerdict TestSmsEmsExtensionsL();
private:
	CSmutTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest,CTestStep& aTestStep);
	void ConstructL();

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL();

	//Test Functions
	TBool TestSmsUtilities1L(CSmsHeader& aHeader);
	TBool TestSmsUtilities2L(CSmsHeader& aHeader);
	TBool TestSmsUtilities3L(CSmsHeader& aHeader);
	TBool TestInvalidTelephone(const TDesC& aTel, TDes& aDetails);
	TBool TestInvalidContact(const TDesC& aTel, TDes& aDetails);
	TBool TestDuplicateContact(const TDesC& aTel, TDes& aDetails);
	TBool TestValidTel(const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails);
	//CSmsSettings utilities/tests
	TBool TestSettingsStoreToCommDbL();
	TBool CompareSettingsAndCommDbL(const CSmsSettings& aSettings) const;
	TBool CompareSettingsAndSmsBearerL(const CSmsSettings& aSettings) const;
//	void OpenCommDbModemTableLCC(CCommsDatabase*& rCommDb, CCommsDbTableView*& rTable) const;
	void TestSettingsRestoreDefectL();
	void TestRevertSettingsL();
	TBool CompareSettings(const CSmsSettings& aLeft, const CSmsSettings& aRight, TBool aTestSC = ETrue);
	TBool CompareMessageSettings(const CSmsMessageSettings& aLeft, const CSmsMessageSettings& aRight);
	TBool TestSettingsMembersL(CSmsSettings& aSettings);
	TBool TestMessageSettingsMembersL(CSmsMessageSettings& aSettings);
	void StoreSettingsL(const CSmsSettings& aSettings);
	void RestoreSettingsL(CSmsSettings& aSettings);
	void StoreHeaderL(const CSmsHeader& aHeader);
	void RestoreHeaderL(CSmsHeader& aHeader);
	TBool TestStoreRestoreSettingsL(const CSmsSettings& aSettings);
	TBool TestSmsSettingsCopyL(const CSmsSettings& aSettings);
	TBool CompareSettingsFiles(
		RFs& aFs, const TDesC& aCompare1, const TDesC& aCompare2
		);

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
	void StoreNumberL(const CSmsNumber& aNumber, TUid aStreamId = KFileStreamId);
	void RestoreNumberL(CSmsNumber& rNumber, TUid aStreamId = KFileStreamId);
	TBool CompareNumbers(const CSmsNumber& aLeft, const CSmsNumber& aRight);
	TBool CompareServiceCenters(const CSmsServiceCenter& aLeft, const CSmsServiceCenter& aRight);

	HBufC8* ConvertToBinaryLC(const TDesC8& aHex) const;
	CSmsMessage* CreateSmsMessageLC(const TDesC8& aHexPdu) const;
	TBool CheckDescriptionL(RResourceFile& aResFile, TInt aId, TRefByValue<const TDesC> aDesc, ...);
	TBool TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageType, TUint8 aMessageCount, TInt aId);
	TBool TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TSmsUtilities::TSmsUtilitiesSpecialMessageType aMessageType, TInt aId);
	TBool TestVideoMessageIndicationPduL(RResourceFile& aResFile, CSmsHeader& aHeader, TInt aId);
	TBool TestVideoMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageCount, CSmsHeader& aHeader, TInt aId);
	TBool TestEnhancedVoiceMailIndicationPduL(RResourceFile& aResFile, CSmsHeader& aHeader, TInt aId);
	TBool TestEnhancedVoiceMailIndicationPduL(RResourceFile& aResFile, TUint8 aMessageCount, CSmsHeader& aHeader, TInt aId);
	
	// Test CSmsEmailFields
	void StoreEmailFieldsL(const CSmsEmailFields& aEmailFields);
	void RestoreEmailFieldsL(CSmsEmailFields& aEmailFields);
	TBool TestEmailFieldsParsePassL(const TDesC& aMessage, const TDesC& aAddress, const TDesC& aSubject, const TDesC& aBody);
	TBool TestEmailFieldsParseFailL(const TDesC& aMessage);
	TBool TestEmailFieldsComposeL(const TDesC& aFields, const TDesC& aAddress1, const TDesC& aAddress2, const TDesC& aSubject);
	TBool CompareEmailFields(const CSmsEmailFields& aFields1, const CSmsEmailFields& aFields2);
	TBool CompareEmailFieldsAddresses(const MDesCArray& aAddresses1, const MDesCArray& aAddresses2);
	
	CSmsHeader*			iSmsHeader;
	CRichText*			iRichText;
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
	CMDBSession*		iDBSession;
	CTestStep& 			iTestStep;
	};


#endif //T_SMUTTE_H_
