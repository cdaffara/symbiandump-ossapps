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
*             This class contains utility methods related to usage of 
*             Calendar Interim API.
*
*/



#ifndef __CALENINTERIMUTILS2_H__
#define __CALENINTERIMUTILS2_H__

//  INCLUDES
#include <e32base.h>
#include <calcommon.h>

// FORWARD DECLARATIONS
class CCalEntry;
class CCalEntryView;
class CCalenInterimUtils2Impl;

// CLASS DECLARATION

/**
* This class contains utility methods related to usage of
* Calendar Interim API
*/
NONSHARABLE_CLASS(CCalenInterimUtils2) : public CBase
    {
    public: // Construction/destruction
        /**
         * Create a new instance of CalenInterimUtils2
         */
        IMPORT_C static CCalenInterimUtils2* NewL();

        /**
         * Destructor
         */
        IMPORT_C ~CCalenInterimUtils2();

    public: // New functions
        /**
         * Allocates and returns a new global UID.
         * @return UID, ownership is transferred to caller
         */
        IMPORT_C HBufC8* GlobalUidL();

        /**
         * Populates empty fields of a child entry with data from
         * the parent entry.
         * @param aChild child (modifying) entry to be populated
         * @param aParent parent (originating) entry
         */
        IMPORT_C static void PopulateChildFromParentL( CCalEntry& aChild,
                                                const CCalEntry& aParent );

        /**
         * Checks to see if Meeting Request Viewer functionality
         * is enabled and is available to use
         * @return current state of MR Viewers implementation
         */
        IMPORT_C TBool MRViewersEnabledL(TBool aForceCheck = EFalse);

        /**
         * This function is a wrapper around CCalEntryView::StoreL() and CCalEntryView::UpdateL().
         * The function makes the appropriate call to either of those functions, depending on several
         * factors, such as whether the entry is a child or parent, and whether it has exceptions or not.
         * If aCopyToChildren is set to ETrue, extra logic is applied as follows:
         * For each exception, for each field, if the field is NOT the reason for the
         * entry being an exception, copy the new parent's field across to the exception.
         * @param aEntryView view of the entries
         * @param aEntry entry to be stored
         * @param aCopyToChildren whether the extra logic regarding exception data should be run
         */
        IMPORT_C static void StoreL( CCalEntryView& aEntryView,
                                     CCalEntry& aEntry,
                                     TBool aCopyToChildren = EFalse );

        /**
         * Checks to see if the given entry has properties consistent with a meeting request.
         * @param aEntry Entry to test
         * @return ETrue if the entry is a meeting request, EFalse otherwise
         */
        IMPORT_C static TBool IsMeetingRequestL( CCalEntry& aEntry );

    private: // Construction/destruction
        void ConstructL();
        CCalenInterimUtils2();

    private: // Member data
        CCalenInterimUtils2Impl* iImpl;
    };

#endif // __CALENINTERIMUTILS2_H__

// End of File
