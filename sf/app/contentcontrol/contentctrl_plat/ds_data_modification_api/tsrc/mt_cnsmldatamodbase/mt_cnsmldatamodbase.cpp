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
* Description:  Source code file for data mod unit tests.
*
*/


// INCLUDE FILES
#include "MT_CNSmlDataModBase.h"

#include <SmlDataFormat.h>
#include <VCARD.H>
#include <CNTVCARD.H>
#include <BARSC.H>  
#include <s32mem.h>
#include <bautils.h>
#include <S32FILE.H>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <data_caging_path_literals.hrh>

#include "NSmlDataModBase.h"

// - Construction -----------------------------------------------------------

MT_CNSmlDataModBase* MT_CNSmlDataModBase::NewL()
    {
    MT_CNSmlDataModBase* self = MT_CNSmlDataModBase::NewLC();
    CleanupStack::Pop();
    return self;
    }

MT_CNSmlDataModBase* MT_CNSmlDataModBase::NewLC()
    {
    MT_CNSmlDataModBase* self = new( ELeave ) MT_CNSmlDataModBase();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

MT_CNSmlDataModBase::~MT_CNSmlDataModBase()
    {
    delete iDataMod;
    delete iOwnStoreFormat;
    delete iPartnerStoreFormat;
    iOwnStringPool.Close();
    iPartnerStringPool.Close();
    iFs.Close();
    }

MT_CNSmlDataModBase::MT_CNSmlDataModBase()
    {
    }

void MT_CNSmlDataModBase::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    User::LeaveIfError( iFs.Connect() );
    }


// - Test methods -----------------------------------------------------------

void MT_CNSmlDataModBase::SetupL(  )
    {
    iDataMod = new (ELeave) CNSmlVCardMod();
    }

void MT_CNSmlDataModBase::SetupAndConfigure1L()
    {
    iDataMod = new (ELeave) CNSmlVCardMod();
    
    // Same store format for the "client" and the "server"
    // The client supports TITLE
    // The server supports TITLE
    _LIT( KOwnStoreFormat, "DataModTestStoreFormat_1.rsc" );  
    _LIT( KPartnerStoreFormat, "DataModTestStoreFormat_1.rsc" ); 
    ConfigureDataModL( KOwnStoreFormat, KPartnerStoreFormat ); 
    }

void MT_CNSmlDataModBase::SetupAndConfigure2L()
    {
    iDataMod = new (ELeave) CNSmlVCardMod();
    
    // Different store format for the "client" and the "server"
    // The client supports TITLE
    // The server does not support TITLE
    _LIT( KOwnStoreFormat, "DataModTestStoreFormat_1.rsc" );  
    _LIT( KPartnerStoreFormat, "DataModTestStoreFormat_2.rsc" ); 
    ConfigureDataModL( KOwnStoreFormat, KPartnerStoreFormat ); 
    }

void MT_CNSmlDataModBase::Teardown(  )
    {
    delete iDataMod;
    iDataMod = NULL;
    
    delete iOwnStoreFormat;
    iOwnStoreFormat = NULL;
    
    delete iPartnerStoreFormat;
    iPartnerStoreFormat = NULL;

    iOwnStringPool.Close();
    iPartnerStringPool.Close();
    }
    
void MT_CNSmlDataModBase::EmptyL(  )
    {
    }

void MT_CNSmlDataModBase::NeedsMerge1L()
    {
    // Store format not set on Setup phase. No merge supported.
    EUNIT_ASSERT( !iDataMod->NeedsMerge() );
    }

void MT_CNSmlDataModBase::NeedsMerge2L()
    {
    // Store formats sets. Merging is needed.
    EUNIT_ASSERT( iDataMod->NeedsMerge() );
    }

void MT_CNSmlDataModBase::StripTx1L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    // Partner store format not set on Setup phase. Leave should occus.
    EUNIT_ASSERT_SPECIFIC_LEAVE( iDataMod->StripTxL( *buffer1 ), KErrNotFound );
    CleanupStack::PopAndDestroy( buffer1 );
    }

