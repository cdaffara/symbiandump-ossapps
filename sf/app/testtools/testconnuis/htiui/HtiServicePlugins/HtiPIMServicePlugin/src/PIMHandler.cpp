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
* Description:  Functional implementation of PIM service (for vCalendar, vCard)
*
*/


// INCLUDE FILES
#include "../../../symbian_version.hrh"

#include "HtiPIMServicePlugin.h"
#include "PIMHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <utf.h>
#include <calcommon.h>
#include <calsession.h>
#include <calentryview.h>

#include <cntdb.h> 
#include <cntitem.h> 
#include <cntfldst.h> 
#include <cntvcard.h> 
#include <cntfilt.h> 
#include <caldataexchange.h> 
#include <caldataformat.h> 

// CONSTANTS
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
_LIT8( KErrorCalendarFileFormat, "Invalid format of calendar file name");
_LIT8( KErrorVCardImportFailed, "vCard import failed" );
_LIT8( KErrorVCalendarImportFailed, "vCalendar import failed" );
_LIT8( KErrorMissingVCalendar, "Missing vCalendar object" );
_LIT8( KErrorMissingVCard, "Missing vCard object" );
_LIT8( KErrorInvalidId, "Invalid ID parameter" );
_LIT8( KErrorItemNotFound, "Item not found" );
_LIT8( KErrorFailedDelete, "Failed to delete item" );
_LIT8( KErrorFailedDeleteAll, "Failed to delete all items" );
_LIT8( KErrorFailedOpenCalendar, "Failed to open calendar file");
//_LIT8( KErrorFailedOpenContact, "Failed to open contact database");



_LIT( KDefaultAgendaFile, "" ); // A default file is opened if fileName is KNullDesC

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )  
_LIT8( KErrorMissingText, "Text parameter missing" );
_LIT8( KErrorMissingFilepath, "Filepath parameter missing" );
_LIT8( KErrorNotepadAddMemoFailed, "Notepad add memo failed" );
_LIT8( KErrorNotepadAddMemoFromFileFailed, "Notepad add memo from file failed" );
_LIT8( KErrorNotepadDeleteAllFailed, "Notepad delete all failed" );

_LIT( KHtiNpdHlpExe,       "HtiNpdHlp.exe" );
_LIT( KCmdAddMemo,         "AddMemo" );
_LIT( KCmdAddMemoFromFile, "AddMemoFromFile" );
_LIT( KCmdDeleteAll,       "DeleteAll" );
_LIT( KCmdDelim,           " " );
#else
_LIT8( KErrorNotepadNotSupported, "Notepad not supported" );
#endif

// ----------------------------------------------------------------------------
CPIMHandler* CPIMHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::NewL" );
    CPIMHandler* self = new (ELeave) CPIMHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CPIMHandler::Done" );
    return self;
    }

// ----------------------------------------------------------------------------
CPIMHandler::CPIMHandler():iIsBusy( EFalse ), iEntryViewErr( KErrNone ),
        iCalSession(NULL)
    {
    }

// ----------------------------------------------------------------------------
CPIMHandler::~CPIMHandler()
    {
    HTI_LOG_TEXT( "CPIMHandler destroy" );
    HTI_LOG_TEXT( "Deleting iCalSession");
    delete iCalSession;
    HTI_LOG_TEXT( "Deleting iWaiter" );
    delete iWaiter;
    }

// ----------------------------------------------------------------------------
void CPIMHandler::ConstructL()
    {
    HTI_LOG_TEXT( "CPIMHandler::ConstructL" );
    iWaiter = new ( ELeave ) CActiveSchedulerWait;
    }

// ----------------------------------------------------------------------------
void CPIMHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;
    }

