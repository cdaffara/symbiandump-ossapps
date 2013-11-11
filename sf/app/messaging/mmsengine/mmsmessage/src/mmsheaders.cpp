/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mmsheaders implementation
*
*/



// INCLUDE FILES
#include    <msvstore.h>
#include    <badesca.h>
#include    <msvids.h>
#include    <mtmdef.h>
#include    <e32def.h>
#include    <e32math.h>
#include    <mmsvattachmentmanager.h>
#include    <cmsvmimeheaders.h>
#include    "mmsconst.h"
#include    "mmsheaders.h"
#include    "mmssettings.h"
#include    "mmsversion.h"
#include    "mmselementdescriptor.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsmmboxviewheaders.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KMmsArrayAllocationNumber = 6;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMmsHeaders::CMmsHeaders():
    iLinearOrder( CMmsSendingChain::Compare ),
    iDeleteResultOrder( CMmsDeleteResultArray::Compare )
// in a class derived from CBase all member variables are set initially to 0
    {
    }

// Symbian OS default constructor can leave.
void CMmsHeaders::ConstructL( TInt16 aMmsDefaultVersion )
    {
    iToArray = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    iCcArray = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    iBccArray = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    iMmsDefaultVersion = aMmsDefaultVersion;
    if ( iMmsDefaultVersion < KMmsMinimumSupportedVersion ||
        iMmsDefaultVersion > KMmsMaximumSupportedVersion )
        {
        // The supported default version must be within limits.
        // The actual messages can carry any version
        iMmsDefaultVersion = KMmsDefaultVersion;
        }
    Reset( NULL );
    }

// Two-phased constructor.
EXPORT_C CMmsHeaders* CMmsHeaders::NewL( TInt16 aMmsVersion )
    {
    CMmsHeaders* self = new ( ELeave ) CMmsHeaders;
    
    CleanupStack::PushL( self );
    // default value for legacy applications that are not aware of mms version
    self->ConstructL( aMmsVersion );
    CleanupStack::Pop( self );

    return self;
    }

