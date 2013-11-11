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
* 
* Description:
*     The header file of drive event watcher
*/

#ifndef CFMDRIVEEVENT_H
#define CFMDRIVEEVENT_H


// INCLUDES
#include <e32base.h>
#include <f32file.h>

class MFmDriveEventObserver
	{
	public:
		virtual void OnDriveAddedOrChangedL() = 0;
	};
// CLASS DECLARATION

/**
*  This class listens file system events
*
*  @lib FileManagerEngine.lib
*  @since 2.0
*/
NONSHARABLE_CLASS(CFmDriveEvent) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFmDriveEvent* NewL( RFs& aFs, MFmDriveEventObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CFmDriveEvent();

    public:

        /**
        * Sets file system event
        * @since 2.0
        */
        void Setup();

    protected:  // Functions from base classes

        /**
        * From CActive Called when asynchronous request has completed
        * @since 2.0
        */
        void RunL();

        /**
        * From CActive Called when asynchronous request has failed
        * @since 2.0
        */
        TInt RunError( TInt aError );

        /**
        * From CActive Called when asynchronous request was cancelled
        * @since 2.0
        */
        void DoCancel();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CFmDriveEvent( RFs& aFs, MFmDriveEventObserver* aObserver );

    private:    // Data
        // Ref: Reference to opened File System session
        RFs& iFs;

        // Ref: This object is called when RunL occurs
        MFmDriveEventObserver *iObserver;
    };

#endif // CFMDRIVEEVENT_H

// End of File
