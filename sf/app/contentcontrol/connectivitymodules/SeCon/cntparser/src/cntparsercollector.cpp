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
* Description:  CCntParserCollector implementation.
*
*/



#include <cntfilt.h>
#include <cntitem.h>
#include <cntfldst.h>

#include <vcard.h>
#include <versit.h>
#include <s32mem.h>

#include "cntparsercollector.h"
#include "cntparserserver.h"
#include "irmcconsts.h"
#include "debug.h"

//Next values are defined in cntdef.h
const TUid typesToBeSearched[]={{KUidContactFieldPhoneNumberValue},{KUidContactFieldFaxValue}};

_LIT( KTotalCountFilePath, "count.txt" );

_LIT8( KNameProperty, "N" );
_LIT8( KTelProperty, "TEL" );


//------------------------------------------------------------
// CCntParserCollector::CCntParserCollector():CActive( 0 )
//------------------------------------------------------------
CCntParserCollector::CCntParserCollector():CActive( 0 )
    {
    }
//------------------------------------------------------------
// CCntParserCollector::~CCntParserCollector()
//------------------------------------------------------------
CCntParserCollector::~CCntParserCollector()
    {
    LOGGER_ENTERFN( "CntParserCollector::~CCntParserCollector()" );

    if( iVersitTLS )
        {
        iVersitTLS->VersitTlsDataClose(); //Close reference 
        }
    LOGGER_WRITE( "CntParserCollector: Versit OK!" );

    iFsSession.Close();
    LOGGER_WRITE( "CntParserCollector: FsSession OK!" );
    iWriter.Close(); 
    LOGGER_WRITE( "CntParserCollector: iWriter OK!" );
    delete iFilter;
    LOGGER_LEAVEFN( "CntParserCollector::~CCntParserCollector()" );
    }
//------------------------------------------------------------
// CCntParserCollector::NewL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
//------------------------------------------------------------
CCntParserCollector* CCntParserCollector::NewL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
    {
    CCntParserCollector* self=NewLC(aServer, aCurrentDatabase);
    CleanupStack::Pop();
    return self;
    }
//------------------------------------------------------------
// CCntParserCollector::NewLC(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
//------------------------------------------------------------
CCntParserCollector* CCntParserCollector::NewLC(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
    {
    CCntParserCollector* self=new (ELeave) CCntParserCollector();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aCurrentDatabase );
    return self;
    }
//------------------------------------------------------------
// CCntParserCollector::ConstructL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
//------------------------------------------------------------
void CCntParserCollector::ConstructL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase)
    {
    LOGGER_WRITE( "CntParserCollector: ConstructL" );

    CVersitTlsData& VersitTLS = CVersitTlsData::VersitTlsDataL();   //For speed optimization take a reference to TLS, SEE DESTRUCTOR!!
    iVersitTLS = &VersitTLS;

    iServer = aServer;
    iContactsDb = aCurrentDatabase;
    PrepareParserL();

    CActiveScheduler::Add(this);
    }
