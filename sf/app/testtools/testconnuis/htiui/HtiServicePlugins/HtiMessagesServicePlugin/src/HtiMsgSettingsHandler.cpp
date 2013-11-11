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
* Description:  Implementation of message settings handling.
*
*/


// INCLUDE FILES
#include "HtiMessagesServicePlugin.h"
#include "HtiMsgSettingsHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>
#include <cmmanager.h>

#include <csmsaccount.h>
#include <mmssettings.h>
#include <smutset.h>
#include <centralrepository.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUid KCRUidSmum = { 0x101F87EF }; // from MessagingInternalCRKeys.h

const TInt KMinSetDefaultSmsCenterLength = 4;
const TInt KMinDeleteSmsCenterLength     = 2;
const TInt KMinSetMmsSettingsLength      = 10;

// set SMS settings has a fixed length
const TInt KSetSmsSettingsLength         = 6;

// message validity times in seconds
const TInt KValidityTimeMax  = 0;
const TInt KValidityTime1h   = 3600;
const TInt KValidityTime6h   = 6 * 3600;
const TInt KValidityTime24h  = 24 * 3600;
const TInt KValidityTime3d   = 3 * 24 * 3600;
const TInt KValidityTime1w   = 7 * 24 * 3600;

// from MmsSettingsDefs.h
const TInt KMmsUniImageSmallWidth = 640;
const TInt KMmsUniImageSmallHeight = 480;
const TInt KMmsUniImageLargeWidth = 1600;
const TInt KMmsUniImageLargeHeight = 1200;

const TInt KApMaxConnNameLength = 30;

// MACROS

// LOCAL CONSTANTS AND MACROS

_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorSetDefaultSmsCenterFailed, "Setting default SMS center failed" );
_LIT8( KErrorDeleteSmsCenterFailed, "SMS center deletion failed" );
_LIT8( KErrorSmsCenterAlreadyExists, "SMS center with given name already exists with different number" );
_LIT8( KErrorSmsCenterNotFound, "SMS center with given name not found" );
_LIT8( KErrorSetSmsSettingsFailed, "Setting SMS service settings failed" );
_LIT8( KErrorSetMmsSettingsFailed, "Setting MMS service settings failed" );
_LIT8( KErrorApNotFound, "Access Point not found" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiMsgSettingsHandler* CHtiMsgSettingsHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::NewL" );
    CHtiMsgSettingsHandler* self = new (ELeave) CHtiMsgSettingsHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::NewL" );
    return self;
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::CHtiMsgSettingsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiMsgSettingsHandler::CHtiMsgSettingsHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::CHtiMsgSettingsHandler" );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::CHtiMsgSettingsHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::~CHtiMsgSettingsHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiMsgSettingsHandler::~CHtiMsgSettingsHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::~CHtiMsgSettingsHandler" );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::~CHtiMsgSettingsHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::ConstructL" );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiMsgSettingsHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::SetDispatcher" );
    }


