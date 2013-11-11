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
* Description: Header file for CCalProgressCallBackListener class.
*
*/

#ifndef CALPROGRESSCALLBACKLISTENER_H
#define CALPROGRESSCALLBACKLISTENER_H

// INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntryView;
class CCalInstanceView;

/**
*  CCalProgressCallBackListener implements the MCalProgressCallBack interface.
*
*  @lib CalendarCommonUtilsTest.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalProgressCallBackListener ) : public CBase, public MCalProgressCallBack
    {
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalProgressCallBackListener* NewL();

        /**
        * Destructor.
        */
        virtual ~CCalProgressCallBackListener();

    public: // Functions from base classes

        /**
        * From MCalProgressCallBack.
        * @param aPercentageCompleted The percentage complete.
        */
        void Progress( TInt aPercentageCompleted );
        
        /**
        * From MCalProgressCallBack.
        * @param aError The error if the operation failed, 
        * or KErrNone if successful.
        */
        void Completed( TInt aError );
        
        /**
        * From MCalProgressCallBack.
        * @return If the observing class returns EFalse, 
        * then the Progress() function will not be called. 
        */
        TBool NotifyProgress();
        
    public: // New functions
        
        
    private:
        
        /**
        * C++ default constructor.
        */
        CCalProgressCallBackListener();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        
    };

#endif      // CALPROGRESSCALLBACKLISTENER_H

// End of file

