/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global Data for Calendar application
*
*/

#ifndef CALENGLOBALDATA_H
#define CALENGLOBALDATA_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <calprogresscallback.h>
#include <qglobal.h>		// IMPORT_C macro
#include "calennotificationhandler.h"   // MCalenNotificationHandler

#ifdef  CALENGLOBALDATA_DLL
#define CALENGLOBALDATA_EXPORT Q_DECL_EXPORT
#else
#define CALENGLOBALDATA_EXPORT Q_DECL_IMPORT
#endif

// FORWARD DECLARATIONS
class CCalSession;                      // Calendar session
class CCalEntryView;                    // Calendar entry view
class CCalInstanceView;                 // Calendar instance view
class CalenDbChangeNotifier;           // Issues notifications of database changes
class MCalenContextChangeObserver;      // Context changes
class CalenContextImpl;                // context implementation
class MCalenContext;

// CLASS DECLARATION

/**
*  CCalenGlobalData is a singleton using Thread local storage
*  It provides easy access to instances of the following classes:
*       CMRMailboxUtils
*       CMRUtils
*       CCalSession
*       CCalInstanceView
*       CCalEntryView
*       CCalenInterimUtils2
*       CCalenSend
*       CCalenDbChangeNotifier
*       CCalenContext
*/
class CCalenGlobalData : public CBase,
                                 public MCalenNotificationHandler
    {
    public:  // Constructors and destructor
        /**
         * The only classes that should call NewL are the controller
         * and server classes. Otherwise use InstanceL.
         * Doesn't take ownership of aNotifier.
         */
        CALENGLOBALDATA_EXPORT static CCalenGlobalData*
                            NewL( MCalProgressCallBack& aCalCallBack,
                                  MCalenContextChangeObserver* aNotifier=NULL );

        /**
        * Constructor.  Returns a new instance of CCalenGlobalData if
        * none exists, or returns the existing instance.
        * @return CCalenGlobalData pointer
        */
        CALENGLOBALDATA_EXPORT static CCalenGlobalData* InstanceL();

        /**
        * Constructs CCalenGlobalData with an existing CCalSession.  It
        * is the responsibility of the caller to ensure that the session
        * remains open until ALL references to the CCalenGlobalData have been
        * released.  If the CCalenGlobalData already has created a different
        * CCalSessionthis call will leave with KErrAlreadyExists.  If the
        * CCalenGlobalData has previously been initialised with the same
        * external CCalSession, the  existing instance will be returned.
        * @param aSession initialised calendar session
        * @return CCalenGlobalData pointer
        */
        CALENGLOBALDATA_EXPORT static CCalenGlobalData* InstanceL(CCalSession& aSession);

        /**
        * Non-leaving version of constructor. Returns an instance of
        * CCalenGlobalData if one exists or NULL otherwise
        */
        CALENGLOBALDATA_EXPORT static CCalenGlobalData* Instance();

        /**
        * CCalenGlobalData is a reference counting singleton.  Call Release()
        * when you are done with it, it will clean itself up when it needs to
        */
        CALENGLOBALDATA_EXPORT void Release();

    public: // New functions
        
        /**
        * Returns a reference to the calendar session
        * @return reference to a CCalSession
        */
        CALENGLOBALDATA_EXPORT CCalSession& CalSessionL();

        /**
        * Returns a pointer to the calendar instance view
        * if it has been constructed otherwise it
        * returns NULL and constructs the instance view
        * asynchronously.
        * @return pointer to a CCalInstanceView
        */
        CALENGLOBALDATA_EXPORT CCalInstanceView* InstanceViewL();

       /**
        * Returns a pointer to the calendar entry view
        * if it has been constructed otherwise it
        * returns NULL and constructs the entry view
        * asynchronously.
        * @return pointer to a CCalEntryView
        */
        CALENGLOBALDATA_EXPORT CCalEntryView* EntryViewL();

        /**
        * Returns information does complete entry view exist
        * @return ETrue if exists EFalse if does not exist.
        */
        CALENGLOBALDATA_EXPORT TBool EntryViewExists();

        /**
        * Returns a reference to the calendar context
        * @return reference to a CCalenContext
        */
        CALENGLOBALDATA_EXPORT MCalenContext& Context();

    public: // MCalenNotificationHandler
        void HandleNotification( const TCalenNotification aNotification );

    private:
        /**
        * C++ constructor.
        *
        */
        CCalenGlobalData( MCalProgressCallBack& aCalCallBack);

        /**
        * By default Symbian 2nd phase constructor is private. 
        * Doesn't take ownership of aNotifier.
        */
        void ConstructL(MCalenContextChangeObserver* aNotifier );

        /**
        * Private Destructor.  Users of this class should call Release() instead
        */
        ~CCalenGlobalData();

        /**
        * Sets the CCalenGlobalData to use an externally created CCalSession.
        * Leaves with KErrAlreadyExists if a different session has already been
        * created by the CCalenGlobalData
        */
        void SetSessionL(CCalSession* aSession);

        /**
        * Create entryview
        */
        void CreateEntryViewL();
        
        /**
        * Create instanceview
        */
        void CreateInstanceViewL();

        /**
        * Handle clean up after entryview/ instanceview being created
        */
        void ViewCreationCompleted( TBool aSuccess );

    private:
        // Member data owned by this class.  Member data is only initialised on first use
        
        CCalSession* iCalSession;
        CCalEntryView* iEntryView;
        CCalEntryView* iEntryViewCreation;
        CCalInstanceView* iInstanceView;
        CCalInstanceView* iInstanceViewCreation;
        CalenContextImpl* iContext;       
        TInt iRefCount;        
        TBool iGlobalDataOwnsCalSession;        

        /* We need queueing to avoid two immediate instance and entry view creation requests to symbian.
         * Though symbian handle two immediate requests ,this we require because we have only one observer
         * for callback and we would not be able to differentiate whether instance view got created or entry view got created.
         * If we dont queue the second request, we may end up giving incomplete object to the caller function.
         */
        typedef void (CCalenGlobalData::*QueuedFunction)();
        QueuedFunction iQueued;

        // Not owned by class. Needed for entry and instance view
        // creation.
        MCalProgressCallBack& iCalCallBack;
        
                
    };

#endif  // CALENGLOBALDATA_H

// End of File
