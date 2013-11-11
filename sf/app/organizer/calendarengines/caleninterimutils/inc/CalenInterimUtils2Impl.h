/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This class implements functions from CCalenInterimUtils2
*
*/


#ifndef __CALENINTERIMUTILS2IMPL_H__
#define __CALENINTERIMUTILS2IMPL_H__

//  INCLUDES
#include <e32base.h>
#include <calcommon.h>
#include "calenecomchangeobserver.h"    // MCalenEComChangeObserver

// FORWARD DECLARATIONS
class CCalEntry;
class CCalAttendee;
class CCalUser;
class CCalEntryView;
class CCalInstanceView;
class TCalTime;
class TCalRRule;
class CCalInstance;
class CCalenEComWatcher;    // Watches for installed MR solutions

// CLASS DECLARATION

/**
* This class contains utility methods related to usage of
* Calendar Interim API
*/
NONSHARABLE_CLASS(CCalenInterimUtils2Impl) : public CBase,
                                    public MCalenEComChangeObserver
    {
    public: // Construction/destruction
        /**
        * Create a new instance of CCalenInterimUtils2
        */
        static CCalenInterimUtils2Impl* NewL();

        /**
        * Destructor
        */
        ~CCalenInterimUtils2Impl();

    public: // New functions
        /**
        * Allocates and returns a new global UID.
        * @return UID, ownership is transferred to caller
        */
        HBufC8* GlobalUidL();
        
        /**
        * Populates empty fields of a child entry with data from
        * the parent entry.
        * @param aChild child (modifying) entry to be populated
        * @param aParent parent (originating) entry
        */
        static void PopulateChildFromParentL( CCalEntry& aChild, const CCalEntry& aParent );

        /**
        * Checks to see if Meeting Request Viewer functionality
        * is enabled and is available to use
        * @return current state of MR Viewers implementation
        */
        TBool MRViewersEnabledL(TBool aForceCheck);

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
        static void StoreL( CCalEntryView& aEntryView,
                                     CCalEntry& aEntry,
                                     TBool aCopyToChildren = EFalse );

        /**
        * Checks to see if the given entry has properties consistent with a meeting request.
        * @param aEntry Entry to test
        * @return ETrue if the entry is a meeting request, EFalse otherwise
        */
        static TBool IsMeetingRequestL( CCalEntry& aEntry );

        /**
         * From MCalenEComChangeObserver
         * Called by when the ecom registry gets changed.
         */
        void EComChanged();

    private: // enumerations

        enum TDifferenceFlag
            {
            EEntryDifferentStartTimeAndEndTime  = 0x001,
            EEntryDifferentSummary              = 0x002,
            EEntryDifferentDescription          = 0x004,
            EEntryDifferentLocation             = 0x08,
            EEntryDifferenceCount               = 0x010 // Must be last!
            };

    private: // New functions

        void ConstructL();

        static TInt FindInstanceL( CCalInstanceView& aInstanceView,
                                   CCalEntry& aEntry,
                                   const TCalTime& aInstanceDate, 
                                   RPointerArray<CCalInstance>& aInstances );
        /**
        * Helper method which populates child's attendee list if is empty.
        */
        static void PopulateAttendeeListL( CCalEntry& aChild,
                                           const CCalEntry& aParent );

        /*
        * Helper which creates a new copy of the given attendee. Item is
        * left in the cleanup stack.
        *
        * @param aSource attendee to copy
        * @return attendee, ownership is transferred to caller.
        */
        static CCalAttendee* CopyAttendeeLC( const CCalAttendee& aSource );

        /*
        * Helper which creates a new copy of the given cal user. Item is
        * left in the cleanup stack.
        *
        * @param aSource user to copy
        * @return user, ownership is transferred to caller.
        */
        static CCalUser* CopyUserLC( const CCalUser& aSource );

        /**
        * Helper which reads handset's imei. Caller must pass long enough
        * descriptor (RMobilePhone::KPhoneSerialNumberSize).
        * @param aImei handset's imei
        */
        static void GetImeiL( TDes& aImei );

        /**
        * Helper which sets the last modified date of a given entry
        * and, if aEntry is a meeting request, sets the phone owner of
        * the entry, if not previously set.
        * @param aEntry entry to update
        */
        static void PrepareForStorageL( CCalEntry& aEntry );

        /**
        * Helper which puts the given entry into an entry array and
        * calls StoreL on it.
        * @param aEntryView entry view upon which StoreL is performed
        * @param aEntry entry to store
        */
        static void SingleStoreL( CCalEntryView& aEntryView,
                                  CCalEntry& aEntry );

        /**
        * Helper which puts the given entry into an entry array and
        * calls UpdateL on it.
        * @param aEntryView entry view upon which UpdateL is performed
        * @param aEntry entry to store
        */
        static void SingleUpdateL( CCalEntryView& aEntryView,
                                   CCalEntry& aEntry );

        /**
        * Check to see if the two given entries' fields are the same.
        * @param aEntryOne first entry
        * @param aEntryTwo second entry
        * @param aFlag Difference flag containing ONE field to copy
        */
        static TBool FieldIsTheSameL( CCalEntry& aEntryOne,
                                      CCalEntry& aEntryTwo,
                                      TDifferenceFlag aFlag );

        /**
        * Copys a single field from one entry to another.
        * @param aSrc source entry
        * @param aDst destination entry
        * @param aField Difference flag containing ONE field to copy
        */
        static void CopyFieldL( const CCalEntry& aSrc,
                                CCalEntry& aDst,
                                TDifferenceFlag aField );

        /**
        * Compares the start and end datetimes of two entries
        * @param aNewEntry The entry to be updated
        * @param aOldEntry The original entry
        */
        static TBool HasTimeOrDateChangedL(const CCalEntry& aNewEntry, const CCalEntry& aOldEntry);


        /**
        * Checks to see if any information can be saved from old entries.
        * Logic of: For every field, if the exception's field is the same field as the old parent
        * entry's field, and the new parent entry field is different from the old parent entry field,
        * copy the new parent's field to the exception's field.
        * @param aEditedEntry entry currently being edited
        * @param aOldEntries previous array of exception entries, with the parent entry being at position 0.
        * This can be retrieved using CCalEntryView::FetchL().
        */
        static void CopyChildrenExceptionDataL( CCalEntry& aEditedEntry,
                                                RPointerArray<CCalEntry>& aOldEntries );

        /**
        * Stores each child entry in aEntries in a separate call to CCalEntryView::StoreL().
        * This function traps and ignores any failures.
        * @param aEntryView view of the entries
        * @param aEntry current entry being updated. Used for UID and RecurrenceId.
        * @param aEntries array of CCalEntries to be stored
        * @param aResetLocalUid resets local uid of restored child entries
        */
        static void StoreEachChildEntryL( CCalEntryView &aEntryView,
                                          CCalEntry &aEntry,
                                          RPointerArray<CCalEntry> &aEntries,
                                          TBool aResetLocalUid);

        /**
        * Compares the repeat properties (RDates and RRules) of two entries
        * @param aNewEntry The entry to be updated
        * @param aOldEntry The original entry
        */
        static TBool HaveRepeatPropertiesChangedL(const CCalEntry& aNewEntry, const CCalEntry& aOldEntry);

        /**
        * Return ETrue if the entry is a repeating entry
        */
        static TBool IsEntryRepeatingL(const CCalEntry& aEntry);
        
        // Just a wrapper for leaves
        void DoMRViewersEnabledL();
	
private: 
    //Default Constructor
    CCalenInterimUtils2Impl();
    
    // Helper functions for GUID generation.
    static TInt64 GetTicksFromGregorianCalendarStartL();
    static TInt64 GetImeiAsNodeValueL();
    static HBufC8* DoCreateUidLC(const TUint32& aClockSeq, const TUint64& aTimeStamp, const TUint64& aNodeValue);
    static TChar ConvertToCharacterL(TUint8 aChar);

private: // Time and date utility functions, copied from commonui to remove dependency
    static TTimeIntervalMinutes TimeOfDay( const TTime& aDateTime );
    static TTime BeginningOfDay(const TTime& aStartTime);

private: // Member data
    CCalenEComWatcher* iEcomWatcher;
    TInt64 iImeiNode;
    TBool iMrEnabled;
    TBool iMrEnabledCheck;
    };

#endif // __CALENINTERIMUTILS2IMPL_H__

// End of File
