// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cimfinder.h
//

/**
 * @file 
 * @internalComponent
 * @released
 */

#if !defined (__CIMFINDER_H__)
#define __CIMFINDER_H__

class CImFinder : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C void FindFirstL(TMsvId aRootEntry, TRequestStatus &aStatus);
	IMPORT_C ~CImFinder();
	IMPORT_C virtual void FindNextL(TRequestStatus &aStatus);
	void FindFirstL(const CMsvEntrySelection& aInitialSelection, TRequestStatus &aStatus);

	class CImEntryStack : public CBase
		{
	public:
		static CImEntryStack* NewL();
		~CImEntryStack();
		inline void PushL(TMsvId aId);
		inline TMsvId PopL();
		inline TBool Empty() const;
		inline void Reset();

	private:
		void ConstructL();

		CMsvEntrySelection* iFolders;
		};

protected:
	void ConstructL();
	CImFinder(CMsvEntry& aEntry);

private:
	void DoRunL();
	void DoCancel();
	virtual void AddChildEntriesL() = 0;
	virtual TBool IsRequiredEntryType(TUid aEntryType) const = 0;

protected:
	CImEntryStack* iEntryStack;
	CMsvEntry& iCurrentEntry;

private:
	enum TImmfState
		{
		EImmfEntryFound,
		EImmfFindingEntry,
		EImmfNothingFound
		};

	TImmfState iState;
	};


class CImMessageFinder : public CImFinder
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C static CImMessageFinder* NewL(CMsvEntry& aEntry);
	IMPORT_C static CImMessageFinder* NewLC(CMsvEntry& aEntry);
	
protected:
	virtual void AddChildEntriesL();
	virtual TBool IsRequiredEntryType(TUid aEntryType) const;

	CImMessageFinder(CMsvEntry& aEntry);
	};


class CImEntryFinder : public CImFinder
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C static CImEntryFinder* NewL(CMsvEntry& aEntry);
	IMPORT_C static CImEntryFinder* NewLC(CMsvEntry& aEntry);
	
protected:
	virtual void AddChildEntriesL();
	virtual TBool IsRequiredEntryType(TUid aEntryType) const;

	CImEntryFinder(CMsvEntry& aEntry);
	};


class CImMessageCounter : public CImFinder
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C static CImMessageCounter* NewL(CMsvEntry& aEntry);
	IMPORT_C static CImMessageCounter* NewLC(CMsvEntry& aEntry);
	IMPORT_C TInt Count();
	
protected:
	virtual void AddChildEntriesL();
	virtual TBool IsRequiredEntryType(TUid aEntryType) const;

	CImMessageCounter(CMsvEntry& aEntry);

private:
	TInt iCount;
	};

#endif //__CIMFINDER_H__
