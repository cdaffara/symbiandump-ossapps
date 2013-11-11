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
 * Description:  Implementation of SIM card contacts handling using the new
 *                Virtual Phonebook API
 *
 */

// INCLUDE FILES
#include "HtiSimDirHandler.h"
#include "HtiPIMServicePlugin.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <mmtsy_names.h>
#include <mpbutil.h> 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KSimInfoResponseLength = 12;
const TInt KOneSimContactBufferSize = 512;
// MACROS
// LOCAL CONSTANTS AND MACROS
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorImportFailed, "Contact import failed" );
_LIT8( KErrorDeleteFailed, "Failed to delete contact" );
_LIT8( KErrorSimCardInfoFailed, "Failed to get SIM card info" );
_LIT8( KErrorFieldNotSupported, "Field is not supported");
_LIT8( KErrorFieldTooBig, "Filed is too long");
//_LIT8( KErrorSimStoreOpenFailed, "Failed to open SIM contact store" );
//_LIT8( KErrorSimStoreUnavailable, "SIM contact store unavailable" );
//_LIT8( KErrorContactOperationFailed, "SIM contact operation failed" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiSimDirHandler* CHtiSimDirHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::NewL" );
    CHtiSimDirHandler* self = new (ELeave) CHtiSimDirHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::CHtiSimDirHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiSimDirHandler::CHtiSimDirHandler() :
    iIsBusy(EFalse), iStoreIsOpen(EFalse)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::CHtiSimDirHandler" );

    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::CHtiSimDirHandler" );
    }

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::~CHtiSimDirHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiSimDirHandler::~CHtiSimDirHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::~CHtiSimDirHandler" );
    iEtelStore.Close();
    iEtelPhone.Close();
    iEtelServer.Close();
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::~CHtiSimDirHandler" );
    }

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiSimDirHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::ConstructL" );

    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiSimDirHandler::SetDispatcher(MHtiDispatcher* aDispatcher)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::SetDispatcher" );
    }

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiSimDirHandler::ProcessMessageL(const TDesC8& aMessage,
        THtiMessagePriority /*aPriority*/)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::ProcessMessageL" );

    if (iStoreIsOpen == EFalse)
        {
        User::LeaveIfError(iEtelServer.Connect());
        User::LeaveIfError(iEtelServer.LoadPhoneModule(KMmTsyModuleName));
        User::LeaveIfError(iEtelPhone.Open(iEtelServer, KMmTsyPhoneName));
        User::LeaveIfError(iEtelStore.Open(iEtelPhone, KETelIccAdnPhoneBook));
        HTI_LOG_TEXT( "SIM card open" );
        iStoreIsOpen = ETrue;
        }

    if (iIsBusy)
        {
        HTI_LOG_TEXT( "HtiSimDirHandler is busy - leaving" );
        User::Leave(KErrInUse);
        }

    // Will be set to EFalse in the SendOkMsgL or SendErrorMessageL methods.
    iIsBusy = ETrue;

    // Zero legth of aMessage tested already in CHtiPIMServicePlugin.
    // Other sanity checks must be done here.
    TInt err = KErrNone;
    TUint8 command = aMessage.Ptr()[0];
    switch (command)
        {
        case CHtiPIMServicePlugin::ESimCardInfo:
            {
            TRAP(err ,HandleSimCardInfoL(aMessage.Right(aMessage.Length() - 1)));
            break;
            }
        case CHtiPIMServicePlugin::EImportSimContact:
            {
            TRAP(err ,HandleSimContactImportL(aMessage.Right(aMessage.Length() - 1)));
            break;
            }
        case CHtiPIMServicePlugin::EDeleteSimContact:
            {
            TRAP(err ,HandleSimContactDeleteL(aMessage.Right(aMessage.Length() - 1)));
            break;
            }
        default:
            {
            SendErrorMessageL(KErrArgument, KErrorUnrecognizedCommand);
            return;
            }
        }

    if (err != KErrNone)
        {
        iIsBusy = EFalse;
        User::Leave(err);
        } 
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::ProcessMessageL" );
    }

