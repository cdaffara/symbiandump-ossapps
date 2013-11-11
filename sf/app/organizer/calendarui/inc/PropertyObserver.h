/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Property Observer class for handling property change events.
 *
*/



#ifndef CPROPERTYOBSERVER_H
#define CPROPERTYOBSERVER_H

// INCLUDES
#include <E32Base.h>
#include <E32Property.h>

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
    virtual void HandlePropertyChange(const TUid& aCategory, const TUint& aKey, const TInt& aValue) = 0;

};


// CLASS DECLARATION

/**
 * ?one_line_short_description.
 * ?other_description_lines
 *
 * @since Series 60 3.0
 **/
NONSHARABLE_CLASS(CPropertyObserver) : public CActive
{
 public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     * @since Series 60 3.0
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     **/
    static CPropertyObserver* NewL(MPropertyChangeHandler& aHandler, const TUid& aCategory, const TUint& aKey, const TBool aSkipValue=EFalse)
    {
        CPropertyObserver* self = new( ELeave )CPropertyObserver( aHandler, aCategory, aKey, aSkipValue );
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
    };

    /**
     * Destructor.
     **/
    ~CPropertyObserver()
    {
        Deque();
        iProperty.Cancel();
        iProperty.Close();
    };

 private:
    /**
     * C++ default constructor overload.
     **/
    CPropertyObserver(MPropertyChangeHandler& aHandler, const TUid& aCategory, const TUint& aKey, const TBool aSkipValue)
    : CActive( CActive::EPriorityStandard ),
      iCategory( aCategory ),
      iKey( aKey ),
      iHandler( aHandler ),
      iSkipValue( aSkipValue )
    {
    };

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL(void)
    {
        CActiveScheduler::Add( this );

        User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );

        //NotifyCurrentValue(); // notify initial value

        // Start observing...
        Subscribe();
    };

 protected:  // Functions from base classes
    /**
     * From CActive ?member_description
     **/
    inline void RunL(void);

    /**
     * From CActive ?member_description
     **/
    inline void DoCancel(void);

 private:  // New functions
    /**
     * ?member_description.
     * @since Series 60 3.0
     **/
    inline void Subscribe(void);

    /**
     * ?member_description.
     * @since Series 60 3.0
     **/
    inline void NotifyCurrentValue(void);

 private:  // Data
    // ?one_line_short_description_of_data
    RProperty iProperty;

    // ?one_line_short_description_of_data
    const TUid iCategory;

    // ?one_line_short_description_of_data
    const TUint iKey;

    // ?one_line_short_description_of_data
    MPropertyChangeHandler& iHandler;

    // ?one_line_short_description_of_data
    const TBool iSkipValue;

};

#include "PropertyObserver.inl"

#endif  // CPROPERTYOBSERVER_H


// End of File
