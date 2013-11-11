/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper class for active objects
*
*/

#ifndef IPSPLGSINGLEOPWATCHER_H
#define IPSPLGSINGLEOPWATCHER_H



// FORWARD DECLARATIONS
class CMsvOperation;
class MIpsPlgSingleOpWatcher;
class CIpsPlgBaseOperation;
class CIpsPlgImap4MoveRemoteOpObserver;

/**
* CMsvOperation watcher class.
*/
NONSHARABLE_CLASS ( CIpsPlgSingleOpWatcher ) : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIpsPlgSingleOpWatcher* NewLC( 
            MIpsPlgSingleOpWatcher& aObserver );

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIpsPlgSingleOpWatcher* NewL( 
            MIpsPlgSingleOpWatcher& aObserver );

        /**
        * Destructor
        */
        IMPORT_C ~CIpsPlgSingleOpWatcher();

        /**
        * Sets operation
        * Must only be called once during the 
        * lifetime of a CIpsPlgSingleOpWatcher
        * object.
        */
        IMPORT_C void SetOperation( CMsvOperation* aOperation );
        
        
        void SetOperation( CIpsPlgBaseOperation* aBaseOperation );

        /**
        * Sets request observer.
        */
        void SetRequestObserver( CIpsPlgImap4MoveRemoteOpObserver* aObserver );

        /**
        * Returns reference to the operation
        */
        IMPORT_C CMsvOperation& Operation() const;
        
        const CIpsPlgBaseOperation* BaseOperation( ) const;

    private:
        /**
        * C++ constructor
        */
        CIpsPlgSingleOpWatcher( MIpsPlgSingleOpWatcher& aObserver );

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

    private:
        MIpsPlgSingleOpWatcher& iObserver;
        CMsvOperation *iOperation;
        CIpsPlgBaseOperation* iBaseOperation;
        CIpsPlgImap4MoveRemoteOpObserver* iRequestObserver;
    };


/**
* Mixin class MIpsPlgSingleOpWatcher
*
* Function OpCompleted is called when operation is completed.
*/
class MIpsPlgSingleOpWatcher
    {
    public:
        virtual void OpCompleted( 
            CIpsPlgSingleOpWatcher& aOpWatcher, 
            TInt aCompletionCode ) = 0;
    };


/**
* Class CIpsPlgSingleOpWatcherArray
*
* Use this define when we want to use an array 
* of CIpsPlgSingleOpWatcher objects.
*/

//typedef CArrayPtrFlat<CIpsPlgSingleOpWatcher> CIpsPlgSingleOpWatcherArray;


#endif // IPSPLGSINGLEOPWATCHER_H

// End of file