// ----------------------------------------------------------------------------
void CPIMHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::ProcessMessageL" );

    iIsBusy = ETrue;
    TInt err = KErrNone;

    // Zero legth of aMessage tested already in CHtiPIMServicePlugin.
    // Other sanity checks must be done here.

    TUint8 command = aMessage.Ptr()[0];
    switch ( command )
        {
        case CHtiPIMServicePlugin::EImportVCard:
            {
            TRAP( err, HandleVCardImportFuncL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::EImportVCalendar:
            {
            TRAP( err, HandleVCalendarImportFuncL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::EDeleteContact:
            {
            TRAP( err, HandleContactDeleteFuncL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::EDeleteCalendar:
            {
            TRAP( err, HandleCalendarDeleteFuncL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::EOpenCalendarFile:
            {
            TRAP( err, HandleOpenCalendarFileL(
                    aMessage.Right( aMessage.Length() - 1 )));
            break;
            }
        case CHtiPIMServicePlugin::EListCalendarFiles:
            {
            TRAP( err, HandleListCalendarFilesL());
            break;
            }
        case CHtiPIMServicePlugin::ENotepadAddMemo:
            {
            TRAP( err, HandleNotepadAddMemoFuncL( aMessage.Mid( 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::ENotepadAddMemoFromFile:
            {
            TRAP( err, HandleNotepadAddMemoFromFileFuncL( aMessage.Mid( 1 ) ) );
            break;
            }
        case CHtiPIMServicePlugin::ENotepadDeleteAll:
            {
            TRAP( err, HandleNotepadDeleteAllFuncL() );
            break;
            }
        default:
            {
            TRAP( err, SendErrorMessageL(
                    KErrArgument, KErrorUnrecognizedCommand ) );
            break;
            }
        }

    if ( err != KErrNone )
        {
        iIsBusy = EFalse;
        User::Leave( err );
        }

    HTI_LOG_FUNC_OUT( "CPIMHandler::ProcessMessageL: Done" );
    }

// ----------------------------------------------------------------------------
TBool CPIMHandler::IsBusy()
    {
    return iIsBusy;
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleOpenCalendarFileL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleOpenCalendarFileL" );
    
    // check the format, the correct format is DriveLetter:FileName
    if (aData.Length() != 0)
        {
        if(aData.Length() < 3 || aData[1] != ':'||
                !(aData[0] >= 'a' && aData[0] <= 'z' || aData[0] >= 'A' && aData[0] <= 'Z'))
            {
            SendErrorMessageL( KErrArgument, KErrorCalendarFileFormat );
            return;
            }
        }
    
    delete iCalSession;
    iCalSession = NULL;
    
    // Open iCalSession
    HTI_LOG_TEXT("Open calendar session");
    iCalSession = CCalSession::NewL();
    TBuf<KMaxFileName + 2> calFile;
    calFile.Copy(aData);
    TRAPD(err, iCalSession->OpenL(calFile));
    if(err == KErrNone)
        {
        HTI_LOG_TEXT("Calendar session open");
        SendOkMsgL( KNullDesC8 );
        }
    else
        {
        HTI_LOG_TEXT("Failed to open calendar file");
        SendErrorMessageL( err, KErrorFailedOpenCalendar );
        delete iCalSession;
        iCalSession = NULL;
        }
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleOpenCalendarFileL: Done" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleListCalendarFilesL()
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleListCalendarFiles" );
    
    CCalSession* calSession = CCalSession::NewL();
    CleanupStack::PushL(calSession);
    HTI_LOG_TEXT("List all calendar files");
    CDesCArray* calFiles = calSession->ListCalFilesL();
    
    if(calFiles == NULL) //No calendar files
        {
        TBuf8<2> reply;
        reply.AppendFill(0, 2);
        SendOkMsgL(reply);
        CleanupStack::PopAndDestroy(); // calSession
        return;
        }
    
    CleanupStack::PushL(calFiles);
    TInt count = calFiles->Count();
    
    // files count + file count * (file name length + max file name + driver letter + ':')
    TInt bufSize = 2 + count * (2 + KMaxFileName + 1 + 1);
    CBufFlat* calListBuf = CBufFlat::NewL( bufSize );
    CleanupStack::PushL(calListBuf);
    
    HBufC8* calArray = HBufC8::NewLC( bufSize );
    TPtr8 calArrayPtr = calArray->Des();
    
    calArrayPtr.Append((TUint8*)(&count), 2);
    TInt pos = 0;
    calListBuf->ExpandL(pos, 2);
    calListBuf->Write(pos, *calArray, 2);
    calArrayPtr.Zero();
    pos += 2;
    
    for(int i = 0; i < count; ++i)
        {
        TInt len = calFiles->MdcaPoint(i).Length();
        calArrayPtr.Append((TUint8*)(&len), 2);
        calArrayPtr.Append(calFiles->MdcaPoint(i));
        calListBuf->ExpandL(pos, calArray->Length());
        calListBuf->Write(pos,*calArray, calArray->Length());
        pos += calArray->Length();
        calArrayPtr.Zero();
        }
    SendOkMsgL( calListBuf->Ptr( 0 ) );
    CleanupStack::PopAndDestroy(4); //calArray, calListBuf, calFiles, calSession;    
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleListCalendarFiles: Done" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleVCardImportFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleVCardImportFuncL" );
    if ( aData.Length() == 0 )
        {
        SendErrorMessageL( KErrArgument, KErrorMissingVCard );
        return;
        }
    
    CContactDatabase* contactDatabase = CContactDatabase::OpenL();
    HTI_LOG_TEXT( "CPIMHandler: Contact database open" );
    CleanupStack::PushL(contactDatabase);
    
    CBufFlat* buffer = CBufFlat::NewL(aData.Length());
    CleanupStack::PushL(buffer);
    buffer->ExpandL(0, aData.Length());
    buffer->Ptr(0).Copy(aData.Right(aData.Length()));
    RBufReadStream readStream;
    readStream.Open(*buffer, 0);
    CleanupClosePushL<RBufReadStream>( readStream );
    
    // Imports vCard
    TBool success = EFalse;
    TUid format = TUid::Uid(KUidVCardConvDefaultImpl);
    CArrayPtr<CContactItem>* contacts = NULL;
    TRAPD(err,contacts = contactDatabase->ImportContactsL(format, readStream, success, 
            CContactDatabase::EImportSingleContact | CContactDatabase::ETTFormat));
    CleanupStack::PushL(contacts);
    if(err != KErrNone || success == EFalse || contacts == NULL || contacts->Count() == 0)
        {
        HTI_LOG_TEXT("Failed to import vCard");
        SendErrorMessageL( err, KErrorVCardImportFailed );
        }
    else
        {
        // Returns the imported contact id 
        TInt32 entryId = contacts->At(0)->Id();
        TBuf8<4> idBuf;
        idBuf.Append( ( TUint8* ) &entryId, 4 );
        HTI_LOG_TEXT("vCard imported");
        SendOkMsgL( idBuf );
        }
    
    contacts->ResetAndDestroy();
    
    CleanupStack::PopAndDestroy(4); // contacts, readStream, buffer, contactDatabase
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleVCardImportFuncL: Done" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleVCalendarImportFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleVCalendarImportFuncL" );

    if ( aData.Length() == 0 )
        {
        HTI_LOG_TEXT( "CPIMHandler::HandleVCalendarImportFuncL: Error: length of data is zero" )
        SendErrorMessageL( KErrArgument, KErrorMissingVCalendar );
        return;
        }

    CBufFlat* buffer = CBufFlat::NewL( aData.Length() );
    CleanupStack::PushL(buffer);
    buffer->ExpandL( 0, aData.Length() );
    buffer->Ptr( 0 ).Copy( aData );
    RBufReadStream readStream;
    readStream.Open( *buffer, 0 );
    CleanupClosePushL( readStream );

    if(iCalSession == NULL)
        {
        HTI_LOG_TEXT( "CPIMHandler: Open default calendar file" );
        iCalSession = CCalSession::NewL();
        iCalSession->OpenL( KDefaultAgendaFile );
        HTI_LOG_TEXT( "CPIMHandler: Calendar session open" );
        }

    CCalDataExchange* importer = CCalDataExchange::NewL(*iCalSession);
    CleanupStack::PushL( importer );
    HTI_LOG_TEXT( "CPIMHandler: Calendar importer created" );

    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL( entryArray );

    TInt err = KErrNone;
    TInt size = 0;
    // Import as VCalendar
    TRAP( err, importer->ImportL( KUidVCalendar, readStream, entryArray ) );
    HTI_LOG_FORMAT( "ImportL return value %d", err );
    size = entryArray.Count();
    HTI_LOG_FORMAT( "Import VCalendarL imported %d entries", size );
    TCalLocalUid uniqueId = 0;
    TInt success = 0;
    if ( size > 0 )
        {
        CCalEntryView* entryView = CCalEntryView::NewL( *iCalSession, *this );
        iWaiter->Start();
        CleanupStack::PushL(entryView);
        if ( iEntryViewErr == KErrNone )
            {
            TRAP( err, entryView->StoreL( entryArray, success ) );
            HTI_LOG_FORMAT( "StoreL return value %d", err );
            HTI_LOG_FORMAT( "Successfully stored %d entries", success );
            uniqueId = entryArray[0]->LocalUidL();
            }
        CleanupStack::PopAndDestroy();
        }
    entryArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy(); // entryArray

    if ( err == KErrNone && success > 0 )
        {
        HTI_LOG_TEXT("vCalendar imported");
        TBuf8<8> uniqueIdStr;
        uniqueIdStr.Copy( ( TUint8* ) ( &uniqueId ), sizeof( uniqueId ) );
        SendOkMsgL( uniqueIdStr );
        }
    else
        {
        HTI_LOG_TEXT("Failed to import vCalendar");
        if ( err == KErrNone ) 
            {
            err = KErrGeneral;
            }
        SendErrorMessageL( err, KErrorVCalendarImportFailed );
        }

    CleanupStack::PopAndDestroy( 3 ); // buffer, readStream, importer
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleVCalendarImportFuncL: Done" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleContactDeleteFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleContactDeleteFuncL" );
    TInt dataLength = aData.Length();
    if ( dataLength != 0 && dataLength != 4 )
        {
        HTI_LOG_TEXT( "CPIMHandler: Error: wrong length of data" )
        SendErrorMessageL( KErrArgument, KErrorInvalidId );
        return;
        }
    CContactDatabase* contactDatabase = CContactDatabase::OpenL();
    HTI_LOG_TEXT( "CPIMHandler: Contact database open" );
    CleanupStack::PushL(contactDatabase);
    
    if(dataLength == 0)   // delete all contacts
        {
        //const CContactIdArray* array = iContactDatabase->SortedItemsL();
        CCntFilter *filter = CCntFilter::NewLC();
        filter->SetContactFilterTypeCard(ETrue);
        filter->SetContactFilterTypeGroup(EFalse);
        contactDatabase->FilterDatabaseL(*filter);
        TRAPD(err, contactDatabase->DeleteContactsL(*filter->iIds));
        CleanupStack::PopAndDestroy();
        if(err == KErrNone)
            {
            HTI_LOG_TEXT("All contacts deleted");
            SendOkMsgL( KNullDesC8 );
            }
        else
            {
            HTI_LOG_TEXT("Failed to delete all contacts");
            SendErrorMessageL( err, KErrorFailedDeleteAll );
            }
        }
    else  // delete one contact by id
        {
        TUint id = aData[0] + ( aData[1] << 8 )
                + ( aData[2] << 16 )
                + ( aData[3] << 24 );
        TRAPD(err, contactDatabase->DeleteContactL(id));
        
        if(err == KErrNone)
            {
            HTI_LOG_TEXT("Contact deleted");
            SendOkMsgL( KNullDesC8 );
            }
        else
            {
            HTI_LOG_TEXT("Failed to delete contact");
            SendErrorMessageL( err, KErrorFailedDelete );
            }
        }
    
    CleanupStack::PopAndDestroy();
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleContactDeleteFuncL" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleCalendarDeleteFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleVCalendarDeleteFuncL" );

    TInt dataLength = aData.Length();
    if ( dataLength != 0 && dataLength != 4 )
        {
        HTI_LOG_TEXT( "CPIMHandler: Error: wrong length of data" )
        SendErrorMessageL( KErrArgument, KErrorInvalidId );
        return;
        }


    if(iCalSession == NULL)
        {
        HTI_LOG_TEXT( "CPIMHandler: Open default calendar file" );
        iCalSession = CCalSession::NewL();
        iCalSession->OpenL( KDefaultAgendaFile );
        HTI_LOG_TEXT( "CPIMHandler: Calendar session open" );
        }

    HTI_LOG_TEXT( "CPIMHandler: Creating entry view" );
    CCalEntryView* entryView = CCalEntryView::NewL( *iCalSession, *this );
    iWaiter->Start();
    CleanupStack::PushL(entryView);
    if ( iEntryViewErr != KErrNone )
        {
        User::Leave( iEntryViewErr );
        }

    // If dataLength is 0, no ID given, delete all calendar entries
    if ( dataLength == 0 )
        {
        HTI_LOG_TEXT( "CPIMHandler: Deleting all calendar entries" );
        TCalTime minTime;
        TCalTime maxTime;
        minTime.SetTimeUtcL( TCalTime::MinTime() );
        maxTime.SetTimeUtcL( TCalTime::MaxTime() );
        CalCommon::TCalTimeRange timeRange( minTime, maxTime );
        TRAPD( err, entryView->DeleteL( timeRange,
                CalCommon::EIncludeAll, *this ) );
        iWaiter->Start();
        if ( err == KErrNone && iEntryViewErr == KErrNone )
            {
            HTI_LOG_TEXT("All calendar entries deleted");
            SendOkMsgL( KNullDesC8 );
            }
        else
            {
            HTI_LOG_TEXT("Failed to delete all calendar entries");
            SendErrorMessageL( KErrGeneral, KErrorFailedDeleteAll );
            }
        }

    // If id given, delete only calendar entry having that id
    else
        {
        TCalLocalUid id = aData[0] + ( aData[1] << 8 )
                             + ( aData[2] << 16 )
                             + ( aData[3] << 24 );
        HTI_LOG_FORMAT( "CPIMHandler: Deleting one calendar entry %d", id );
        CCalEntry* entryToDelete = NULL;
        TRAPD( err, entryToDelete = entryView->FetchL( id ) );

        if ( err || entryToDelete == NULL )
            {
            HTI_LOG_TEXT( "CPIMHandler: Calendar entry not found" );
            SendErrorMessageL( KErrNotFound, KErrorItemNotFound );
            }
        else
            {
            CleanupStack::PushL( entryToDelete );
            TRAP( err, entryView->DeleteL( *entryToDelete ) );
            if ( err == KErrNone )
                {
                HTI_LOG_TEXT("calendar entrie deleted");
                SendOkMsgL( KNullDesC8 );
                }
            else
                {
                HTI_LOG_TEXT( "Failed to delete calendar entry" )
                SendErrorMessageL( KErrGeneral, KErrorFailedDelete );
                }
            CleanupStack::PopAndDestroy( entryToDelete );
            }
        }
    CleanupStack::PopAndDestroy(); //entryView;
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleVCalendarDeleteFuncL" );
    }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )  
TInt CallNpdHlp( const TDesC& aCmd )
    {
    HTI_LOG_FUNC_IN( "CallNpdHlp" );

    RProcess HtiNpdHlp;
    TInt err = HtiNpdHlp.Create( KHtiNpdHlpExe, aCmd );
    if ( err )
        {
        HTI_LOG_FORMAT( "Could not create HtiNpdHlp.Exe process %d", err );
        return err;
        }

    TRequestStatus status;
    HtiNpdHlp.Logon( status );
    HtiNpdHlp.Resume();
    User::WaitForRequest( status );
    if ( status.Int() != KErrNone )
        {
        HTI_LOG_FORMAT( "status     %d", status.Int() );
        HTI_LOG_FORMAT( "ExitReason %d", HtiNpdHlp.ExitReason() );
        HTI_LOG_FORMAT( "ExitType   %d", HtiNpdHlp.ExitType() );
        HtiNpdHlp.Close();
        return status.Int();
        }

    HtiNpdHlp.Close();

    HTI_LOG_FUNC_OUT( "CallNpdHlp" );
    return KErrNone;
    }
#endif

// ----------------------------------------------------------------------------
void CPIMHandler::HandleNotepadAddMemoFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleNotepadAddMemoFuncL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 ) 
    if ( aData.Length() < 1 )
        {
        SendErrorMessageL( KErrArgument, KErrorMissingText );
        return;
        }

    // convert text from TDesC8 -> TDesC
    // expecting the input TDesC8 contains UTF-8 data
    HBufC* text = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aData );
    HTI_LOG_TEXT( "CPIMHandler: Conversion to Unicode done" );
    CleanupStack::PushL( text );

    HBufC* cmd = HBufC::NewLC( KCmdAddMemo().Length() + 1 + ( *text ).Length() );
    cmd->Des().Copy( KCmdAddMemo );
    cmd->Des().Append( KCmdDelim );
    cmd->Des().Append( *text );

    TInt err = CallNpdHlp( *cmd );
    if ( err )
        {
        SendErrorMessageL( err, KErrorNotepadAddMemoFailed );
        }
    else
        {
        SendNotepadOkMsgL( CHtiPIMServicePlugin::ENotepadAddMemo );
        }

    CleanupStack::PopAndDestroy( 2 ); // text, cmd
#else
    SendErrorMessageL(KErrNotSupported, KErrorNotepadNotSupported);
#endif   
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleNotepadAddMemoFuncL" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleNotepadAddMemoFromFileFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleNotepadAddMemoFromFileFuncL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 ) 
    if ( aData.Length() < 1 )
        {
        SendErrorMessageL( KErrArgument, KErrorMissingFilepath );
        return;
        }

    // convert filename from TDesC8 -> TDesC
    // expecting the input TDesC8 contains UTF-8 data
    HBufC* filename = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aData );
    HTI_LOG_TEXT( "CPIMHandler: Conversion to Unicode done" );
    CleanupStack::PushL( filename );

    HBufC* cmd = HBufC::NewLC( KCmdAddMemoFromFile().Length() + 1 + ( *filename ).Length() );
    cmd->Des().Copy( KCmdAddMemoFromFile );
    cmd->Des().Append( KCmdDelim );
    cmd->Des().Append( *filename );

    TInt err = CallNpdHlp( *cmd );
    if ( err )
        {
        SendErrorMessageL( err, KErrorNotepadAddMemoFromFileFailed );
        }
    else
        {
        SendNotepadOkMsgL( CHtiPIMServicePlugin::ENotepadAddMemoFromFile );
        }

    CleanupStack::PopAndDestroy( 2 ); // filename, cmd
#else
    SendErrorMessageL(KErrNotSupported, KErrorNotepadNotSupported);
#endif 
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleNotepadAddMemoFromFileFuncL" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::HandleNotepadDeleteAllFuncL()
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::HandleNotepadDeleteAllFuncL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 ) 
    TInt err = CallNpdHlp( KCmdDeleteAll() );
    if ( err )
        {
        SendErrorMessageL( err, KErrorNotepadDeleteAllFailed );
        }
    else
        {
        SendNotepadOkMsgL( CHtiPIMServicePlugin::ENotepadDeleteAll );
        }
#else
    SendErrorMessageL(KErrNotSupported, KErrorNotepadNotSupported);
#endif 
    HTI_LOG_FUNC_OUT( "CPIMHandler::HandleNotepadDeleteAllFuncL" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::SendNotepadOkMsgL( CHtiPIMServicePlugin::TCommand aCommand )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::SendNotepadOkMsgL" );
    TBuf8<1> msg;
    msg.Append( aCommand );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
                        msg.AllocL(), KPIMServiceUid ) );
    iIsBusy = EFalse;
    HTI_LOG_FUNC_OUT( "CPIMHandler::SendNotepadOkMsgL" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::SendOkMsgL: Starting" );

    User::LeaveIfNull( iDispatcher );

    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiPIMServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KPIMServiceUid ) );
    iIsBusy = EFalse;
    HTI_LOG_FUNC_OUT( "CPIMHandler::SendOkMsgL: Done" );
    }

