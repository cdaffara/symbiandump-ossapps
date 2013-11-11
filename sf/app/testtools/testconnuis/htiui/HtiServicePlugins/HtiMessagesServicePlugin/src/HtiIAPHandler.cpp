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
* Description:  Implementation of internet access point handling.
*
*/


// INCLUDE FILES
#include "HtiMessagesServicePlugin.h"
#include "HtiIAPHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include <cmconnectionmethodext.h>
#include <cmconnectionmethoddef.h>
#include <cmdestinationext.h>
#include <cmmanagerext.h>
#include <cmplugincsddef.h>
#include <cmpluginhscsddef.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>
#include <rconnmon.h>

using namespace CMManager;

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMinCreateMsgLength = 5;
const TInt KMinDeleteMsgLength = 3;
const TInt KMinCreateDestMsgLength = 3;
const TInt KMinDeleteDestMsgLength = 3;
const TInt KMinModifyDestMsgLength = 5;
const TInt KMinSetDefConMsgLength = 3;

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorCreateFailed, "Access Point creation failed" );
_LIT8( KErrorDeleteFailed, "Access Point deletion failed" );
_LIT8( KErrorDestCreateFailed, "Destination creation failed" );
_LIT8( KErrorDestDeleteFailed, "Destination deletion failed" );
_LIT8( KErrorApAlreadyExists, "Access Point with same name already exists" );
_LIT8( KErrorApNotFound, "Access Point with given name not found" );
_LIT8( KErrorConnCloseFailed,
           "Failed to close a connection using the Access Point" );
_LIT8( KErrorAddToDestFailed, "Adding to destination failed" );
_LIT8( KErrorRemoveFromDestFailed, "Removing from destination failed" );
_LIT8( KErrorDestNotFound, "Destination with given name not found" );
_LIT8( KErrorSetDefConFailed, "Setting default connection failed" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiIAPHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiIAPHandler* CHtiIAPHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::NewL" );
    CHtiIAPHandler* self = new (ELeave) CHtiIAPHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::NewL" );
    return self;
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::CHtiIAPHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiIAPHandler::CHtiIAPHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::CHtiIAPHandler" );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::CHtiIAPHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiIAPHandler::~CHtiIAPHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiIAPHandler::~CHtiIAPHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::~CHtiIAPHandler" );

    for ( TInt i = 0; i < iFields.Count(); i++ )
        {
        delete iFields[i].iData;
        }
    iFields.Reset();
    iFields.Close();

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::~CHtiIAPHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiIAPHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiIAPHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::ConstructL" );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CHtiIAPHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiIAPHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::SetDispatcher" );
    }