void  MT_CNSmlDataModBase::StripTx2L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    _LIT( KTestVCardNoTitle, "c:\\Data\\TestData\\TestVCard_no_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    // TITLE field shall be not be dropped becuase the field is supported 
    // by the partner store format
    iDataMod->StripTxL( *buffer1 );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    CleanupStack::PopAndDestroy( buffer1 );
    }


void  MT_CNSmlDataModBase::StripTx3L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    // TITLE field shall be dropped because the field is not supported 
    // by the partner store format
    iDataMod->StripTxL( *buffer1 );
    EUNIT_ASSERT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    CleanupStack::PopAndDestroy( buffer1 );
    }

void MT_CNSmlDataModBase::BasicMerge1L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    _LIT( KTestVCardNoTitle, "c:\\Data\\TestData\\TestVCard_no_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // "Received" vCard does not include TITLE field
    CBufFlat* buffer2 = ReadBufFromFileLC( KTestVCardNoTitle ); 
    EUNIT_ASSERT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // Merge vCards. Do not utilize field level merging.
    iDataMod->MergeRxL( *buffer2, *buffer1, EFalse );

    // Merged item _shall not_ include TITLE. It is dropped during the merge.
    EUNIT_ASSERT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    CleanupStack::PopAndDestroy( 2, buffer1 );      
    }

void MT_CNSmlDataModBase::FieldLevelMerge1L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    _LIT( KTestVCardNoTitle, "c:\\Data\\TestData\\TestVCard_no_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // "Received" vCard does not include TITLE field
    CBufFlat* buffer2 = ReadBufFromFileLC( KTestVCardNoTitle );
    EUNIT_ASSERT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // Merge vCards. Utilize field level merging.
    iDataMod->MergeRxL( *buffer2, *buffer1, ETrue );

    // Merged item _shall_ include TITLE. It is was maintained due Field level flag.
    EUNIT_ASSERT_NOT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    CleanupStack::PopAndDestroy( 2, buffer1 );      
    }

void MT_CNSmlDataModBase::BasicMerge2L()
    {
    _LIT( KTestVCardTitle, "c:\\Data\\TestData\\TestVCard_title.vcf" );
    _LIT( KTestVCardNoTitle, "c:\\Data\\TestData\\TestVCard_no_title.vcf" );
    
    // "Local" vCard includes TITLE field
    CBufFlat* buffer1 = ReadBufFromFileLC( KTestVCardTitle );
    EUNIT_ASSERT_NOT_EQUALS( buffer1->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // "Received" vCard does not include TITLE field
    CBufFlat* buffer2 = ReadBufFromFileLC( KTestVCardNoTitle ); 
    EUNIT_ASSERT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );
    
    // Merge vCards. Do not utilize field level merging.
    iDataMod->MergeRxL( *buffer2, *buffer1, EFalse );

    // Merged item shall include TITLE. It is not dropped because
    // partner store format does not support TITLE.
    EUNIT_ASSERT_NOT_EQUALS( buffer2->Ptr(0).Find( _L8("TITLE") ), KErrNotFound );

    CleanupStack::PopAndDestroy( 2, buffer1 );      
    }

//--------------------------- Helper functions ------------------------

void MT_CNSmlDataModBase::ConfigureDataModL( const TDesC& aOwnFormat, const TDesC& aPartnerFormat )
    {
    HBufC8* ownFormat = ReadStoreFormatLC( aOwnFormat );

    TResourceReader ownReader;
    ownReader.SetBuffer( ownFormat );
    iOwnStringPool.OpenL();
    
    CSmlDataStoreFormat* ownStoreFormat = 
        CSmlDataStoreFormat::NewLC( iOwnStringPool, ownReader );
    EUNIT_PRINT( _L("3.1.1"));
    delete iOwnStoreFormat;
    iOwnStoreFormat = ownStoreFormat;
    CleanupStack::Pop( ownStoreFormat );
    iDataMod->SetOwnStoreFormat( *iOwnStoreFormat );
    
    CleanupStack::PopAndDestroy( ownFormat );

    HBufC8* partnerFormat = ReadStoreFormatLC( aPartnerFormat );
    EUNIT_PRINT( *partnerFormat );

    TResourceReader partnerReader;
    partnerReader.SetBuffer( partnerFormat );    
    
    iPartnerStringPool.OpenL();
    CSmlDataStoreFormat* partnerStoreFormat = 
        CSmlDataStoreFormat::NewLC( iPartnerStringPool, partnerReader );
    delete iPartnerStoreFormat;
    iPartnerStoreFormat = partnerStoreFormat;
    CleanupStack::Pop( partnerStoreFormat );
    iDataMod->SetPartnerStoreFormat( *iPartnerStoreFormat );

    CleanupStack::PopAndDestroy( partnerFormat );
    
    iDataMod->SetUsedMimeType( iOwnStoreFormat->MimeFormat( 0 ).MimeType(), iOwnStoreFormat->MimeFormat( 0 ).MimeVersion() );    
    }

