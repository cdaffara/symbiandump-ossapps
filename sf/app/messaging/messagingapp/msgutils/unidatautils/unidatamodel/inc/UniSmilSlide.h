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
* Description: 
*       CUniSmilSlide
*
*
*/


#ifndef __UNISMILSLIDE_H
#define __UNISMILSLIDE_H


//  INCLUDES

#include <MsgMedia.hrh>

#include <UniModelConst.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CUniObject;

// DATA TYPES
typedef CArrayPtrFlat<CUniObject> CSmilObjectArray;

// FUNCTION PROTOTYPES



// CLASS DECLARATION

// NOTE: Since slide cannot have same object type twice unique key to object within
//       slide is it object type ( EImage, EText, EAudio )
//       Object can also be referenced by it's position in slide ( 0,1,2 )

/**
* CUniSmilSlide - Representation of a single slide in a SMIL presentation.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniSmilSlide :public CBase
    {
    public:  // Constructors and destructor

    /**
    * Factory method
    *
    * @since    3.1
    * @return   Pointer to instance in cleanup stack
    */
    static CUniSmilSlide* NewLC( TUniLayout aLayout );

    /**
    * Factory method
    *
    * @since    3.1
    * @return   Pointer to instance
    */
    static CUniSmilSlide* NewL( TUniLayout aLayout );

    /**
    * Destructor.
    *
    * @since    3.1
    */
    ~CUniSmilSlide();

    public: // New functions

    /*
    * AddObjectL
    * Add object to slide in position specified by Layout.
    * NOTE: Takes ownership of the aObject.
    * NOTE: Leaves with KErrArgument if object cannot be added to slide
    *
    * @since    3.1
    * @param    aObject IN object to be added
    */
    void AddObjectL( CUniObject* aObject );

    /*
    * AddObjectL
    * Add object to slide in position specified by Layout.
    * NOTE: Takes ownership of the aObject.
    * NOTE: Leaves with KErrArgument if object cannot be added to slide
    *
    * @since    3.1
    * @param    aObject IN object to be added
    * @param    aregion IN region where object is in SMIL.
    */
    void AddObjectL( CUniObject* aObject, TUniRegion aRegion );

    /**
    * RemoveObject
    * Removes object from slide
    * NOTE: Object is not deleted.
    *
    * @since    3.1
    * @param    aObject IN object to be removed
    */
    void RemoveObject( CUniObject* aObject );

    /**
    * GetObject
    * Get object of given type from slide.
    *
    * @since    3.1
    * @param    aObjType    IN object type
    * @return   Pointer to object,
    *           NULL, if object not found
    */
    CUniObject* GetObject( TUniRegion aRegion ) const;

    /**
    * GetObjectByIndex
    * Get object at given index from slide.
    *
    * @since    3.1
    * @param    aObjNum IN index to slides object array,
    *                   0 (zero) is first object.
    * @return   Pointer to object,
    *           NULL, if object not found
    */
    CUniObject* GetObjectByIndex( TInt aObjNum ) const;

    /**
    * IsObject
    * Checks if slide contains given object.
    *
    * @since    3.1
    * @param    aObject IN object to be checked
    * @return   ETrue slide contains the object,
    *           EFalse, otherwise
    */
    TBool IsObject( CUniObject* aObject ) const;

    /**
    * IsObject
    * Checks if slide has an object of given type
    *
    * @since    3.1
    * @param    aObjType    IN object type
    * @return   ETrue slide contains the object,
    *           EFalse, otherwise
    */
    TBool IsObject( TUniRegion aRegion ) const;

    /**
    * ObjectCount
    * Accessor
    *
    * @since    3.1
    * @return   Number of objects in this slide
    */
    TInt ObjectCount() const;

    /**
    * UpdateLayoutL
    * Update the layout of the slide
    *
    * @since    3.1
    * @param    aNewLayout  IN new layout
    */
    void UpdateLayoutL( TUniLayout aNewLayout );

    private:

    /*
    * Constructor
    *
    * @since    3.1
    */
    CUniSmilSlide( TUniLayout aLayout );

    /*
    * 2nd phase constructor
    *
    * @since    3.1
    */
    void ConstructL();

    /**
    * UpdateLayoutL
    * Update the layout of the slide
    *
    * @since    3.1
    */
    void UpdateLayoutL();

    private:

    CSmilObjectArray*   iObjectArray;
    TUniLayout          iLayout;

    };

#endif // __UNISMILSLIDE_H




