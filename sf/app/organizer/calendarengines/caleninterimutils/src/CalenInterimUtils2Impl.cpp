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
* Description:  This class contains utility method implementation 
*               related to usage of Calendar Interim API.
*
*/


// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CalenInterimUtils2Impl.h"

//debug
#include "calendarengines_debug.h"

#include <calentry.h>
#include <caluser.h>
#include <calentryview.h>
#include <etelmm.h>
#include <e32math.h>
#include <calrrule.h>
#include <featmgr.h>
#include <ecom/ecom.h>
#include <utf.h>
#include <openssl/md5.h>
#include <string.h>

#include "CleanupResetAndDestroy.h"
#include "RImplInfoPtrArrayOwn.inl"
#include "CalenEcomWatcher.h"       // Watches for ECOM registry changes

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#ifdef __WINSCW__
    _LIT( KEmulatorImei, "123456789012345" );
#endif

    const TInt KImeiLength = 15; // 15 chars from left of imei

    const TInt KAsciiFirstNumber = 48;             // ASCII character 48 = '0'.
    const TInt KAsciiFirstLowercaseLetter = 97;    // ASCII character 97 = 'a'.

}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::NewL()
// Creates and returns a new CCalenInterimUtils2Impl object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenInterimUtils2Impl* CCalenInterimUtils2Impl::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenInterimUtils2Impl* self = new(ELeave) CCalenInterimUtils2Impl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::CCalenInterimUtils2Impl()
// Default constructor
// -----------------------------------------------------------------------------
CCalenInterimUtils2Impl::CCalenInterimUtils2Impl()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::ConstructL()
// -----------------------------------------------------------------------------
void CCalenInterimUtils2Impl::ConstructL()
    {
    TRACE_ENTRY_POINT;

    FeatureManager::InitializeLibL();
    // Only start the ecom watcher if the feature flags are on.
    if ((FeatureManager::FeatureSupported(KFeatureIdMeetingRequestEnabler)) || 
        (FeatureManager::FeatureSupported(KFeatureIdMeetingRequestSupport)) )
        {
        iEcomWatcher  = CCalenEComWatcher::NewL(*this);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::~CCalenInterimUtils2Impl()
// -----------------------------------------------------------------------------
CCalenInterimUtils2Impl::~CCalenInterimUtils2Impl()
    {
    TRACE_ENTRY_POINT;
    
    delete iEcomWatcher;
	FeatureManager::UnInitializeLib();
	
	TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::PopulateChildFromParentL
// ----------------------------------------------------------------------------
void CCalenInterimUtils2Impl::PopulateChildFromParentL(CCalEntry& aChild, const CCalEntry& aParent)
    {
    TRACE_ENTRY_POINT;

    if ( aChild.DescriptionL() == KNullDesC )
        {
        aChild.SetDescriptionL( aParent.DescriptionL() );
        }
    if ( aChild.LocationL() == KNullDesC )
        {
        aChild.SetLocationL( aParent.LocationL() );
        }
    if ( aChild.PriorityL() == 0 )
        { // zero is undefined priority according to iCal spec.
        aChild.SetPriorityL( aParent.PriorityL() );
        }
    if ( aChild.SummaryL() == KNullDesC )
        {
        aChild.SetSummaryL( aParent.SummaryL() );
        }
    if ( aChild.StatusL() == CCalEntry::ENullStatus )
        {
        aChild.SetStatusL( aParent.StatusL() );
        }
    CCalAlarm* childAlarm = aChild.AlarmL();
    CCalAlarm* parentAlarm = aParent.AlarmL();
    
    CleanupStack::PushL(childAlarm);
    CleanupStack::PushL(parentAlarm);
    
    if( !childAlarm && parentAlarm )
        {
        aChild.SetAlarmL( parentAlarm);
        }
    
    CleanupStack::PopAndDestroy(childAlarm);
    CleanupStack::PopAndDestroy(parentAlarm);
		
    if (aChild.ReplicationStatusL() != aParent.ReplicationStatusL())
        {
        aChild.SetReplicationStatusL(aParent.ReplicationStatusL());
        }
    if (aChild.MethodL() == CCalEntry::EMethodNone)
        {
        aChild.SetMethodL(aParent.MethodL());
        }

    if ( !aChild.OrganizerL() && aParent.OrganizerL() )
        {
        CCalUser* owner = CopyUserLC( *( aParent.OrganizerL() ) );
        aChild.SetOrganizerL( owner );
        CleanupStack::Pop(owner); // ownership transferred
        }
    
    PopulateAttendeeListL( aChild, aParent );
    
    TRACE_EXIT_POINT;
    // "Categories" property is omitted
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::PopulateAttendeeListL
// ----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::PopulateAttendeeListL(
    CCalEntry& aChild,
    const CCalEntry& aParent )
    {
    TRACE_ENTRY_POINT;
    
    RPointerArray<CCalAttendee>& childAtt = aChild.AttendeesL();
    RPointerArray<CCalAttendee>& parentAtt = aParent.AttendeesL();
 
    CCalUser *po = NULL;
    if ( !aChild.PhoneOwnerL() )
        { // Phone owner is internal data, not part of ical protocol, it's
          // benefit is to avoid heavy "who am I?" resolving operation
        po = aParent.PhoneOwnerL();
        }
    TInt parentAttCount( parentAtt.Count() );
    if ( childAtt.Count() == 0 && parentAttCount > 0 )
        {
        for ( TInt i( 0 ); i < parentAttCount; ++i )
            {
            CCalAttendee* copy = CopyAttendeeLC( *( parentAtt[i] ) );
            aChild.AddAttendeeL( copy );
            CleanupStack::Pop(copy); // ownership transferred
            if(po == parentAtt[i])
                {
                aChild.SetPhoneOwnerL(copy);

                }
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::CopyAttendeeLC
// ----------------------------------------------------------------------------
//
CCalAttendee* CCalenInterimUtils2Impl::CopyAttendeeLC(
    const CCalAttendee& aSource )
    {
    TRACE_ENTRY_POINT;
    
    CCalAttendee* copy = CCalAttendee::NewL( aSource.Address(),
                                             aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
    copy->SetRoleL( aSource.RoleL() );
    copy->SetStatusL( aSource.StatusL() );
    copy->SetResponseRequested( aSource.ResponseRequested() );
    
    TRACE_EXIT_POINT;
    return copy;
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::CopyUserLC
// ----------------------------------------------------------------------------
//
CCalUser* CCalenInterimUtils2Impl::CopyUserLC( const CCalUser& aSource )
    {
    TRACE_ENTRY_POINT;
    
    CCalUser* copy = CCalUser::NewL( aSource.Address(),
                                     aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
    
    TRACE_EXIT_POINT;
    return copy;
    }


// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::GlobalUidL
// ----------------------------------------------------------------------------
//
HBufC8* CCalenInterimUtils2Impl::GlobalUidL()
    {
    TRACE_ENTRY_POINT;
    
    // Loosely using UID generation algorithm from
    // http://www.webdav.org/specs/draft-leach-uuids-guids-01.txt

    // Number of 100ns ticks since Oct 15 1582.
    TInt64 timeStamp = GetTicksFromGregorianCalendarStartL();

    // This differs slightly from the spec in that the clock sequence is just a pseudo-random number.
       TUint32 clockSeq = Math::Random();
       // IMEI is read the first time this is called, and stored for subsequent calls.
       if(!iImeiNode)
           {
           iImeiNode = GetImeiAsNodeValueL();
           }

    HBufC8* resultBuf = DoCreateUidLC(clockSeq, timeStamp, iImeiNode);
    CleanupStack::Pop(resultBuf);
    
    TRACE_EXIT_POINT;
    return resultBuf;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::GetTicksFromGregorianCalendarStartL()
// This function returns the number of 100ns ticks since 0:00 15th October 1582.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt64 CCalenInterimUtils2Impl::GetTicksFromGregorianCalendarStartL()
    {
    TRACE_ENTRY_POINT;
    
    TTime timeNow;
    timeNow.HomeTime();
    TDateTime gregorianStartDT(1582, EOctober, 15, 0, 0, 0, 0);
    TTime gregorianStart(gregorianStartDT);
    TTimeIntervalMicroSeconds msDifference = timeNow.MicroSecondsFrom(gregorianStart);
    
    TRACE_EXIT_POINT;
    return msDifference.Int64() * 10; // * 10 to convert from micro sec (==1000 ns) count to 100ns count.
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::GetImeiAsNodeValueL()
// Formats the IMEI returned from GetImeiL() into a 64-bit integer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt64 CCalenInterimUtils2Impl::GetImeiAsNodeValueL()
    {
    TRACE_ENTRY_POINT;
    
    TBuf<RMobilePhone::KPhoneSerialNumberSize> serialNumber;
    GetImeiL( serialNumber );

    // Trim off all but the first KImeiLength digits.
    TLex lex( serialNumber.Left(KImeiLength) );

    TInt64 val;
    User::LeaveIfError(lex.Val(val));
    
    TRACE_EXIT_POINT;
    return val;
    }

inline TUint8 LastFourBits(TUint8& aNum)
    {
    TRACE_ENTRY_POINT;
    
    // 00001111 b
    TRACE_EXIT_POINT;
    return aNum & 0x0F;
    }

inline TUint8 FirstFourBits(TUint8& aNum)
    {
    TRACE_ENTRY_POINT;
    
    // 11110000 b
    TRACE_EXIT_POINT;
    return (aNum & 0xF0) >> 4;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::DoCreateUidLC()
// Performs the work in creating a GUID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CCalenInterimUtils2Impl::DoCreateUidLC( const TUint32& aClockSeq,
                                               const TUint64& aTimeStamp,
                                               const TUint64& aNodeValue )
    {
    TRACE_ENTRY_POINT;
    
    // The roast beef of the algorithm. Does all the shifting about as described in the web draft.
    TUint32 time_low = aTimeStamp & 0xFFFFFFFF;
    TUint16 time_mid = (aTimeStamp >> 32) & 0xFFFF;
    TUint16 time_high = (aTimeStamp >> 48) & 0x0FFF;
    time_high |= (1 << 12);
    TUint8 clock_seq_low = aClockSeq & 0xFF;
    TUint8 clock_seq_high = (aClockSeq & 0x3F00) >> 8;
    clock_seq_high |= 0x80;

    // Can't use RArray as that's set up for minimum 4 bytes per item.
    CArrayFixFlat<TUint8> *node = new (ELeave) CArrayFixFlat<TUint8>(6);
    CleanupStack::PushL(node);

    // The rest of the function is mapping the 64, 32 and 16 bit numbers to 8 bit numbers
    // while losing as little data as possible.

    TUint64 mask = 0xFF0000000000;
    for(TInt i=0; i<=6; ++i)
        {
        TInt64 temp = aNodeValue & mask;
        temp >>= ((5-i)*8);
        node->AppendL(temp);
        mask = mask >> 8;
        }

    TBuf8<16> rawOutput;

    rawOutput.Append( (time_low  & 0xFF000000) >> 24 );
    rawOutput.Append( (time_low  & 0x00FF0000) >> 16 );
    rawOutput.Append( (time_low  & 0x0000FF00) >> 8 );
    rawOutput.Append( (time_low  & 0x000000FF) );

    rawOutput.Append( (time_mid  & 0xFF00) >> 8 );
    rawOutput.Append( (time_mid  & 0x00FF) );

    rawOutput.Append( (time_high & 0xFF00) >> 8 );
    rawOutput.Append( (time_high & 0x00FF) );

    rawOutput.Append( clock_seq_low );
    rawOutput.Append( clock_seq_high );

    for(TInt i=0; i<6; ++i)
        {
        rawOutput.Append( node->At(i) );
        }
    CleanupStack::PopAndDestroy(); // node
    
    TUint8 digest[16];
    HBufC8* resultBuf = rawOutput.AllocLC();
    TPtr8 resultBufPtr = resultBuf->Des();
    TUint length = resultBufPtr.Length();
    
    // Create a new buffer to provide space for '\0'
    HBufC8* newBuf = HBufC8::NewLC( length + 1 );//+1 space for '\0'
    TPtr8 newBufPtr = newBuf->Des();
    newBufPtr.Copy(resultBufPtr);
    
    // Appends a zero terminator onto the end of this descriptor's data
    // and returns a pointer to the data.
    char* chPtrTemp = ( char*)newBufPtr.PtrZ();
    char* chPtr = ( char*) User::AllocL( length + 1 );
    strcpy( chPtr , chPtrTemp );
    
    //md5 context
    MD5_CTX* context = new MD5_CTX();
    //initialize the context
    MD5_Init(context);
    //Append a string to the message
    MD5_Update(context, chPtr, length );
    //Finish the message and return the digest.
    MD5_Final(digest, context );
    
    // Add the version field in the msb 4 bits. The value of version is 3.
    digest[6] = digest[6] & 0x0F;
    digest[6] |= (3 << 4);
    
    //Add the variant field in the msb 2 bits. The value of variant is 2.
    digest[9] = digest[9] & 0x3F;
    digest[9] |= 0x80;
    
    delete chPtr;
    delete context;
    CleanupStack::PopAndDestroy( newBuf );
    CleanupStack::PopAndDestroy( resultBuf );
    TBuf8<36> output;
    TInt i;
    for(i=0; i<16; ++i)
        {
        output.Append( ConvertToCharacterL( FirstFourBits( digest[i] ) ) ); 
        output.Append( ConvertToCharacterL( LastFourBits( digest[i] ) ) );
        if(i == 3 || i == 5 || i == 7 ||i == 9)
            {
            output.Append( '-' );
            }
        }
    HBufC8* md5ResultBuf = output.AllocLC();
    
    TRACE_EXIT_POINT;
    return md5ResultBuf;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::ConvertToCharacterL()
// Converts from a number between 0-15 to a hexadecimal character representation.
// 0-9, a-f
// -----------------------------------------------------------------------------
//
TChar CCalenInterimUtils2Impl::ConvertToCharacterL(TUint8 aChar)
    {
    TRACE_ENTRY_POINT;
    
    if(aChar < 10)
        {
        // Convert to a digit.
        TRACE_EXIT_POINT;
        return TChar(aChar + KAsciiFirstNumber);
        }
    else if(aChar < 16)
        {
        // Convert to lower case character.
        TRACE_EXIT_POINT;
        return TChar(aChar - 10 + KAsciiFirstLowercaseLetter);
        }

    // This function should only be passed numbers between 0-15.
    User::Leave(KErrArgument);
    
    TRACE_EXIT_POINT;
    return TChar(KAsciiFirstLowercaseLetter); // Stops compiler warning.
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::GetImeiL
// Gets the IMEI of the device. On the emulator, this will return a fake IMEI.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::GetImeiL( TDes& aImei )
    {
    TRACE_ENTRY_POINT;
    
    #ifndef __WINSCW__

    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL(telServer);

    TInt numPhones = 0;
    User::LeaveIfError( telServer.EnumeratePhones(numPhones) );
    if(numPhones < 1)
        {
        User::Leave(KErrNotFound);
        }
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( telServer.GetPhoneInfo(0, info) );
    RMobilePhone mobilePhone;
    User::LeaveIfError( mobilePhone.Open(telServer, info.iName) );
    CleanupClosePushL(mobilePhone);

    TUint32 identityCaps;
    User::LeaveIfError( mobilePhone.GetIdentityCaps( identityCaps ) );

    if ( identityCaps & RMobilePhone::KCapsGetSerialNumber )
        {
        TRequestStatus status;
        RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;
        mobilePhone.GetPhoneId( status, mobilePhoneIdentity );
        // Needs testing on target - May lock device
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        aImei = mobilePhoneIdentity.iSerialNumber;
        }
    else
        {
        User::Leave(KErrNotSupported);
        }

    CleanupStack::PopAndDestroy(); // mobilePhone;
    CleanupStack::PopAndDestroy(); // telServer;

    #else

      // Return a fake IMEI when working on emulator
      aImei = KEmulatorImei;

    #endif // __WINSCW__
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::MRViewersEnabledL()
// Checks to see if Meeting Request Viewer functionality
// is enabled and an implementation is available to use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenInterimUtils2Impl::MRViewersEnabledL(TBool /*aForceCheck*/)
    {
    TRACE_ENTRY_POINT;
    // Need to implement it once we have meeting request viewer in 10.1
    TRACE_EXIT_POINT;
    return false;    
    }
// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::DoMRViewersEnabledL()
// Checks to see if Meeting Request Viewer functionality
// is enabled and an implementation is available to use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::DoMRViewersEnabledL()
    {
    TRACE_ENTRY_POINT;
		// Need to implement it once we have meeting request viewer in 10.1
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::HasTimeOrDateChangedL()
// Checks to see if the date or time has changed from the given entry to the
// entry with the same UID stored in the database. This function should only be
// called on parent entries with a repeat.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenInterimUtils2Impl::HasTimeOrDateChangedL(const CCalEntry& aNewEntry, const CCalEntry& aOldEntry)
    {
    TRACE_ENTRY_POINT;
    
    TBool hasChanged = EFalse;

    if( aNewEntry.StartTimeL().TimeUtcL() != aOldEntry.StartTimeL().TimeUtcL() ||
        aNewEntry.EndTimeL().TimeUtcL()   != aOldEntry.EndTimeL().TimeUtcL())
        {
        hasChanged = ETrue;
        }

    TRACE_EXIT_POINT;
    return hasChanged;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::PrepareForStorageL()
// Sets the last modified date of an entry, and if aEntry is a meeting request,
// sets the phone owner if not previously set.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::PrepareForStorageL( CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;

    FeatureManager::InitializeLibL();

    aEntry.SetLastModifiedDateL();

    if (FeatureManager::FeatureSupported(KFeatureIdMeetingRequestEnabler) ||
        FeatureManager::FeatureSupported(KFeatureIdMeetingRequestSupport))
        {
        if( IsMeetingRequestL(aEntry) && aEntry.PhoneOwnerL() == NULL )
            {
            // Need to implement it once we have meeting request viewer in 10.1
            }
        }
    FeatureManager::UnInitializeLib();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::SingleStoreL()
// Calls CCalEntryView::StoreL() on the given entry. Puts the entry into an
// RPointerArray, as CalInterimApi expects.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::SingleStoreL( CCalEntryView& aEntryView,
                                      CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;

    PrepareForStorageL( aEntry );
    TInt successCount=0;
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    entries.Append( &aEntry );
    aEntryView.StoreL( entries, successCount );
    CleanupStack::PopAndDestroy( &entries );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::SingleUpdateL()
// Calls CCalEntryView::UpdateL() on the given entry. Puts the entry into an
// RPointerArray, as CalInterimApi expects.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::SingleUpdateL( CCalEntryView& aEntryView,
                                       CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;
    
    PrepareForStorageL( aEntry );
    TInt successCount=0;
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    entries.Append( &aEntry );
    aEntryView.UpdateL( entries, successCount );
    CleanupStack::PopAndDestroy( &entries );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::FieldIsTheSameL()
// Checks to see if the ONE given field is the same on one entry as another.
// To check if more than one field is the same, call the function more than once
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenInterimUtils2Impl::FieldIsTheSameL( CCalEntry& aEntryOne,
                                          CCalEntry& aEntryTwo,
                                          TDifferenceFlag aFlag)
    {
    TRACE_ENTRY_POINT;
    
    switch( aFlag )
        {
        case EEntryDifferentStartTimeAndEndTime:
            TRACE_EXIT_POINT;
            return      ( TimeOfDay( aEntryOne.StartTimeL().TimeUtcL() )
                        == TimeOfDay( aEntryTwo.StartTimeL().TimeUtcL() ))
                    &&  ( TimeOfDay( aEntryOne.EndTimeL().TimeUtcL() )
                        == TimeOfDay( aEntryTwo.EndTimeL().TimeUtcL() ) );
        case EEntryDifferentSummary:
            TRACE_EXIT_POINT;
            return aEntryOne.SummaryL() == aEntryTwo.SummaryL();
        case EEntryDifferentDescription:
            TRACE_EXIT_POINT;
            return aEntryOne.DescriptionL() == aEntryTwo.DescriptionL();
        case EEntryDifferentLocation:
            TRACE_EXIT_POINT;
            return aEntryOne.LocationL() == aEntryTwo.LocationL();
        default:
            _LIT(KCalenInterimUtils2Panic, "CCalenInterimUtils2");
            User::Panic(KCalenInterimUtils2Panic, KErrArgument);
        }
    TRACE_EXIT_POINT;
    return EFalse; // Never hit.
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::CopyFieldL()
// Copies ONE given field from aSrc to aDst. To copy more than one field,
// call the function more than one time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::CopyFieldL( const CCalEntry& aSrc,
                                    CCalEntry& aDst,
                                    TDifferenceFlag aField )
    {
    TRACE_ENTRY_POINT;

    switch( aField )
        {
        case EEntryDifferentStartTimeAndEndTime:
            {
            // START TIME
            // Keep aDst's start date, but copy the start time (h/m/s) from aSrc to aDst.
            TTimeIntervalMinutes dtStartTimeOfDay = TimeOfDay( aSrc.StartTimeL().TimeUtcL() );
            TTime dtStartDay = BeginningOfDay( aDst.StartTimeL().TimeUtcL() );

            TCalTime startTime;
            startTime.SetTimeUtcL( dtStartDay + (TTimeIntervalMinutes)dtStartTimeOfDay );


            TTimeIntervalMinutes duration;
            aSrc.EndTimeL().TimeUtcL().MinutesFrom(aSrc.StartTimeL().TimeUtcL(), duration);

            // END TIME
            // Calculate the duration of aSrc, and make aDst endtime equal aDst startTime
            // + duration.  This will allow for events spanning multiple days.
            TCalTime endTime;
            endTime.SetTimeUtcL(startTime.TimeUtcL() + duration);

            aDst.SetStartAndEndTimeL(startTime, endTime);

            break;
            }
        case EEntryDifferentSummary:
            aDst.SetSummaryL(aSrc.SummaryL());
            break;
        case EEntryDifferentDescription:
            aDst.SetDescriptionL(aSrc.DescriptionL());
            break;
        case EEntryDifferentLocation:
            aDst.SetLocationL(aSrc.LocationL());
            break;
        default:
            _LIT(KCalenInterimUtils2Panic, "CCalenInterimUtils2");
            User::Panic(KCalenInterimUtils2Panic, KErrArgument);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::CopyChildrenExceptionDataL()
// For each exception, for each field, if the field is NOT the reason for the
// entry being an exception, copy the new parent's field across to the exception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::CopyChildrenExceptionDataL( CCalEntry& aEditedEntry,
                                                    RPointerArray<CCalEntry>& aOldEntries )
    {
    TRACE_ENTRY_POINT;

    // For each oldChild...
    for(TInt i=1; i<aOldEntries.Count(); ++i)
        {
        // For each field...
        for(TDifferenceFlag j=(TDifferenceFlag)1; j<EEntryDifferenceCount; j=(TDifferenceFlag)(j<<1))
            {
            // Where oldChild field == oldParent Field
            // and newParent field != oldParent Field...
            if( FieldIsTheSameL(*aOldEntries[i], *aOldEntries[0], j ) &&
                !FieldIsTheSameL(aEditedEntry,  *aOldEntries[0], j ) )
                {
                // ...copy newParent field to oldChild.
                CopyFieldL(aEditedEntry, *aOldEntries[i], j);
                }
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::StoreL()
// Public exported function. Checks to see if the given entry should be entered
// using StoreL() or UpdateL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::StoreL( CCalEntryView& aEntryView,
                                CCalEntry& aEntry,
                                TBool aCopyToChildren )
    {
    TRACE_ENTRY_POINT;
    
    if (!IsEntryRepeatingL(aEntry))
        {
        // This is not a repeating entry.  Technically we should make a
        // decision to see if we should be calling StoreL or UpdateL, but
        // the cost of all the checking required to determine this would 
        // be greater than the benefit of calling UpdateL if possible.
        // Just call StoreL, as ultimately it is a lighter call.
        SingleStoreL(aEntryView, aEntry);
        return;
        }


    // This entry is repeating. Does it have EXDATEs which could be due to children?
    RArray<TCalTime> exceptionDates;
    CleanupClosePushL( exceptionDates );
    aEntry.GetExceptionDatesL( exceptionDates );
    TInt exceptionCount = exceptionDates.Count();
    CleanupStack::PopAndDestroy( &exceptionDates );

    if( exceptionCount == 0 )
        {
        // No exception dates so do a StoreL().
        // We have no exceptions, so there are no children to re-store
        // Same logic as above applies, we call StoreL rather than check to 
        // see if we could have called UpdateL
        SingleStoreL( aEntryView, aEntry );
        return;
        } 

    //Is this a child entry?
    if(aEntry.RecurrenceIdL().TimeUtcL() != Time::NullTTime())
        {
        SingleStoreL( aEntryView, aEntry );
        return;
        }

    //Entry is not a child, but does it have any children?

    // Fetch array of entries associated with this UID.
    RPointerArray<CCalEntry> oldEntries;
    CleanupResetAndDestroyPushL(oldEntries);
    aEntryView.FetchL(aEntry.UidL(), oldEntries);
    TBool hasChildren = oldEntries.Count() > 0;
       
    if (oldEntries.Count() == 0)
        {
        //This is a new repeating entry, with exceptions
        //This must have come from an external application, as the 
        //calendar UI does not allow creation of this type of entry
        SingleStoreL(aEntryView, aEntry);
        }
        
    //Have the RRule or time fields changed
    else if(aCopyToChildren || HasTimeOrDateChangedL(*oldEntries[0], aEntry) 
        || HaveRepeatPropertiesChangedL(*oldEntries[0], aEntry))
        {
        if (hasChildren && aCopyToChildren)
            {
            CopyChildrenExceptionDataL( aEntry, oldEntries );
            }
        SingleStoreL(aEntryView, aEntry);
        
        if(hasChildren)
            {
            StoreEachChildEntryL( aEntryView, aEntry, oldEntries, !aCopyToChildren );
            }
        }
    else
        {
        SingleUpdateL(aEntryView, aEntry);
        }

    CleanupStack::PopAndDestroy( &oldEntries );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// GenerateRecurrenceIdFromEntryL
// Static helper function for StoreEachChildEntryL. Generates a TCalTime which is at the
// same TIME as aEntry but at the same DATE as aInstanceDate
// ---------------------------------------------------------
TCalTime GenerateRecurrenceIdFromEntryL( CCalEntry& aEntry, TCalTime aInstanceDate )
    {
    TRACE_ENTRY_POINT;
    
    TDateTime theTime = aEntry.StartTimeL().TimeUtcL().DateTime();
    TDateTime theDate = aInstanceDate.TimeUtcL().DateTime();

    theTime.SetYear(theDate.Year());
    theTime.SetMonth(theDate.Month());
    theTime.SetDay(theDate.Day());

    TCalTime toRet;
    toRet.SetTimeUtcL(theTime);
    
    TRACE_EXIT_POINT;
    return toRet;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::StoreEachChildEntryL()
// Run through each of the given CCalEntries and call CCalEntryView::StoreL()
// on them. Any Leaves in StoreL are ignored to make sure all items get entered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::StoreEachChildEntryL( CCalEntryView &aEntryView,
                                              CCalEntry &aEntry,
                                              RPointerArray<CCalEntry> &aOldEntries,
                                              TBool aResetLocalUid)
    {
    TRACE_ENTRY_POINT;
    
    // Start from 1 as we don't want to copy the old parent entry.
    for(TInt i=1; i<aOldEntries.Count(); ++i)
        {
        if (aResetLocalUid)
            {
            // Reset the local UID of the exception.  When we store the exception, it will
            // be added as a new entry rather than an update.
            aOldEntries[i]->SetLocalUidL( TCalLocalUid( 0 ) );
            }

        // The RecurrenceId of child (exception) entries should never be a null time by definition.
        // The code below will attempt to generate a RecurrenceId from the start time of the
        // exception if no RecurrenceId is found.  This should never actually happen, and
        // will not work if the start time/start date is changed.  The if case below should remain
        // until the Symbian defect fix for NULL RecurrenceIds is verified.

        if(aOldEntries[i]->RecurrenceIdL().TimeUtcL() == Time::NullTTime())
            {
            // This is being hit, but shouldn't be. Hence we create a new Recurrence ID.
            // Without doing this, the SingleStoreL below fails with Agenda Model -35: No agenda server.
            TCalTime recId = GenerateRecurrenceIdFromEntryL( aEntry, aOldEntries[i]->StartTimeL() );
            CCalEntry *exception = CCalEntry::NewL( aOldEntries[i]->EntryTypeL(),
                                                aEntry.UidL().AllocL(),
                                                aOldEntries[i]->MethodL(),
                                                aOldEntries[i]->SequenceNumberL(),
                                                recId,
                                                aOldEntries[i]->RecurrenceRangeL() );
            CleanupStack::PushL(exception);
            exception->CopyFromL(*aOldEntries[i]);

            PIM_TRAPD_HANDLE( SingleStoreL(aEntryView, *exception) );
            CleanupStack::PopAndDestroy(exception);
            }
        else
            {
            // If the start time of the series has been changed, the call below will
            // leave with -1, and the child entries will be lost.  To prevent this
            // we need to regenerate a new recurrence id for each child, create a copy
            // of the child with the new recurrence id, and store that instead.
            // Fixing this may cause issues with sync though, as some servers delete the
            // children when changing the start time of the series anyway.
            PIM_TRAPD_HANDLE( SingleStoreL(aEntryView, *aOldEntries[i]) );
            }
        }
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::IsMeetingRequestL()
// Check to see if the given entry is a meeting request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenInterimUtils2Impl::IsMeetingRequestL( CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return aEntry.EntryTypeL() == CCalEntry::EAppt
        && aEntry.OrganizerL();
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::IsEntryRepeatingL()
// Check to see if the given entry is a repeating
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCalenInterimUtils2Impl::IsEntryRepeatingL(const CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;
    
    TBool isRepeating = EFalse;
    RArray<TCalTime> rdates;
    CleanupClosePushL(rdates);
    aEntry.GetRDatesL(rdates);
    TBool isRDate = rdates.Count() > 0;
    CleanupStack::PopAndDestroy(); // rdates

    TCalRRule newRule;

    if ((isRDate) || (aEntry.GetRRuleL(newRule) && newRule.Type() != TCalRRule::EInvalid ))
        {
        isRepeating = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return isRepeating;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::HaveRepeatPropertiesChangedL()
// Check to see if the RRule or RDates have been modified.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCalenInterimUtils2Impl::HaveRepeatPropertiesChangedL(const CCalEntry& aNewEntry, const CCalEntry& aOldEntry)
    {
    TRACE_ENTRY_POINT;
    
    //Have the RRules Changed?
    TCalRRule newEntryRule;
    aNewEntry.GetRRuleL(newEntryRule);

    TCalRRule oldEntryRule;
    aOldEntry.GetRRuleL(oldEntryRule);

    if ((newEntryRule.Type() != oldEntryRule.Type()) ||
    (newEntryRule.DtStart().TimeUtcL() != oldEntryRule.DtStart().TimeUtcL()) ||
    (newEntryRule.Until().TimeUtcL() != oldEntryRule.Until().TimeUtcL()) ||
    (newEntryRule.Count() != oldEntryRule.Count()))
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    // Did the RDates change?
    TBool rDatesChanged = EFalse;
    RArray<TCalTime> newRDates;
    RArray<TCalTime> oldRDates;
    CleanupClosePushL(newRDates);
    CleanupClosePushL(oldRDates);
    aNewEntry.GetRDatesL(newRDates);
    aOldEntry.GetRDatesL(oldRDates);

    if (newRDates.Count() != oldRDates.Count())
        {
        rDatesChanged = ETrue;
        }
    else
        {
        for (TInt x = 0; x < newRDates.Count(); ++x)
            {
            if (newRDates[x].TimeUtcL() != oldRDates[x].TimeUtcL())
                {
                rDatesChanged = ETrue;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy(&oldRDates);
    CleanupStack::PopAndDestroy(&newRDates);

    TRACE_EXIT_POINT;
    return rDatesChanged;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::TimeOfDay
// Returns minimum time allowed, 1.1.1900 0:00 is min
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TTimeIntervalMinutes CCalenInterimUtils2Impl::TimeOfDay( const TTime& aDateTime )
    {
    TRACE_ENTRY_POINT;
    
    TTime midnight = BeginningOfDay( aDateTime );
    TTimeIntervalMinutes result;
    aDateTime.MinutesFrom( midnight, result );
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::BeginningOfDay
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TTime CCalenInterimUtils2Impl::BeginningOfDay(const TTime& aStartTime)
    {
    TRACE_ENTRY_POINT;

    TTime zero(TInt64(0));
    
    TRACE_EXIT_POINT;
    return zero + aStartTime.DaysFrom(zero);
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Impl::EComChanged
// From MCalenEComChangeObserver, called by when the ecom registry is changed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Impl::EComChanged()
    {
    TRACE_ENTRY_POINT;

    TRAP_IGNORE(MRViewersEnabledL(ETrue));

    TRACE_EXIT_POINT;
    }

// End of file
