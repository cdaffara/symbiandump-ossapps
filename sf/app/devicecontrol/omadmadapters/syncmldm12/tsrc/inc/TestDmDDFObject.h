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






#ifndef __TESTDMDDFOBJECT_H
#define __TESTDMDDFOBJECT_H

//  INCLUDES
//#include <?include_file>
#include <smldmadapter.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CStifLogger;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CTestDmDDFObject : public CBase, public MSmlDmDDFObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestDmDDFObject* NewL( CStifLogger *aLog );
        static CTestDmDDFObject* NewLC( CStifLogger *aLog );
        
        /**
        * Destructor.
        */
        virtual ~CTestDmDDFObject();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

		virtual void SetNameL( const TDesC8& aName );
		
		void DumpL( const TDesC8& aParentName, TBool aFullDump = ETrue );
		void ExternalizeL(RWriteStream& aStream) const;
		
		

    public: // Functions from base classes
    
//sf-    *********************************
    
		void SetAccessTypesL( TSmlDmAccessTypes aAccessTypes );
		void SetDefaultValueL( const TDesC8& aDefaultValue );
		void SetDescriptionL( const TDesC8& aDescription );
		void SetDFFormatL( TDFFormat aFormat );
		void SetOccurenceL( TOccurence aOccurence );
		void SetScopeL( TScope aScope );
		void SetDFTitleL( const TDesC8& aTitle );
		void AddDFTypeMimeTypeL( const TDesC8& aMimeType );
		void SetAsObjectGroup();
		MSmlDmDDFObject& AddChildObjectL(const TDesC8& aNodeName);
		MSmlDmDDFObject& AddChildObjectGroupL();

//sf-    #############################

		const TDesC8& Name();
		TSmlDmAccessTypes  AccessTypes();
		const TDesC8& DefaultValue();
		const TDesC8& Description();
		TDFFormat DFFormat();
		TOccurence Occurence();
		TScope Scope();
		const TDesC8& DFTitle();
		const TDesC8& DFTypeMimeType();
		TBool ObjectGroup();
		
		TInt ChildObjectCount();
		CTestDmDDFObject& ChildObject( TInt aIndex );

		CTestDmDDFObject* FindChildObject( const TDesC8& aName );
		TInt SubObjectsCount();


    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CTestDmDDFObject( CStifLogger *aLog );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CTestDmDDFObject( const CTestDmDDFObject& );
        // Prohibit assigment operator if not deriving from CBase.
        // CTestDmDDFObject& operator=( const CTestDmDDFObject& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		HBufC8* iName;

		TSmlDmAccessTypes iAccessTypes;
		HBufC8* iDefaultValue;
		HBufC8* iDescription;
		TDFFormat iFormat;
		TOccurence iOccurence;
		TScope iScope;
		HBufC8* iTitle;
		HBufC8* iMimeType;
		TBool iObjectGroup;
		
		RPointerArray<CTestDmDDFObject> iChildren;

         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
        /**
        * Logger.
        */
        CStifLogger*    iLog;

    };

#endif      // __TESTDMDDFOBJECT_H
            
// End of File
