/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


 
#ifndef CALENDAR_TEST_UTITLITY__INLINED_
#define CALENDAR_TEST_UTITLITY__INLINED_

//destroys all pointers in pointer array
template < class T >
void CleanupResetDestroy< T >::ResetDestroy( TAny *aPtr )
	{
	( static_cast< RPointerArray< T >* >( aPtr ) )->ResetAndDestroy();
	}

//pushes pointer array onto stack	
template < class T >
void CleanupResetDestroy< T >::PushL( RPointerArray< T >& aRef )
	{
	CleanupStack::PushL( TCleanupItem( &ResetDestroy, &aRef ) );
	}

//pushes pointer array onto stack
template < class T >
void CleanupResetDestroyPushL( RPointerArray< T >& aRef )
    {
    CleanupResetDestroy< T >::PushL( aRef );
    }

//shallow copies pointer array
template < class T >
void CCalendarTestUtility::CopyArrayL(
			const RPointerArray< T >& aOriginal,
			RPointerArray< T >& aCopy
			)
	{
	const TInt KOrignalEntryCount = aOriginal.Count();
	for( TInt i = 0; i < KOrignalEntryCount; ++i )
		{
		User::LeaveIfError( aCopy.Append( aOriginal[ i ] ) );
		}
	}

//shallow copies RArray	
template< class T > void CCalendarTestUtility::CopyArrayL( 
	const RArray< T >& aOriginal, 
	RArray< T >& aCopy )
	{
	const TInt KOrignalEntryCount = aOriginal.Count();
	for( TInt i = 0; i < KOrignalEntryCount; ++i )
		{
		User::LeaveIfError( aCopy.Append( aOriginal[ i ] ) );
		}
	}

//delete pointer and set to null
#define DELETE( x ) { delete x; x = NULL; }

//necessary, so that __FILE__ macro is executed
#define	FILE(file) _S(file)

//no iterations
#define TESTPRINT( compare )\
    TESTPRINTI2( compare, -1, -1 )

//nested within one loop
#define TESTPRINTI1( compare, iteration )\
    TESTPRINTI2( compare, iteration, -1 )

//nested within two loops
#define TESTPRINTI2( compare, iteration, subiteration )\
    PrintBooleanTest( compare, _S(#compare), iteration, subiteration, FILE(__FILE__), __LINE__ )

//no iterations
#define COMPARE( compareLeft, compareOperation, compareRight )\
    COMPAREI2( compareLeft, compareOperation, compareRight, -1, -1 )

//nested within one loop
#define COMPAREI1( compareLeft, compareOperation, compareRight, iteration )\
    COMPAREI2( compareLeft, compareOperation, compareRight, iteration, -1 )

//nested within two loops
#define COMPAREI2( compareLeft, compareOperation, compareRight, iteration, subiteration )\
	PrintComparison(compareLeft, _S(#compareOperation), compareRight,\
	(compareLeft compareOperation compareRight), iteration, subiteration, FILE(__FILE__), __LINE__ )

//prints error message and fails test if testvalue is false
#define PRINTFAIL( errorstring, testvalue )					\
	{														\
	_LIT( KErrorString, errorstring );						\
	ERR_PRINTF1( KErrorString );							\
	TEST1( testvalue, ETrue );								\
	}

#endif//CALENDAR_TEST_UTITLITY__INLINED_

