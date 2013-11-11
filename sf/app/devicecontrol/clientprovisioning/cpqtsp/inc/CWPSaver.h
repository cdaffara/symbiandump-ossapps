/*
* ============================================================================
*  Name        : CWPSaver.h
*  Part of     : Provisioning / ProvisioningBC
*  Description : Settings saver with progress note.
*  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2002-2006 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#ifndef CWPSAVER_H
#define CWPSAVER_H

// INCLUDES

#include <HbProgressDialog>

// FORWARD DECLARATIONS
class CWPEngine;
//class CActiveFavouritesDbNotifier;

// CLASS DECLARATION

/**
 * Helper class for saving Provisioning settings. Provides a progress note.
 * @since 2.0
 */
class CWPSaver : public CActive
                 
    {
    public:
        /**
        * C++ default constructor.
        * @param aEngine Engine to be used for saving
        * @param aSetAsDefault EFalse => call CWPEngine::SaveL, ETrue
        *        => call CWPEngine::SetAsDefaultL
        */
        CWPSaver( CWPEngine& aEngine, TBool aSetAsDefault );

        /**
        * Destructor.
        */
        virtual ~CWPSaver();

    public:
        /**
        * Prepares the object for saving.
        */
        void PrepareLC();

        /**
        * Executes save with a progress note. Ownership of the
        * CWPSaver object is transferred.
        * @param aNumSaved When returns, contains number of settings saved.
        * @return Status code. >= 0 if saving was completed
        */
        TInt ExecuteLD( TInt& aNumSaved, TBufC<256>& value );

    protected: // From CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private:
        /**
        * Complete the request so that RunL() gets called.
        */
        void CompleteRequest();
        void Retry();


    private:
        /// The engine used for performing the save. Refs.
        CWPEngine& iEngine;

        /// ETrue if setting as default
        TBool iSetAsDefault;     

        /// The item to be saved next
        TInt iCurrentItem;

        /// Contains result to be passed to the called of ExecuteLD
        TInt iResult;

        /// Active scheduler.
        CActiveSchedulerWait iWait;

        /// Timer for retry. Owns.
        CPeriodic* iRetryTimer;

        /// Retry counter
        TInt iRetryCount;
        
        HbProgressDialog *iProgress;
        TBufC<200> iValue;
    };


#endif // CWPSAVER_H

// End of File
