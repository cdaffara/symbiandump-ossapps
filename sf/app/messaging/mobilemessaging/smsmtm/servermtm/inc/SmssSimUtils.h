// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SMSS_SIM_UTILS_H_
#define SMSS_SIM_UTILS_H_

//Includes

#include <smsustrm.h>
#include <msvstd.h>
#include "smssactive.h"
#include <smsuaddr.h>
#include <csmsgetdetdescinterface.h>

//Forward declarations

class RSocket;
class RSocketServ;
class CParaFormatLayer;
class CCharFormatLayer;
class CSmsHeader;
class CBIODatabase;


//CSmsSimUtils
const TUid KSmsSimUtilsDataUid = {0x101f747e};
const TInt16 KSmsSimUtilsDataVersion = 1;

class TSmsSimUtilsData
/**
@internalComponent
@released
*/
	{
	public:

		inline TSmsSimUtilsData();
		void StoreL(CMsvStore& aStore) const;
		void RestoreL(const CMsvStore& aStore);
		inline TBool IsPresentL(const CMsvStore& aStore) const;
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);

	public:

		TMsvId iLastEnumerateFolder; //< Last folder used for enumerating SIM messages
	};

class CSmsSimUtils : public CSmssActive
/**
@internalComponent
@released
*/
	{
public:
	static CSmsSimUtils* NewL(CMsvServerEntry& aServerEntry, RFs& aFs, TMsvId aSmsServiceId);
	void EnumerateL(const TDesC8& aParameter, TRequestStatus& aStatus);

	void DeleteFromPhoneStoreL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void CopyFromPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus);
	void MoveFromPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus);

	void ReadSimParamsL(TRequestStatus& aStatus);
	void WriteSimParamsL(TRequestStatus& aStatus);

	inline const TSmsProgress& Progress(); 
	~CSmsSimUtils();

	static void ConnectL(RSocketServ& arSocketServ, RSocket& arSocket, TSmsAddrFamily aSmsAddrFamily);

	//Write messages to a phone store
	void CopyToPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus);
	void MoveToPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus);
private:
	CSmsSimUtils(CMsvServerEntry& aServerEntry, RFs& aFs, TMsvId aSmsServiceId);
	void ConstructL();

	void ConstructHeaderAndBodyL();
	void DoRunL();
	void DoSmssCancel();
	void DoComplete(TInt& aStatus); //from CSmssActive
	void ConnectL();
	void RetrieveMessageFromPhoneStoreL();
	void SetupMoveDeleteL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void DoDeleteFromPhoneStoreL();
	void DoDeleteThenMoveFromPhoneStoreL();
	void DeleteEachMessageFromPhoneStoreL();

	void SetDestination(const TDesC8& aParameter);
	void MoveEntriesL(const CMsvEntrySelection& aSelection, TBool aCopy=EFalse);
	
	//Count the messages in the phone stores
	void CountMessagesInPhoneStoresL();
	void CreateBioDbL();
	TBool GetDefaultSendBearerL(TInt aBioType, TBioMsgIdType aBearer, TBioMsgId& rBioMsgId) const;
	TUid DecodeBioMessageL(TBioMsgId& rId);
	void EncodeBioMessageL();

	void DoRunReadSimParamsL();

	void DoCopyToPhoneStoreL();

	void StoreSimUtilsDataL();
	void RestoreSimUtilsDataL();
	TMsvId CreateNewEnumerateFolderL();
	void SetAndCleanEnumerateFolderL(const TDesC8& aParameter);
	void DeleteChildrenL(TMsvId aId);
	void DeleteEnumerateFolderL(TMsvId aId);
	void LoadClass2FolderIdL();
	void SetLocalStorageInfoL(const CMsvEntrySelection& aSelection);
	void RestoreSmsSettingsL();
		
private:
	
	enum TSmsSimUtilsState
		{
		ESimUtilsStateOther,
		ESimUtilsDeleteEachMessage,
		EWritingToSIM,
		EUpdatingClass2,
		EMovingEntries,
		ESimUtilsDeletedLastMessage
		} iState;
	
	RSocket iSocket;
	RSocketServ iSocketServ;
	
	TSmsProgress iProgress;
	CMsvEntrySelection* iSelection;
	
	//Enumerating Messages & Reading SMS Parameters
	TPckgBuf<TUint> iEnumerateCountBuf;
	RSmsSocketReadStream iReadStream;
	TMsvId iSmsServiceId;
	
	//Copy, Move, Delete messages from SIM
	CParaFormatLayer* iParaFormat;
	CCharFormatLayer* iCharFormat;
	CRichText* iBody;
	CSmsHeader* iHeader;
	TMsvId iDestination;
	CBIODatabase* iBioDb;
	CMsvEntrySelection* iCompletedSelection;
	TInt iRecipientCount;
	TInt iRecipientIndex;
	
	//Writing messages to phone stores and writing SMS Parameters
	RSmsSocketWriteStream iWriteStream;  //< SMS socket write stream
	TBool iReadingSimParams;
	TSmsSimUtilsData iSimUtilsData;
	TMsvId iClass2Folder;
	CSmsSettings*	iSettings;
	TBuf8<IoctlWriteSMSBufferSize> iSlotBuffer;
	CSmsGetDetDescInterface* iGetDetDesc;
	};

#include "SmssSimUtils.inl"

#endif
