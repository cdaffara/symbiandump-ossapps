/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains all functions that can be called from cfg file.
*				 These functions call the necessary methods for getting the operation done and 
*				 writes the output to the Log.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include<BADESCA.H> 
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestCntDataStoreExtn.h"
#include <utf.h>
#include <f32file.h>
#include <e32base.h> 

_LIT8(KContact1, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Contact;Test1;;;\r\nTEL;CELL:1234567890\r\nEND:VCARD\r\n");
_LIT8(KContact2, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Contact;Test2;;;\r\nTEL;CELL:0987654321\r\nEND:VCARD\r\n");
_LIT8(KContact3, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Contact;Test3;;;\r\nTEL;CELL:1324576809\r\nEND:VCARD\r\n");
_LIT8(KContact4, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Contact;Test4;;;\r\nTEL;CELL:0897645321\r\nEND:VCARD\r\n");
_LIT8(KContact5, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Contact;Test5;;;\r\nTEL;CELL:1432586790\r\nEND:VCARD\r\n");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestCntDataStoreExtn::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "ExportContacts"   , CTestCntDataStoreExtn::ExportContactsL ),
        ENTRY( "ImportContacts"   , CTestCntDataStoreExtn::ImportContactsL ),
        ENTRY( "ReadContact"      , CTestCntDataStoreExtn::ReadContactL ),
        ENTRY( "ListContacts"     , CTestCntDataStoreExtn::ListContactsL ),
        ENTRY( "DeleteContact"    , CTestCntDataStoreExtn::DeleteContactL ),
        ENTRY( "DeleteContacts"   , CTestCntDataStoreExtn::DeleteContactsL ),
        ENTRY( "DeleteAllContacts", CTestCntDataStoreExtn::DeleteAllContactsL ),
        ENTRY( "ListStores"       , CTestCntDataStoreExtn::ListStoresL ),
        ENTRY( "MachineID"        , CTestCntDataStoreExtn::MachineIdL ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ListContactsL
// Method to List contacts
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::ListContactsL( CStifItemParser& aItem )
    {  
    iLog->Log( _L("ListContactsL Begin") );
    
    if( iEntryArray )
        {
        iEntryArray->Reset();
        TRAPD( err, iCntDataStoreExtension->ListContactsL( iEntryArray ) );
        if( err )
            {
            return KErrGeneral;
            }
        }
    else
        {
        return KErrGeneral;
        }
    iLog->Log( _L("ListContactsL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ExportContactsL
// Method to export contacts
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::ExportContactsL( CStifItemParser& aItem )
    {   
    iLog->Log( _L("ExportContactsL Begin") );
    
    TInt ret = KErrNone;
    if( iEntryArray )
        {
        if( iEntryArray->Count() > 0 )
            {
            for( TInt count = 0; count < iEntryArray->Count(); count++ )
                {
                CBufBase* tempData = CBufFlat::NewL( 1024 );
                TRAPD( err, ret = iCntDataStoreExtension->ExportContactsL( iEntryArray->At(count), *tempData ) );
                delete tempData;
                if ( err || ret )
                    {
                    return KErrGeneral; 
                    }
                }
            }
        else
           {
           return KErrGeneral;
           }    
        }
    else
        {
        return KErrGeneral;
        }
    
    iLog->Log( _L("ExportContactsL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ImportContactsL
// Method to import contacts
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::ImportContactsL( CStifItemParser& aItem )
    {   
    iLog->Log( _L("ImportContactsL Begin") );
    
    HBufC8* cntbuf = HBufC8::NewL(500);
    CArrayFixFlat<TUid>* entryArray = NULL;
    
    *cntbuf = KContact1;    
    
    TPtr8 ptr = cntbuf->Des();
    ptr.Append(KContact2);
    ptr.Append(KContact3);
    ptr.Append(KContact4);
    ptr.Append(KContact5);
    
    TRAPD( ret, entryArray = iCntDataStoreExtension->ImportContactsL(*cntbuf) );
    delete cntbuf;
    
    
    if( ret || (entryArray == NULL) )
        {
        iLog->Log( _L("ImportContactsL Error") );
        return KErrGeneral;
        }
      
    delete entryArray;
    
    iLog->Log( _L("ImportContactsL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ReadContactL
// Method to read contact
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::ReadContactL( CStifItemParser& aItem )
    {
    iLog->Log( _L("ReadContactL Begin") );
    if( iEntryArray )
        {
        if( iEntryArray->Count() > 0 )
           {
           TInt ret( KErrNone );
           for( TInt count = 0; count < iEntryArray->Count(); count++ )
               {
               CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
               TRAPD(err, ret = iCntDataStoreExtension->ReadContactL( iEntryArray->At(count), contactitem ));
               delete contactitem;
               if( err || ret )
                   {
                   return KErrGeneral;
                   }
               }
           }
        else
            {
            return KErrGeneral;
            }
        }
    else
        {
        return KErrGeneral;
        }     
    
    iLog->Log( _L("ReadContactL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::DeleteContactL
// Method to delete contact
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::DeleteContactL( CStifItemParser& aItem )
    {
    iLog->Log( _L("DeleteContactL Begin") );
    
    if( iEntryArray )
        {
        if( iEntryArray->Count() > 0 )
           {
           TBool ret;
           for( TInt count = 0; count < iEntryArray->Count(); count++ )
               {
               TRAPD( err, ret = iCntDataStoreExtension->DeleteContactL( iEntryArray->At(count) ) );
               if( err || ( ret == EFalse) )
                   {
                   return KErrGeneral;
                   }               
               }
           }
        else
            {
            return KErrGeneral;
            }
        }
    else
        {
        return KErrGeneral;
        }
    iEntryArray->Reset();
    iLog->Log( _L("DeleteContactL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::DeleteContacts
// Method to delete contacts
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::DeleteContactsL( CStifItemParser& aItem )
    {
    iLog->Log( _L("DeleteContactsL Begin") );
    
    if( iEntryArray )
        {
        if( iEntryArray->Count() > 0 )
           {
           TBool ret;           
           TRAPD( err, ret = iCntDataStoreExtension->DeleteContactsL( iEntryArray ) );
           if( err || ( ret == EFalse) )
               {
               return KErrGeneral;
               }
           }
        else
            {
            return KErrGeneral;
            }
        }
    else
        {
        return KErrGeneral;
        }
    iEntryArray->Reset();
    iLog->Log( _L("DeleteContactsL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::DeleteAllContactsL
// Method to delete all contacts
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::DeleteAllContactsL( CStifItemParser& aItem )
    {
    iLog->Log( _L("DeleteAllContacts Begin") );
    
    if( iEntryArray )
        {
        if( iEntryArray->Count() > 0 )
           {
           TBool ret;
           TRAPD( err, ret = iCntDataStoreExtension->DeleteAllContactsL() );
           if( err || ( ret == EFalse) )
               {
               return KErrGeneral;
               }
           }
        else
            {
            return KErrGeneral;
            }
        }
    else
        {
        return KErrGeneral;
        }
    iEntryArray->Reset();
    iLog->Log( _L("DeleteAllContactsL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ListStoresL
// Method to list stores
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::ListStoresL( CStifItemParser& aItem )
    {
    iLog->Log( _L("ListStoresL Begin") );
    
    CDesCArray* cntStores = new (ELeave) CDesCArrayFlat(1);
    TRAPD( err, iCntDataStoreExtension->ListStoresL( cntStores ) );
    delete cntStores;
    if(err )
        {
        return KErrGeneral;
        }
    
    iLog->Log( _L("ListStoresL End") );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::MachineIdL
// Method to fetch machine id
// -----------------------------------------------------------------------------
//
TInt CTestCntDataStoreExtn::MachineIdL( CStifItemParser& aItem )
    {
    iLog->Log( _L("MachineIdL Begin") );
    TRAPD( err, iCntDataStoreExtension->MachineIdL( ) );
    if(err)
        {
        return KErrGeneral;
        }    
    iLog->Log( _L("MachineIdL End") );
    return KErrNone;
    }

//  End of File
