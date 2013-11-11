/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Test for singleton store
*
*/



//  CLASS HEADER
#include "ut_cglxsingletonstore.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>


//  INTERNAL INCLUDES
#include "glxsingletonstore.h"

// CONSTRUCTION
ut_CGlxSingletonStore* ut_CGlxSingletonStore::NewL()
    {
    ut_CGlxSingletonStore* self = ut_CGlxSingletonStore::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_CGlxSingletonStore* ut_CGlxSingletonStore::NewLC()
    {
    ut_CGlxSingletonStore* self = new( ELeave ) ut_CGlxSingletonStore();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_CGlxSingletonStore::~ut_CGlxSingletonStore()
    {
    }

// Default constructor
ut_CGlxSingletonStore::ut_CGlxSingletonStore()
    {
    }

// Second phase construct
void ut_CGlxSingletonStore::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void ut_CGlxSingletonStore::SetupL(  )
    {

    }

void ut_CGlxSingletonStore::Teardown(  )
    {

    }

// Destruction observer
struct TDestructionInfo
    {
    TDestructionInfo() 
        {
        iDestroyed = EFalse;
        }
    TBool iDestroyed;
    };
    
// CClass1
class CClass1 : public CBase 
    {
public: 
    static CClass1* NewL() 
        {
        return new (ELeave) CClass1;
        }
         
    static CClass1* InstanceL()
        {
        return CGlxSingletonStore::InstanceL(&NewL);
        }
       
    void Close()
        {
        CGlxSingletonStore::Close(this);
        }
        
    ~CClass1()
        {
        iDestructionInfo->iDestroyed = ETrue;
        }
        
    TDestructionInfo* iDestructionInfo; 
    };

// CClass2
class CClass2 : public CBase 
    {
public:
    static CClass2* NewL() 
        {
        return new (ELeave) CClass2;
        }
         
    static CClass2* InstanceL()
        {
        return CGlxSingletonStore::InstanceL(&NewL);
        }
       
    void Close()
        {
        CGlxSingletonStore::Close(this);
        }
            
    ~CClass2()
        {
        iDestructionInfo->iDestroyed = ETrue;
        }
        
    TDestructionInfo* iDestructionInfo; 
    };
    
void ut_CGlxSingletonStore::ut_CGlxSingletonStore_TestL(  )
    {
    // Check that can get the same instance
    CClass1* class1A = CClass1::InstanceL();
    CleanupClosePushL(*class1A);
    EUNIT_ASSERT( class1A );
    CClass1* class1B = CClass1::InstanceL();
    CleanupClosePushL(*class1B);
    EUNIT_ASSERT( class1A == class1B);

    // Check that can get two different classes
    CClass2* class2A = CClass2::InstanceL();
    CleanupClosePushL(*class2A);
    EUNIT_ASSERT( class2A && reinterpret_cast<CBase*>(class2A) != reinterpret_cast<CBase*>(class1A));
    CClass2* class2B = CClass2::InstanceL();
    CleanupClosePushL(*class2B);
    EUNIT_ASSERT( class2A == class2B);
    
    // Close
    TDestructionInfo info1;
    TDestructionInfo info2;
    class1A->iDestructionInfo = &info1;
    class2A->iDestructionInfo = &info2;
    EUNIT_ASSERT(!info1.iDestroyed && !info2.iDestroyed);
    // Close first ref on 1
    class1A->Close();
    CleanupStack::Pop();
    EUNIT_ASSERT(!info1.iDestroyed && !info2.iDestroyed);
    // Close last ref on 1
    class1B->Close();
    CleanupStack::Pop();
    EUNIT_ASSERT(info1.iDestroyed && !info2.iDestroyed);
    // Close first ref on 2
    class2A->Close();
    CleanupStack::Pop();
    EUNIT_ASSERT(info1.iDestroyed && !info2.iDestroyed);
    // Close last ref on 2
    class2B->Close();
    CleanupStack::Pop();
    EUNIT_ASSERT(info1.iDestroyed && info2.iDestroyed);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    ut_CGlxSingletonStore,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test - test ",
    "CGlxSingletonStore",
    "InstanceL & Close",
    "FUNCTIONALITY",
    SetupL, ut_CGlxSingletonStore_TestL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
