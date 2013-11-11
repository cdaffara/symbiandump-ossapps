// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// popservermtm module header
// @internalComponent
// @released
// 
//

#ifndef __POPSRFSH_H__
#define __POPSRFSH_H__

#include <pop3set.h>
#include <imsk.h>

class CImPop3Session;
class CImPop3RefreshOperation;
class CImTextServerSession;
class CImPop3UidlMap;
class CImPop3Stat;
class CImPop3List;
class CImPop3Uidl;
class CImPop3Top;
class CImPop3Retr;
class CImPop3Capa;
class CImRecvConvert;

//
// CImPop3RefreshMailBox
//
// Changes the modes for CImPop3RefreshOperation depending on 
// operation failures,
//
// CImPop3RefreshMailBox was the original name of CImPop3RefreshOperation
// This version is written to trap a Leave while downloading equal UIDLs
// and re-run CImPop3RefreshOperation skiping UIDL downloading. (BFSW1-2016)
//
// The public section is left to be exactly the same as the original 
// CImPop3RefreshOperation structure.
// 
//
//
class CImPop3RefreshMailBox : public CMsgActive
{
private :
		enum TState {
			EPop3RefreshBoxDefault,	
			EPop3RefreshBoxTop,
			EPop3RefreshBoxPurgeInput
			};		
public:
	static CImPop3RefreshMailBox* NewL(CMsvServerEntry&, CImPop3Session&, RFs&);
	~CImPop3RefreshMailBox();
	void Start(TRequestStatus& aStatus, CArrayFixFlat<TMsvId>* aMsgIdArray);

	TPop3Progress Progress();

	void SetMessagesToKeepL(const CMsvEntrySelection* aMessagesToKeep);

	TUint RemoteMessageSizeL(TMsvId aId);
	void Pause();
	void ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus);
	void CancelAllowResume();
	
private:
	CImPop3RefreshMailBox(CImPop3Session& aPopSession);

	void ConstructL(CMsvServerEntry&, RFs&);

	void DoRunL();
	void DoCancel();
	void ChangeStateL(TState aState);

private :
	TState	 					iState;
	CImPop3RefreshOperation*	iRefreshOperation;
	CImTextServerSession*		iTextServer;
	CImPop3Session&				iPopSession;
	TImMailBuffer				iTextServerResponse;
	
	TBufC8<3>					iFullStopTerminator;
	CArrayFixFlat <TMsvId>*		iMsvIdArray;
	TBool 						iMigratingToNewBearer;
};

//
// CImPop3RefreshOperation
//
// 
// Performs a refresh email sink
//
class CImPop3RefreshOperation : public CMsgActive
	{
public:
	static CImPop3RefreshOperation *NewL(CMsvServerEntry&, CImPop3Session*, RFs&);
	~CImPop3RefreshOperation();
	void Start(TRequestStatus& aStatus, CArrayFixFlat<TMsvId>* aMsvIdArray);
	void Start(TRequestStatus& aStatus);  

	TPop3Progress Progress();

	void SetMessagesToKeepL(const CMsvEntrySelection* aMessagesToKeep);
	TUint RemoteMessageSizeL(TMsvId aId);
	void Pause();
	void ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus);
	void CancelAllowResume();

private:
	enum TState {
		EPopRefreshSyncCollection,
		EPopRefreshStat,
		EPopRefreshList,
		EPopRefreshUidl,
		EPopRefreshSyncUidl,
		EPopRefreshGetHeader,
		EPopRefreshDeleteDeadMessages,
		EPopRefreshComplete,
		EPopRefreshDeleteExcessMessages,
		EPopMigratingBearer
		};

	class TMsgUidlStore
		{
	public:
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		enum {EInvalid=-1,EPresent=-2};
	public:
		HBufC8*						iPopId;
		TMsvId						iMsvId;
		TUint						iRemoteSize;
	//
		TInt						iSelectionPos;	// transient member
		};

private:
	CImPop3RefreshOperation(CMsvServerEntry&, CImPop3Session*, RFs&);
	void ConstructL();

	void ChangeStateL(TState aState);			// the state machine engine
	void ChangeStateForMigrate(TState aState);
	TInt SelectNextState();

	void DoRunL();
	void DoComplete(TInt&);
	void DoCancel();
	
	void AllocateArraysL();
	void GetNoMessagesL();
	void GetMsgSizesL();
	void GetMsgUidlsL();
	void GetCapabilitiesL();

	void DeleteUidl(TInt anIndex);

	void RetrieveHeadersL();

	void GetHeaderByIndexL(TInt);
	void GetMessageByIndexL(TInt);

	void CreateNewHeaderEntryL(TMsvEntry& aNewEntry);
	void GetRemoteMessagesL();
	void QueueRemoteUpdate();	// will be asynch
	void SyncCollectionL();
	void CompleteSyncCollection();
	void SyncUidlL();

	TBool CompareIdAndSizeL();
	TBool GetMessagesIfAny();

	void OpenUidlFileL();
	void CreateUidlFileL();

	void RefreshFinishedL();

	TBool CanDeleteEntry(const TMsvEmailEntry& aEntry) const;

private:
	TState				iState;
	TUint				iNoMessages;
	TUint				iMboxSize;
	TInt32*				iIdTab;
	TUint*				iSize;
	CArrayFixFlat<TMsvId>* iNewMsg;
	TPtrC8				iImMsgId;

	TInt				iRemoteArrayCtr;
	CImPop3UidlMap*		iPop3Uidls;
	CArrayFix<TMsgUidlStore>*	iUidlArray;
	TKeyArrayFix		iUidlKey;

	CMsvServerEntry&	iRemoteServerEntry;
	CMsvServerEntry*	iLocalEntry;
	CMsvEntrySelection*	iMsvSelection;

	CImPop3Session*		iPopSession;
	CImPop3Stat*		iPopStat;
	CImPop3List*		iPopList;
	CImPop3Uidl*		iPopUidl;
	CImPop3Top*			iPopTop;
	CImPop3Retr*		iPopRetr;

	CImRecvConvert*		iRecvConverter;
	RArray <TInt> iUniqueUidlPosArray;
	RArray <TInt> iTemporaryUidlPosArray;
	TInt 				iPreviousIndex;

	TInt				iMsgCtr;
	RFs&				iFs;

	TMsvId				iRemoteId;

	TPop3Progress		iProgress;
	TMsvEntry			iEntry;

	// flags for existence of UIDL / TOP command
	TBool				iUidlExists;
	TBool				iTopExists;
	TBool				iFirstCallToTop;
	TBool				iEmptyHeaders;
	// array of unwanted entries
	CMsvEntrySelection*	iUnwantedEntries;

	TInt iCheckDiskSpaceCounter;
	TInt iCurrentDrive;

	TInt iLastHeaderToGet;
	TInt iRemoteArraySize;

	CMsvEntrySelection*	iMessagesToKeep;
	
	TBool				iPipeliningSupported;
	TBool 				iMigratingToNewBearer;
	};
#endif
