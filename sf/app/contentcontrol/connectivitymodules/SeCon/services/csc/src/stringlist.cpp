/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CStringList implementation
*
*/


// INCLUDE FILES

#include "stringlist.h"
#include "capability.h"
#include "debug.h"

const TInt KMaxStringlistSize( 1000 );

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStringList::NewL()
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CStringList* CStringList::NewL()
    {
    CStringList* self = CStringList::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CStringList::NewLC()
// Two-phase constructor. The created instance is placed to cleanup stack
// -----------------------------------------------------------------------------
//
CStringList* CStringList::NewLC()
    {
    CStringList* self = new ( ELeave ) CStringList();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CStringList::CStringList()
// Default constuctor
// -----------------------------------------------------------------------------
//
CStringList::CStringList()
    {
    }

// -----------------------------------------------------------------------------
// CStringList::~CStringList()
// Destructor
// -----------------------------------------------------------------------------
//
CStringList::~CStringList()
    {
    delete iLines;
    iLines = NULL;
    }

// -----------------------------------------------------------------------------
// CStringList::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CStringList::ConstructL()
    {
    const TInt KDefaultArrayGranularity = 10;
    iLines = new (ELeave) CDesCArrayFlat(KDefaultArrayGranularity);
    SetMark(0);
    }

// -----------------------------------------------------------------------------
// CStringList::Count() const
// Returns the count of lines
// -----------------------------------------------------------------------------
//
TInt CStringList::Count() const
    {
    return iLines->Count();
    }

// -----------------------------------------------------------------------------
// CStringList::Panic(TInt aPanic) const
// Creates a panic
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CStringList::Panic(TInt aPanic) const
#else
void CStringList::Panic(TInt /*aPanic*/) const
#endif
    {
    TRACE_FUNC_ENTRY;
#ifdef _DEBUG
    _LIT(KPanicCategory,"CStringList");

    User::Panic(KPanicCategory, aPanic);
#endif
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CStringList::Reset()
// Resets iLines
// -----------------------------------------------------------------------------
//
void CStringList::Reset()
    {
    iLines->Reset();
    }

// -----------------------------------------------------------------------------
// CStringList::WriteL(const TDesC& aText)
// Writes a string
// -----------------------------------------------------------------------------
//
void CStringList::WriteL(const TDesC& aText)
    {
    iLines->AppendL(aText);
    }

// -----------------------------------------------------------------------------
// CStringList::ReadPtr(TInt aIndex)
// Returns pointer to the string
// -----------------------------------------------------------------------------
//
TPtrC16 CStringList::ReadPtr(TInt aIndex)
    {
    if (aIndex<0 || aIndex>=Count())
        {
        Panic(KErrArgument);
        }
        
    return iLines->MdcaPoint(aIndex);
    }

// -----------------------------------------------------------------------------
// CStringList::CopyL(CStringList* aSource, TInt aStart, TInt aStop)
// Copies a string / strings
// -----------------------------------------------------------------------------
//
void CStringList::CopyL(CStringList* aSource, TInt aStart, TInt aStop)
    {
    if (aStart<0 || aStop>=aSource->Count() || aStart>aStop)
        {
        Panic(KErrArgument);
        }        
    
    for (TInt i=aStart; i<=aStop; i++)
        {
        WriteL(aSource->ReadPtr(i));
        }
    }

// -----------------------------------------------------------------------------
// CStringList::StrCopy(TDes& aTarget, const TDesC& aSource) const
// Copies the string
// -----------------------------------------------------------------------------
//
TBool CStringList::StrCopy(TDes& aTarget, const TDesC& aSource) const
    {
    TInt len=aTarget.MaxLength();
    if(len<aSource.Length()) 
        {
        aTarget.Copy(aSource.Left(len));
        return EFalse;
        }
    aTarget.Copy(aSource);
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CStringList::ReadFromFileL(const TDesC& aFileName)
// Read strings from file.
// -----------------------------------------------------------------------------
//

void CStringList::ReadFromFileL( RFs& aFs, const TDesC& aFileName )
    {
    InternalizeL( aFs, aFileName );
    }

// -----------------------------------------------------------------------------
// CStringList::InternalizeL(const TDesC& aFileName)
// Internalizes from file
// -----------------------------------------------------------------------------
//
void CStringList::InternalizeL( RFs& aFs, const TDesC& aFileName )
    {
    
    RFile file;
    User::LeaveIfError( file.Open( aFs, aFileName, EFileRead ) );
    CleanupClosePushL( file );
    
    
    TFileText textFile;
    textFile.Set( file );
    textFile.Seek( ESeekStart );
    TBuf<KMaxSize> buffer;
    TInt count(0);
    for (;;)
        {
        count++;
        if ( count > KMaxStringlistSize )
            {
            break;
            }           

        buffer = KNullDesC;
        
        //
        // Read seems to read chars until newline is reached or
        // the descriptor is full. In case descriptor becomes full,
        // err is KErrTooBig and next read starts from the next line. 
        //
        TInt err = textFile.Read( buffer );
        if ( err == KErrEof )
            {
            break;
            }
            
        if ( err != KErrNone )
            {
            User::Leave( err );
            }           

        iLines->AppendL( buffer );
        }
    
    CleanupStack::PopAndDestroy( &file ); // file 
    }


// -----------------------------------------------------------------------------
// CStringList::Mark() const
// Returns the mark
// -----------------------------------------------------------------------------
//
TInt CStringList::Mark() const
    {
    return iMark;
    }

// -----------------------------------------------------------------------------
// CStringList::SetMark(TInt aMark)
// Sets mark
// -----------------------------------------------------------------------------
//
void CStringList::SetMark(TInt aMark)
    {
    iMark=aMark;
    }

// End of files
