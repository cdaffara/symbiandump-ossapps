/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CIDLEPRINTEVENTCATCHER_H
#define CIDLEPRINTEVENTCATCHER_H

#include <e32std.h>
#include <e32base.h>

#include "imageprint.h"
#include "midleprinteventregisterif.h"

//  FORWARD DECLARATIONS
class CRealFactory;
class MIdlePrintEventObserver;
class MIdlePrintEventRegisterIF;

// CLASS DEFINITION

/**
 * This class catches idle events from Image Print engine.
 */
class CIdlePrintEventCatcher :
    public CBase,
    public MIdleObserver,
    public MIdlePrintEventRegisterIF
    {
    public:     // Construction and destruction

        /**
         *  Two phase constructor
         *
         *  @param aFactory The factory
         *  @param aIdleUIObserver  Observer to notify UI about idle events
         *
         *  @return Initialized instance of the engine
         */
        static CIdlePrintEventCatcher* NewL(
            CRealFactory* aFactory /*, MIdlePrintEventObserver* aIdleUIObserver*/ );
        static CIdlePrintEventCatcher* NewLC(
            CRealFactory* aFactory /*, MIdlePrintEventObserver* aIdleUIObserver*/ );

        /**
         *  Destructor
         */
        virtual ~CIdlePrintEventCatcher();

    private:    // Construction and destruction

        /**
         *  Constructor
         *
         *  @param aFactory The factory
         *  @param aIdleUIObserver  Observer to notify UI about idle events
         */
        CIdlePrintEventCatcher( CRealFactory* aFactory /*,
                                MIdlePrintEventObserver* aIdleUIObserver*/ );

        /**
         *  2nd phase constructor
         */
        void ConstructL();

    public:     // Methods derived from MIdleObserver

        void StatusEvent(const TEvent &aEvent, TInt aError, TInt aMsgCode);

    public: // Methods derived from MIdlePrintEventRegisterIF

        void RegisterObserver( MIdlePrintEventObserver* aIdleObserver );
        void UnRegisterObserver( MIdlePrintEventObserver* aIdleObserver );

    private:    // Data

        // Interface factory
        CRealFactory* iFactory;

        // Observer to notify UI about idle print events
        MIdlePrintEventObserver* iIdleUIObserver;
    
        MIdlePrintEventObserver* iIdleUIObserver2;
        
        RPointerArray<MIdlePrintEventObserver> iObservers;
        
        HBufC* iBuf;

    };


#endif  // CIDLEPRINTEVENTCATCHER_H

//  End of File
