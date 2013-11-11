/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef IRALARMOBSERVER_H
#define IRALARMOBSERVER_H

#include <asclisession.h> 

class MIRAlarmObserverInterface;

class CIRAlarmObserver : public CActive
{
								 
public:
    
	static CIRAlarmObserver* NewL(MIRAlarmObserverInterface* aObserver);
	static CIRAlarmObserver* NewLC(MIRAlarmObserverInterface* aObserver);
	~CIRAlarmObserver();
 
	void Start();

protected:
	
 	void RunL();

	void DoCancel();
	
	TInt RunError(TInt aError);

private:	
 
	void ConstructL(MIRAlarmObserverInterface* aObserver);
	
	CIRAlarmObserver();
							 
private:
	
	//The client-side interface to the Symbian OS alarm server
	RASCliSession iAlarmSession;
	
	//Alarm id
	TAlarmId iAlarmId;
	
	//instance of an event observer
	MIRAlarmObserverInterface* iObserver;	 
};
	
#endif	//IRALARMOBSERVER_H

