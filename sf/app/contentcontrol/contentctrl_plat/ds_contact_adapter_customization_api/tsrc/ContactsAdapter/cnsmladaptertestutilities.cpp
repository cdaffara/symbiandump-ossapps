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
* Description:  Source code file for contact adapter test utilities.
*
*/

#include "cnsmladaptertestutilities.h"
#include <eunitmacros.h>
#include <utf.h> 
#include <cntitem.h>  // CContactCard
#include <cntfield.h> // CContactItemField
#include <cntfldst.h> // CContactTextField
#include <s32mem.H>

namespace
    {
    // EUNIT_PRINT will truncate descriptors longer than 512 characters.
    const TInt KEunitPrintMaxLength = 512;
    }

CNSmlAdapterTestUtilities* CNSmlAdapterTestUtilities::NewL()
    {
    CNSmlAdapterTestUtilities* self = CNSmlAdapterTestUtilities::NewLC();
    CleanupStack::Pop();
    return self;
    }

CNSmlAdapterTestUtilities* CNSmlAdapterTestUtilities::NewLC()
    {
    CNSmlAdapterTestUtilities* self = new( ELeave ) CNSmlAdapterTestUtilities();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }

CNSmlAdapterTestUtilities::CNSmlAdapterTestUtilities()
    {
    }

CNSmlAdapterTestUtilities::~CNSmlAdapterTestUtilities()
    {
    iFs.Close();
    delete iIdArray;
    delete iContactDatabase;
    }

void CNSmlAdapterTestUtilities::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

void CNSmlAdapterTestUtilities::OpenVCardFromFileLC( const TDesC& aFileName, RFileReadStream& aReadStream )
    {
    TInt err = aReadStream.Open( iFs, aFileName, EFileRead );
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            EUNIT_PRINT( _L("%S ,vCard file not found"), &aFileName );
            }
        else if ( err == KErrPathNotFound )
            {
            EUNIT_PRINT( _L("%S ,vCard path not found"), &aFileName );        
            }
        else 
            {
            EUNIT_PRINT( _L("%S , vCard file cannot be opened. Error code %d"), &aFileName, err );
            }
        User::Leave( err  );
        }
    EUNIT_PRINT( _L("VCard opened") );
    CleanupClosePushL( aReadStream );
    }

HBufC* CNSmlAdapterTestUtilities::ReadVCardFromFileLC( const TDesC& aFileName )
    {
    HBufC8* vCard8 = ReadVCard8FromFileLC( aFileName );
    HBufC* vCard = HBufC::NewL( vCard8->Length() ); 
    vCard8->Des().Copy( *vCard8 );
    CleanupStack::PopAndDestroy( vCard8 );
    
    CleanupStack::PushL( vCard );    
    return vCard;
    }

HBufC8* CNSmlAdapterTestUtilities::ReadVCard8FromFileLC( const TDesC& aFileName )
    {
    RFileReadStream readStream;
    OpenVCardFromFileLC(aFileName, readStream);
    
    const TInt fileSize = readStream.Source()->SizeL();
    CBufFlat* buffer = CBufFlat::NewL( fileSize );
    CleanupStack::PushL( buffer );

    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    writeStream.WriteL( readStream );
    writeStream.CommitL();
    
    HBufC8* vCard8 = buffer->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy( 3, &readStream );
    
    CleanupStack::PushL( vCard8 );
    return vCard8;
    }

