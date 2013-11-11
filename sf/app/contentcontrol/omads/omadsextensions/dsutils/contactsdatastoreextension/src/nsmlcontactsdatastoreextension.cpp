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
* Description:  DS contacts data store extension api
*
*/

#include <e32base.h>
#include <s32strm.h>
#include <badesca.h>
#include <nsmlcontactsdatastoreextension.h>
#include "nsmldebug.h"
#include "nsmlcontactsdatastoreextension_p.h"

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::NewL()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactsDataStoreExtension* CNsmlContactsDataStoreExtension::NewL( const TDesC& aManager )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::NewL: BEGIN");
    CNsmlContactsDataStoreExtension* self = NewLC( aManager );
    CleanupStack::Pop();
    _DBG_FILE("CNsmlContactsDataStoreExtension::NewL: END");
    return self;
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::NewLC()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactsDataStoreExtension* CNsmlContactsDataStoreExtension::NewLC( const TDesC& aManager )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::NewLC: BEGIN");
    CNsmlContactsDataStoreExtension* self = new (ELeave) CNsmlContactsDataStoreExtension();
    CleanupStack::PushL( self );
    self->ConstructL( aManager );
    _DBG_FILE("CNsmlContactsDataStoreExtension::NewLC: END");
    return self;
    }
    
// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::CNsmlContactsDataStoreExtension()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactsDataStoreExtension::CNsmlContactsDataStoreExtension()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::CNsmlContactsDataStoreExtension: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::CNsmlContactsDataStoreExtension: END");
    }
    
// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ConstructL()
// ----------------------------------------------------------------------------
EXPORT_C void CNsmlContactsDataStoreExtension::ConstructL( const TDesC& aManager )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ConstructL: BEGIN");
    d_ptr = new CNsmlContactsDataStoreExtensionPrivate( aManager );    
    _DBG_FILE("CNsmlContactsDataStoreExtension::ConstructL: END");
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::~CNsmlContactsDataStoreExtension()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactsDataStoreExtension::~CNsmlContactsDataStoreExtension()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::~CNsmlContactsDataStoreExtension: BEGIN");
    if( d_ptr)
        {
        delete d_ptr;
        d_ptr = NULL;
        }
    _DBG_FILE("CNsmlContactsDataStoreExtension::~CNsmlContactsDataStoreExtension: END");
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ExportContactsL()
// ----------------------------------------------------------------------------
EXPORT_C TInt CNsmlContactsDataStoreExtension::ExportContactsL( const TUid& aUid, 
                                                                CBufBase& aContactbufbase )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ExportContactsL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::ExportContactsL: END");
    if( d_ptr )
        {
        return d_ptr->ExportContactsL( aUid, aContactbufbase );
        }
    else
        {
        return KErrGeneral;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ImportContactsL()
// ----------------------------------------------------------------------------
EXPORT_C CArrayFixFlat<TUid>* CNsmlContactsDataStoreExtension::ImportContactsL( const TDesC8& aContactbufbase )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ImportContactsL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::ImportContactsL: END");
    if( d_ptr )
        {
        return d_ptr->ImportContactsL( aContactbufbase );
        }
    else
        {
        return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ReadContactL()
// ----------------------------------------------------------------------------
EXPORT_C TInt CNsmlContactsDataStoreExtension::ReadContactL( const TUid& aUid,
                                                             CNsmlContactItem* aContactitem )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ReadContactL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::ReadContactL: END");
    if( d_ptr )
        {
        return d_ptr->ReadContactL( aUid, aContactitem );
        }
    else
        {
        return KErrGeneral;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::DeleteContactL()
// ----------------------------------------------------------------------------
EXPORT_C TBool CNsmlContactsDataStoreExtension::DeleteContactL( const TUid& aUid )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteContactL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteContactL: END");
    if( d_ptr )
        {
        return d_ptr->DeleteContactL( aUid );
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::DeleteContactsL()
// ----------------------------------------------------------------------------
EXPORT_C  TBool CNsmlContactsDataStoreExtension::DeleteContactsL( CArrayFixFlat<TUid>* aArrDelete )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteContactsL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteContactsL: END");
    if( d_ptr )
        {
        return d_ptr->DeleteContactsL( aArrDelete );
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::DeleteAllContactsL()
// ----------------------------------------------------------------------------
EXPORT_C TBool CNsmlContactsDataStoreExtension::DeleteAllContactsL()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteAllContactsL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::DeleteAllContactsL: END");
    if( d_ptr )
        {
        return d_ptr->DeleteAllContactsL();
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ListStoresL()
// ----------------------------------------------------------------------------
EXPORT_C void CNsmlContactsDataStoreExtension::ListStoresL( CDesCArray* aCntstores )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ListStoresL: BEGIN");
    if( d_ptr )
        {
        d_ptr->ListStoresL( aCntstores );
        }
    _DBG_FILE("CNsmlContactsDataStoreExtension::ListStoresL: END");
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::MachineIdL()
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CNsmlContactsDataStoreExtension::MachineIdL()
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::MachineIdL: BEGIN");
    _DBG_FILE("CNsmlContactsDataStoreExtension::MachineIdL: END");
    if( d_ptr )
        {
        return d_ptr->MachineIdL();
        }
    else
        {
        return KErrGeneral;
        }
    }

// ----------------------------------------------------------------------------
// CNsmlContactsDataStoreExtension::ListContactsL()
// ----------------------------------------------------------------------------
EXPORT_C void CNsmlContactsDataStoreExtension::ListContactsL( CArrayFixFlat<TUid>* aContactuidlist )
    {
    _DBG_FILE("CNsmlContactsDataStoreExtension::ListContactsL: BEGIN");
    if( d_ptr )
        {
        d_ptr->ListContactsL( aContactuidlist );
        }
    _DBG_FILE("CNsmlContactsDataStoreExtension::ListContactsL: END");
    }

// ----------------------------------------------------------------------------
// CNsmlContactItem::NewL()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactItem* CNsmlContactItem::NewL()
    {
    _DBG_FILE("CNsmlContactItem::NewL: BEGIN");
    CNsmlContactItem* self = NewLC();
    CleanupStack::Pop();
    _DBG_FILE("CNsmlContactItem::NewL: END");
    return self;
    }

// ----------------------------------------------------------------------------
// CNsmlContactItem::NewLC()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactItem* CNsmlContactItem::NewLC()
    {
    _DBG_FILE("CNsmlContactItem::NewLC: BEGIN");
    CNsmlContactItem* self = new (ELeave) CNsmlContactItem();
    CleanupStack::PushL( self );
    _DBG_FILE("CNsmlContactItem::NewLC: END");
    return self;
    }
    
// ----------------------------------------------------------------------------
// CNsmlContactItem::CNsmlContactItem()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactItem::CNsmlContactItem()
    {
    _DBG_FILE("CNsmlContactItem::CNsmlContactItem: BEGIN");
    _DBG_FILE("CNsmlContactItem::CNsmlContactItem: END");
    }

// ----------------------------------------------------------------------------
// CNsmlContactItem::~CNsmlContactItem()
// ----------------------------------------------------------------------------
EXPORT_C CNsmlContactItem::~CNsmlContactItem()
    {
    _DBG_FILE("CNsmlContactItem::~CNsmlContactItem: BEGIN");
    delete iGuid;
    _DBG_FILE("CNsmlContactItem::~CNsmlContactItem: END");
    }


// End of File