// ----------------------------------------------------------------------------
void CPIMHandler::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::SendErrorMessageL: Starting" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KPIMServiceUid ) );
    iIsBusy = EFalse;
    HTI_LOG_FUNC_OUT( "CPIMHandler::SendErrorMessageL: Done" );
    }

// ----------------------------------------------------------------------------
// CPIMHandler::Progress
// Called during calendar entry view creation and operations.
// Called only if NotifyProgress returns ETrue.
// From MCalProgressCallBack
// ----------------------------------------------------------------------------
void CPIMHandler::Progress( TInt /*aProgress*/ )
    {
    }

// ----------------------------------------------------------------------------
// CPIMHandler::Completed
// Called on completion of calendar entry view creation and operations
// From MCalProgressCallBack
// ----------------------------------------------------------------------------
void CPIMHandler::Completed( TInt aError )
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::Completed" );
    HTI_LOG_FORMAT( "Completed with error code %d", aError );
    iEntryViewErr = aError;
    iWaiter->AsyncStop();
    HTI_LOG_FUNC_OUT( "CPIMHandler::Completed" );
    }

// ----------------------------------------------------------------------------
// CPIMHandler::NotifyProgress
// Returns whether or not progress notification is required
// From MCalProgressCallBack
// ----------------------------------------------------------------------------
TBool CPIMHandler::NotifyProgress()
    {
    HTI_LOG_FUNC_IN( "CPIMHandler::NotifyProgress" );
    HTI_LOG_FUNC_OUT( "CPIMHandler::NotifyProgress" );
    return EFalse; // Don't notify about progress
    }
