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

#ifndef CIRDISKSPACEOBSERVER_H
#define CIRDISKSPACEOBSERVER_H

#include <e32base.h>  // CActive
#include <f32file.h>  // RFs

class MIRDiskSpaceObserverInterface;

/**
 * Defines disk space observer. 
 *
 * Observers defined disk and notifies observer if disk space drops below
 * certain level.
 */
class CIRDiskSpaceObserver : public CActive
{

public:

    /**
     * Static constructor 
     *
     * @param aObserver Observer who is interested if disk 
     * space drops below certain level.
     */
    static CIRDiskSpaceObserver* NewL(MIRDiskSpaceObserverInterface* aObserver);
    static CIRDiskSpaceObserver* NewLC(MIRDiskSpaceObserverInterface* aObserver);

    /**
     * Destructor
     */
    ~CIRDiskSpaceObserver();

    /**
     * Start Monitoring 
     */
    void Start(const TInt64 aCriticalLevel);

    /**
     * Tells observed critical level.
     *
     * @return critical level which is been observed by this session.
     */
    TInt64 CriticalLevel() const;

    /**
     * Returns whether or disk defined by parameter 
     * contains less than the critical level free disk space.
     */
    TBool IsBelowCriticalLevel(const TInt64 aCriticalLevel) const;

protected:
    /**
     * From CActive 
     */
    void DoCancel();
    void RunL();

private:

    /**
     * C++ constructor.  
     */
    CIRDiskSpaceObserver(MIRDiskSpaceObserverInterface* aObserver);

    /**
     * Second phase constructor
     */
    void ConstructL();

    void Run();

private:

    /** 
     * The observer to be notified when critical disk space limit is reached. 
     */
    MIRDiskSpaceObserverInterface* iObserver;

    /** 
     * Critical disk space level. 
     */
    TInt64 iCriticalLevel;

    /** 
     * Handle to the file system. 
     */
    RFs iFs;
};

#endif // CIRDISKSPACEOBSERVER_H
