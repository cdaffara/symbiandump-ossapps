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
* Description:  email internal extension base class
*
*/

#include "emailtrace.h"

#include "cemailextensionbase.h"

/**
*
*/
enum TEmailFwPanic {
    EEmailExtensionIndexOutOfRange
    };

_LIT( KEmailExtensionPanic, "EmailFw" );
    
void Panic( TEmailFwPanic aPanic )
    {
    NM_FUNCTION;
    
    User::Panic( KEmailExtensionPanic, aPanic );
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailExtension::CEmailExtension( const TUid& aUid ) : 
    iUid( TUid::Uid(aUid.iUid ) )
    {
    NM_FUNCTION;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailExtension::~CEmailExtension()
    {
    NM_FUNCTION;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUid CEmailExtension::Uid() const
    {
    NM_FUNCTION;
    
    return iUid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CEmailExtension::DecRef()
    {
    NM_FUNCTION;
    
    if ( iRefCount )
        {
        iRefCount--;
        }
    return iRefCount;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailExtension::IncRef()
    {
    NM_FUNCTION;
    
    ++iRefCount;
    }
        
// ---------------------------------------------------------------------------
// deletes extension and removes it from extension array
// ---------------------------------------------------------------------------
//
void CExtendableEmail::ReleaseExtension( CEmailExtension* aExtension )
    {
    NM_FUNCTION;
    
    if ( !aExtension->DecRef() )
        {
        iExtensions.Remove( aExtension );
        delete aExtension;
        }
    }
    
// ---------------------------------------------------------------------------
// Finds and returns extension
// ---------------------------------------------------------------------------
//
CEmailExtension* CExtendableEmail::ExtensionL( const TUid& aInterfaceUid )
    {
    NM_FUNCTION;
    
    TInt index = iExtensions.FindExtension( aInterfaceUid );
    CEmailExtension* ext = NULL;
    if ( index != KErrNotFound )
        {
        ext = iExtensions.Extension( index );
        ext->IncRef();
        }
    return ext;
    }

//<qmail>
// ---------------------------------------------------------------------------
// Constructor is required for sbs2 to generate the same .defs for urel/udeb
// ---------------------------------------------------------------------------
//
CExtendableEmail::CExtendableEmail()
    {
    NM_FUNCTION;
    
    }
//</qmail>

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
TEmailExtensions::~TEmailExtensions()
    {
    NM_FUNCTION;
    
    iExtensions.Close();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
TEmailExtensions::TEmailExtensions() : iExtensions( 1 )
    {
    NM_FUNCTION;
    
    }

// ---------------------------------------------------------------------------
// returns index of extension in extension array
// ---------------------------------------------------------------------------
//
TInt TEmailExtensions::FindExtension( const TUid& aUid ) const
    {
    NM_FUNCTION;
    
    TInt index = KErrNotFound;
    for ( TInt i = 0; i < iExtensions.Count(); i++ )
        {        
        const CEmailExtension* tested = iExtensions[i];
        if ( aUid == tested->Uid() )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// ---------------------------------------------------------------------------
// Returns extension by index. Panics if index is out of range.
// ---------------------------------------------------------------------------
//
CEmailExtension* TEmailExtensions::Extension( const TInt aIndex ) const
    {
    NM_FUNCTION;
    
    __ASSERT_ALWAYS( aIndex>=0 && aIndex < iExtensions.Count(),
        Panic( EEmailExtensionIndexOutOfRange ) );
    return iExtensions[aIndex];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TEmailExtensions::AddL( CEmailExtension* aExtension )
    {
    NM_FUNCTION;
    
    if ( !aExtension )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PushL( aExtension );
    iExtensions.AppendL( aExtension );    
    CleanupStack::Pop();
    aExtension->IncRef();
    }

// ---------------------------------------------------------------------------
// Removes extension from array
// ---------------------------------------------------------------------------
//
void TEmailExtensions::Remove( 
    const CEmailExtension* aExtension )
    {
    NM_FUNCTION;
    
    const TInt index( FindExtension( aExtension->Uid() ) );
    if ( index != KErrNotFound )    
        {
        iExtensions.Remove( index );
        }    
    }

// End of file    