// -----------------------------------------------------------------------------
// CHtiIAPHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiIAPHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::ProcessMessageL" );

    for ( TInt i = 0; i < iFields.Count(); i++ )
        delete iFields[i].iData;
    iFields.Reset();
    iFields.Close();
    iConnName.Zero();
    iBearerType = 0;

    // Zero length message and command code validity already checked
    // in HtiMessagesServicePlugin.

    if ( aMessage[0] == CHtiMessagesServicePlugin::ECreateIAP )
        {
        if ( aMessage.Length() < KMinCreateMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleCreateIapL( aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorCreateFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::EDeleteIAP )
        {
        if ( aMessage.Length() < KMinDeleteMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleDeleteIapL( aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorDeleteFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::ECreateDestination )
        {
        if ( aMessage.Length() < KMinCreateDestMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleCreateDestinationL( aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorDestCreateFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::EDeleteDestination )
        {
        if ( aMessage.Length() < KMinDeleteDestMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleDeleteDestinationL( aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorDestDeleteFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::EAddToDestination ||
            aMessage[0] == CHtiMessagesServicePlugin::ERemoveFromDestination )
        {
        if ( aMessage.Length() < KMinModifyDestMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, ModifyDestinationL( aMessage[0], aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                if ( aMessage[0] == CHtiMessagesServicePlugin::EAddToDestination )
                    {
                    SendErrorMessageL( err, KErrorAddToDestFailed );
                    }
                else
                    {
                    SendErrorMessageL( err, KErrorRemoveFromDestFailed );
                    }
                }
            }
        }
    else if ( aMessage[0] == CHtiMessagesServicePlugin::ESetDefaultConnection )
        {
        if ( aMessage.Length() < KMinSetDefConMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, SetDefaultConnectionL( aMessage.Mid( 1 ) ) );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorSetDefConFailed );
                }
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::ProcessMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::HandleCreateIapL
// Creates new Internet Access Point.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::HandleCreateIapL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::HandleCreateIapL" );

    TRAPD( err, ParseCreateMessageL( aData ) );
    if ( err != KErrNone )
        {
        SendErrorMessageL( err, KErrorInvalidParameters );
        return;
        }

    if ( GetAccessPointUIDL() != KErrNotFound )
        {
        SendErrorMessageL( KErrAlreadyExists, KErrorApAlreadyExists );
        return;
        }

    MapLegacyFieldIDs();
    ResolveFieldDataTypes();

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    RCmConnectionMethodExt cm = cmManager.CreateConnectionMethodL( iBearerType );
    CleanupClosePushL( cm );
    HTI_LOG_TEXT( "Connection method created" );

    cm.SetStringAttributeL( ECmName, iConnName );
    HTI_LOG_TEXT( "Name set" );

    TUint32 requestedSeamlessness = 1; // default

    // Set attributes
    HTI_LOG_FORMAT( "Fields to write: %d", iFields.Count() );
    for ( TInt i = 0; i < iFields.Count(); i++ )
        {
        if ( err != KErrNone ) break;

        HTI_LOG_FORMAT( "Writing field %d", ( i + 1 ) );
        TApField field = iFields[i];
        HTI_LOG_FORMAT( "Field ID = %d", field.iId );

        switch ( field.iDataType )
            {
            case EDataTypeText:
                {
                TRAP( err, cm.SetStringAttributeL( field.iId, *field.iData ) );
                break;
                }
            case EDataTypeBool:
                {
                TLex lex( *field.iData );
                TInt result = 0;
                lex.Val( result );
                TRAP( err, cm.SetBoolAttributeL( field.iId, ( TBool ) result ) );
                break;
                }
            case EDataTypeUint:
                {
                TLex lex( *field.iData );
                TUint32 result;
                err = lex.Val( result, EDecimal );
                if ( err == KErrNone )
                    TRAP( err, cm.SetIntAttributeL( field.iId, result ) );
                if ( field.iId == ECmSeamlessnessLevel )
                    requestedSeamlessness = result;
                break;
                }
            case EDataTypeText8:
                {
                HBufC8* buf8 = HBufC8::NewL( field.iData->Length() );
                buf8->Des().Copy( *field.iData );
                TRAP( err, cm.SetString8AttributeL( field.iId, *buf8 ) );
                delete buf8;
                buf8 = NULL;
                break;
                }
            default:
                {
                HTI_LOG_FORMAT( "Unsupported field type %d", field.iDataType );
                err = KErrNotSupported;
                break;
                }
            }
        }

    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "Error occurred %d", err );
        User::Leave( err );
        }

    cm.UpdateL();
    TUint32 uid = cm.GetIntAttributeL( ECmId );
    CleanupStack::PopAndDestroy(); // cm

    // The seamlessness value is not always set correctly when the connection
    // method is created. Have to update seamlessness value separately after
    // creation.
    cm = cmManager.ConnectionMethodL( uid );
    CleanupClosePushL( cm );
    TUint32 currentSeamlessness = cm.GetIntAttributeL( ECmSeamlessnessLevel );
    HTI_LOG_FORMAT( "Requested seamlessness = %d", requestedSeamlessness );
    HTI_LOG_FORMAT( "Current seamlessness = %d", currentSeamlessness );
    if ( currentSeamlessness != requestedSeamlessness )
        {
        cm.SetIntAttributeL( ECmSeamlessnessLevel, requestedSeamlessness );
        cm.UpdateL();
        HTI_LOG_TEXT( "Seamlessness value updated" );
        }
    CleanupStack::PopAndDestroy(); // cm
    CleanupStack::PopAndDestroy(); // cmManager

    HTI_LOG_FORMAT( "AP created with uid %d", uid );

    TBuf8<4> idBuf;
    idBuf.Append( ( TUint8* ) &uid, 4 );
    SendOkMsgL( idBuf );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::HandleCreateIapL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::HandleDeleteIapL
// Deletes the named Internet Access Point
// ----------------------------------------------------------------------------
void CHtiIAPHandler::HandleDeleteIapL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::HandleDeleteIapL" );
    if ( aData.Length() < KMinDeleteMsgLength )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    TInt nameLength = aData[0];
    if ( nameLength > KMaxConnNameLength ||
         aData.Length() != ( nameLength + 1 ) )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    iConnName.Copy( aData.Mid( 1, nameLength ) );
    HTI_LOG_FORMAT( "Searching connection with name: %S", &iConnName );
    TInt uid = GetAccessPointUIDL();

    if ( uid == KErrNotFound )
        {
        SendErrorMessageL( KErrNotFound, KErrorApNotFound );
        return;
        }

    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL( cmManagerExt );
    RCmConnectionMethodExt connMethod = cmManagerExt.ConnectionMethodL( uid );
    CleanupClosePushL( connMethod );
    if ( connMethod.GetBoolAttributeL( ECmConnected ) )
        {
        HTI_LOG_TEXT( "AP in use - trying to close connections" );
        TRAPD( err, CloseActiveConnectionsL() );
        if ( err != KErrNone )
            {
            SendErrorMessageL( err, KErrorConnCloseFailed );
            CleanupStack::PopAndDestroy( 2 ); // connMethod, cmManagerExt
            return;
            }
        }

    HTI_LOG_TEXT( "AP not in use - unlinking from all destinations" );
    cmManagerExt.RemoveAllReferencesL( connMethod );
    HTI_LOG_TEXT( "Deleting the AP" );
    TBool deleted = connMethod.DeleteL(); // returns ETrue if really deleted
    if ( !deleted )
        {
        HTI_LOG_TEXT( "Delete failed" );
        User::Leave( KErrGeneral );
        }
    HTI_LOG_FORMAT( "AP deleted with uid %d", uid );
    CleanupStack::PopAndDestroy(); // connMethod
    CleanupStack::PopAndDestroy(); // cmManagerExt

    SendOkMsgL( KNullDesC8 );
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::HandleDeleteIapL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::HandleCreateDestinationL
// Creates a new Destination.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::HandleCreateDestinationL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::HandleCreateDestinationL" );

    TInt nameLength = aData[0];
    if ( aData.Length() - 1 != nameLength || nameLength > KMaxConnNameLength )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    iDestName.Copy( aData.Mid( 1 ) );

    RCmDestinationExt dest = cmManager.CreateDestinationL( iDestName );
    CleanupClosePushL( dest );
    dest.UpdateL();

    CleanupStack::PopAndDestroy( 2 ); // dest, cmManager
    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::HandleCreateDestinationL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::HandleDeleteDestinationL
// Deletes a named Destination.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::HandleDeleteDestinationL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::HandleDeleteDestinationL" );

    TInt nameLength = aData[0];
    if ( aData.Length() - 1 != nameLength || nameLength > KMaxConnNameLength )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    iDestName.Copy( aData.Mid( 1 ) );
    TInt id = GetDestinationIDL( iDestName );

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    RCmDestinationExt dest = cmManager.DestinationL ( id );
    dest.DeleteLD();

    CleanupStack::PopAndDestroy(); // cmManager
    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::HandleDeleteDestinationL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::ModifyDestinationL
// Adds or removes IAP to/from a Destination.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::ModifyDestinationL( const TInt aCommand, const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::ModifyDestinationL" );

    TInt dataLength = aData.Length();
    TInt offset = 0;
    TInt cmNameLength = aData[offset];
    offset++;
    if ( dataLength < cmNameLength + 3 || cmNameLength > KMaxConnNameLength )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }
    iConnName.Copy( aData.Mid( offset, cmNameLength ) );
    offset += cmNameLength;
    TInt destNameLength = aData[offset];
    offset++;
    if ( dataLength != cmNameLength + destNameLength + 2 ||
            destNameLength > KMaxConnNameLength )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }

    iDestName.Copy( aData.Mid( offset, destNameLength ) );

    TInt cmId = GetAccessPointUIDL();
    if ( cmId == KErrNotFound )
        {
        SendErrorMessageL( KErrNotFound, KErrorApNotFound );
        return;
        }
    TInt destId = GetDestinationIDL( iDestName );
    if ( destId == KErrNotFound )
        {
        SendErrorMessageL( KErrNotFound, KErrorDestNotFound );
        return;
        }

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    RCmConnectionMethodExt cm = cmManager.ConnectionMethodL( cmId );
    CleanupClosePushL( cm );
    HTI_LOG_TEXT( "Got Connection Method" );

    RCmDestinationExt dest = cmManager.DestinationL( destId );
    CleanupClosePushL( dest );
    HTI_LOG_TEXT( "Got Destination" );

    if ( aCommand == CHtiMessagesServicePlugin::EAddToDestination )
        {
        dest.AddConnectionMethodL( cm );
        }
    else if ( aCommand == CHtiMessagesServicePlugin::ERemoveFromDestination)
        {
        dest.RemoveConnectionMethodL( cm );
        }
    else
        {
        // Programming error
        User::Panic( _L( "CHtiIAPHandler"), 1 );
        }
    dest.UpdateL();
    HTI_LOG_TEXT( "Destination updated" );

    CleanupStack::PopAndDestroy( 3 ); // dest, cm, cmManager
    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::ModifyDestinationL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::SetDefaultConnectionL
// Sets the default connection setting.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::SetDefaultConnectionL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::SetDefaultConnectionL" );

    if ( aData[0] > 3 || aData.Length() != aData[1] + 2 )
        {
        User::Leave( KErrArgument );
        }

    TCmDefConnValue defConnValue;
    defConnValue.iType = ( TCmDefConnType ) aData[0]; // cmdefconnvalues.h
    HTI_LOG_FORMAT( "Requested type: %d", defConnValue.iType );

    TInt id = 0;
    if ( defConnValue.iType == ECmDefConnDestination )
        {
        if ( aData[1] == 0 || aData[1] > KMaxConnNameLength )
            {
            User::Leave( KErrArgument );
            }
        iDestName.Copy( aData.Mid( 2 ) );
        HTI_LOG_FORMAT( "Destination name: %S", &iDestName );
        id = GetDestinationIDL( iDestName );
        if ( id == KErrNotFound )
            {
            SendErrorMessageL( KErrNotFound, KErrorDestNotFound );
            return;
            }
        }

    else if ( defConnValue.iType == ECmDefConnConnectionMethod )
        {
        if ( aData[1] == 0 || aData[1] > KMaxConnNameLength )
            {
            User::Leave( KErrArgument );
            }
        iConnName.Copy( aData.Mid( 2 ) );
        HTI_LOG_FORMAT( "Connection method name: %S", &iConnName );
        id = GetAccessPointUIDL();
        if ( id == KErrNotFound )
            {
            SendErrorMessageL( KErrNotFound, KErrorApNotFound );
            return;
            }
        }

    defConnValue.iId = id;

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );
    cmManager.WriteDefConnL( defConnValue );
    CleanupStack::PopAndDestroy();
    SendOkMsgL( KNullDesC8 );

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::SetDefaultConnectionL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::ParseCreateMessageL
// Parses the parameters from the create IAP message. Leaves on error.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::ParseCreateMessageL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::ParseCreateMessageL" );

    TInt dataLength = aData.Length();
    TInt offset = 0;

    iBearerType = GetBearerUID( aData[offset] );
    offset++;
    HTI_LOG_FORMAT( "Bearer type %d", iBearerType );
    if ( iBearerType == 0 )
        {
        User::Leave( KErrArgument );
        }

    TInt nameLength = aData[offset];
    offset++;
    if ( dataLength < ( nameLength + offset + 1 ) ||
         nameLength > KMaxConnNameLength )
        {
        User::Leave( KErrArgument );
        }
    iConnName.Copy( aData.Mid( offset, nameLength ) );
    offset += nameLength;
    HTI_LOG_FORMAT( "Connection name: %S", &iConnName );

    TInt fieldCount = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Field count %d", fieldCount );

    for ( TInt i = 0; i < fieldCount; i++ )
        {
        if ( dataLength < ( offset + 3 ) )
            {
            User::Leave( KErrArgument );
            }
        HTI_LOG_FORMAT( "Parsing field %d", i + 1 );
        TApField field;
        field.iId = aData[offset];
        offset++;
        TInt fieldLength = aData[offset];
        offset++;
        HTI_LOG_FORMAT( "Field data length %d", fieldLength );
        if ( fieldLength < 1 || dataLength < ( fieldLength + offset ) )
            {
            User::Leave( KErrArgument );
            }
        field.iData = HBufC::NewL( fieldLength );
        field.iData->Des().Copy( aData.Mid( offset, fieldLength ) );
        offset += fieldLength;
        HTI_LOG_FORMAT( "Field data: %S", field.iData );
        field.iDataType = EDataTypeUnknown; // set later
        User::LeaveIfError( iFields.Append( field ) );
        }

    if ( offset != dataLength )  // too much data
        {
        User::Leave( KErrArgument );
        }

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::ParseCreateMessageL" );
    }

// ----------------------------------------------------------------------------
// CHtiIAPHandler::GetAccessPointUIDL
// Gets the UID of Access Point named by iConnName.
// Returns KErrNotFound if AP not found.
// ----------------------------------------------------------------------------
TInt CHtiIAPHandler::GetAccessPointUIDL()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::GetAccessPointUIDL" );
    TInt uid = KErrNotFound;

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    // Search from uncategorised first
    RArray<TUint32> array = RArray<TUint32>();
    cmManager.ConnectionMethodL( array );
    CleanupClosePushL( array );
    TInt i = 0;
    while ( i < array.Count() && uid == KErrNotFound )
        {
        RCmConnectionMethodExt cm = cmManager.ConnectionMethodL( array[i] );
        CleanupClosePushL( cm );
        HBufC* name = cm.GetStringAttributeL( ECmName );
        HTI_LOG_FORMAT( "Found name: %S", name );
        CleanupStack::PushL( name );
        if ( iConnName.Compare( *name ) == 0 )
            {
            uid = cm.GetIntAttributeL( ECmId );
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
            RCmDestinationExt dest = cmManager.DestinationL( destIdArray[i] );
            CleanupClosePushL( dest );
            TInt j = 0;
            while ( j < dest.ConnectionMethodCount() && uid == KErrNotFound )
                {
                HBufC* name = dest.ConnectionMethodL( j ).GetStringAttributeL(
                        ECmName );
                CleanupStack::PushL( name );
                HTI_LOG_FORMAT( "Found name: %S", name );
                if ( iConnName.Compare( *name ) == 0 )
                    {
                    uid = dest.ConnectionMethodL( j ).GetIntAttributeL(
                            ECmId );
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
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::GetAccessPointUIDL" );
    return uid;
    }

// ----------------------------------------------------------------------------
// CHtiIAPHandler::GetDestinationIDL
// Gets the ID of Destination named by aName.
// Returns KErrNotFound if Destination is not found.
// ----------------------------------------------------------------------------
TInt CHtiIAPHandler::GetDestinationIDL( const TDesC& aName )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::GetDestinationIDL" );
    HTI_LOG_FORMAT( "Searching Destination %S", &aName );

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    TInt id = KErrNotFound;
    RArray<TUint32> destIdArray = RArray<TUint32>( 8 );
    CleanupClosePushL( destIdArray );

    cmManager.AllDestinationsL( destIdArray );
    TInt i = 0;
    while ( i < destIdArray.Count() && id == KErrNotFound )
        {
        RCmDestinationExt dest = cmManager.DestinationL( destIdArray[i] );
        CleanupClosePushL( dest );
        HBufC* destName = dest.NameLC();
        HTI_LOG_FORMAT( "Found Destination: %S", destName );
        if ( destName->Compare( aName ) == 0 )
            {
            id = dest.Id();
            HTI_LOG_FORMAT( "Match: ID = %d", id );
            }
        CleanupStack::PopAndDestroy( 2 ); // destName, dest
        i++;
        }

    CleanupStack::PopAndDestroy( 2 ); // destIdArray, cmManager
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::GetDestinationIDL" );
    return id;
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::CloseActiveConnectionsL
// Closes connection(s) that are using the Access Point named by iConnName.
// Leaves if closing fails.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::CloseActiveConnectionsL()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::CloseActiveConnectionsL" );

    RConnectionMonitor monitor;
    monitor.ConnectL();
    CleanupClosePushL( monitor );
    HTI_LOG_TEXT( "RConnectionMonitor connected" );

    TUint connCount = 0;
    TUint subConnCount = 0;
    TUint connId = 0;
    //TUint iapId = 0;
    TName iapName;
    TRequestStatus status;

    monitor.GetConnectionCount( connCount, status );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
    HTI_LOG_FORMAT( "Found %d connections", connCount );

    for ( TUint i = 1; i <= connCount; i++ )
        {
        HTI_LOG_FORMAT( "Connection %d", i );
        User::LeaveIfError(
            monitor.GetConnectionInfo( i, connId, subConnCount ) );
        HTI_LOG_FORMAT( " Connection ID = %d", connId );
        HTI_LOG_FORMAT( " Sub connections = %d", subConnCount );
        /*
        monitor.GetUintAttribute( connId, 0, KIAPId, iapId, status );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        HTI_LOG_FORMAT( " IAP ID = %d", iapId );
        */
        monitor.GetStringAttribute( connId, 0, KIAPName, iapName, status );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        HTI_LOG_FORMAT( " IAP name = %S", &iapName );
        if ( iapName.Compare( iConnName ) == 0 )
            {
            HTI_LOG_TEXT( " Match: Trying to close this connection" );
            User::LeaveIfError(
                monitor.SetBoolAttribute( connId, 0, KConnectionStop, ETrue ) );
            }
        }

    CleanupStack::PopAndDestroy(); // monitor
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::CloseActiveConnectionsL" );
    }

// ----------------------------------------------------------------------------
// CHtiIAPHandler::GetBearerUID
// Helper function for mapping old TApBearerType value to new bearer type
// ECom UID.
// ----------------------------------------------------------------------------
TUint CHtiIAPHandler::GetBearerUID( const TInt aBearerType )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::GetBearerUID" );
    TUint uid = 0;
    switch ( aBearerType )
        {
        case 0x01:  // CSD
            uid = KUidCSDBearerType;
            break;
        case 0x02:  // GPRS
            uid = KUidPacketDataBearerType;
            break;
        case 0x04:  // HSCSD
            uid = KUidHSCSDBearerType;
            break;
        case 0x10:  // CDMA
            uid = KUidPacketDataBearerType;
            break;
        case 0x20:  // WLAN
            uid = KUidWlanBearerType;
            break;
        default:
            break;

        }
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::GetBearerUID" );
    return uid;
    }

// ----------------------------------------------------------------------------
// CHtiIAPHandler::MapLegacyFieldIDs
// Helper function for mapping the old access point field ID numbers to
// new ones.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::MapLegacyFieldIDs()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::MapLegacyFieldIDs" );

    for ( TInt i = 0; i < iFields.Count(); i++ )
        {
        TApField field = iFields[i];
        switch ( field.iId )
            {
            case EApWapStartPage:
                {
                field.iId = ECmStartPage;
                break;
                }
            case EApIspDefaultTelNumber:
                {
                field.iId = EDialDefaultTelNum;
                break;
                }
            case EApIspUseLoginScript:
                {
                field.iId = EDialUseLoginScript;
                break;
                }
            case EApIspLoginScript:
                {
                field.iId = EDialLoginScript;
                break;
                }
            case EApIspLoginName:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIFAuthName;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIFAuthName;
                else
                    field.iId = ECmIFAuthName;
                break;
                }
            case EApIspLoginPass:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIFAuthPass;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIFAuthPass;
                else
                    field.iId = ECmIFAuthPass;
                break;
                }
            case EApIspIfPromptForAuth:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIFPromptForAuth;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIFPromptForAuth;
                else
                    field.iId = ECmIFPromptForAuth;
                break;
                }
            case EApIspIfCallbackEnabled:
                {
                field.iId = EDialIFCallbackEnabled;
                break;
                }
            case EApIspIfCallbackType:
                {
                field.iId = EDialIFCallbackType;
                break;
                }
            case EApIspIfCallbackInfo:
                {
                field.iId = EDialIFCallbackInfo;
                break;
                }
            case EApIspIPAddr:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIPAddress;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIPAddr;
                else if ( iBearerType == KUidWlanBearerType )
                    field.iId = EWlanIpAddr;
                else
                    field.iId = ECmIPAddress;
                break;
                }
            case EApIspIPNameServer1:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIPNameServer1;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIPNameServer1;
                else if ( iBearerType == KUidWlanBearerType )
                    field.iId = EWlanIpNameServer1;
                else
                    field.iId = ECmIPNameServer1;
                break;
                }
            case EApIspIPNameServer2:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIPNameServer2;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIPNameServer2;
                else if ( iBearerType == KUidWlanBearerType )
                    field.iId = EWlanIpNameServer2;
                else
                    field.iId = ECmIPNameServer2;
                break;
                }
            case EApIspEnableIpHeaderComp:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialEnableIPHeaderComp;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataHeaderCompression;
                break;
                }
            case EApIspDisablePlainTextAuth:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialDisablePlainTextAuth;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataDisablePlainTextAuth;
                else
                    field.iId = ECmDisablePlainTextAuth;
                break;
                }
            case EApIspBearerSpeed:
                {
                field.iId = EDialMaxConnSpeed;
                break;
                }
            case EApIspBearerCallTypeIsdn:
                {
                field.iId = EDialCallType;
                break;
                }
            case EApIspInitString:
                {
                field.iId = EDialInitString;
                break;
                }
            case EApGprsAccessPointName:
                {
                field.iId = EPacketDataAPName;
                break;
                }
            case EApGprsPdpType:
                {
                field.iId = EPacketDataPDPType;
                break;
                }
            case EApProxyServerAddress:
                {
                field.iId = ECmProxyServerName;
                break;
                }
            case EApProxyPortNumber:
                {
                field.iId = ECmProxyPortNumber;
                break;
                }
            case EApIP6NameServer1:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIP6NameServer1;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIPIP6NameServer1;
                else if ( iBearerType == KUidWlanBearerType )
                    field.iId = EWlanIp6NameServer1;
                else
                    field.iId = ECmIP6NameServer1;
                break;
                }
            case EApIP6NameServer2:
                {
                if ( iBearerType == KUidCSDBearerType || iBearerType == KUidHSCSDBearerType )
                    field.iId = EDialIP6NameServer2;
                else if ( iBearerType == KUidPacketDataBearerType )
                    field.iId = EPacketDataIPIP6NameServer2;
                else if ( iBearerType == KUidWlanBearerType )
                    field.iId = EWlanIp6NameServer2;
                else
                    field.iId = ECmIP6NameServer2;
                break;
                }
            case EApWlanNetworkName:
                {
                field.iId = EWlanSSID;
                break;
                }
            case EApWlanNetworkMode:
                {
                field.iId = EWlanConnectionMode;
                break;
                }
            case EApWlanSecurityMode:
                {
                field.iId = EWlanSecurityMode;
                break;
                }
            case EApSeamlessnessLevel:
                {
                field.iId = ECmSeamlessnessLevel;
                break;
                }
            default:
                break;
            }
        iFields[i] = field;
        }

    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::MapLegacyFieldIDs" );
    }

