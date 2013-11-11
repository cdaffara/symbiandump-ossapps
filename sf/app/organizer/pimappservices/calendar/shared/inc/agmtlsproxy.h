// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMTLSPROXY_H__
#define __AGMTLSPROXY_H__

#include <e32property.h>
#include <tz.h>

class TAgnCalendarFixedTimeMode;
class TAgnCalendarFloatingTimeMode;
class TAgnCalendarFixedUsingRulesTimeMode;
class MAgnCalendarTimeMode;
class CTzConverter;
class CAgnTlsTzRules;
class CAgnTlsProxy;

class MAgnTzChangeObserver
/** The interface for a system time zone change observer.
If an object that implements this interface and supplied to 
CAgnTlsProxy::AddSystemTimeObserverL(), its HandleCurrentTimeZoneChangeL() 
function is called if the system time zone is changed.

@internalAll
@released
*/
	{
public:
	/** Observer interface that is called when system time zone has changed. */
	virtual void HandleCurrentTimeZoneChangeL(const CTzId& aSystemTzId) = 0;
	virtual void HandleTzRulesChangeL(const TTime& aTime) = 0;
	};
	
NONSHARABLE_CLASS(CAgnTzChangeObserver ) : public CActive
	{
public:
	static CAgnTzChangeObserver* NewL(CAgnTlsProxy& aTlsProxy, NTzUpdate::TPropertyKeys aTzChangePropertyKey);
	~CAgnTzChangeObserver();

	
private:
	CAgnTzChangeObserver(CAgnTlsProxy& aTlsProxy, NTzUpdate::TPropertyKeys aTzChangePropertyKey);
	void ConstructL();
	
	void RunL();
	void DoCancel();

private:
	CAgnTlsProxy&				iAgnTlsProxy;			
	NTzUpdate::TPropertyKeys 	iTzChangePropertyKey;
	RProperty 					iTzChangeProperty;
	};
/** Class for accessing time zone converter, and caching time zone rules and modes.

This class is held in thread local storage in the Calendar client, server and shared DLLs 
so it can be accessed easily from anywhere.
*/
NONSHARABLE_CLASS(CAgnTlsProxy) : public CBase
	{
public:
	enum TAgnTlsTzRulesType
		{
		TAgnTlsTzRulesType_None,
		TAgnTlsTzRulesType_Client,
		TAgnTlsTzRulesType_Server,
		};
	
	IMPORT_C static CAgnTlsProxy* CreateL(TAgnTlsTzRulesType aTzRulesType);
	IMPORT_C static void Release(TAny* aRef);
	
	const TAgnCalendarFixedTimeMode& FixedTimeMode() const;
	const TAgnCalendarFloatingTimeMode& FloatingTimeMode() const;
	
	IMPORT_C RTz& TzServer();
	TUint16 CurrentTzId() const;
	CTzRules* GetZoneForRulesL(const CTzRules& aRules);
	void ReferenceRemoved(const CTzRules* aReference);
	IMPORT_C void __dbgClearTzClientCacheL(TBool aRestartCaching);
	
	IMPORT_C void AddSystemTimeObserverL(MAgnTzChangeObserver* aObserver);
	IMPORT_C void RemoveSystemTimeObserver(MAgnTzChangeObserver* aObserver);

	IMPORT_C CTzRules* GetCurrentSystemTzRulesL();
	void HandleTzPropertyChangeL(NTzUpdate::TPropertyKeys aTzChangePropertyKey);
private:
	CAgnTlsProxy();
	void ConstructL(TAgnTlsTzRulesType aTzRulesType);
	
	static CAgnTlsProxy* NewL(TAgnTlsTzRulesType aTzRulesType);
	~CAgnTlsProxy();	
private:
	TInt iRefCount;
	TAgnCalendarFixedTimeMode* iFixedTimeMode;
	TAgnCalendarFloatingTimeMode* iFloatingTimeMode;
	RTz iTzServer;
	CTzConverter* iTimeConverter;
	CAgnTlsTzRules* iTlsTzRules;
	RPointerArray<MAgnTzChangeObserver> iObservers;
	CAgnTzChangeObserver*	iCurrentTzChangeObserver;
	CAgnTzChangeObserver*	iTzRulesChangeObserver;
	};

NONSHARABLE_CLASS(CAgnTlsTzRules) : public CBase
	{
public:
	CAgnTlsTzRules();
	virtual ~CAgnTlsTzRules();
	virtual CTzRules* GetSharedTzRulesL(const CTzRules& aTzRules) = 0;
	virtual void ReferenceRemoved(const CTzRules* aReference) = 0;
	virtual CTzRules* GetCurrentSystemTzRulesL(RTz& aTzServer) = 0;
	virtual void SystemTzZoneChangedL(RTz& aTzServer, NTzUpdate::TPropertyKeys aTzChangePropertyKey) = 0;

protected:
	CTzRules* GetSystemTzFromTzServerL(RTz& aTzServer);

protected:	
	CTzRules* iCurrentSystemTzRules;
	};

NONSHARABLE_CLASS(CAgnServerTlsTzRules) : public CAgnTlsTzRules
	{
public:
	virtual ~CAgnServerTlsTzRules();
	virtual CTzRules* GetSharedTzRulesL(const CTzRules& aTzRules);
	virtual void ReferenceRemoved(const CTzRules* aReference);
	virtual CTzRules* GetCurrentSystemTzRulesL(RTz& aTzServer);
	virtual void SystemTzZoneChangedL(RTz& aTzServer, NTzUpdate::TPropertyKeys aTzChangePropertyKey);
	};

NONSHARABLE_CLASS(CAgnClientTlsTzRules) : public CAgnTlsTzRules
	{
public:
	virtual ~CAgnClientTlsTzRules();
	virtual CTzRules* GetSharedTzRulesL(const CTzRules& aTzRules);
	virtual void ReferenceRemoved(const CTzRules* aRefTzRules);
	virtual CTzRules* GetCurrentSystemTzRulesL(RTz& aTzServer);
	virtual void SystemTzZoneChangedL(RTz& aTzServer, NTzUpdate::TPropertyKeys aTzChangePropertyKey);

private:
	void AddReference(const CTzRules* aTzRules);
	void SetCurrentSystemTzRulesL(CTzRules* aTzRules, TUint aTzId);
	CTzRules* GetCurrentSystemTzFromTzServerL();
	void UpdateCurrentSystemTzRulesL(RTz& aTzServer);
	void UpdateCacheL(RTz& aTzServer);

private:
	//The following three arrays are conrespondent each other, i.e. 
	//for each tz rule, there is a rule (CTzRules), a reference count 
	//(The number of entries using it) and a Tz Id (It is the numerical id if the 
	//rule is obtained from the TzServer, otherwise it is set to KInvalidTzId)
	RPointerArray<CTzRules> iTzRules;
	RArray<TInt> iTzRulesReferences;
	RArray<TUint> iTzIds;
	};


#endif //