// Two-phased constructor.
EXPORT_C CMmsHeaders* CMmsHeaders::NewL()
    {
    CMmsHeaders* self = new ( ELeave ) CMmsHeaders;
    
    CleanupStack::PushL( self );
    // default value for legacy applications that are not aware of mms version
    self->ConstructL( KMmsDefaultVersion );
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
CMmsHeaders::~CMmsHeaders()
    {
    iPreviouslySentArray.ResetAndDestroy();
    iDeleteResultArray.ResetAndDestroy();
    delete iOperatorResponseText;
    delete iReplyChargingId;
    delete iToArray;
    delete iCcArray;
    delete iBccArray;
    delete iSender;
    delete iContentLocation;
    delete iSubject;
    delete iTID;
    delete iMessageId;
    delete iRootContentID;
    delete iElementDescriptor;
    delete iMmBoxMessageHeaders;
    delete iMmBoxViewHeaders;
    delete iExtendedNotificationText;
    delete iApplicationId;
    delete iReplyToApplicationId;
    delete iApplicationInfo;
    delete iRecommendedRetrievalModeText;
    delete iReplaceCancelId;
    }

// ---------------------------------------------------------
// CMmsHeaders::RestoreL
//
// ---------------------------------------------------------
//

EXPORT_C void CMmsHeaders::RestoreL(
    CMsvStore& aStore )
    {

    RMsvReadStream stream;
    Reset( NULL ); // all old pointers are deleted here
    if ( aStore.IsPresentL( KUidMmsHeaderStream ) )
        {
        stream.OpenLC( aStore, KUidMmsHeaderStream ); // pushes 'stream' to the stack
        InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    // restore MMBox header streams if present
    if ( aStore.IsPresentL( KUidMMsElementDescriptorStream ) )
        {
        stream.OpenLC( aStore, KUidMMsElementDescriptorStream ); // pushes 'stream' to the stack
        iElementDescriptor = new( ELeave )CMmsElementDescriptor;
        iElementDescriptor->InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    if ( aStore.IsPresentL( KUidMMsMMBoxMessageHeaderStream ) )
        {
        stream.OpenLC( aStore, KUidMMsMMBoxMessageHeaderStream ); // pushes 'stream' to the stack
        iMmBoxMessageHeaders = CMmsMMBoxMessageHeaders::NewL();
        iMmBoxMessageHeaders->InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    if ( aStore.IsPresentL( KUidMMsMMBoxViewHeadersStream ) )
        {
        stream.OpenLC( aStore, KUidMMsMMBoxViewHeadersStream ); // pushes 'stream' to the stack
        iMmBoxViewHeaders = new( ELeave )CMmsMMBoxViewHeaders;
        iMmBoxViewHeaders->InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }

    // Extended notification also restored here
    TInt length = 0; // string length

    // Completeness indicator is not saved or restored.
    // If we have stored the text, it normally indicates that the message is not complete
    if ( aStore.IsPresentL( KUidMMsExtendedNotificationStream ) )
        {
        stream.OpenLC( aStore, KUidMMsExtendedNotificationStream ); // pushes 'stream' to the stack
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iExtendedNotificationText = HBufC::NewL( stream, length );
            }
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    
    if ( aStore.IsPresentL( KUidMmsApplicationInfoStream ) )
        {
        stream.OpenLC( aStore, KUidMmsApplicationInfoStream ); // pushes 'stream' to the stack
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iApplicationId = HBufC::NewL( stream, length );
            }
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iReplyToApplicationId = HBufC::NewL( stream, length );
            }
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iApplicationInfo = HBufC8::NewL( stream, length );
            }
        
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
        
    if ( aStore.IsPresentL( KUidMmsReserved ) )
        {
        stream.OpenLC( aStore, KUidMmsReserved ); // pushes 'stream' to the stack
        iRecommendedRetrievalMode = stream.ReadInt32L();
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iRecommendedRetrievalModeText = HBufC16::NewL( stream, length );
            }
        length = stream.ReadInt32L();
        if ( length > 0 )
            {
            iReplaceCancelId = HBufC8::NewL( stream, length );
            }
        iCancelStatus = stream.ReadInt32L();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
        
    } 

// ---------------------------------------------------------
// CMmsEntry::StoreL
//
// ---------------------------------------------------------
//

EXPORT_C void CMmsHeaders::StoreL(
    CMsvStore& aStore )
    {

    // Check if message root is correctly set
    // We have an edit store in our hands, so we can access the attachment manager
    
    if ( iStart != 0 )
        {
        MMsvAttachmentManager& attaMan = aStore.AttachmentManagerL();
        CMsvAttachment* attaInfo = NULL;
        TRAPD( error, attaInfo = attaMan.GetAttachmentInfoL( iStart ) );
        if ( error == KErrNotFound )
            {
            // The attachment is not present, clear the root indicator
            iStart = 0;
            delete iRootContentID;
            iRootContentID = 0;
            }
        else if ( error == KErrNone )
            {
            // got the attachment, check content id
            CleanupStack::PushL( attaInfo );
            CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
            CleanupStack::PushL( mimeHeaders );
            mimeHeaders->RestoreL( *attaInfo );
            if ( RootContentId().Compare( mimeHeaders->ContentId() ) != 0 )
                {
                if ( mimeHeaders->ContentId().Length() > 0 )
                    {
                    // mime headers win
                    SetRootContentIdL( mimeHeaders->ContentId() );
                    }
                else if ( RootContentId().Length() > 0 )
                    {
                    // if mime headers not set, but root set, root wins
                    mimeHeaders->SetContentIdL( RootContentId() );
                    mimeHeaders->StoreL( *attaInfo );
                    }
                else
                    {
                    // keep LINT happy
                    }
                }
            CleanupStack::PopAndDestroy( mimeHeaders );
            CleanupStack::PopAndDestroy( attaInfo );
            }
        // If error == KErrNoMemory there is nothing we can do
        // We try to continue anyway, maybe we still can save our message
        else
            {
            // Keep LINT happy.
            }
        }
    
    // caller must commit the store
    RMsvWriteStream stream;
    stream.AssignLC( aStore, KUidMmsHeaderStream ); // pushes 'stream' to the stack
    ExternalizeL( stream );
    stream.CommitL();
    stream.Close();
    CleanupStack::PopAndDestroy( &stream ); // close stream

    // store element descriptor and  mmbox headers, if they are present
    if ( iElementDescriptor )
        {
        stream.AssignLC( aStore, KUidMMsElementDescriptorStream ); // pushes 'stream' to the stack
        iElementDescriptor->ExternalizeL( stream );
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    if ( iMmBoxMessageHeaders )
        {
        stream.AssignLC( aStore, KUidMMsMMBoxMessageHeaderStream ); // pushes 'stream' to the stack
        iMmBoxMessageHeaders->ExternalizeL( stream );
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
    if ( iMmBoxViewHeaders )
        {
        stream.AssignLC( aStore, KUidMMsMMBoxViewHeadersStream ); // pushes 'stream' to the stack
        iMmBoxViewHeaders->ExternalizeL( stream );
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }

    TInt length = 0; // 
    // store extended notification if present
    if ( iExtendedNotificationText )
        {
        stream.AssignLC( aStore, KUidMMsExtendedNotificationStream ); // pushes 'stream' to the stack
        length = ExtendedNotification().Length();
        stream.WriteInt32L( length );
        if ( length > 0 )
            {
            stream << ExtendedNotification();
            }
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }
        
    if ( iApplicationId ||
        iReplyToApplicationId ||
        iApplicationInfo )
        {
        stream.AssignLC( aStore, KUidMmsApplicationInfoStream ); // pushes 'stream' to the stack
        length = ApplicId().Length();
        stream.WriteInt32L( length );
        if ( length > 0 && iApplicationId )
            {
            stream << iApplicationId->Des();
            }
        
        length = ReplyApplicId().Length();
        stream.WriteInt32L( length );
        if ( length > 0 && iReplyToApplicationId )
            {
            stream << iReplyToApplicationId->Des();
            }
        
        length = AuxApplicInfo().Length();
        stream.WriteInt32L( length );
        if ( length > 0 && iApplicationInfo )
            {
            stream << iApplicationInfo->Des();
            }
        
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }

    // reserved transaction info stream
    if ( iRecommendedRetrievalMode != 0 ||
        iRecommendedRetrievalModeText ||
        iReplaceCancelId ||
        iCancelStatus != 0 )
        {
        stream.AssignLC( aStore, KUidMmsReserved ); // pushes 'stream' to the stack
        
        stream.WriteInt32L( iRecommendedRetrievalMode );

        length = RecommendedRetrievalModeText().Length();
        stream.WriteInt32L( length );
        if ( length > 0 && iRecommendedRetrievalModeText )
            {
            stream << iRecommendedRetrievalModeText->Des();
            }
            
        length = ReplaceCancelId().Length();
        stream.WriteInt32L( length );
        if ( length > 0 && iReplaceCancelId )
            {
            stream << iReplaceCancelId->Des();
            }
        
        stream.WriteInt32L( iCancelStatus );
        
        stream.CommitL();
        stream.Close();
        CleanupStack::PopAndDestroy( &stream ); // close stream
        }

    }

// ---------------------------------------------------------
// CMmsHeaders::TypedAddresseeList
//
// ---------------------------------------------------------
//
EXPORT_C const CDesCArray& CMmsHeaders::TypedAddresseeList( 
    TMmsRecipients aType ) 
    {

    TInt type = aType;

    if (type == EMmsTo)
        {
        return *iToArray;
        }
    else if ( type == EMmsCc )
        {
        return *iCcArray;
        }
    else 
        {
        return *iBccArray;
        }
    }
   
// ---------------------------------------------------------
// CMmsHeaders::AddTypedAddresseeL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::AddTypedAddresseeL( 
    const TDesC&  aRealAddress, 
    TMmsRecipients aType  )
    {
    
    TInt type = aType;

    if ( type == EMmsTo )
        {
        iToArray->AppendL( aRealAddress );
        }
    if ( type == EMmsCc )
        {
        iCcArray->AppendL( aRealAddress );
        }
    if ( type == EMmsBcc ) 
        {
        iBccArray->AppendL( aRealAddress );
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::RemoveAddresseeL
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMmsHeaders::RemoveAddressee( const TDesC& aRealAddress )
    {

    if ( RemoveAddressee( *iToArray, aRealAddress) )
        {
        return ETrue;
        }
    if ( RemoveAddressee( *iCcArray, aRealAddress) )
        {
        return ETrue;
        }
    if ( RemoveAddressee( *iBccArray, aRealAddress) )
        {
        return ETrue;
        }
      
    return EFalse;

    }

// ---------------------------------------------------------
// CMmsHeaders::SetSubjectL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetSubjectL( const TDesC& aSubject )
    {
    HBufC* newAttrib = aSubject.AllocL();
    delete iSubject;
    iSubject = newAttrib;
    }

// ---------------------------------------------------------
// CMmsHeaders::Subject
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMmsHeaders::Subject() const
    {
    return iSubject ? TPtrC( *iSubject ) : TPtrC(); 
    }

// ---------------------------------------------------------
// CMmsHeaders::SetSenderL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetSenderL( const TDesC& aAlias )
    {
    HBufC* newAttrib = aAlias.AllocL();
    delete iSender;
    iSender = newAttrib;
    }

// ---------------------------------------------------------
// CMmsHeaders::Sender
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMmsHeaders::Sender() const
    {
    return iSender ? TPtrC( *iSender ) : TPtrC(); 
    }

// ---------------------------------------------------------
// CMmsHeaders::SetExpiryInterval
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetExpiryInterval( 
    TInt aInterval )
    {
    iExpiry = aInterval;
    iExpiryAbs = EFalse;
    }

// ---------------------------------------------------------
// CMmsHeaders::ExpiryInterval
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMmsHeaders::ExpiryInterval()
    {
    return iExpiryAbs ? 0 : I64LOW( iExpiry );
    }


// ---------------------------------------------------------
// CMmsHeaders::SetExpiryDate
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetExpiryDate( TInt64 aDate )
    {
    iExpiry = aDate;
    iExpiryAbs = ETrue;
    }

// ---------------------------------------------------------
// CMmsHeaders::ExpiryDate
//
// ---------------------------------------------------------
// 
EXPORT_C TInt64 CMmsHeaders::ExpiryDate()
    {
    return iExpiryAbs ? iExpiry : TInt64( 0 );
    }

// ---------------------------------------------------------
// CMmsHeaders::SetDeliveryTimeInterval
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetDeliveryTimeInterval( TInt aInterval )
    {
    iDelivery = aInterval;
    iDeliveryAbs = EFalse;
    }

// ---------------------------------------------------------
// CMmsHeaders::DeliveryTimeInterval
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMmsHeaders::DeliveryTimeInterval()
    {
    return iDeliveryAbs ? 0 : I64LOW( iDelivery );
    }

// ---------------------------------------------------------
// CMmsHeaders::SetDeliveryDate
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetDeliveryDate( TInt64 aDate )
    {
    iDelivery = aDate;
    iDeliveryAbs = ETrue;
    }

// ---------------------------------------------------------
// CMmsHeaders::DeliveryDate
//
// ---------------------------------------------------------
//
EXPORT_C TInt64 CMmsHeaders::DeliveryDate()
    {
    return iDeliveryAbs ? iDelivery : TInt64( 0 );
    }

// ---------------------------------------------------------
// CMmsHeaders::SetTidL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetTidL( const TDesC8& aCid )
    {
    // If New TID has length zero, TID is cleared
    if ( aCid.Length() == 0 )
        {
        delete iTID;
        iTID = NULL;
        }
    else
        {
        HBufC8 * newAttrib = aCid.AllocL();
        if ( newAttrib->Length() > KMaxHeaderStringLength )
            {
            newAttrib->Des().SetLength( KMaxHeaderStringLength );
            }
        delete iTID;
        iTID = newAttrib;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::ContentLocation
//
// ---------------------------------------------------------
//

EXPORT_C TPtrC8 CMmsHeaders::ContentLocation() const
    {
    return iContentLocation ? TPtrC8( *iContentLocation ) : TPtrC8();
    }
     
// ---------------------------------------------------------
// CMmsHeaders::SetContentLocationL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetContentLocationL( const TDesC8& aURL )
    {
    HBufC8 * newAttrib = aURL.AllocL();
    if ( newAttrib->Length() > KMaxHeaderStringLength )
        {
        newAttrib->Des().SetLength( KMaxHeaderStringLength );
        }
    delete iContentLocation;
    iContentLocation = newAttrib;
    }
  
// ---------------------------------------------------------
// CMmsHeaders::Tid
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::Tid() const
    {
    return iTID ? TPtrC8( *iTID ) : TPtrC8();
    } 

// ---------------------------------------------------------
// CMmsHeaders::InternalizeL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::InternalizeL( RMsvReadStream& aStream )
    {

    aStream >> iVersion;
    aStream >> iMsgType;

    iSubject = HBufC::NewL( aStream, KMaxHeaderStringLength );
    iTID = HBufC8::NewL( aStream, KMaxHeaderStringLength );
    aStream >> iDate;

    iSender = HBufC::NewL( aStream, KMaxHeaderStringLength );
    aStream >> iMsgClass;

    aStream >> iExpiry;
    iExpiryAbs = aStream.ReadInt8L();
    aStream >> iDelivery;
    iDeliveryAbs = aStream.ReadInt8L();

    aStream >> iPriority;
    aStream >> iSenderVisi;

    aStream >> iDelivReport;
    aStream >> iReadReply;

    iStart = aStream.ReadInt32L();
    iContentLocation = HBufC8::NewL( aStream, KMaxHeaderStringLength );

    aStream >> iMessageSize;
    aStream >> iReportAllowed;
    aStream >> iResponseStatus;

    iMessageId = HBufC8::NewL( aStream, KMaxHeaderStringLength );
    aStream >> iStatus;

    aStream >> iMaxImageWidth;
    aStream >> iMaxImageHeight;

    // Internalize fields which may have multiple values.
    InternalizeArrayL( *iToArray, aStream );
    InternalizeArrayL( *iCcArray, aStream );
    InternalizeArrayL( *iBccArray, aStream );

    TInt32 length = 0;
    length = aStream.ReadInt32L();
    
    if ( length > 0 )
        {
        iRootContentID = HBufC8::NewL( aStream, length );
        }

    length = aStream.ReadInt32L();
    if ( length > 0 )
        {
        iOperatorResponseText = HBufC::NewL( aStream, length );
        }
  
    TInt i;
    aStream >> iReadStatus;
    aStream >> iReplyCharging;
    aStream >> iReplyChargingDeadline;
    iReplyChargingAbs = aStream.ReadInt8L();
    aStream >> iReplyChargingSize;
    aStream >> length;
    if ( length > 0 )
        {
        iReplyChargingId = HBufC8::NewL( aStream, length );
        }

    aStream >> length;
          
    for ( i = 0; i < length ; i++ )
        {
        CMmsSendingChain* item = new( ELeave )CMmsSendingChain;
        CleanupStack::PushL( item );
        item->InternalizeL( aStream );
        User::LeaveIfError( iPreviouslySentArray.InsertInOrder( item, iLinearOrder ));
        CleanupStack::Pop( item ); // item is in member array now
        }

    iDistributionIndicator = aStream.ReadInt32L();

    iRelatedEntry = aStream.ReadInt32L();
    iMultipartType = aStream.ReadInt32L();
    
    TInt64 tempTime;
    aStream >> tempTime;
    iReceivingTime = TTime( tempTime );
     
    iContentClass = aStream.ReadInt32L();
    iDrmContent = aStream.ReadInt32L();
    iAdaptationAllowed = aStream.ReadInt32L();
    }

// ---------------------------------------------------------
// CMmsHeaders::ExternalizeL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::ExternalizeL( RMsvWriteStream& aStream ) const
    {

    // This MUST be the 1st item written into the stream    
    aStream << iVersion;
    // This must be 2nd. 
    aStream << iMsgType;

    aStream << LimitStringSize( Subject(), KMaxHeaderStringLength );
    aStream << LimitStringSize( Tid(), KMaxHeaderStringLength );
    aStream << iDate;

    aStream << LimitStringSize( Sender(), KMaxHeaderStringLength );
    aStream << iMsgClass;

    aStream << iExpiry;
    aStream.WriteInt8L( iExpiryAbs );
    aStream << iDelivery;
    aStream.WriteInt8L( iDeliveryAbs );

    aStream << iPriority;
    aStream << iSenderVisi;

    aStream << iDelivReport;
    aStream << iReadReply;

    aStream.WriteInt32L( iStart );
    aStream << LimitStringSize( ContentLocation(), KMaxHeaderStringLength );

    aStream << iMessageSize;
    aStream << iReportAllowed;
    aStream << iResponseStatus;

    aStream << LimitStringSize( MessageId(), KMaxHeaderStringLength );
    aStream << iStatus;

    aStream << iMaxImageWidth;
    aStream << iMaxImageHeight;

    // Externalize fields which may have multiple values.
    ExternalizeArrayL( *iToArray, aStream );
    ExternalizeArrayL( *iCcArray, aStream );
    ExternalizeArrayL( *iBccArray, aStream );

    TInt32 length;
    length = RootContentId().Length();
    aStream << length;
    if ( length > 0 )
        {
        aStream << RootContentId();
        }
    length = ResponseText().Length();
    aStream << length;
    if ( length > 0 )
        {
        aStream << ResponseText();
        }

    TInt i;
    aStream << iReadStatus;
    aStream << iReplyCharging;
    aStream << iReplyChargingDeadline;
    aStream.WriteInt8L( iReplyChargingAbs );
    aStream << iReplyChargingSize;
    length = ReplyChargingId().Length();
    aStream << length;
    if ( length > 0 )
        {
        aStream << ReplyChargingId();
        }

    length = iPreviouslySentArray.Count();
    aStream << length;
    for ( i = 0; i < iPreviouslySentArray.Count(); i++ )
        {
        iPreviouslySentArray[ i ]->ExternalizeL( aStream );
        }

    aStream.WriteInt32L( iDistributionIndicator );
    aStream.WriteInt32L( iRelatedEntry );
    aStream.WriteInt32L( iMultipartType );
    
    aStream << iReceivingTime.Int64();
    
    aStream.WriteInt32L( iContentClass );
    aStream.WriteInt32L( iDrmContent );
    aStream.WriteInt32L( iAdaptationAllowed );

    }

// ---------------------------------------------------------
// CMmsHeaders::ElementDescriptorL
//
// ---------------------------------------------------------
//
EXPORT_C CMmsElementDescriptor& CMmsHeaders::ElementDescriptorL()
    {
    if ( !iElementDescriptor )
        {
        iElementDescriptor = new( ELeave )CMmsElementDescriptor;
        }
    return *iElementDescriptor;
    }

// ---------------------------------------------------------
// CMmsHeaders::MMBoxMessageHeadersL
//
// ---------------------------------------------------------
//
EXPORT_C CMmsMMBoxMessageHeaders& CMmsHeaders::MMBoxMessageHeadersL()
    {
    if ( !iMmBoxMessageHeaders )
        {
        iMmBoxMessageHeaders = CMmsMMBoxMessageHeaders::NewL();
        }
    return *iMmBoxMessageHeaders;
    }

// ---------------------------------------------------------
// CMmsHeaders::MMBoxViewHeadersL
//
// ---------------------------------------------------------
//
EXPORT_C CMmsMMBoxViewHeaders& CMmsHeaders::MMBoxViewHeadersL()
    {
    if ( !iMmBoxViewHeaders )
        {
        iMmBoxViewHeaders = new( ELeave )CMmsMMBoxViewHeaders;
        }
    return *iMmBoxViewHeaders;
    }

// ---------------------------------------------------------
// CMmsHeaders::SetExtendedNotificationL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetExtendedNotificationL( const TDesC& aText )
    {
    HBufC* newAttrib = NULL;
    if ( aText.Length() > 0 )
        {
        newAttrib = aText.AllocL();
        }
    delete iExtendedNotificationText;
    iExtendedNotificationText = newAttrib;
    }

// ---------------------------------------------------------
// CMmsHeaders::ExtendedNotification
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMmsHeaders::ExtendedNotification() const
    {
    return iExtendedNotificationText ? TPtrC( *iExtendedNotificationText ) : TPtrC();
    }

// ---------------------------------------------------------
// CMmsHeaders::RemoveAddressee
//
// ---------------------------------------------------------
//
TBool CMmsHeaders::RemoveAddressee(
    CDesCArray& aList,
    const TDesC& aAddress )
    {

    TInt count = aList.Count();
    TInt i;

    // We must compare only pure addresses.
    // The list may contain addresses that include aliases.    
    for ( i = count - 1; i >= 0; i-- )
        {
        if ( TMmsGenUtils::PureAddress( aAddress ).CompareF(
            TMmsGenUtils::PureAddress( aList.MdcaPoint( i ) ) ) == 0 )
            {
            aList.Delete( i );
            aList.Compress();
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMmsHeaders::Reset
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::Reset( CMmsSettings* aSettings )
    {

    iMsgType = 0;
    
    delete iTID;
    iTID = NULL;

    // Always restore the default version - even if not reading other settings
    iVersion = iMmsDefaultVersion;  
    iDate = 0;    

    delete iSender;
    iSender = NULL;

    iToArray->Reset();
    iCcArray->Reset(); 
    iBccArray->Reset(); 

    delete iSubject;   
    iSubject = NULL;

    iMsgClass = 0;  

    iExpiry = 0;    
    iExpiryAbs = 0; 
    iDelivery = 0;  
    iDeliveryAbs = 0;
    iPriority = 0;  
    iSenderVisi = 0;
    iDelivReport = 0;
    iReadReply = 0;
    iStart = KMsvNullIndexEntryId; 

    delete iContentLocation; 
    iContentLocation = NULL;

    iMessageSize = 0;
    iReportAllowed = 0;  
    iResponseStatus = 0; 

    delete iMessageId; 
    iMessageId = NULL;

    iStatus = 0;

    iMaxImageHeight = 0;
    iMaxImageWidth = 0;

    delete iOperatorResponseText;
    iOperatorResponseText = NULL;

    delete iRootContentID;
    iRootContentID = NULL;

    iReadStatus = 0;
    iReplyCharging = 0;
    iReplyChargingDeadline = 0;
    iReplyChargingAbs = EFalse;
    delete iReplyChargingId;
    iReplyChargingId = NULL;
    iReplyChargingSize = 0;
    iPreviouslySentArray.ResetAndDestroy();
    iDeleteResultArray.ResetAndDestroy();

    iDistributionIndicator = 0;
    iRelatedEntry = 0;
    iMultipartType = 0;
    iReceivingTime = 0;
    // encapsulation 1.3 headers
    iContentClass = 0;
    iDrmContent = 0;
    iAdaptationAllowed = 0;

    delete iElementDescriptor;
    iElementDescriptor = NULL;
    delete iMmBoxMessageHeaders;
    iMmBoxMessageHeaders = NULL;
    delete iMmBoxViewHeaders;
    iMmBoxViewHeaders = NULL;

    delete iExtendedNotificationText;
    iExtendedNotificationText = NULL;

    iExtensionStatus = 0; // set unknown status
    
    delete iApplicationId;
    iApplicationId = NULL;
    delete iReplyToApplicationId;
    iReplyToApplicationId = NULL;
    delete iApplicationInfo;
    iApplicationInfo = NULL;
    
    iRecommendedRetrievalMode = 0;
    delete iRecommendedRetrievalModeText;
    iRecommendedRetrievalModeText = NULL;
    delete iReplaceCancelId; 
    iReplaceCancelId = NULL; 
    iCancelStatus = 0;
    
    SetSettings( aSettings );
    
    }

// ---------------------------------------------------------
// CMmsHeaders::Size
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMmsHeaders::Size() const
    {

    TInt size = sizeof( iMsgType );

    size+= Tid().Size();

    size+= sizeof( iVersion );

    size+= sizeof( iDate );

    TInt i;

    TInt nbr = iToArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size += iToArray->MdcaPoint(i).Size();
        }

    nbr = iCcArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size+=iCcArray->MdcaPoint(i).Size();
        }

    nbr = iBccArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size+=iBccArray->MdcaPoint(i).Size();
        }

    size+= Sender().Size();

    size+= Subject().Size();
    
    size+= sizeof( iMsgClass );

    size+= sizeof( iExpiry );

    size+= sizeof( iExpiryAbs );

    size+= sizeof( iDelivery );

    size+= sizeof( iDeliveryAbs );

    size+= sizeof( iPriority );

    size+= sizeof( iSenderVisi );

    size+= sizeof( iDelivReport );

    size+= sizeof( iReadReply );

    size+= sizeof( iStart );

    size+= ContentLocation().Size();

    size+= sizeof( iMessageSize );

    size+= sizeof( iReportAllowed );
    
    size+= sizeof( iResponseStatus );
    
    size+= MessageId().Size();

    size+= sizeof( iStatus );

    size+= sizeof( iMaxImageWidth );

    size+= sizeof( iMaxImageHeight );

    // operator response text
    size += sizeof( TInt32 );
    size += ResponseText().Length();

    // length of root content 
    size+= sizeof( TInt32 );
    size += RootContentId().Length();

    if ( iVersion > KMmsVersion1 )
        {
        size+= sizeof( iReadStatus );

        size+= sizeof( iReplyCharging );

        size+= sizeof( iReplyChargingDeadline );

        size+= sizeof( iReplyChargingAbs );

        size+= sizeof( iReplyChargingSize );

        // length of reply charging id
        size+= sizeof( TInt32 );
        size+= ReplyChargingId().Length();

        // number of items in array
        size+= sizeof( TInt32 );
        for ( i = 0; i < iPreviouslySentArray.Count(); i++ )
            {
            size += iPreviouslySentArray[ i ]->Size();
            }
        }

    size += sizeof( iDistributionIndicator );
    size += sizeof( iRelatedEntry );
    size += sizeof( iMultipartType );
    size += sizeof( iReceivingTime );
    
    // iContentClass, iDrmContent & iAdaptationAllowed
    size += sizeof( TInt32 );
    size += sizeof( TInt32 );
    size += sizeof( TInt32 );

    // add element descriptor and mmbox header sizes
    if ( iElementDescriptor )
        {
        size += iElementDescriptor->Size();
        }

    if ( iMmBoxMessageHeaders )
        {
        size += iMmBoxMessageHeaders->Size();
        }

    if ( iMmBoxViewHeaders )
        {
        size += iMmBoxViewHeaders->Size();
        }

    // add extended notification size
    if ( ExtendedNotification().Length() > 0 )
        {
        // size of lenghth
        size+= sizeof( TInt32 );
        size += ExtendedNotification().Size();
        }

    if ( iApplicationId ||
        iReplyToApplicationId ||
        iApplicationInfo )
        {
        // size of lenghth
        size+= sizeof( TInt32 );
        size += ApplicId().Size();
        size+= sizeof( TInt32 );
        size += ReplyApplicId().Size();
        size+= sizeof( TInt32 );
        size += AuxApplicInfo().Size();
        }
        
    if ( iRecommendedRetrievalMode > 0 ||
        iRecommendedRetrievalModeText ||
        iReplaceCancelId ||
        iCancelStatus )
        {
        size+= sizeof( TInt32 );
        size+= sizeof( TInt32 );
        size += RecommendedRetrievalModeText().Size();
        size+= sizeof( TInt32 );
        size += ReplaceCancelId().Size();
        size+= sizeof( TInt32 );
        }

    return size;

    }

// ---------------------------------------------------------
// CMmsHeaders::SetMessageIdL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetMessageIdL( const TDesC8& aId ) 
    {
    HBufC8 * newAttrib = aId.AllocL();
    if ( newAttrib->Length() > KMaxHeaderStringLength )
        {
        newAttrib->Des().SetLength( KMaxHeaderStringLength );
        }
    delete iMessageId;
    iMessageId = newAttrib;
    }

// ---------------------------------------------------------
// CMmsHeaders::MessageId
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::MessageId() const
    {
    return iMessageId ? TPtrC8( *iMessageId ) : TPtrC8();
    }

// ---------------------------------------------------------
// CMmsHeaders::CopyL
//
// ---------------------------------------------------------
//

EXPORT_C CMmsHeaders* CMmsHeaders::CopyL(
    TMsvPartList aParts,
    TBool aReply )
    {
    // legacy function, opens new file system handle
    // DEPRECATED
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    CMmsHeaders* mms = CopyL( aParts, aReply, fs );
    
    CleanupStack::PopAndDestroy( &fs ); //fs
    
    return mms; // caller takes posession

    }

// ---------------------------------------------------------
// CMmsHeaders::CopyL
//
// ---------------------------------------------------------
//

EXPORT_C CMmsHeaders* CMmsHeaders::CopyL(
    TMsvPartList aParts,
    TBool aReply,
    RFs& aFs )
    {

    CMmsHeaders* mms = CMmsHeaders::NewL();
    CleanupStack::PushL( mms );
    HBufC* buffer = NULL;

    // Copy those values as specified by aParts.

    if ( aReply )
        { 

        // Copy original sender as recipient
        if ( this->Sender().Length() > 0 )
            {
            buffer = AliasedAddressL( this->Sender(), aFs  );
            CleanupStack::PushL( buffer );
            mms->AddTypedAddresseeL( buffer->Des(), EMmsTo );
            CleanupStack::PopAndDestroy( buffer );
            buffer = NULL;
            }

        if ( aParts & KMsvMessagePartRecipient )
            {
            // Copy all recipients for reply-to-all message.
            const CDesCArray& array1 = this->ToRecipients();
            mms->CopyAddresseeListL( array1, EMmsTo, this->Sender(), aFs );

            const CDesCArray& array2 = this->BccRecipients();
            mms->CopyAddresseeListL( array2, EMmsBcc, this->Sender(), aFs );

            const CDesCArray& array3 = this->CcRecipients();
            mms->CopyAddresseeListL( array3, EMmsCc, this->Sender(), aFs );
            }
        }
    else
        {
        // forward
        if ( aParts & KMsvMessagePartRecipient )
            {
            // Copy original Recipient to recipient
            const CDesCArray& array1 = this->ToRecipients();
            mms->CopyAddresseeListL( array1, EMmsTo, TPtrC(), aFs );

            const CDesCArray& array2 = this->BccRecipients();
            mms->CopyAddresseeListL( array2, EMmsBcc, TPtrC(), aFs );

            const CDesCArray& array3 = this->CcRecipients();
            mms->CopyAddresseeListL( array3, EMmsCc, TPtrC(), aFs );
            }
        if ( aParts & KMsvMessagePartOriginator )
            {
            // Copy original originator to originator
            mms->SetSenderL( Sender() );
            }

        // message class and priority must always be copied
        // when forwarding a message
        mms->SetMessageClass( MessageClass() );
        mms->SetMessagePriority( MessagePriority() );

        // When forwarding a message, root part pointer must be
        // copied, too. However, root is referred to by TMsvId,
        // and when the message is copied, new id is generated.
        // The root pointer copying must be done by someone else.

        }
    if ( aParts & KMsvMessagePartDescription )
        {
        mms->SetSubjectL( Subject() );
        }

    CleanupStack::Pop( mms );
    return mms;
    }

// ---------------------------------------------------------
// CMmsHeaders::SetSettings
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetSettings(
    CMmsSettings* aSettings )
    {
    if( !aSettings )
        {
        return;
        }
    // Make sure the version number follows the default setting    
    iMmsDefaultVersion = aSettings->MmsVersion();
    iVersion = iMmsDefaultVersion;
    SetExpiryInterval( aSettings->ExpiryInterval() );
    SetSenderVisibility( aSettings->SenderVisibility() ); 
    SetDeliveryReport( aSettings->DeliveryReportWanted() );
    SetReadReply( aSettings->ReadReplyReportWanted() );
    // Don't override priority if it has been set already.
    // It gets set when message is forwarded.
    if ( iPriority == 0 )
        {
        SetMessagePriority( aSettings->MessagePriority() );
        }
    // Don't override message class if it has been set already.
    // It gets set when message is forwarded.
    if ( iMsgClass == 0 )
        {
        SetMessageClass( aSettings->MessageClass() );
        }
    SetMaximumImage( aSettings->ImageWidth(), aSettings->ImageHeight());
    }


// ---------------------------------------------------------
// CMmsHeaders::SetMaximumImage
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetMaximumImage( 
    TInt aWidth, TInt aHeight )
    {
    iMaxImageHeight = aHeight;
    iMaxImageWidth = aWidth;
    }

// ---------------------------------------------------------
// CMmsHeaders::GetMaximumImage
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::GetMaximumImage( 
    TInt& aWidth, TInt& aHeight ) const
    {
    aHeight = iMaxImageHeight;
    aWidth = iMaxImageWidth;
    }

// ---------------------------------------------------------
// CMmsHeaders::SetRootContentIdL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetRootContentIdL( const TDesC8& aId ) 
    {
    delete iRootContentID;
    iRootContentID = NULL;
    if ( aId.Length() > 0 )
        {
        iRootContentID = aId.AllocL();
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::RootContentId
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::RootContentId() const
    {
    return iRootContentID ? TPtrC8( *iRootContentID ) : TPtrC8();
    }


// ---------------------------------------------------------
// CMmsHeaders::SetReplyChargingIdL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetReplyChargingIdL( const TDesC8& aId ) 
    {
    delete iReplyChargingId;
    iReplyChargingId = NULL;
    iReplyChargingId = aId.AllocL();
    }


// ---------------------------------------------------------
// CMmsHeaders::ReplyChargingId
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::ReplyChargingId() const
    {
    return iReplyChargingId ? TPtrC8( *iReplyChargingId ) : TPtrC8();
    }

// ---------------------------------------------------------
// CMmsHeaders::SetReplyChargingInterval
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetReplyChargingInterval( 
    TInt aInterval )
    {
    iReplyChargingDeadline = aInterval;
    iReplyChargingAbs = EFalse;
    }

// ---------------------------------------------------------
// CMmsHeaders::ReplyChargingInterval
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMmsHeaders::ReplyChargingInterval()
    {
    return iReplyChargingAbs ? 0 : I64LOW( iReplyChargingDeadline );
    }


// ---------------------------------------------------------
// CMmsHeaders::SetReplyChargingDate
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetReplyChargingDate( TInt64 aDate )
    {
    iReplyChargingDeadline = aDate;
    iReplyChargingAbs = ETrue;
    }

// ---------------------------------------------------------
// CMmsHeaders::ReplyChargingDate
//
// ---------------------------------------------------------
// 
EXPORT_C TInt64 CMmsHeaders::ReplyChargingDate()
    {
    return iReplyChargingAbs ? iReplyChargingDeadline : TInt64( 0 );
    }

// ---------------------------------------------------------
// CMmsHeaders::InsertPreviouslySentByL
//
// ---------------------------------------------------------
// 
EXPORT_C void CMmsHeaders::InsertPreviouslySentByL( TInt aOrder, const TDesC& aSender )
    {
    TInt error = KErrNone;
    CMmsSendingChain* item = new (ELeave)CMmsSendingChain;
    CleanupStack::PushL( item );
    item->SetOrder( aOrder );
    item->SetSenderL( aSender );

    TInt index = KErrNotFound;
    index = iPreviouslySentArray.FindInOrder( item, iLinearOrder );
    CleanupStack::Pop( item );
    if ( index == KErrNotFound )
        {
        error = iPreviouslySentArray.InsertInOrder( item, iLinearOrder );
        if ( error != KErrNone )
            {
            delete item;
            }
        }
    else
        {
        delete item;
        // modify existing entry
        // order is already set, as we use that as our match
        // we just put in new sender.
        // Index is valid because we just set it to the index where match was found
        iPreviouslySentArray[index]->SetSenderL( aSender );
        }
    item = NULL; // this was either deleted or inserted into our array.

    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory

    }

// ---------------------------------------------------------
// CMmsHeaders::InsertPreviouslySentDateL
//
// ---------------------------------------------------------
// 
EXPORT_C void CMmsHeaders::InsertPreviouslySentDateL( TInt aOrder, TInt64 aDate )
    {

    TInt error = KErrNone;
    CMmsSendingChain* item = new (ELeave)CMmsSendingChain;
    CleanupStack::PushL( item );
    item->SetOrder( aOrder );
    item->SetDate( aDate );

    TInt index = KErrNotFound;
    index = iPreviouslySentArray.FindInOrder( item, iLinearOrder );
    CleanupStack::Pop( item );
    if ( index == KErrNotFound )
        {
        error = iPreviouslySentArray.InsertInOrder( item, iLinearOrder );
        if ( error != KErrNone )
            {
            delete item;
            }
        }
    else
        {
        delete item;
        // modify existing entry
        // order is already set, as we use that as our match
        // we just put in new sender.
        // Index is valid because we just set it to the index where match was found
        iPreviouslySentArray[index]->SetDate( aDate );
        }
    item = NULL; // this was either deleted or inserted into our array.

    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory

    }

// ---------------------------------------------------------
// CMmsHeaders::AppendPreviouslySentItemL
//
// ---------------------------------------------------------
// 
EXPORT_C void CMmsHeaders::AppendPreviouslySentItemL( TInt64 aDate, const TDesC& aSender )
    {
    TInt error = KErrNone;
    CMmsSendingChain* item = new (ELeave)CMmsSendingChain;
    CleanupStack::PushL( item );
    item->SetDate( aDate );
    item->SetSenderL( aSender );
    TInt count = iPreviouslySentArray.Count();
    // we are creating a new item that must go to the end of the array
    if ( count == 0 )
        {
        // First item in array
        item->SetOrder( 1 );
        }
    else
        {
        item->SetOrder( iPreviouslySentArray[ count - 1 ]->Order() + 1 );
        }
    CleanupStack::Pop( item );
    error = iPreviouslySentArray.InsertInOrder( item, iLinearOrder );
    if ( error != KErrNone )
        {
        delete item;
        }
    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory
    }

// ---------------------------------------------------------
// CMmsHeaders::SetResponseTextL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetResponseTextL( const TDesC& aText )
    {
    // If the length of the new text is 0, the text is cleared
    delete iOperatorResponseText;
    iOperatorResponseText = NULL;
    if ( aText.Length() > 0 )
        {
        HBufC* newAttrib = aText.AllocL();
        iOperatorResponseText = newAttrib;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::ResponseText
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMmsHeaders::ResponseText() const
    {
    return iOperatorResponseText ? TPtrC( *iOperatorResponseText ) : TPtrC(); 
    }

// ---------------------------------------------------------
// CMmsHeaders::InsertDeleteStatusL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::InsertDeleteStatusL( TUint aOrder, const TInt32 aStatus )
    {
    TInt error = KErrNone;
    CMmsDeleteResultArray* item = new (ELeave)CMmsDeleteResultArray;
    CleanupStack::PushL( item );
    item->SetOrder( aOrder );
    item->SetResponseStatus( aStatus );

    TInt index = KErrNotFound;
    index = iDeleteResultArray.FindInOrder( item, iDeleteResultOrder );
    CleanupStack::Pop( item );

    if ( index == KErrNotFound )
        {
        error = iDeleteResultArray.InsertInOrder( item, iDeleteResultOrder );
        if ( error != KErrNone )
            {
            delete item;
            }
        }
    else
        {
        delete item;
        // modify existing entry - find all possible matches and modify all.
        TInt i;
        for ( i = 0; i < iDeleteResultArray.Count(); i++ )
            {
            if ( iDeleteResultArray[i]->Order() == aOrder )
                {
                iDeleteResultArray[i]->SetResponseStatus( aStatus );
                }
            }
        }
    item = NULL; // this was either deleted or inserted into our array.

    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory

    }

// ---------------------------------------------------------
// CMmsHeaders::InsertDeleteContentLocationL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::InsertDeleteContentLocationL(
    TUint aOrder, const TDesC8& aContentLocation )
    {
    TInt error = KErrNone;
    CMmsDeleteResultArray* item = new (ELeave)CMmsDeleteResultArray;
    CleanupStack::PushL( item );
    item->SetOrder( aOrder );
    item->SetContentLocationL( aContentLocation );

    TInt index = KErrNotFound;
    index = iDeleteResultArray.FindInOrder( item, iDeleteResultOrder );
    CleanupStack::Pop( item );

    if ( index == KErrNotFound )
        {
        error = iDeleteResultArray.InsertInOrder( item, iDeleteResultOrder );
        if ( error != KErrNone )
            {
            delete item;
            }
        }
    else
        {
        // If we find an entry without content location we modify existing entry
        // otherwise we insert a new entry
        TInt i;
        TBool updated = EFalse;
        for ( i = 0; i < iDeleteResultArray.Count() && !updated ; i++ )
            {
            if ( iDeleteResultArray[i]->Order() == aOrder &&
                iDeleteResultArray[i]->ContentLocation().Length() == 0 )
                {
                iDeleteResultArray[i]->SetContentLocationL( aContentLocation );
                updated = ETrue;
                }
            }
        if ( !updated )
            {
            item->SetResponseStatus( iDeleteResultArray[index]->ResponseStatus() );
            item->SetResponseTextL( iDeleteResultArray[index]->ResponseText() );
            // order and contentlocation are already there
            error = iDeleteResultArray.InsertInOrderAllowRepeats( item, iDeleteResultOrder );
            if ( error != KErrNone )
                {
                delete item;
                }
            }
        else
            {
            delete item;
            }
        }
    item = NULL; // this was either deleted or inserted into our array.

    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory

    }

// ---------------------------------------------------------
// CMmsHeaders::InsertDeleteResponseTextL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::InsertDeleteResponseTextL( TUint aOrder, const TDesC& aResponseText )
    {
    TInt error = KErrNone;
    CMmsDeleteResultArray* item = new (ELeave)CMmsDeleteResultArray;
    CleanupStack::PushL( item );
    item->SetOrder( aOrder );
    item->SetResponseTextL( aResponseText );

    TInt index = KErrNotFound;
    index = iDeleteResultArray.FindInOrder( item, iDeleteResultOrder );
    CleanupStack::Pop( item );

    if ( index == KErrNotFound )
        {
        error = iDeleteResultArray.InsertInOrder( item, iDeleteResultOrder );
        if ( error != KErrNone )
            {
            delete item;
            }
        }
    else
        {
        delete item;
        // modify existing entry
        // order is already set, as we use that as our match
        TInt i;
        for ( i = 0; i < iDeleteResultArray.Count(); i++ )
            {
            if ( iDeleteResultArray[i]->Order() == aOrder )
                {
                iDeleteResultArray[i]->SetResponseTextL( aResponseText );
                }
            }
        }
    item = NULL; // this was either deleted or inserted into our array.

    User::LeaveIfError( error ); // The only error that should be possible is KErrNoMemory

    }

// ---------------------------------------------------------
// CMmsHeaders::SetApplicIdL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetApplicIdL( const TDesC16& aApplicId )
    {
    // If the length of the new text is 0, the text is cleared
    delete iApplicationId;
    iApplicationId = NULL;
    if ( aApplicId.Length() > 0 )
        {
        HBufC* newAttrib = aApplicId.AllocL();
        iApplicationId = newAttrib;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::ApplicId
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC16 CMmsHeaders::ApplicId() const
    {
    return iApplicationId ? TPtrC16( *iApplicationId ) : TPtrC16();
    }
    
// ---------------------------------------------------------
// CMmsHeaders::SetReplyApplicIdL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetReplyApplicIdL( const TDesC16& aApplicId )
    {
    // If the length of the new text is 0, the text is cleared
    delete iReplyToApplicationId;
    iReplyToApplicationId = NULL;
    if ( aApplicId.Length() > 0 )
        {
        HBufC* newAttrib = aApplicId.AllocL();
        iReplyToApplicationId = newAttrib;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::ReplyApplicId
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC16 CMmsHeaders::ReplyApplicId() const
    {
    return iReplyToApplicationId ? TPtrC16( *iReplyToApplicationId ) : TPtrC16();
    }

// ---------------------------------------------------------
// CMmsHeaders::SetAuxApplicInfoL
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetAuxApplicInfoL( const TDesC8& aAuxApplicInfo )
    {
    // If the length of the new text is 0, the text is cleared
    delete iApplicationInfo;
    iApplicationInfo = NULL;
    if ( aAuxApplicInfo.Length() > 0 )
        {
        HBufC8* newAttrib = aAuxApplicInfo.AllocL();
        iApplicationInfo = newAttrib;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::AuxApplicInfo
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::AuxApplicInfo() const
    {
    return iApplicationInfo ? TPtrC8( *iApplicationInfo ) : TPtrC8();
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetRecommendedRetrievalModeTextL(
    const TDesC16& aRecommendedRetrievalModeText )
    {
    // If the length of the new text is 0, the text is cleared
    delete iRecommendedRetrievalModeText;
    iRecommendedRetrievalModeText = NULL;
    if ( aRecommendedRetrievalModeText.Length() > 0 )
        {
        HBufC* newAttrib = aRecommendedRetrievalModeText.AllocL();
        iRecommendedRetrievalModeText = newAttrib;
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC16 CMmsHeaders::RecommendedRetrievalModeText() const
    {
    return iRecommendedRetrievalModeText ?
        TPtrC16( *iRecommendedRetrievalModeText ) : TPtrC16();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::SetReplaceCancelIdL( const TDesC8& aReplaceCancelId )
    {
    HBufC8 * newAttrib = aReplaceCancelId.AllocL();
    if ( newAttrib->Length() > KMaxHeaderStringLength )
        {
        newAttrib->Des().SetLength( KMaxHeaderStringLength );
        }
    delete iReplaceCancelId;
    iReplaceCancelId = newAttrib;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsHeaders::ReplaceCancelId() const
    {
    return iReplaceCancelId ? TPtrC8( *iReplaceCancelId ) : TPtrC8();
    }

// ---------------------------------------------------------
// CMmsHeaders::ExternalizeArrayL
//
// ---------------------------------------------------------
//
void CMmsHeaders::ExternalizeArrayL(
    CDesC16Array& anArray,
    RWriteStream& aStream ) const
    {

    TInt count=anArray.Count();
    aStream << TCardinality( count );       // compressed value
    for ( TInt ii = 0; ii < count; ++ii )
        {
        aStream << anArray[ii].Left( Min( anArray[ii].Length(), KMaxHeaderStringLength ) ) ;
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::InternalizeArrayL
//
// ---------------------------------------------------------
//
void CMmsHeaders::InternalizeArrayL(
    CDesC16Array& anArray,
    RReadStream& aStream )
    {
    TCardinality card;
    aStream >> card;
    TInt count=card;
    anArray.Reset();
    for ( TInt ii = 0; ii < count; ++ii )
        {
        HBufC16* buf=HBufC16::NewLC( aStream, KMaxHeaderStringLength );
        anArray.CArrayFixBase::InsertL( ii, &buf );
        CleanupStack::Pop( buf );
        }
    }

// ---------------------------------------------------------
// CMmsHeaders::CopyAddresseeListL()
//
// ---------------------------------------------------------
//
void CMmsHeaders::CopyAddresseeListL(
    const CDesCArray& aArray,
    TMmsRecipients aType,
    const TPtrC& aExclude,
    RFs& aFs ) 
    {
    TInt size;
    size = aArray.Count();
    CDesCArray* array = NULL;
    array = AliasedAddressL( aArray, aFs );
    CleanupStack::PushL( array );
    
    for (TInt i=0; i < size; i++)
        {
        if ( aExclude.Length() == 0 ||
            ( aExclude.Length() > 0 &&
            TMmsGenUtils::PureAddress( aExclude ).CompareF(
            TMmsGenUtils::PureAddress( array->MdcaPoint(i) ) ) != 0 ) )
                {
                this->AddTypedAddresseeL( array->MdcaPoint(i), aType );
                }
        }
    CleanupStack::PopAndDestroy( array );
    } 

// ---------------------------------------------------------
// CMmsHeaders::AliasedAddressL()
//
// ---------------------------------------------------------
// 
HBufC* CMmsHeaders::AliasedAddressL(
    const TDesC& aOriginalAddress, 
    RFs& aFs )
    {
    HBufC* result = NULL;
    TPtrC realAddress;
    TInt error = KErrNone;

    realAddress.Set( TMmsGenUtils::PureAddress( aOriginalAddress ) ); 
    HBufC* alias = HBufC::NewL( KMaxHeaderStringLength );
    CleanupStack::PushL( alias );
    TPtr aliasPtr = alias->Des();
    error = TMmsGenUtils::GetAlias( realAddress, aliasPtr, KMaxHeaderStringLength, aFs );
    if ( error == KErrNone && aliasPtr.Length() > 0 )
        {
        result = TMmsGenUtils::GenerateAddressL( realAddress, aliasPtr );
        }
    else
        {
        result = HBufC::NewL( aOriginalAddress.Length() );
        result->Des().Copy( aOriginalAddress );
        }

    CleanupStack::PopAndDestroy( alias );

    // It is the caller's responsibility to put result on cleanup stack
    return result;

    }
// ---------------------------------------------------------
// CMmsHeaders::AliasedAddressL()
//
// ---------------------------------------------------------
//
CDesCArray* CMmsHeaders::AliasedAddressL(
    const CDesCArray& aOriginalAddress, 
    RFs& aFs )
    {
    TInt size;
    TInt error( KErrNone );
    TInt i( 0 );
    size = aOriginalAddress.MdcaCount();
 
    CDesCArray* aliasArray = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    CleanupStack::PushL( aliasArray ); 

    CDesCArray* realAddress = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    CleanupStack::PushL( realAddress ); 
    
    //This is popped later because of the caller ...
    CDesCArray* result = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    CleanupStack::PushL( result );
   
    for ( i=0; i < size; i++ )
        {
        // index is valid as i is always less than aOriginalAddress size
   	    realAddress->InsertL( i,
   	        ( TMmsGenUtils::PureAddress( aOriginalAddress[i] ) ) );
        }

    //GetAlias and GetAliasForAll returns error only in very rare case, in case on oom
    //or in case of programming error. That's why if some of the alias matchings fails let's
    //ignore the whole alias matching
    TRAP( error,
         TMmsGenUtils::GetAliasForAllL( *realAddress, *aliasArray, KMaxHeaderStringLength, aFs )
         );
     
    size = aliasArray->MdcaCount();
    
    HBufC* tempAddress( NULL );
    
    if ( error != KErrNone )
        {
    	//Something badly wrong, let's not set any alias
    	for ( i = 0; i < size; i++ )
    	    {
            // index is valid as i is always less than aOriginalAddress size
    		result->InsertL( i, aOriginalAddress[i] );
    	    }
        }
    else
        {
        for ( i = 0; i < size; i++ )
            {
        	if ( ( aliasArray->MdcaPoint(i)).Length() > 0 )
        	    {
        		tempAddress = TMmsGenUtils::GenerateAddressL( 
        		    realAddress->MdcaPoint(i), 
        		    aliasArray->MdcaPoint(i) );
        		CleanupStack::PushL( tempAddress );    
        		result->InsertL( i, tempAddress->Des() );   
        		CleanupStack::PopAndDestroy( tempAddress ); 
        	    }
        	else
        	    {
                // index is valid as i is always less than aOriginalAddress size
        	    result->InsertL( i, aOriginalAddress[i] );	
        	    }
            }
        }
    
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( realAddress );
    CleanupStack::PopAndDestroy( aliasArray );

    return result;

    }    
    
// ---------------------------------------------------------
// CMmsHeaders::CreateContentId()
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsHeaders::CreateContentId( TDes8& aCid )
    {
    TTime now;
    now.UniversalTime();
    TInt random;
    TInt64 seed = now.Int64();
    random = Math::Rand( seed );

    aCid.Num( random );
    aCid.Insert(0, KMmsLeftAngle );
    aCid.Append( KMmsRightAngle );
    }

// ---------------------------------------------------------
// CMmsHeaders::LimitStringSize
//
// ---------------------------------------------------------
//
TPtrC8 CMmsHeaders::LimitStringSize( const TPtrC8& aString, TInt aMaxSize)
	{
	if (aString.Length() < aMaxSize)
		return aString;
	else
		return aString.Left(aMaxSize);
	}

// ---------------------------------------------------------
// CMmsHeaders::LimitStringSize
//
// ---------------------------------------------------------
//
TPtrC16 CMmsHeaders::LimitStringSize( const TPtrC16& aString, TInt aMaxSize)
	{
	if (aString.Length() < aMaxSize)
		return aString;
	else
		return aString.Left(aMaxSize);
	}
	
// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

