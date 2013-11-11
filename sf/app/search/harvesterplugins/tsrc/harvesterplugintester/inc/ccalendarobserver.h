/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CCALENDAROBSERVER_H_
#define CCALENDAROBSERVER_H_
#include "calsession.h"
#include "calentryview.h"
#include <calentry.h>
#include "calprogresscallback.h"
_LIT8(KGuid, "AGlobalUid");

class CCalendarObserver : public CBase, public MCalProgressCallBack 
{
public:
    static CCalendarObserver* NewL();   
    virtual ~CCalendarObserver();
    
private:
    void ConstructL();
    CCalendarObserver();
    
public:    
    
    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();
    
    void AddEntryL();
    void UpdateCalenderEntryL();
    void DeleteEntryL();
    void AddNoteL( TPtr8 aMemo);
    void DeleteNoteEntryL();
    void UpdateNoteEntryL( TPtr8 aOldString, TPtr8 aNewString );
    
private:
    CCalSession* iSession;
    CCalEntryView* iCalEntryView;
    CActiveSchedulerWait* iWaiter;
    HBufC8* iUid;
};

#endif /* CCALENDAROBSERVER_H_ */
