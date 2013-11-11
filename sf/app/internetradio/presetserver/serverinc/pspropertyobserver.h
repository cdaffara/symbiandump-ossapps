/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer for Publish & Subscribe keys
*
*/


#ifndef CPSPROPERTYOBSERVER_H
#define CPSPROPERTYOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/** 
 * Defines text property with maximum size, that is RProperty::
 * KKMaxPropertySize bytes. 
 */
typedef TBuf8<RProperty::KMaxPropertySize> TPSTextProperty;

/**
 * MPSPropertyChangeObserver
 *
 * Abstract interface for handling property change events.
 */
class MPSPropertyChangeObserver
{

public:
 
    /**
     * This is a callback function which is called when a property value of 
     * type int is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     *
     * @param aValue the new value
     */
    virtual void HandlePropertyChangeL(
        const TUid& aCategory, 
        const TUint aKey, 
        const TInt aValue ) = 0;
    
    /**
     * This is a callback function which is called when a property value of 
     * type text is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aValue the new value
     **/
    /*lint -save -e1053*/
    virtual void HandlePropertyChangeL( 
        const TUid& aCategory, 
        const TUint aKey, 
        const TPSTextProperty& aValue ) = 0;
    /*lint -restore*/
    
    /**
     * This is a callback function which is called when a P&S components 
     * returns an error
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event
     * @param aError an error code
     */
    virtual void HandlePropertyChangeErrorL(
        const TUid& aCategory, 
        const TUint aKey, 
        TInt aError) = 0;
    
};



/**
 * Observer class that observes changes of Property values and propogates 
 * them further.
 * The class defines a handle to a property, a single data value representing
 * an item of state information.
 *
 */
 class CPSPropertyObserver : public CActive
	{  
	
 public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aPropertyType property type to observe
     */
    static CPSPropertyObserver* NewL(
        MPSPropertyChangeObserver& aObserver, 
        const TUid& aCategory, 
        const TUint aKey, 
        const RProperty::TType aPropertyType);

    /**
     * Destructor.
     */
    ~CPSPropertyObserver();
    
    /**
     * Updates an int value reference in correspondence with the current 
     * Property value
     *
     * @param aValue handle to a value which will be updated
     **/	
	void GetValue( TInt& aValue ) const;
	
	/**
     * Updates a text value reference in correspondence with the current 
     * Property value
     *
     * @param aValue handle to a value which will be updated
     */	
    /*lint -save -e1053*/
	void GetValue( TPSTextProperty& aValue ) const;
    /*lint -restore*/
		
private:

    /**
     * C++ default constructor overload.
     * Two-phased constructor.
     *
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aPropertyType property type to observe
     */
	CPSPropertyObserver(
	    MPSPropertyChangeObserver& aObserver, 
	    const TUid& aCategory, 
	    const TUint aKey, 
	    const RProperty::TType aPropertyType);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL();

protected:  
    
    // from base class Cactive
    /**
     * From CActive
     *
     * @see CActive::RunL()
     */
    void RunL();

    /**
     * From CActive, 
     * 
     * @see CActive::DoCancel()
     */
    void DoCancel();

private:  

 	/** 
 	 * Value of a subscribed category property of type int. 
 	 */
 	TInt iValueInt;
 	
 	/** 
 	 * Value of a subscribed category property of type text. 
 	 */
 	TPSTextProperty iValueText;
 	
    /** 
     * Handle to Publish And Subscribe component
     */ 
    RProperty iProperty;
    
    /**
     * Observer to be notified when particular Property value has changed
     */
    MPSPropertyChangeObserver& iObserver;
    
    /** 
     * UID of Publish And Subscribe category
     */
    TUid iCategory;
    
    /**
     * Subkey to be used with Publish And Subscribe
     */
    TUint iKey;
    
    /** 
     * Type of the observed property. 
     */
    RProperty::TType iPropertyType;
    
	};

#endif  // CPSPROPERTYOBSERVER_H

