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
* Description:  Represents a pending preset that has not yet been committed
*
*/


#ifndef C_CPSPENDINGPRESET_H
#define C_CPSPENDINGPRESET_H

#include <e32base.h>

/**
 * Represents a pending preset that has not yet been committed.
 */
NONSHARABLE_CLASS( CPSPendingPreset ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param   aId     Id of the preset.
     */
    static CPSPendingPreset* NewL( TInt aId );

    /**
     * Destructor.
     */
    ~CPSPendingPreset();

    /**
     * Returns the id of the preset.
     *
     * @return  Id of the preset.
     */    
    TInt Id() const;

    /**
     * Sets the index of the preset.
     *
     * @param   aIndex      Index of the preset.
     */    
    void SetIndex( TInt aIndex );

    /**
     * Returns the index of the preset.
     *
     * @return  Index of the preset.
     */    
    TInt Index() const;

    /**
     * Sets the name of the preset.
     *
     * @param   aName       Name of the preset.
     */
    void SetNameL( const TDesC& aName );

    /**
     * Returns the name of the preset.
     *
     * @return  Name of the preset.
     */
    const TDesC& Name() const;    

    /**
     * Sets the data of the preset.
     *
     * @param   aData       Data of the preset.
     */    
    void SetDataL( const TDesC8& aData );

    /**
     * Returns the data of the preset.
     *
     * @return  Data of the preset.
     */    
    const TDesC8& Data() const;
    
private:

    /**
     * Constructor.
     *
     * @param   aId     Id of the preset.
     */
    CPSPendingPreset( TInt aId );

    /**
     * Second-phase constructor.
     */
    void ConstructL();

private:

    /** Id of the preset. */
    TInt iId;
    /** Index of the preset. */
    TInt iIndex;
    /** Name of the preset. */
    RBuf iName;
    /** Data of the preset. */
    RBuf8 iData;

    };

#endif // C_CPSPENDINGPRESET_H
