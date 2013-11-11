/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCapList implementation
*
*/


// INCLUDE FILES

#include "caplist.h"
#include "caputils.h"
#include "capparser.h"
#include "capability.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCapList::NewL()
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CCapList* CCapList::NewL()
    {
    CCapList* self = new(ELeave) CCapList();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCapList::~CCapList()
// Gets phone serial number from etel.
// -----------------------------------------------------------------------------
//
CCapList::~CCapList()
    {
    delete iList;
    }

// -----------------------------------------------------------------------------
// CCapList::CCapList()
// Constructor
// -----------------------------------------------------------------------------
//
CCapList::CCapList()
    {
    }

// -----------------------------------------------------------------------------
// CCapList::ConstructL()
// Initializes the member data
// -----------------------------------------------------------------------------
//
void CCapList::ConstructL()
    {
    iList = CStringList::NewL();
    }

// -----------------------------------------------------------------------------
// CCapList::List()
// Returns the list
// -----------------------------------------------------------------------------
//
CStringList* CCapList::List()
    {
    __ASSERT_DEBUG(iList, CapUtil::Panic(KErrGeneral));
    return iList;
    }

// -----------------------------------------------------------------------------
// CCapList::FindFromMark(TInt aId, TInt aType)
// Find from mark
// -----------------------------------------------------------------------------
//
TInt CCapList::FindFromMark(TInt aId, TInt aType)
    {
    TInt mark = List()->Mark();
    TInt count= List()->Count();
    
    if ( mark >= count )
        {
        return KErrNotFound;
        }       

    TInt index=Find(aId, aType, mark);
    if ( index == KErrNotFound )
        {
        return KErrNotFound;
        }
        

    __ASSERT_DEBUG(index>=mark, CapUtil::Panic(KErrGeneral));

    List()->SetMark(index+1);
    return index;
    }

// -----------------------------------------------------------------------------
// CCapList::FindServiceHeader()
// Finds service xml-file identification header. This header must be the first 
// line in every service file.
// -----------------------------------------------------------------------------
//
TBool CCapList::FindServiceHeader()
    {
    TRACE_FUNC_ENTRY;
    if (List()->Count()<1)
        {
        LOGGER_WRITE( "CCapList::FindServiceHeader() : returned EFalse" );
        return EFalse;
        }

    TBuf<KBufSize> buf;
    TPtrC ptr=List()->ReadPtr(0);
    CapUtil::StrCopy(buf, ptr);
    buf.Trim();
    if (buf.Compare(KServiceHeader)==0)
        {
        LOGGER_WRITE( "CCapList::FindServiceHeader() : returned ETrue" );
        return ETrue;
        }       
    else
        {
        LOGGER_WRITE( "CCapList::FindServiceHeader() : returned EFalse" );
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCapList::FindServiceL(CStringList* aList)
// Find service
// -----------------------------------------------------------------------------
//
TBool CCapList::FindServiceL(CStringList* aList)
    {
    TRACE_FUNC_ENTRY;
    aList->Reset();
    List()->SetMark(0);

    if (!FindServiceHeader())
        {
        LOGGER_WRITE( "CCapList::FindServiceL(CStringList* aList) : returned EFalse" );
        return EFalse;
        }
        

    for (;;)
        {
        TInt start=FindFromMark(EService, TXmlParser::EElementBegin);
        TInt stop=FindFromMark(EService, TXmlParser::EElementEnd);
        
        if (start==KErrNotFound || stop==KErrNotFound || start>=stop)
            {
            break;
            }
                    
        aList->CopyL(List(), start, stop);
        }

    if (aList->Count()>0)
        {
        LOGGER_WRITE( "CCapList::FindServiceL(CStringList* aList) : returned ETrue" );
        return ETrue;
        }
    
    LOGGER_WRITE( "CCapList::FindServiceL(CStringList* aList) : returned EFalse" );   
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCapList::Find(TInt aId, TInt aType, TInt aIndex)
// Find element
// -----------------------------------------------------------------------------
//
TInt CCapList::Find(TInt aId, TInt aType, TInt aIndex)
    {
    TInt count=List()->Count();
    if (aIndex<0 || aIndex>=count)
        {
        CapUtil::Panic(KErrArgument);
        }
        
    for (TInt i=aIndex; i<count; i++)
        {
        TPtrC ptr=List()->ReadPtr(i);

        TInt id=0;
        TInt type=0;
        
        CapParser::ParseElement(ptr, id, type);
        if (type==TXmlParser::EElementUnknown)
            {
            continue;  // invalid capability element
            }

        if (id==aId && aType==type)
            {
            return i;
            }
        }
    return KErrNotFound;
    }


// End of files