//------------------------------------------------------------
// CCntParserCollector::CreateIrMCL2FileL(const TDesC& aFileName, TInt aMaxNumberOfContacts,TBool aForced)
//------------------------------------------------------------
TInt CCntParserCollector::CreateIrMCL2FileL(const TDesC& aFileName, TInt aMaxNumberOfContacts,TBool aForced)
    {
    LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL" );
    LOGGER_WRITE_1( "CCntParserCollector::CreateIrMCL2FileL aFileName %S", &aFileName );

    iMaxNumberOfContacts = aMaxNumberOfContacts;

    CContactIdArray* changedItems;
    TTime tTime;

    LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: Does the PB.VCF already exist" );

    //Is there a previously created pb.vcf file?
    RFile file;
    TInt err = file.Open( iFsSession, aFileName, EFileShareExclusive ); //Open file
    if( err == KErrNone )
        {
        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: Yes, when created" );
        file.Modified(tTime);   //When modified
        }
    file.Close();

    err = iWriter.Open( iFsSession, aFileName, EFileShareExclusive ); //Open stream
    CleanupClosePushL( iWriter );   //Yes, this is a member variable, but it's considered

    if( err == KErrPathNotFound )
        {
        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: dir did not exist, creating..." );
        err = iFsSession.MkDir( aFileName );
        LOGGER_WRITE_1( "CCntParserCollector MkDir returned %d", err );
        if ( err == KErrNone )
            {
            LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: New file after MkDir" );
            //This is the situation that the file is created for the first time
            User::LeaveIfError( iWriter.Create( iFsSession, aFileName, EFileShareExclusive ) );
            LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: New file created after MkDir" );
            CleanupStack::Pop();    //iWriter
            ParseCntDatabaseL(); //create IrMC L2 stuff
            SaveTotalCountL();
            return KErrNone;    //File created normally
            }
        }
    if( err == KErrNotFound ) // file does not exist - create it
        {
        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: New file" );

        //This is the situation that the file is created for the first time
        User::LeaveIfError( iWriter.Create( iFsSession, aFileName, EFileShareExclusive ) );
        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: New file created" );
        CleanupStack::Pop();    //iWriter
        ParseCntDatabaseL(); //create IrMC L2 stuff
        SaveTotalCountL();
        return KErrNone;    //File created normally
        }
    else
        {
        if( err != KErrNone )
            {
            LOGGER_WRITE_1( "CreateIrMCL2FileL errror leaving %d", err );
            User::Leave(err);   
            }
        iWriter.Close();

        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: File exists, any changes in CDB?" );
        //The file exists, but are there any changes?

        changedItems = iContactsDb->ContactsChangedSinceL( tTime ); //Create a contactidarray, user takes the ownership
        CleanupStack::PushL(changedItems);

        LOGGER_WRITE_1( "CreateIrMCL2FileL changedItems %d", changedItems->Count() );
        LOGGER_WRITE_1( "CreateIrMCL2FileL aForced %d", aForced );
        //Are there changes in any contact
        if( changedItems->Count()!=0 || aForced || ContactCountChanged() )
            {
            LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: Changes in CDB, replace existing PB.VCF" );

            CleanupStack::PopAndDestroy(changedItems);

            //Create pb.vcf
            User::LeaveIfError( iWriter.Replace( iFsSession, aFileName, EFileShareExclusive) );
            LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: file replaced" );
            CleanupStack::Pop();    //iWriter
            ParseCntDatabaseL(); //create IrMC L2 stuff
            SaveTotalCountL();
            return KErrNone;    //File created normally
            }

        //No changes
        CleanupStack::PopAndDestroy(changedItems);
        CleanupStack::PopAndDestroy();

        LOGGER_WRITE( "CntParserCollector: CreateIrMCL2FileL: No Changes in CDB, keep old PB.VCF" );
        }   
    return KPBNotUpdated;   //No error occured, KPBNotUpdated used as return value to tell that the file was not created..
    }
//------------------------------------------------------------
// CCntParserCollector::ParseCntDatabaseL()
//------------------------------------------------------------
void CCntParserCollector::ParseCntDatabaseL()
    {
    LOGGER_WRITE( "CntParserCollector: ParseCntDatabaseL" );

    iCurrentItem = 0;
    iCancel = EFalse;
    iContactsSaved = 0;

    //Create owncard (it's always included, even if it's empty)

    TFileName defaultDb;
    CContactDatabase::GetDefaultNameL( defaultDb );
    TFileName currentDb;
    iContactsDb->GetCurrentDatabase( currentDb );

    if( defaultDb.CompareF(currentDb) == 0 || currentDb == KNullDesC16 /*|| currentDb.CompareF(KSimDummyDatabase)==0*/) //Is current database also default?
        {
        LOGGER_WRITE( "CntParserCollector: ParseCntDatabaseL: Using default database" );
        TContactItemId ownCardId = iContactsDb->OwnCardId();
        ExternalizeCurrentContactL( iContactsDb, ownCardId, ETrue);
        }
    else
        {
        LOGGER_WRITE( "CntParserCollector: ParseCntDatabaseL: Not using default database" );
        CContactDatabase* db=CContactDatabase::OpenL(); //My contact card is only in default db, so open it
        CleanupStack::PushL(db);

        TContactItemId ownCardId = db->OwnCardId();
        ExternalizeCurrentContactL( db, ownCardId, ETrue );

        CleanupStack::PopAndDestroy( db );
        }

    //Start running RunL method and create everything else..
    TRequestStatus* s = &iStatus;
    User::RequestComplete( s, KErrNone );
    SetActive();
    }
