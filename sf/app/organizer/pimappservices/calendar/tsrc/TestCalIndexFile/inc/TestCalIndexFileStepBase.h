/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __TEST_CAL_INDEX_FILE_STEP_BASE__
#define __TEST_CAL_INDEX_FILE_STEP_BASE__

#include <test/testexecutestepbase.h>
#include <calprogresscallback.h>
#include <calrrule.h>
#include <calentry.h>

class CCalSession;
class CConfigTestEntryInfo;
class CTestCalIndexFileStepBase : public CTestStep, protected MCalProgressCallBack
	{
public:
	CTestCalIndexFileStepBase();
	virtual ~CTestCalIndexFileStepBase();

	virtual TVerdict		doTestStepPreambleL();
	virtual TVerdict		doTestStepPostambleL();

protected:
	void SetUpCalDirL();
	void AddCalendarFileL();
	void AddIndexFileL();
	
	void MakeCalendarFileName(TFileName& aCalFile);

	virtual void OpenAgendaL();
	void CloseAgendaL();
	TBool CheckIndexFilePresentL();
	TBool CheckCalendarFilePresentL();
	virtual TBool ValidateDbContentsL(TBool aPerformCrudOps = EFalse);
	virtual void GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo,
										 TInt& aNumEntries, 
										 TBool aPerformCrudOps);
	void AddEntryL();
	
	inline CCalSession& GetSession();
	inline void SetSession(CCalSession* aSession);
	inline const TFileName&	GetCalenderFileName();

	
private:
	//	MCalProgressCallBack implementation
	void					Progress(TInt aPercentageCompleted);
	void					Completed(TInt aError);
	TBool					NotifyProgress();

protected:
	void ParseEntryStringL(CConfigTestEntryInfo& aInfo, TPtrC& aString);
	void GoToNextEntryItem(TDes& aData, TDes& aItem);
	CCalEntry* CreateEntryL(CConfigTestEntryInfo& aInfo);

	
protected:
	RFs 					iFsSession;

private:
	TBool					iNotifyProgress;
	CActiveScheduler*		iActiveScheduler;
	TBool					iFsSessionConnected;
protected:
	CCalSession*			iSession;
	TFileName				iCalenderFileName;
	
protected:
	TFileName	iDriveName;
	TFileName	iDir;
	TFileName	iCalendarName;
	TFileName	iIndexName;
	};



	
class CConfigTestEntryInfo
	{
public:
	CConfigTestEntryInfo();
	~CConfigTestEntryInfo();

	TBool CompareToEntryL(CCalEntry& aEntry);
	
private:
	TBool	CheckTypeL(CCalEntry& aEntry);
	TBool	CheckTimesL(CCalEntry& aEntry);
	TBool	CheckSummaryL(CCalEntry& aEntry);
	TBool	CheckDescriptionL(CCalEntry& aEntry);
	TBool	CheckLocationL(CCalEntry& aEntry);
	TBool	CheckMethodL(CCalEntry& aEntry);
	TBool	CheckRRuleL(CCalEntry& aEntry);

public:
	CCalEntry::TType 	iType;
	TTime			iStartTime;
	TTime			iEndTime;
	HBufC*				iSummary;
	HBufC*				iDescription;
	HBufC*				iLocation;
	CCalEntry::TMethod	iMethod;
	TBool				iRepeats;
	TCalRRule::TType	iRptType;
	TUint				iRptCount;
	};


class CCalProgress: public CActive, public MCalProgressCallBack
	{
public:	
	CCalProgress();
	~CCalProgress();
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress(); 

	TBool Completed();
	TInt  Progress();
	void  Reset();
	
	void Start();
private:

	void Close();
	void DoCancel();
	void RunL();

	TInt iProgress;
	TBool iCompleted;	
	};




#include "TestCalIndexFileStepBase.inl"

#endif