HBufC8* MT_CNSmlDataModBase::ReadStoreFormatLC(const TDesC& aResourceFile)
    {
    TFileName fileName;
    TParse parse;

    parse.Set( aResourceFile, &KDC_RESOURCE_FILES_DIR, NULL  );
    fileName = parse.FullName();

    RResourceFile resourceFile; 
    BaflUtils::NearestLanguageFile( iFs, fileName );

    resourceFile.OpenL( iFs, fileName );
    CleanupClosePushL( resourceFile );
    
    HBufC8* contactsDataFormat = resourceFile.AllocReadL( 1 );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PushL( contactsDataFormat );
    
    return contactsDataFormat;
    }

CBufFlat* MT_CNSmlDataModBase::ReadBufFromFileLC( const TDesC& aFileName )
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

void MT_CNSmlDataModBase::OpenVCardFromFileLC( const TDesC& aFileName, RFileReadStream& aReadStream )
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

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    MT_CNSmlDataModBase,
    "NSmlDataModBase test suite.",
    "MODULE" )

EUNIT_TEST(
    "Test creating and deleting the object",
    "CNSmlDataModBase",
    "",
    "FUNCTIONALITY",
    SetupL, EmptyL, Teardown)

EUNIT_TEST(
    "Test needs merge, when store formats not set",
    "CNSmlDataModBase",
    "NeedsMerge",
    "FUNCTIONALITY",
    SetupL, NeedsMerge1L, Teardown)

EUNIT_TEST(
    "Test needs merge",
    "CNSmlDataModBase",
    "NeedsMergeL",
    "FUNCTIONALITY",
    SetupAndConfigure1L, NeedsMerge2L, Teardown)    
    
EUNIT_TEST(
    "Test Strip when store format not set",
    "CNSmlDataModBase",
    "StripTxL",
    "FUNCTIONALITY",
    SetupL, StripTx1L, Teardown)

EUNIT_TEST(
    "Test Strip field not supported by partner",
    "CNSmlDataModBase",
    "StripTxL",
    "FUNCTIONALITY",
    SetupAndConfigure1L, StripTx2L, Teardown)
    
EUNIT_TEST(
    "Test Strip field not supported by partner",
    "CNSmlDataModBase",
    "StripTxL",
    "FUNCTIONALITY",
    SetupAndConfigure2L, StripTx3L, Teardown)
    
EUNIT_TEST(
    "Test configure datamod",
    "CNSmlDataModBase",
    "",
    "FUNCTIONALITY",
    SetupAndConfigure1L, EmptyL, Teardown)
    
EUNIT_TEST(
    "Test basic merging of vCards",
    "CNSmlDataModBase",
    "NeedsMerge",
    "FUNCTIONALITY",
    SetupAndConfigure1L, BasicMerge1L, Teardown)    

EUNIT_TEST(
    "Test field level merging of vCards",
    "CNSmlDataModBase",
    "MergeRxL",
    "FUNCTIONALITY",
    SetupAndConfigure1L, FieldLevelMerge1L, Teardown)      
    
EUNIT_TEST(
    "Test basic merging of vCards",
    "CNSmlDataModBase",
    "NeedsMerge",
    "FUNCTIONALITY",
    SetupAndConfigure2L, BasicMerge2L, Teardown)    

EUNIT_TEST(
    "Test field level merging of vCards",
    "CNSmlDataModBase",
    "MergeRxL",
    "FUNCTIONALITY",
    SetupAndConfigure2L, FieldLevelMerge1L, Teardown)      


    
EUNIT_END_TEST_TABLE

// End of file