// -----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::ProcessMessageL( const TDesC8& aMessage,
                                              THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::ProcessMessageL" );

    // Zero length message and command code validity already checked
    // in HtiMessagesServicePlugin.

    if ( aMessage[0] == CHtiMessagesServicePlugin::ESetDefaultSmsCenter )
        {
        if ( aMessage.Length() < 1 + KMinSetDefaultSmsCenterLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleSetDefaultSmsCenterL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorSetDefaultSmsCenterFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::EDeleteSmsCenter )
        {
        if ( aMessage.Length() < 1 + KMinDeleteSmsCenterLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleDeleteSmsCenterL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorDeleteSmsCenterFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::ESetSmsSettings )
        {
        if ( aMessage.Length() != 1 + KSetSmsSettingsLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleSetSmsSettingsL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorSetSmsSettingsFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::ESetMmsSettings )
        {
        if ( aMessage.Length() < 1 + KMinSetMmsSettingsLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleSetMmsSettingsL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorSetMmsSettingsFailed );
                }
            }
        }

    else
        {
        SendErrorMessageL( KErrArgument, KErrorUnrecognizedCommand );
        }

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::ProcessMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::HandleSetDefaultSmsCenterL
// Creates new default SMS center.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::HandleSetDefaultSmsCenterL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::HandleSetDefaultSmsCenterL" );

    TBuf<255> name;
    TBuf<255> number;

    TInt dataLength = aData.Length();
    TInt offset = 0;
    TInt fieldLength = aData[offset];
    offset++;

    if ( offset + fieldLength > dataLength )
        {
        HTI_LOG_TEXT( "Invalid length specified for name" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }
    name.Copy( aData.Mid( offset, fieldLength ) );
    offset += fieldLength;
    fieldLength = aData[offset];
    offset++;

    if ( offset + fieldLength > dataLength )
        {
        HTI_LOG_TEXT( "Invalid length specified for number" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }
    number.Copy( aData.Mid( offset, fieldLength ) );

    HTI_LOG_FORMAT( "SMS center name: %S", &name );
    HTI_LOG_FORMAT( "SMS center number: %S", &number );

    HTI_LOG_TEXT( "Loading current settings..." );
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    smsAccount->LoadSettingsL( *smsSettings );

    HTI_LOG_TEXT( "Looping through existing SMS centers..." );
    TInt i = 0;
    for ( i = 0; i < smsSettings->ServiceCenterCount(); i++ )
        {
        CSmsServiceCenter &center = smsSettings->GetServiceCenter( i );

        HTI_LOG_FORMAT( "Found SMS center: %S", &( center.Name() ) );

        if ( center.Name() == name && center.Address() != number )
            {
            HTI_LOG_TEXT( "Error, SMS center with given name already exists" );
            SendErrorMessageL( KErrAlreadyExists, KErrorSmsCenterAlreadyExists );
            CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings
            return;
            }
        else if ( center.Name() == name && center.Address() == number )
            {
            HTI_LOG_TEXT( "The defined SMS center already exists, setting it to default" );
            smsSettings->SetDefaultServiceCenter( i );
            smsAccount->SaveSettingsL( *smsSettings );

            CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings
            SendOkMsgL( KNullDesC8 );

            return;
            }
        }

    HTI_LOG_TEXT( "Creating new default SMS center" );
    smsSettings->AddServiceCenterL( name, number );
    smsSettings->SetDefaultServiceCenter( i );
    smsAccount->SaveSettingsL( *smsSettings );

    CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings

    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::HandleSetDefaultSmsCenterL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::HandleDeleteSmsCenterL
// Deletes the defined SMS center.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::HandleDeleteSmsCenterL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::HandleDeleteSmsCenterL" );

    TBuf<255> name;

    TInt dataLength = aData.Length();
    TInt offset = 0;
    TInt fieldLength = aData[offset];
    offset++;

    if ( offset + fieldLength > dataLength )
        {
        HTI_LOG_TEXT( "Invalid length specified for name" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }
    name.Copy( aData.Mid( offset, fieldLength ) );

    HTI_LOG_FORMAT( "SMS center name: %S", &name );

    HTI_LOG_TEXT( "Loading current settings..." );
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    smsAccount->LoadSettingsL( *smsSettings );

    HTI_LOG_TEXT( "Looping through existing SMS centers..." );
    TInt i = 0;
    for ( i = 0; i < smsSettings->ServiceCenterCount(); i++ )
        {
        CSmsServiceCenter &center = smsSettings->GetServiceCenter( i );

        HTI_LOG_FORMAT( "Found SMS center: %S", &( center.Name() ) );

        if ( center.Name() == name )
            {
            HTI_LOG_TEXT( "Name matches, deleting..." );
            smsSettings->RemoveServiceCenter( i );
            smsAccount->SaveSettingsL( *smsSettings );

            CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings

            SendOkMsgL( KNullDesC8 );
            return;
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings

    SendErrorMessageL( KErrNotFound, KErrorSmsCenterNotFound );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::HandleDeleteSmsCenterL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::HandleSetSmsSettingsL
// Sets the SMS service settings.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::HandleSetSmsSettingsL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::HandleSetSmsSettingsL" );

    // extract values
    TInt offset = 0;
    TInt characterSupport = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Character support     = %d", characterSupport );

    TBool deliveryReport = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Delivery report       = %d", deliveryReport );

    TInt validityPeriod = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Validity period       = %d", validityPeriod );

    TInt messageConversion = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Message conversion    = %d", messageConversion );

    TInt preferredConnection = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Preferred connection  = %d", preferredConnection );

    TBool replyViaSameCenter = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Reply via same center = %d", replyViaSameCenter );

    // Validate values
    if ( characterSupport > 1 || validityPeriod > 5 ||
         messageConversion > 3 || preferredConnection > 1 )
        {
        HTI_LOG_TEXT( "Invalid values found" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    HTI_LOG_TEXT( "Loading current settings..." );
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    smsAccount->LoadSettingsL( *smsSettings );

    HTI_LOG_TEXT( "Opening SMS settings Central Repository" );
    CRepository* cenRep = CRepository::NewLC( KCRUidSmum );
    HTI_LOG_TEXT( "Setting character support setting to SMS settings Central Repository" );
    TUint32 KSmumCharSupport = 0x00000008; // from MessagingInternalCRKeys.h
    switch ( characterSupport )
        {
        case 0:
            cenRep->Set( KSmumCharSupport, ECharSetFull );
            break;
        case 1:
            cenRep->Set( KSmumCharSupport, ECharSetReduced );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    CleanupStack::PopAndDestroy( cenRep );
    HTI_LOG_TEXT( "SMS settings Central Repository closed" );

    smsSettings->SetDeliveryReport( deliveryReport );

    switch ( validityPeriod )
        {
        case 0:
            smsSettings->SetValidityPeriod( TTimeIntervalMinutes( 0 ) );
            break;
        case 1:
            smsSettings->SetValidityPeriod(
                TTimeIntervalMinutes( KValidityTime1h / 60 ) );
            break;
        case 2:
            smsSettings->SetValidityPeriod(
                TTimeIntervalMinutes( KValidityTime6h / 60 ) );
            break;
        case 3:
            smsSettings->SetValidityPeriod(
                TTimeIntervalMinutes( KValidityTime24h / 60 ) );
            break;
        case 4:
            smsSettings->SetValidityPeriod(
                TTimeIntervalMinutes( KValidityTime3d / 60 ) );
            break;
        case 5:
            smsSettings->SetValidityPeriod(
                TTimeIntervalMinutes( KValidityTime1w / 60 ) );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    switch ( messageConversion )
        {
        case 0:
            smsSettings->SetMessageConversion( ESmsConvPIDNone );
            break;
        case 1:
            smsSettings->SetMessageConversion( ESmsConvFax );
            break;
        case 2:
            smsSettings->SetMessageConversion( ESmsConvPaging );
            break;
        case 3:
            smsSettings->SetMessageConversion( ESmsConvMail );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    switch ( preferredConnection )
        {
        case 0:
            smsSettings->SetSmsBearer( CSmsSettings::ESmsBearerCircuitPreferred );
            break;
        case 1:
            smsSettings->SetSmsBearer( CSmsSettings::ESmsBearerPacketPreferred );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    smsSettings->SetReplyPath( replyViaSameCenter );

    HTI_LOG_TEXT( "Saving the settings" );
    smsAccount->SaveSettingsL( *smsSettings );

    CleanupStack::PopAndDestroy( 2 ); // smsAccount, smsSettings

    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::HandleSetSmsSettingsL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::HandleSetMmsSettingsL
// Sets the MMS service settings.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::HandleSetMmsSettingsL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::HandleSetMmsSettingsL" );
    TInt offset = 0;

    // Get the access point name and search UID for that AP.
    TInt apNameLength = aData[offset];
    offset++;
    if ( apNameLength > KApMaxConnNameLength ||
         aData.Length() != ( KMinSetMmsSettingsLength + apNameLength - 1 ) )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    TBuf<KApMaxConnNameLength> apName;
    apName.Copy( aData.Mid( offset, apNameLength ) );
    offset += apNameLength;
    HTI_LOG_FORMAT( "Searching AP with name: %S", &apName );
    TInt apUid = GetAccessPointUIDL( apName );
    if ( apUid == KErrNotFound )
        {
        SendErrorMessageL( KErrNotFound, KErrorApNotFound );
        return;
        }

    // Extract other values
    TMmsCreationMode creationMode = ( TMmsCreationMode ) aData[offset];
    offset++;
    HTI_LOG_FORMAT( "creation mode      = %d", creationMode );

    TInt imageSize = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "image size         = %d", imageSize );

    TInt receptionMode = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "reception mode     = %d", receptionMode );

    TBool receiveAnonymous = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "receive anonymous  = %d", receiveAnonymous );

    TBool receiveAdvert = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "receive adverts    = %d", receiveAdvert );

    TBool requestDeliveryReport = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "request deliv reps = %d", requestDeliveryReport );

    TBool sendDeliveryReport = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "send deliv reps    = %d", sendDeliveryReport );

    TInt validityPeriod = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "validity period    = %d", validityPeriod );

    // Validate values
    if ( creationMode > 2 || imageSize > 2 ||
         receptionMode > 3 || validityPeriod > 5 ||
         ( creationMode == 0 && imageSize == 2 ) )
        {
        HTI_LOG_TEXT( "Invalid values found" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    // Load current settings
    CMmsSettings* mmsSettings = CMmsSettings::NewL();
    CleanupStack::PushL( mmsSettings );
    mmsSettings->LoadSettingsL();
    HTI_LOG_TEXT( "Settings loaded - now updating" );

    // Update the values
    if ( mmsSettings->AccessPointCount() > 0 )
        {
        mmsSettings->DeleteAccessPointL( 0 );
        }
    mmsSettings->AddAccessPointL( apUid, 0 );

    mmsSettings->SetCreationMode( creationMode );

    switch ( imageSize )
        {
        case 0:
            mmsSettings->SetImageWidth( KMmsUniImageSmallWidth );
            mmsSettings->SetImageHeight( KMmsUniImageSmallHeight );
            break;
        case 1:
            mmsSettings->SetImageWidth( KMmsUniImageLargeWidth );
            mmsSettings->SetImageHeight( KMmsUniImageLargeHeight );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    switch ( receptionMode )
        {
        case 0:
            mmsSettings->SetReceivingModeHome( EMmsReceivingAutomatic );
            mmsSettings->SetReceivingModeForeign( EMmsReceivingAutomatic );
            break;
        case 1:
            mmsSettings->SetReceivingModeHome( EMmsReceivingAutomatic );
            mmsSettings->SetReceivingModeForeign( EMmsReceivingManual );
            break;
        case 2:
            mmsSettings->SetReceivingModeHome( EMmsReceivingManual );
            mmsSettings->SetReceivingModeForeign( EMmsReceivingManual );
            break;
        case 3:
            mmsSettings->SetReceivingModeHome( EMmsReceivingReject );
            mmsSettings->SetReceivingModeForeign( EMmsReceivingReject );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    mmsSettings->SetAcceptAnonymousMessages( receiveAnonymous );
    mmsSettings->SetAcceptAdvertisementMessages( receiveAdvert );

    if ( requestDeliveryReport )
        {
        mmsSettings->SetDeliveryReportWanted( KMmsYes );
        }
    else
        {
        mmsSettings->SetDeliveryReportWanted( KMmsNo );
        }

    if ( sendDeliveryReport )
        {
        mmsSettings->SetDeliveryReportSendingAllowed( KMmsYes );
        }
    else
        {
        mmsSettings->SetDeliveryReportSendingAllowed( KMmsNo );
        }

    switch ( validityPeriod )
        {
        case 0:
            mmsSettings->SetExpiryInterval( KValidityTimeMax );
            break;
        case 1:
            mmsSettings->SetExpiryInterval( KValidityTime1h );
            break;
        case 2:
            mmsSettings->SetExpiryInterval( KValidityTime6h );
            break;
        case 3:
            mmsSettings->SetExpiryInterval( KValidityTime24h );
            break;
        case 4:
            mmsSettings->SetExpiryInterval( KValidityTime3d );
            break;
        case 5:
            mmsSettings->SetExpiryInterval( KValidityTime1w );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    // Check validity of updated settings and save if valid.
    if ( mmsSettings->ValidateSettings() != KErrNone )
        {
        HTI_LOG_TEXT( "Settings not valid - won't save" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        }
    else
        {
        HTI_LOG_TEXT( "Settings valid - now saving" );
        mmsSettings->SaveSettingsL();
        SendOkMsgL( KNullDesC8 );
        }

    CleanupStack::PopAndDestroy(); // mmsSettings
    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::HandleSetMmsSettingsL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::GetAccessPointUIDL
// Gets the UID of Access Point named by aApName.
// Returns KErrNotFound if AP not found.
// ----------------------------------------------------------------------------
TInt CHtiMsgSettingsHandler::GetAccessPointUIDL( const TDesC& aApName )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::GetAccessPointUIDL" );
    TInt uid = KErrNotFound;

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    // Search from uncategorised first
    RArray<TUint32> array = RArray<TUint32>();
    cmManager.ConnectionMethodL( array );
    CleanupClosePushL( array );
    TInt i = 0;
    while ( i < array.Count() && uid == KErrNotFound )
        {
        RCmConnectionMethod cm = cmManager.ConnectionMethodL( array[i] );
        CleanupClosePushL( cm );
        HBufC* name = cm.GetStringAttributeL( CMManager::ECmName );
        HTI_LOG_FORMAT( "Found name: %S", name );
        CleanupStack::PushL( name );
        if ( aApName.Compare( *name ) == 0 )
            {
            uid = cm.GetIntAttributeL( CMManager::ECmWapId );
            HTI_LOG_FORMAT( "Match: UID = %d", uid );
            }
        CleanupStack::PopAndDestroy(); // name
        CleanupStack::PopAndDestroy(); // cm
        i++;
        }
    CleanupStack::PopAndDestroy(); // array

    // If not found from uncategorised, search from all destinations
    if ( uid == KErrNotFound )
        {
        HTI_LOG_TEXT( "Not found from uncategorised" );
        RArray<TUint32> destIdArray = RArray<TUint32>();
        cmManager.AllDestinationsL( destIdArray );
        CleanupClosePushL( destIdArray );
        i = 0;
        while ( i < destIdArray.Count() && uid == KErrNotFound )
            {
            RCmDestination dest = cmManager.DestinationL( destIdArray[i] );
            CleanupClosePushL( dest );
            TInt j = 0;
            while ( j < dest.ConnectionMethodCount() && uid == KErrNotFound )
                {
                HBufC* name = dest.ConnectionMethodL( j ).GetStringAttributeL(
                        CMManager::ECmName );
                CleanupStack::PushL( name );
                HTI_LOG_FORMAT( "Found name: %S", name );
                if ( aApName.Compare( *name ) == 0 )
                    {
                    uid = dest.ConnectionMethodL( j ).GetIntAttributeL(
                            CMManager::ECmWapId );
                    HTI_LOG_FORMAT( "Match: UID = %d", uid );
                    }
                CleanupStack::PopAndDestroy(); // name
                j++;
                }
            CleanupStack::PopAndDestroy(); // dest
            i++;
            }
        CleanupStack::PopAndDestroy(); // destIdArray
        }

    CleanupStack::PopAndDestroy(); // cmManager

    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::GetAccessPointUIDL" );
    return uid;
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::SendOkMsgL" );
    User::LeaveIfNull( iDispatcher );
    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiMessagesServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::SendOkMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiMsgSettingsHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiMsgSettingsHandler::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiMsgSettingsHandler::SendErrorMessageL" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiMsgSettingsHandler::SendErrorMessageL" );
    }

// End of file
