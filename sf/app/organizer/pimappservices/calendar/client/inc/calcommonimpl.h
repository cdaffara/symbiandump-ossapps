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

#ifndef __CALCOMMONIMPL_H__
#define __CALCOMMONIMPL_H__

#include "CalinterimApiPanic.h"
#include "agmrptdef.h"

class TCalTime;
const TInt KAgnEntryMaxGuidLength = 256;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CalUtils)
	{
public:
	static void Panic(TCalInterimApiPanicCode aPanic);
	
	static TAgnRpt::TWeekInMonth WeekNumberToAgnWeekInMonthL(TInt8 aWeek);
	static TInt8 AgnWeekInMonthToWeekNumberL(TAgnRpt::TWeekInMonth aWeek);
	
	static TAgnCalendarTime TCalTimeToTAgnCalendarTimeL(const TCalTime& aCalTime);
	static TCalTime TAgnCalendarTimeToTCalTimeL(const TAgnCalendarTime& aAgnTime);
	};

/** Template class for cleanup that needs ResetAndDestroy to be called 
@internalComponent
*/
template<class T> class CalCleanup
	{
public:
	static void PushResetAndDestroyL(T& aRef);
	static void PushResetAndDestroyPointerL(T& aRef);
	static void PushResetL(T& aRef);
	
private:
	static void ResetAndDestroy(TAny* aPtr);
	static void ResetAndDestroyPointer(TAny* aPtr);
	static void Reset(TAny* aPtr);
	};

/**
@internalComponent
*/
template<class T> void CalCleanup<T>::PushResetAndDestroyL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, &aRef));
	}
	
/**
@internalComponent
*/
template<class T> void CalCleanup<T>::PushResetL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(Reset, &aRef));
	}

/**
@internalComponent
*/	
template <class T> void CalCleanup<T>::ResetAndDestroy(TAny* aPtr)
	{
	T* self = static_cast<T*>(aPtr);
	if (self)
		{
		self->ResetAndDestroy();
		}
	}
	
/**
@internalComponent
*/	
template <class T> void CalCleanup<T>::Reset(TAny* aPtr)
	{
	T self = static_cast<T>(aPtr);
	if (self)
		{
		self->Reset();
		}
	}
	
/** Global function for ease of use of CalCleanup template class
@internalComponent
*/
template<class T> void CleanupResetAndDestroyPushL(T& aRef)
	{
	CalCleanup<T>::PushResetAndDestroyL(aRef);
	}
	
/** Global function for ease of use of CalCleanup template class
@internalComponent
*/
template<class T> void CleanupResetPushL(T& aRef)
	{
	CalCleanup<T>::PushResetL(aRef);
	}

_LIT(KDefaultSecureAgendaFileName,"C:Calendar");
_LIT(KDefaultAgendaFileName,"Calendar");
_LIT(KUpdatedAgendaFileExtension,"_converted");
#endif // __CALCOMMONIMPL_H__