// ----------------------------------------------------------------------------
// CHtiIAPHandler::ResolveFieldDataTypes
// Helper function for resolving the data types for different AP settings
// fields based on field ID and bearer type.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::ResolveFieldDataTypes()
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::ResolveFieldDataTypes" );

    for ( TInt i = 0; i < iFields.Count(); i++ )
        {
        if ( iFields[i].iDataType != EDataTypeUnknown ) continue; // already set

        // Common fields
        switch ( iFields[i].iId )
            {
            // String types
            case ECmStartPage:
            case ECmProxyServerName:
            case ECmProxyProtocolName:
            case ECmProxyExceptions:
            case ECmIFName:
            case ECmIFParams:
            case ECmIFNetworks:
            case ECmIFAuthName:
            case ECmIFAuthPass:
            case ECmIPGateway:
            case ECmIPAddress:
            case ECmIPNameServer1:
            case ECmIPNameServer2:
            case ECmIP6NameServer1:
            case ECmIP6NameServer2:
                iFields[i].iDataType = EDataTypeText;
                break;

            // Bool types
            case ECmProtected:
            case ECmIFPromptForAuth:
            case ECmIPAddFromServer:
            case ECmIPDNSAddrFromServer:
            case ECmIP6DNSAddrFromServer:
            case ECmEnableLPCExtension:
            case ECmDisablePlainTextAuth:
                iFields[i].iDataType = EDataTypeBool;
                break;

            // Uint types
            case ECmSeamlessnessLevel:
            case ECmProxyPortNumber:
            case ECmIFAuthRetries:
                iFields[i].iDataType = EDataTypeUint;
                break;

            default:
                break;
            }

        // If it was found from common fields, we can continue to next field
        if ( iFields[i].iDataType != EDataTypeUnknown ) continue;

        // Check from bearer specific fields.
        // Different bearers have to be in separate switch-cases because
        // there are same values in different bearers.

        // Data call fields
        if ( iBearerType == KUidCSDBearerType ||
                iBearerType == KUidHSCSDBearerType )
            {
            switch ( iFields[i].iId )
                {
                // String types
                case EDialDescription:
                case EDialDefaultTelNum:
                case EDialLoginScript:
                case EDialLoginName:
                case EDialLoginPassword:
                case EDialIFParams:
                case EDialIFNetworks:
                case EDialIFAuthName:
                case EDialIFAuthPass:
                case EDialIPAddress:
                case EDialIPNetmask:
                case EDialIPGateway:
                case EDialIPNameServer1:
                case EDialIPNameServer2:
                case EDialIP6NameServer1:
                case EDialIP6NameServer2:
                    iFields[i].iDataType = EDataTypeText;
                    break;

                // Bool types
                case EDialOut:
                case EDialDialResolution:
                case EDialUseLoginScript:
                case EDialPromptForLogin:
                case EDialDisplayPCT:
                case EDialIFPromptForAuth:
                case EDialIFCallbackEnabled:
                case EDialIFServerMode:
                case EDialIPAddressFromServer:
                case EDialIPDNSAddressFromServer:
                case EDialIP6DNSAddressFromServer:
                case EDialEnableIPHeaderComp:
                case EDialEnableLCPExtension:
                case EDialDisablePlainTextAuth:
                case EDialEnableSWComp:
                case EDialUseEdge:
                    iFields[i].iDataType = EDataTypeBool;
                    break;

                // Uint types
                case EDialCallType:
                case EDialMaxConnSpeed:
                case EDialType:
                case EDialChargeCard:
                case EDialIFAuthRetries:
                case EDialIFCallbackType:
                case EDialCallbackTimeout:
                case EDialBearerName:
                case EDialBearerSpeed:
                case EDialBearerCE:
                case EDialBearerService:
                case EDialBearerProtocol:
                case EDialRLPVersion:
                case EDialIWFtoMS:
                case EDialMStoIWF:
                case EDialAckTimer:
                case EDialRetransmissionAttempts:
                case EDialResequencePeriod:
                case EDialV42Compression:
                case EDialV42CodeWords:
                case EDialV42MaxLength:
                case EHscsdAsymmetry:
                case EHscsdUserInitUpgrade:
                    iFields[i].iDataType = EDataTypeUint;
                    break;

                // 8-bit string types
                case EDialIFCallbackInfo:
                case EDialInitString:
                    iFields[i].iDataType = EDataTypeText8;
                    break;

                default:
                    break;
                }
            }

        // Packet data fields
        else if ( iBearerType == KUidPacketDataBearerType )
            {
            switch ( iFields[i].iId )
                {
                // String types
                case EPacketDataAPName:
                case EPacketDataPDPAddress:
                case EPacketDataIFParams:
                case EPacketDataIFNetworks:
                case EPacketDataIFAuthName:
                case EPacketDataIFAuthPass:
                case EPacketDataIPNetmask:
                case EPacketDataIPGateway:
                case EPacketDataIPAddr:
                case EPacketDataIPNameServer1:
                case EPacketDataIPNameServer2:
                case EPacketDataIPIP6NameServer1:
                case EPacketDataIPIP6NameServer2:
                    iFields[i].iDataType = EDataTypeText;
                    break;

                // Bool types
                case EPacketDataDataCompression:
                case EPacketDataHeaderCompression:
                case EPacketDataUseEdge:
                case EPacketDataAnonymousAccess:
                case EPacketDataIFPromptForAuth:
                case EPacketDataIPAddrFromServer:
                case EPacketDataIPDNSAddrFromServer:
                case EPacketDataIPIP6DNSAddrFromServer:
                case EPacketDataEnableLcpExtension:
                case EPacketDataDisablePlainTextAuth:
                case EPacketDataServiceEnableLLMNR:
                    iFields[i].iDataType = EDataTypeBool;
                    break;

                // Uint types
                case EPacketDataPDPType:
                case EPacketDataIFAuthRetries:
                case EPacketDataApType:
                    iFields[i].iDataType = EDataTypeUint;
                    break;

                default:
                    break;
                }
            }

        // WLAN fields
        else if ( iBearerType == KUidWlanBearerType )
            {
            switch ( iFields[i].iId )
            {
                // String types
                case EWlanIfNetworks:
                case EWlanIpNetMask:
                case EWlanIpGateway:
                case EWlanIpAddr:
                case EWlanIpNameServer1:
                case EWlanIpNameServer2:
                case EWlanIp6NameServer1:
                case EWlanIp6NameServer2:
                case EWlanSSID:
                case EWlanUsedSSID:
                    iFields[i].iDataType = EDataTypeText;
                    break;

                // Bool types
                case EWlanIpAddrFromServer:
                case EWlanIpDNSAddrFromServer:
                case EWlanIp6DNSAddrFromServer:
                case EWlanScanSSID:
                case EWlanAllowSSIDRoaming:
                    iFields[i].iDataType = EDataTypeBool;
                    break;

                // Uint types
                case EWlanServiceId:
                case EWlanConnectionMode:
                case EWlanSecurityMode:
                case EWlanAuthenticationMode:
                case EWlanChannelID:
                    iFields[i].iDataType = EDataTypeUint;
                    break;

                default:
                    break;
                }
            }

        else
            {
            HTI_LOG_TEXT( "Unknown bearer type" )
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::ResolveFieldDataTypes" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::SendOkMsgL" );
    User::LeaveIfNull( iDispatcher );
    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiMessagesServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::SendOkMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiIAPHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiIAPHandler::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiIAPHandler::SendErrorMessageL" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiIAPHandler::SendErrorMessageL" );
    }


// End of file