CBufFlat* CNSmlAdapterTestUtilities::ReadBufFromFileLC( const TDesC& aFileName )
    {
    RFileReadStream readStream;
    OpenVCardFromFileLC(aFileName, readStream);
    
    const TInt fileSize = readStream.Source()->SizeL();
    CBufFlat* buffer = CBufFlat::NewL( fileSize );
    CleanupStack::PushL( buffer );

    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    writeStream.WriteL( readStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( buffer );
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PushL( buffer );
    return buffer;
    }

void CNSmlAdapterTestUtilities::SetupEmptyContactDbL( const TDesC& aDbName )
    {
    if( aDbName.Length() > 0 )
        {
        iContactDatabase = CContactDatabase::ReplaceL( aDbName );
        }
    else
        {
        iContactDatabase = CContactDatabase::OpenL();  // Use default contact DB.
        }
    
    CContactIdArray* array = iContactDatabase->ContactsChangedSinceL( Time::NullTTime() );
    CleanupStack::PushL( array );
    iContactDatabase->DeleteContactsL(*array);        
    CleanupStack::PopAndDestroy( array );
    }

/**
 * Setup contact DB with the following contacts
 *   + Tim Tester (id index 0)
 *   + Teddy McTest (id index 1)
 *   + Tod TestHill (id index 2)
 *   + Simon Software (id index 3)
 *   + Cyd Effect (id index 4)
 *   + Nick Name (id index 5)
 * 
 */
void CNSmlAdapterTestUtilities::SetupAndPopulateContactDbL( const TDesC& aDbName )
    {
    if( aDbName.Length() > 0 )
        {
        iContactDatabase = CContactDatabase::ReplaceL( aDbName );
        }
    else
        {
        iContactDatabase = CContactDatabase::OpenL();  // Use default contact DB.
        }
    
    iIdArray = CContactIdArray::NewL();

    // Create test contacts
    iIdArray->AddL( AddContactL( _L( "Tim" ), _L( "Tester" ), _L( "+358501234567" ) ) );
    iIdArray->AddL( AddContactL( _L( "Teddy" ), _L("McTest" ), _L( "+358501112222" ) ) );
    iIdArray->AddL( AddContactL( _L( "Tod" ), _L( "TestHill" ), _L( "+358506666666" ) ) );
    iIdArray->AddL( AddContactL( _L( "Simon" ), _L( "Software" ), _L( "+3585011223333" ) ) );
    iIdArray->AddL( AddContactL( _L( "Cyd" ), _L( "Effect" ), _L( "+358503334444" ) ) );
    iIdArray->AddL( AddContactL( _L( "Nick" ), _L( "Name" ), _L( "+358402223333" ) ) );
    }

// Helper method to add a simple contact to test contact DB
TContactItemId CNSmlAdapterTestUtilities::AddContactL(
        const TDesC& aFirstName,
        const TDesC& aLastName,
        const TDesC& aPhoneNumber )
    {
    _LIT( KLabelFamilyName, "Last name");
    _LIT( KLabelGivenName, "First name");
    _LIT( KLabelPhone, "Phone");
    //_LIT( KLabelNickname, "X-EPOCSECONDNAME");

    CContactCard* card = CContactCard::NewLC();
    
    // Family name field
    CContactItemField* familyNameField =
        CContactItemField::NewLC( KStorageTypeText, KUidContactFieldFamilyName );
    familyNameField->SetLabelL( KLabelFamilyName );
    familyNameField->SetMapping( KUidContactFieldVCardMapUnusedN );
    familyNameField->TextStorage()->SetTextL( aLastName );
    card->AddFieldL( *familyNameField ); // Takes ownership
    CleanupStack::Pop( familyNameField );
    
    // First name field
    CContactItemField* nameField =
    CContactItemField::NewLC( KStorageTypeText, KUidContactFieldGivenName );
    nameField->SetLabelL( KLabelGivenName );
    nameField->SetMapping( KUidContactFieldVCardMapUnusedN );
    nameField->TextStorage()->SetTextL( aFirstName );
    card->AddFieldL( *nameField );
    CleanupStack::Pop(nameField);
    
    // Phone number field
    CContactItemField* phoneField =
        CContactItemField::NewLC( KStorageTypeText, KUidContactFieldPhoneNumber );
    phoneField->SetLabelL( KLabelPhone );
    phoneField->SetMapping( KUidContactFieldVCardMapTEL );
    phoneField->TextStorage()->SetTextL( aPhoneNumber );
    card->AddFieldL( *phoneField );
    CleanupStack::Pop( phoneField );
    
    // Nickname field
    /*if( aNickname.Length() > 0 )
    	{
        CContactItemField* NicknameField =
            CContactItemField::NewLC( KStorageTypeText, KUidContactFieldSecondName );
        NicknameField->SetLabelL( KLabelNickname );
        NicknameField->SetMapping( KUidContactFieldVCardMapSECONDNAME );
        NicknameField->TextStorage()->SetTextL( aNickname );
        card->AddFieldL( *NicknameField );
        CleanupStack::Pop( NicknameField );
    	}*/

    
    TContactItemId cardId = iContactDatabase->AddNewContactL( *card );        
    CleanupStack::PopAndDestroy( card );
    
    return cardId;
    }

/**
 * Empties the test contact DB to its initial state.
 */ 
void CNSmlAdapterTestUtilities::TeardownAndEmptyContactDbL( const TDesC& aDbName )
    {
    iContactDatabase->DeleteContactsL( *iIdArray );
    
    delete iIdArray;
    iIdArray = NULL;

    delete iContactDatabase;
    iContactDatabase = NULL;
    if( aDbName.Length() > 0 )
        {
        TRAP_IGNORE( CContactDatabase::DeleteDatabaseL( aDbName ) );
        }
    }

CContactDatabase* CNSmlAdapterTestUtilities::GetDb()
    {
    EUNIT_ASSERT( iContactDatabase );
    return iContactDatabase;
    }

CContactIdArray* CNSmlAdapterTestUtilities::GetContactArray()
    {
    return iIdArray;
    }

RFs& CNSmlAdapterTestUtilities::FileSession()
    {
    return iFs;
    }

CBufBase* CNSmlAdapterTestUtilities::ExportContactLC( TContactItemId aId )
    {
    CContactDatabase* db = CContactDatabase::OpenL();  // Use default contact DB.
    CleanupStack::PushL( db );
    CContactIdArray* idArr = CContactIdArray::NewLC();
    idArr->AddL( aId );
    CBufFlat* buffer = CBufFlat::NewL( 1024 );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    
    TRAPD( err, iContactDatabase->ExportSelectedContactsL( TUid::Uid(KVersitEntityUidVCard), *idArr, 
        writeStream, CContactDatabase::EExcludeUid, Versit::EUTF8CharSet ) );

    EUNIT_ASSERT_EQUALS_DESC( err, KErrNone, "ExportSelectedContactsL leave" );
    
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( buffer );
    CleanupStack::PopAndDestroy( idArr );
    CleanupStack::PopAndDestroy( db );
    CleanupStack::PushL( buffer );
    return buffer;
    }

void CNSmlAdapterTestUtilities::PrintData( const TDesC& aTitle, CBufBase& aBuf )
    {

    EUNIT_PRINT( aTitle );

    for (TInt i = 0; i < aBuf.Ptr(0).Length(); i += KEunitPrintMaxLength )
        {
        EUNIT_PRINT( aBuf.Ptr(i) );
        }

    }

void CNSmlAdapterTestUtilities::PrintData( const TDesC8& aTitle, CBufBase& aBuf )
    {

    EUNIT_PRINT( aTitle );

    for (TInt i = 0; i < aBuf.Ptr(0).Length(); i += KEunitPrintMaxLength )
        {
        EUNIT_PRINT( aBuf.Ptr(i) );
        }

    }



