/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: An observer and I/F class to observe Publish and Subscribe key changes.  
*
*/



#ifndef CPROPERTYOBSERVER_H
#define CPROPERTYOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// CLASS DECLARATION

/**
 * MPropertyChangeHandler.
 * Abstract interface for handling property change events.
 **/

NONSHARABLE_CLASS( MPropertyChangeHandler )
{
 public:
    /**
     * This is a callback function which is called when a property is changed.
     *
     * @param aCategory  UID of the category.
     * @param aKey       changed key
     * @param aValue     the new value
     *
     * Note! references aKey and aValue are only valid while executing
     * HandlePropertyChange(). After that the data in which they refer can change.
     **/
    virtual void HandlePropertyChange(const TUid aCategory, const TUint aKey, const TInt aValue) = 0;

};

// INCLUDES

// CONSTANTS

// CLASS DECLARATION

/**
 * ?one_line_short_description.
 * ?other_description_lines
 *
 * @lib AknAlarmService
 * @since Series 60 2.8
 **/
NONSHARABLE_CLASS(CPropertyObserver) : public CActive
{
 public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     * @since Series 60 2.8
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     **/
    static CPropertyObserver* NewL(MPropertyChangeHandler& aHandler, TUid aCategory, TUint aKey);

    /**
     * Destructor.
     **/
    ~CPropertyObserver();

 private:
    /**
     * C++ default constructor overload.
     **/
    CPropertyObserver(MPropertyChangeHandler& aHandler, TUid aCategory, TUint aKey);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL(void);

 protected:  // Functions from base classes
    /**
     * From CActive ?member_description
     **/
    void RunL(void);

    /**
     * From CActive ?member_description
     **/
    void DoCancel(void);

 private:  // New functions
     /**
      * ?member_description.
      * @since Series 60 2.8
      **/
     void Subscribe(void);

    /**
      * ?member_description.
      * @since Series 60 3.0
      **/
     void NotifyCurrentValue(void);

 private:  // Data
    // ?one_line_short_description_of_data
    RProperty iProperty;

    // ?one_line_short_description_of_data
    TUid iCategory;

    // ?one_line_short_description_of_data
    TUint iKey;

    // ?one_line_short_description_of_data
    MPropertyChangeHandler& iHandler;

};

#endif  // CPROPERTYOBSERVER_H


// End of File
