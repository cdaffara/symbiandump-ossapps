/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of DM adapter test component
* 	This is part of omadmextensions/adapter test application.
*
*/






// INCLUDE FILES

#include "TestDmDDFObject.h"
#include "StifLogger.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestDmDDFObject::CTestDmDDFObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestDmDDFObject::CTestDmDDFObject( CStifLogger *aLog )
	: iName( 0 )
	, iAccessTypes()
	, iDefaultValue( 0 )
	, iDescription( 0 )
	, iFormat( (TDFFormat)0 )
	, iOccurence( (TOccurence)0 )
	, iScope( (TScope)0 )
	, iTitle ( 0 )
	, iMimeType( 0 )
	, iObjectGroup( EFalse )
	, iLog( aLog )
//	RPointerArray<CTestDmDDFObject> iChildren;
    {
    }

// -----------------------------------------------------------------------------
// CTestDmDDFObject::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestDmDDFObject::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CTestDmDDFObject::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestDmDDFObject* CTestDmDDFObject::NewL( CStifLogger *aLog )
    {
    CTestDmDDFObject* self = CTestDmDDFObject::NewLC( aLog );
    CleanupStack::Pop( self );

    return self;
    }

CTestDmDDFObject* CTestDmDDFObject::NewLC( CStifLogger *aLog )
    {
    CTestDmDDFObject* self = new( ELeave ) CTestDmDDFObject( aLog );
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

    
// Destructor
CTestDmDDFObject::~CTestDmDDFObject()
    {
	delete iName;
	delete iDefaultValue;
	delete iDescription;
	delete iTitle;
	delete iMimeType;
	iChildren.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CTestDmDDFObject::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*?type CTestDmDDFObject::?member_function(
    ?arg_type arg,
    ?arg_type arg )
    {
    
    ?code
    
    }
*/

void CTestDmDDFObject::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
	{
	iLog->Log( _L8( "SetAccessTypesL, aAccessTypes=%d" ), aAccessTypes.GetACL() );
	iAccessTypes = aAccessTypes;
	}

void CTestDmDDFObject::SetDefaultValueL( const TDesC8& aDefaultValue )
	{
	iLog->Log( _L8( "SetDefaultValueL, aDefaultValue='%S'" ), &aDefaultValue );
	delete iDefaultValue;
	iDefaultValue = 0;
	iDefaultValue = aDefaultValue.AllocL();
	}

void CTestDmDDFObject::SetDescriptionL( const TDesC8& aDescription )
	{
	iLog->Log( _L8( "SetDescriptionL, aDescription='%S'" ), &aDescription );
	delete iDescription;
	iDescription = 0;
	iDescription = aDescription.AllocL();
	}
	
void CTestDmDDFObject::SetDFFormatL( TDFFormat aFormat )
	{
	iLog->Log( _L8( "SetDFFormatL, aFormat=%d" ), aFormat );
	iFormat = aFormat;
	}
	
void CTestDmDDFObject::SetOccurenceL( TOccurence aOccurence )
	{
	iLog->Log( _L8( "SetOccurenceL, aOccurence=%d" ), aOccurence );
	iOccurence = aOccurence;
	}
	
void CTestDmDDFObject::SetScopeL( TScope aScope )
	{
	iLog->Log( _L8( "SetScopeL, aScope=%d" ), aScope );
	iScope = aScope;
	}
	
void CTestDmDDFObject::SetDFTitleL( const TDesC8& aTitle )
	{
	iLog->Log( _L8( "SetDFTitleL, aTitle='%S'" ), &aTitle );
	delete iTitle;
	iTitle = 0;
	iTitle = aTitle.AllocL();
	}
	
void CTestDmDDFObject::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
	{
	iLog->Log( _L8( "AddDFTypeMimeTypeL, aMimeType='%S'" ), &aMimeType );
	delete iMimeType;
	iMimeType = 0;
	iMimeType = aMimeType.AllocL();
	}
	
void CTestDmDDFObject::SetAsObjectGroup()
	{
	iLog->Log( _L8( "SetAsObjectGroup, true" ) );
	iObjectGroup = ETrue;
	}
	
MSmlDmDDFObject& CTestDmDDFObject::AddChildObjectL(const TDesC8& aNodeName)
	{
	iLog->Log( _L8( "AddChildObjectL, aNodeName='%S'" ), &aNodeName );
	CTestDmDDFObject* child = CTestDmDDFObject::NewLC( iLog ); //, aNodeName );
	child->SetNameL( aNodeName );
	iChildren.AppendL( child );
	CleanupStack::Pop( child ); // Don't destroy
	return *child;
	}

MSmlDmDDFObject& CTestDmDDFObject::AddChildObjectGroupL()
	{
	iLog->Log( _L8( "AddChildObjectGroupL, ''" ) );
	CTestDmDDFObject* child = CTestDmDDFObject::NewLC( iLog ); //, KNullDesC );
	child->SetAsObjectGroup();
	iChildren.AppendL( child );
	CleanupStack::Pop( child ); // Don't destroy
	return *child;
	}

// Own functions

void CTestDmDDFObject::SetNameL( const TDesC8& aName )
	{
	delete iName;
	iName = 0;
	iName = aName.AllocL();
	}

void CTestDmDDFObject::DumpL( const TDesC8& aParentName, TBool aFullDump )
	{
	TPtrC8 name( _L8("<X>") );
	if (iName && iName->Length() > 0)
		{
		name.Set( *iName );
		}

	HBufC8* fullName = HBufC8::NewLC( aParentName.Length() + name.Length() + 1 );
	*fullName = aParentName;
	if (aParentName.Length() > 0 && aParentName[ aParentName.Length() - 1 ] != '/')
		{
		fullName->Des().Append( _L8("/"));
		}
	fullName->Des().Append( name );

	/* Translate some members to text for dumping */
	TBuf8<20> strAccessTypes;
	TUint8 accessTypes = iAccessTypes.GetACL();
	if ((accessTypes & iAccessTypes.EAccessType_Add) != 0)
		{
		strAccessTypes.Append( _L8("A") );
		}
	if ((accessTypes & iAccessTypes.EAccessType_Copy) != 0)
		{
		strAccessTypes.Append( _L8("C") );
		}
	if ((accessTypes & iAccessTypes.EAccessType_Delete) != 0)
		{
		strAccessTypes.Append( _L8("D") );
		}
	if ((accessTypes & iAccessTypes.EAccessType_Exec) != 0)
		{
		strAccessTypes.Append( _L8("E") );
		}
	if ((accessTypes & iAccessTypes.EAccessType_Get) != 0)
		{
		strAccessTypes.Append( _L8("G") );
		}
	if ((accessTypes & iAccessTypes.EAccessType_Replace) != 0)
		{
		strAccessTypes.Append( _L8("R") );
		}
	
	TBuf8<20> strFormat;
	switch( iFormat )
		{
	case EB64:
		strFormat = _L8("Base64");
		break;
	case EBool:
		strFormat = _L8("Bool");
		break;
	case EChr:
		strFormat = _L8("Chr");
		break;
	case EInt:
		strFormat = _L8("Int");
		break;
	case ENode:
		strFormat = _L8("Node");
		break;
	case ENull:
		strFormat = _L8("Null");
		break;
	case EXml:
		strFormat = _L8("Xml");
		break;
	case EBin:
		strFormat = _L8("Bin");
		break;
	default:
		break;
		}


	TBuf8<20> strOccurence;
	switch( iOccurence )
		{
	case EOne:
		/** The node appears exactly once */
		strOccurence = _L8("M:1");
		break;
	case EZeroOrOne:
		/** The node is optional and may appear zero or once */
		strOccurence = _L8("O:0-1");
		break;
	case EZeroOrMore:
		/** The node is optional and may appear zero or more times */
		strOccurence = _L8("O:0-*");
		break;
	case EOneOrMore:
		/** The node is mandatory and may appear once or more times */
		strOccurence = _L8("M:1-*");
		break;
	case EZeroOrN:
		/** The node is optional and may appear between once and 'N' times */
		strOccurence = _L8("O:1-N");
		break;
	case EOneOrN:
		/** The node is mandatory and may appear between once and 'N' times */
		strOccurence = _L8("M:1-N");
		break;
	default:
		break;
		}
	
	
	TBuf8<20> strScope;
	switch( iScope)
		{
	case EPermanent:
		/** The node appears exactly once */
		strScope = _L8("Permanent");
		break;
	case EDynamic:
		/** The node is optional and may appear zero or once */
		strScope = _L8("O:Dynamic");
		break;
	default:
		break;
		}

	
	/* Dump main data */
	if (aFullDump)
		{
		iLog->Log( _L8( "DDFObject: '%S'" ), fullName );
		}
		else
		{
		iLog->Log( _L8( "DDFObject: '%S'   (%S), %S, %S, %S" ),
					fullName, &strAccessTypes, &strFormat, &strOccurence, &strScope );
		}
	
	/* Dump members */
	if (aFullDump)
		{
		TPtrC8 empty( _L8( "<null>" ) );
		#define CHK_NULL(a) ((a)==0?(TDesC8*)(&empty):(TDesC8*)(a))

		iLog->Log( _L8( "    iName=='%S'" ), iName );
		iLog->Log( _L8( "    iAccessTypes=%d (%S)" ), iAccessTypes.GetACL(), &strAccessTypes );
		iLog->Log( _L8( "    iDefaultValue='%S'" ), CHK_NULL(iDefaultValue));
		iLog->Log( _L8( "    iDescription='%S'" ), CHK_NULL(iDescription ));
		iLog->Log( _L8( "    iFormat=%d (%S)" ), iFormat, &strFormat );
		iLog->Log( _L8( "    iOccurence=%d (%S)" ), iOccurence, &strOccurence );
		iLog->Log( _L8( "    iScope=%d (%S)" ), iScope, &strScope );
		iLog->Log( _L8( "    iTitle='%S'" ), CHK_NULL(iTitle ));
		iLog->Log( _L8( "    iMimeType='%S'" ), CHK_NULL(iMimeType ));
		iLog->Log( _L8( "----------------------------------" ) );
		}
	
	/* Dump children */
	for (TInt i = 0 ; i < iChildren.Count() ; i++)
		{
		iChildren[i]->DumpL( *fullName, aFullDump );
		}

	CleanupStack::PopAndDestroy( fullName );
	}

/*
void CTestDmDDFObject::ExternalizeL(RWriteStream& aStream) const
	{
	X;
	aStream << *iName << eol;
	}
*/

const TDesC8& CTestDmDDFObject::Name()
	{
	return (iName != 0) ? *iName : KNullDesC8();
	}

TSmlDmAccessTypes  CTestDmDDFObject::AccessTypes()
	{
	return iAccessTypes;
	}

const TDesC8& CTestDmDDFObject::DefaultValue()
	{
	return (iDefaultValue != 0) ? *iDefaultValue : KNullDesC8();
	}

const TDesC8& CTestDmDDFObject::Description()
	{
	return (iDescription != 0) ? *iDescription : KNullDesC8();
	}

CTestDmDDFObject::TDFFormat CTestDmDDFObject::DFFormat()
	{
	return iFormat;
	}

CTestDmDDFObject::TOccurence CTestDmDDFObject::Occurence()
	{
	return iOccurence;
	}

CTestDmDDFObject::TScope CTestDmDDFObject::Scope()
	{
	return iScope;
	}

const TDesC8& CTestDmDDFObject::DFTitle()
	{
	return (iTitle != 0) ? *iTitle : KNullDesC8();
	}

const TDesC8& CTestDmDDFObject::DFTypeMimeType()
	{
	return (iMimeType != 0) ? *iMimeType : KNullDesC8();
	}

TBool CTestDmDDFObject::ObjectGroup()
	{
	return iObjectGroup;
	}

TInt CTestDmDDFObject::ChildObjectCount()
	{
	return iChildren.Count();
	}

CTestDmDDFObject& CTestDmDDFObject::ChildObject( TInt aIndex )
	{
	return *iChildren[aIndex];
	}

CTestDmDDFObject* CTestDmDDFObject::FindChildObject( const TDesC8& aName )
	{
	CTestDmDDFObject* foundObject = 0;

	for (TInt i = 0 ; i < iChildren.Count() ; i++)
		{
		if ( aName.CompareC( iChildren[i]->Name() ) == 0 )
			{
			foundObject = iChildren[i];
			break;
			}
		}

	return foundObject;
	}

TInt CTestDmDDFObject::SubObjectsCount()
	{
	TInt count = 0;

	TInt i;
	for (i = 0 ; i < iChildren.Count() ; i++)
		{
		count += iChildren[i]->SubObjectsCount();
		}
	count += i;

	return count;
	}

/*
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  End of File  