//------------------------------------------------------------
// CCntParserCollector::CloseResources()
//------------------------------------------------------------
void CCntParserCollector::CloseResources()
    {
    LOGGER_WRITE( "CntParserCollector: CloseResources: Closing resources (iWriter)" );
    iWriter.Close();
    }
//------------------------------------------------------------
// CCntParserCollector::PrepareParserL()
//------------------------------------------------------------
void CCntParserCollector::PrepareParserL()
    {
    LOGGER_WRITE( "CntParserCollector: PrepareParserL" );
    iFilter = CCntFilter::NewL();   //Filter all contact cards out of others

    iFilter->SetContactFilterTypeALL( EFalse );
    iFilter->SetContactFilterTypeCard( ETrue );
    iContactsDb->FilterDatabaseL( *iFilter );

    iIds=iFilter->iIds; //Get IDs to filtered contacts
    iCount=iIds->Count(); //Get number of contacts to be collected

    User::LeaveIfError( iFsSession.Connect() ); //Connect to the file server
    
    LOGGER_WRITE_1( "CntParserCollector: PrepareParserL: iCount == %d", iCount);
    }
//------------------------------------------------------------
// CCntParserCollector::RunL()
//------------------------------------------------------------
void CCntParserCollector::RunL()
    {
    //MaxNumberOfContacts-1 for OwnCard
    if( iCurrentItem < iCount && (iCurrentItem < ( iMaxNumberOfContacts - 1 ) || iMaxNumberOfContacts == -1 ) ) //If no more contacts left or maximum number of contacts already parsed
        {
        ExternalizeCurrentContactL( iContactsDb, (*iIds)[iCurrentItem], EFalse );
        iCurrentItem++;
        TRequestStatus* s = &iStatus;       //Let's rerun this RunL method as soon as possible
        User::RequestComplete( s, KErrNone );
        SetActive();
        return;
        }
    iCurrentItem++; //for OwnCard that is created separately
    LOGGER_WRITE( "CntParserCollector: RunL: No more items" );
    CloseResources();
    iServer->CompletePhoneBookRequests( KErrNone );
    }