// -----------------------------------------------------------------------------
// CHtiSimDirHandler::IsBusy
// -----------------------------------------------------------------------------
//
TBool CHtiSimDirHandler::IsBusy()
    {
    return iIsBusy;
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::HandleSimCardInfoL
// Gets information about the SIM card.
// ----------------------------------------------------------------------------
void CHtiSimDirHandler::HandleSimCardInfoL(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::HandleSimCardInfoL" );
    if (aData.Length() != 0)
        {
        HTI_LOG_TEXT( "CHtiSimDirHandler: wrong length of data" );
        SendErrorMessageL(KErrArgument, KErrorInvalidParameters);
        return;
        }

    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg etelStoreInfoPckg(etelStoreInfo);
    
    TRequestStatus requestStatus;
    iEtelStore.GetInfo(requestStatus, (TDes8&) etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone)
        {
        HTI_LOG_TEXT( "CHtiSimDirHandler: Failed to get SIM card info" );
        SendErrorMessageL(requestStatus.Int(), KErrorSimCardInfoFailed);
        return;
        }

    // Create and send response message
    TBuf8<KSimInfoResponseLength> reply;
    reply.Append(etelStoreInfo.iMaxSecondNames > 0 ? etelStoreInfo.iMaxSecondNames : 0);
    reply.Append(etelStoreInfo.iMaxAdditionalNumbers > 0 ? etelStoreInfo.iMaxAdditionalNumbers : 0);
    reply.Append(etelStoreInfo.iMaxEmailAddr > 0 ? etelStoreInfo.iMaxEmailAddr : 0);
    reply.Append(etelStoreInfo.iMaxTextLength > 0 ? etelStoreInfo.iMaxTextLength : 0);
    reply.Append(etelStoreInfo.iMaxNumLength > 0 ? etelStoreInfo.iMaxNumLength : 0);
    reply.Append(etelStoreInfo.iMaxTextLengthSecondName > 0 ? 
        etelStoreInfo.iMaxTextLengthSecondName : 0);
    reply.Append(etelStoreInfo.iMaxNumLengthAdditionalNumber > 0?
        etelStoreInfo.iMaxNumLengthAdditionalNumber : 0);
    reply.Append(etelStoreInfo.iMaxTextLengthEmailAddr > 0 ? etelStoreInfo.iMaxTextLengthEmailAddr : 0);
    reply.Append( ( TUint8* ) ( &etelStoreInfo.iTotalEntries ), 2 );
    reply.Append( ( TUint8* ) ( &etelStoreInfo.iUsedEntries ), 2 );
    SendOkMsgL(reply);
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::HandleSimCardInfoL" );
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::HandleSimContactImportL
// Imports the contact to SIM card.
// ----------------------------------------------------------------------------
void CHtiSimDirHandler::HandleSimContactImportL(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::HandleSimContactImportL" );
    
    if(CheckImportMsg(aData) == EFalse)
        {
        return;
        }
    
    RBuf8 buffer;
    buffer.CreateL(KOneSimContactBufferSize);
    CleanupClosePushL(buffer);
    CPhoneBookBuffer* pbBuffer = new (ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    pbBuffer->Set(&buffer);

    //add new enty tag
    User::LeaveIfError(pbBuffer->AddNewEntryTag());

    TInt offset = 0;
    TInt fieldCount = aData[offset];
    offset++;

    for (TInt i = 0; i < fieldCount; i++)
        {
        HTI_LOG_FORMAT( "Processing field %d", i + 1 );

        TContactFieldType type = (TContactFieldType) aData[offset];
        offset++;
        TInt fieldLength = aData[offset];
        offset++;
        HBufC* fieldData = HBufC::NewLC(fieldLength);
        fieldData->Des().Copy(aData.Mid(offset, fieldLength));
        switch (type)
            {
            case ENameField:
                User::LeaveIfError(pbBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBText, fieldData->Des()));
                break;
            case ESecondNameField:
                User::LeaveIfError(pbBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBSecondName,
                        fieldData->Des()));
                break;
            case EPhoneNumberField:
                User::LeaveIfError(
                        pbBuffer->PutTagAndValue(
                                RMobilePhoneBookStore::ETagPBNumber,
                                fieldData->Des()));
                break;
            case EEMailField:
                User::LeaveIfError(pbBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBEmailAddress,
                        fieldData->Des()));
                break;
            case EAdditNumberField:
                User::LeaveIfError(pbBuffer->AddNewNumberTag());
                User::LeaveIfError(
                        pbBuffer->PutTagAndValue(
                                RMobilePhoneBookStore::ETagPBNumber,
                                fieldData->Des()));
                break;
            default:
                HTI_LOG_FORMAT( "Unknown field type %d", type );
                User::Leave(KErrArgument);
                break;
            }
        CleanupStack::PopAndDestroy(); // fieldData
        offset += fieldLength;
        }
    
    // save contact into sim card
    TInt index = -1;
    TRequestStatus status;
    //store the entry in the first free location and then return 
    //this location within index when it completes the request
    iEtelStore.Write(status, buffer, index);
    User::WaitForRequest(status);
    if(status.Int() != KErrNone)
        {
        HTI_LOG_TEXT("Failed to add SIM contact");
        SendErrorMessageL( status.Int(), KErrorImportFailed );
        }
    else
        {
        HTI_LOG_TEXT( "SIM contact added" );
        TBuf8<4> idBuf;
        idBuf.Append( ( TUint8* ) &index, 4 );
        SendOkMsgL( idBuf );
        }
    CleanupStack::PopAndDestroy(2); // buffer, pbBuffer

    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::HandleSimContactImportL" );
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::HandleSimContactDeleteL
// Creates a contact view containing the contacts to be deleted.
// ----------------------------------------------------------------------------
void CHtiSimDirHandler::HandleSimContactDeleteL(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::HandleSimContactDeleteL" );
    TInt dataLength = aData.Length();
    if ( dataLength != 0 && dataLength != 4 )
        {
        HTI_LOG_TEXT( "CHtiSimDirHandler: Wrong length of data" )
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return;
        }
    
    TRequestStatus status;
    if (dataLength == 0) //delete all
        {
        iEtelStore.DeleteAll(status);
        HTI_LOG_TEXT("Delete all SIM contacts");
        }
    else //delete one contact with given id
        {
        TInt id = aData[0] + (aData[1] << 8) + (aData[2] << 16) + (aData[3]
                << 24);
        HTI_LOG_FORMAT( "Delete SIM contact with id %d", id );
        iEtelStore.Delete(status, id);
        }
    
    User::WaitForRequest(status);
    if(status.Int() != KErrNone)
        {
        HTI_LOG_TEXT("Failed to delete contact(s)");
        SendErrorMessageL( status.Int(), KErrorDeleteFailed );
        }
    else
        {
        HTI_LOG_TEXT("SIM contact(s) deleted");
        SendOkMsgL( KNullDesC8 );
        }
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::HandleSimContactDeleteL" );
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::CheckImportMsg
// Validates the syntax of import contact message.
// ----------------------------------------------------------------------------
TBool CHtiSimDirHandler::CheckImportMsg(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::CheckImportMsg" );
    // Import command syntax:
    //   amount of fields     (1 byte)   __
    //   type of field        (1 byte)     |
    //   length of data field (1 byte)     | repeated <amount of fields> times
    //   field data           (variable) __|

    TInt length = aData.Length();
    if (length < 4) // min length 4 bytes
        {
        HTI_LOG_FORMAT( "Message too short %d", length );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg etelStoreInfoPckg(etelStoreInfo);
    
    TRequestStatus requestStatus;
    iEtelStore.GetInfo(requestStatus, (TDes8&) etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone)
        {
        HTI_LOG_TEXT( "CHtiSimDirHandler: Failed to get SIM card info" );
        SendErrorMessageL(requestStatus.Int(), KErrorSimCardInfoFailed);
        return EFalse;
        }
    TInt offset = 0;
    TInt fieldCount = aData[offset];
    HTI_LOG_FORMAT( "Fields %d", fieldCount );
    if (fieldCount < 1) // must be at least one field
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    offset++;
    TInt fieldsFound = 0;
    while (offset < length)
        {
        fieldsFound++;
        TContactFieldType fieldType = (TContactFieldType) aData[offset];
        HTI_LOG_FORMAT( "Field type %d", fieldType );
        TInt maxLength = 0;
        if(fieldType == ENameField)
            {
            maxLength = etelStoreInfo.iMaxTextLength;
            }
        else if(fieldType == ESecondNameField)
            {
            maxLength = etelStoreInfo.iMaxTextLengthSecondName;
            }
        else if(fieldType == EPhoneNumberField)
            {
            maxLength = etelStoreInfo.iMaxNumLength;
            }
        else if(fieldType == EEMailField)
            {
            maxLength = etelStoreInfo.iMaxTextLengthEmailAddr;
            }
        else if(fieldType == EAdditNumberField)
            {
            maxLength = etelStoreInfo.iMaxNumLengthAdditionalNumber;
            }
        else
            {
            HTI_LOG_TEXT("Unknown field type");
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            return EFalse; // invalid field type
            }
        
        if(maxLength <= 0)
            {
            HTI_LOG_TEXT("Field not supported");
            SendErrorMessageL(KErrArgument, KErrorFieldNotSupported);
            return EFalse;
            }

        offset++; // the type of field byte
        if (offset >= length)
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            return EFalse;
            }
        TInt fieldLength = aData[offset];
        HTI_LOG_FORMAT( "Field length %d", fieldLength );
        if (fieldLength < 1)
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            return EFalse; // Field data can not be empty
            }
        else if(fieldLength > maxLength)
            {
            HTI_LOG_TEXT("The length of field is too long");
            SendErrorMessageL( KErrArgument, KErrorFieldTooBig );
            return EFalse;
            }
        offset++; // advance over the length of data byte
        offset += fieldLength; // and the field data
        }

    if (offset == length && fieldsFound == fieldCount)
        {
        HTI_LOG_TEXT( "Message OK" );
        return ETrue;
        }
    
    SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiSimDirHandler::SendOkMsgL(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::SendOkMsgL" );
    iIsBusy = EFalse; // Done with the current request
    User::LeaveIfNull(iDispatcher);
    HBufC8* temp = HBufC8::NewL(aData.Length() + 1);
    TPtr8 response = temp->Des();
    response.Append((TChar) CHtiPIMServicePlugin::EResultOk);
    response.Append(aData);
    User::LeaveIfError(iDispatcher->DispatchOutgoingMessage(temp,
            KPIMServiceUid));
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::SendOkMsgL" );
    }

// ----------------------------------------------------------------------------
// CHtiSimDirHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiSimDirHandler::SendErrorMessageL(TInt aError,
        const TDesC8& aDescription)
    {
    HTI_LOG_FUNC_IN( "CHtiSimDirHandler::SendErrorMessageL" );
    iIsBusy = EFalse; // Done with the current request
    User::LeaveIfNull(iDispatcher);
    User::LeaveIfError(iDispatcher->DispatchOutgoingErrorMessage(aError,
            aDescription, KPIMServiceUid));
    HTI_LOG_FUNC_OUT( "CHtiSimDirHandler::SendErrorMessageL" );
    }

// End of file