//------------------------------------------------------------
// CCntParserCollector::ExternalizeCurrentContactL(CContactDatabase* aContactsDb,TInt aCurrentItem,TBool ExternalizeEmpty)
//------------------------------------------------------------
void CCntParserCollector::ExternalizeCurrentContactL(CContactDatabase* aContactsDb,TInt aCurrentItem,TBool ExternalizeEmpty)
    {
    CContactItem* ownCard;

    if( aCurrentItem < 0 )
        {
        ownCard = CContactCard::NewLC();
        }
    else
        {
        ownCard = aContactsDb->ReadContactLC( aCurrentItem );   //Take a new contact item
        }

    CParserVCard* VCard=CParserVCard::NewL();   //Create new VCard
    CleanupStack::PushL( VCard );

    TInt nField = ownCard->CardFields().Find( KUidContactFieldGivenName );
    TInt fField = ownCard->CardFields().Find( KUidContactFieldFamilyName );
    TInt cField = ownCard->CardFields().Find( KUidContactFieldCompanyName );

    //FOR NAMES
    //N: -field always included even if it's empty
    CDesC16Array* ValueArray=new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( ValueArray );
    CParserPropertyValueCDesCArray* NameValues=new (ELeave) CParserPropertyValueCDesCArray( ValueArray );
    CleanupStack::PushL( NameValues );
    CArrayPtr<CParserParam>* parameters=new (ELeave) CArrayPtrFlat<CParserParam>(1);
    CleanupStack::PushL( parameters );

    TPtrC fName;
    TPtrC nName;
    TPtrC cName;

    if( fField != KErrNotFound )
        {
        fName.Set( ownCard->CardFields()[fField].TextStorage()->Text() );
        }
    if( nField != KErrNotFound )
        {
        nName.Set( ownCard->CardFields()[nField].TextStorage()->Text() );
        }
    if( cField != KErrNotFound )
        {
        cName.Set( ownCard->CardFields()[cField].TextStorage()->Text() );
        }

    if( fName.Length() == 0)
        {
        fField = KErrNotFound;  
        }
    if(nName.Length()==0)
        {
        nField = KErrNotFound;
        if( fField==KErrNotFound )
            {
            ValueArray->AppendL( cName );   
            }
        else
            {
            ValueArray->AppendL( fName );   
            }
        }
    else
        {
        ValueArray->AppendL( fName );
        ValueArray->AppendL( nName );
        }

    VCard->AddPropertyL( CParserProperty::NewL((*NameValues),KNameProperty,parameters) );   //Add name property to VCard (VCard takes the ownership of the property)
    CleanupStack::Pop(3); //parameters namevalues valuearray

    TBool isNumber = EFalse;

    TInt typeIndex = 0;
    TInt index = -1;    //NOTE: +1 added before used first time

    for(;;) //Go through all fields (Number of fields is unknown)
        {
        index = ownCard->CardFields().FindNext(typesToBeSearched[typeIndex],index+1);//Find next field (of current type)
        if( index == KErrNotFound ) //No more phone numbers, move to the next group -> (See TypesToBeSearched)
            {
            typeIndex++;
            if( typeIndex >= (signed) (sizeof(typesToBeSearched) / sizeof(TUid)) )
                {
                break;  //if no more types to search -> exit FOR-loop
                }

            index=ownCard->CardFields().FindNext(typesToBeSearched[typeIndex],0); //Find next field
            if( index==KErrNotFound )
                {
                break;  //No fields -> exit
                }
            }

        TPtrC number = ownCard->CardFields()[index].TextStorage()->Text();
        if( number.Length() != 0)
            {
            CDesC16Array* ValueArray2=new (ELeave) CDesCArrayFlat(4);
            CleanupStack::PushL( ValueArray2 );
            CParserPropertyValueCDesCArray* NumberValues=new (ELeave) CParserPropertyValueCDesCArray(ValueArray2);
            CleanupStack::PushL( NumberValues );
            CArrayPtr<CParserParam>* parameters2=new (ELeave) CArrayPtrFlat<CParserParam>(1);
            CleanupStack::PushL( parameters2 );

            //Take label name to parameter

            for( TInt i=0;i<ownCard->CardFields()[index].ContentType().FieldTypeCount();i++ )
                {
                TFieldType fieldType=ownCard->CardFields()[index].ContentType().FieldType(i);

                if(fieldType==KUidContactFieldVCardMapWORK) parameters2->AppendL(CParserParam::NewL(KVersitParam8Work,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapHOME) parameters2->AppendL(CParserParam::NewL(KVersitParam8Home,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapMSG)  parameters2->AppendL(CParserParam::NewL(KVersitParam8Msg,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapVOICE)parameters2->AppendL(CParserParam::NewL(KVersitParam8Voice,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapFAX)  parameters2->AppendL(CParserParam::NewL(KVersitParam8Fax,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapPREF) parameters2->AppendL(CParserParam::NewL(KVersitParam8Pref,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapCELL) parameters2->AppendL(CParserParam::NewL(KVersitParam8Cell,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapPAGER)parameters2->AppendL(CParserParam::NewL(KVersitParam8Pager,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapBBS)  parameters2->AppendL(CParserParam::NewL(KVersitParam8Bbs,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapMODEM)parameters2->AppendL(CParserParam::NewL(KVersitParam8Modem,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapCAR)  parameters2->AppendL(CParserParam::NewL(KVersitParam8Car,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapISDN) parameters2->AppendL(CParserParam::NewL(KVersitParam8Isdn,KNullDesC8));
                if(fieldType==KUidContactFieldVCardMapVIDEO)parameters2->AppendL(CParserParam::NewL(KVersitParam8Video,KNullDesC8));
                }

            ValueArray2->AppendL( number );
            VCard->AddPropertyL(CParserProperty::NewL((*NumberValues),KTelProperty,parameters2)); //VCard takes the ownership of the property
            isNumber=ETrue;

            CleanupStack::Pop(3); //parameters2 numbervalues valuearray2
            }
        }

    if( (nField!=KErrNotFound) || (fField!=KErrNotFound) || (cField!=KErrNotFound) || isNumber || ExternalizeEmpty )
        {
        if( !isNumber )
            {
            //Create empty TEL field (required by the specs)

            CDesC16Array* ValueArray3=new (ELeave) CDesCArrayFlat(4);
            CleanupStack::PushL(ValueArray3);
            CParserPropertyValueCDesCArray* NumberValues2=new (ELeave) CParserPropertyValueCDesCArray(ValueArray3);
            CleanupStack::PushL(NumberValues2);
            CArrayPtr<CParserParam>* parameters3=new (ELeave) CArrayPtrFlat<CParserParam>(1);
            CleanupStack::PushL(parameters3);

            VCard->AddPropertyL(CParserProperty::NewL((*NumberValues2),KTelProperty,parameters3)); //VCard takes the ownership of the property

            CleanupStack::Pop(3); //parameters3 numbervalues valuearray3
            }

        TRAPD( err, VCard->ExternalizeL( iWriter );) //If there are no name or numbers, then no externalizing..
        //if error then only current contact card is discarded, but others will be still collected..
        if( err == KErrNone )
            {
            iContactsSaved++;
            }
        else
            {
            LOGGER_WRITE( "CntParserCollector: RunL: Error,VCARD _NOT_ saved" );
            }
        }

    aContactsDb->CloseContactL( aCurrentItem ); //Close current contact item

    CleanupStack::PopAndDestroy( VCard ); // VCard and all arrays (parameters2, numvervalues, valuearray2) related to this VCard are deleted through the ownership relations
    CleanupStack::PopAndDestroy( ownCard );
    }
//------------------------------------------------------------
// CCntParserCollector::DoCancel()
//------------------------------------------------------------
void CCntParserCollector::DoCancel()
    {
    LOGGER_WRITE( "CntParserCollector: DoCancel" );
    CloseResources();
    }
//------------------------------------------------------------
// CCntParserCollector::RunError(TInt aError)
//------------------------------------------------------------
TInt CCntParserCollector::RunError( TInt aError )
    {
    LOGGER_WRITE( "CntParserCollector: RunError!" );
    CloseResources(); //JIC

    iServer->CompletePhoneBookRequests( aError );   //complete all requests with an error
    return KErrNone;
    }
//------------------------------------------------------------
// CCntParserCollector::CurrentItem()
//------------------------------------------------------------
TInt CCntParserCollector::CurrentItem()
    {
    return iCurrentItem;
    }
//------------------------------------------------------------
// CCntParserCollector::Count()
//------------------------------------------------------------
TInt CCntParserCollector::Count()
    {
    return iCount;
    }
//------------------------------------------------------------
// CCntParserCollector::ContactsSaved()
//------------------------------------------------------------
TInt CCntParserCollector::ContactsSaved()
    {
    return iContactsSaved;
    }
//------------------------------------------------------------
// CCntParserCollector::SaveTotalCountL() const
//------------------------------------------------------------
void CCntParserCollector::SaveTotalCountL()
    {
    LOGGER_WRITE( "CntParserCollector: SaveTotalCount begin" );
        
    if( iContactsDb )
        {
        iTotalCount = iContactsDb->CountL();
        LOGGER_WRITE_1( "CntParserCollector: SaveTotalCount iTotalCount %d", iTotalCount );
        }
        
    RFs Fs;
    RFile writer;
    User::LeaveIfError(Fs.Connect());   //Connect to the file server
    CleanupClosePushL(Fs);
    
    User::LeaveIfError(Fs.CreatePrivatePath( EDriveC ));
    User::LeaveIfError(Fs.SetSessionToPrivate( EDriveC ));
    
    User::LeaveIfError(writer.Replace(Fs,KTotalCountFilePath,EFileWrite)); //Open file
    CleanupClosePushL(writer);
    LOGGER_WRITE( "CntParserCollector: SaveTotalCount file replaced" );
    
    // Prepare externalized representation of iTotalCount
    TBuf8< 256 > countData;
    RDesWriteStream writeStream( countData );
    CleanupClosePushL( writeStream );
    writeStream.WriteInt32L( iTotalCount );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    // Then write them to the file itself
    User::LeaveIfError(writer.Write( countData ));

    CleanupStack::PopAndDestroy();  //writer
    CleanupStack::PopAndDestroy();  //Fs
    
    LOGGER_WRITE( "CntParserCollector: SaveTotalCount end" );
    }
//------------------------------------------------------------
// CCntParserCollector::ReadTotalCountL() const
//------------------------------------------------------------
TInt CCntParserCollector::ReadTotalCountL() const
    {
    LOGGER_WRITE( "CntParserCollector: ReadTotalCount begin" );
    TInt total;
    RFs Fs;
    RFile reader;
    User::LeaveIfError(Fs.Connect());   //Connect to the file server
    CleanupClosePushL(Fs);
    
    User::LeaveIfError(Fs.CreatePrivatePath( EDriveC ));
    User::LeaveIfError(Fs.SetSessionToPrivate( EDriveC ));
        
    User::LeaveIfError(reader.Open(Fs,KTotalCountFilePath,EFileRead)); //Open file
    CleanupClosePushL(reader);
    LOGGER_WRITE( "CntParserCollector: ReadTotalCount file opened" );
        
    RFileReadStream readStream(reader);
    readStream.PushL();

    total = readStream.ReadInt32L();
    
    LOGGER_WRITE_1( "CntParserCollector: ReadTotalCount total %d", total );
    
    CleanupStack::PopAndDestroy();  //readStream
    CleanupStack::PopAndDestroy();  //writer
    CleanupStack::PopAndDestroy();  //Fs
    LOGGER_WRITE( "CntParserCollector: ReadTotalCount end" );
    return total;
    }
//------------------------------------------------------------
// CCntParserCollector::ContactCountChanged() const
//------------------------------------------------------------
TBool CCntParserCollector::ContactCountChanged()
    {
    LOGGER_WRITE( "CntParserCollector: ContactCountChanged begin" );
    TInt oldCount = 0;
    TInt err( KErrNone );
    
    if( iContactsDb )
        {
        TRAP( err, iTotalCount = iContactsDb->CountL() );
        if ( err != KErrNone )
            {
            LOGGER_WRITE_1( "iContactsDb->CountL leaved: %d", err );
            LOGGER_WRITE( "CntParserCollector: ContactCountChanged ETrue" );
            return ETrue;
            }
        LOGGER_WRITE_1( "CntParserCollector: ContactCountChanged iTotalCount %d", iTotalCount );
        }
    
    TRAP( err, oldCount = ReadTotalCountL() );

    LOGGER_WRITE_1( "CntParserCollector: ContactCountChanged err %d", err );
    LOGGER_WRITE_1( "CntParserCollector: ContactCountChanged oldCount %d", oldCount );
    LOGGER_WRITE_1( "CntParserCollector: ContactCountChanged iTotalCount %d", iTotalCount );
    
    if( err == KErrNone )
        {
        if( iTotalCount != oldCount )
            {
            LOGGER_WRITE( "CntParserCollector: ContactCountChanged ETrue" );
            return ETrue;
            }
        else
            {
            LOGGER_WRITE( "CntParserCollector: ContactCountChanged EFalse" );
            return EFalse;
            }
        }
    else
        {
        LOGGER_WRITE( "CntParserCollector: ContactCountChanged ETrue" );
        return ETrue;               
        }
    }
    
// end of file
