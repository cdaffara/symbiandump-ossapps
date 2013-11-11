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
* Description:  Flag container class definition.
*
*/


#ifndef T_LCVTFLAGCONTAINER_H
#define T_LCVTFLAGCONTAINER_H

#include <e32base.h>

/**
 *  TLcVtFlagContainer
 *
 *  Flag container is a class that holds maximum of 32 different flags. Also
 *  contains methods for setting, clearing and getting the flag state.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TLcVtFlagContainer )
    {
public:

    /**
     * Constructor.
     */
    inline TLcVtFlagContainer();

protected:

    /**
     * Clears all flags.
     */
    inline void Clear();

    /**
     * Tests flag.
     */
    inline TBool IsFlag( TUint32 aFlags ) const;

    /**
     * Sets flag(s).
     */
    inline void SetFlag( TUint32 aFlags );

    /**
     * Clears flag(s).
     */
    inline void ClearFlag( TUint32 aFlags );

    /**
    * Gets flag value.
    */
    inline TUint32 Value() const;

    /**
    * Sets flag value.
    */
    inline void SetValue( TUint32 aValue );

private:

    // Current flags
    TUint32 iFlags;

    };

/**
 *  TLcVtUpdateableFlagContainer
 *
 *  A flag container with Update() method that can be called to get
 *  flags updated in one pass.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TLcVtUpdateableFlagContainer ) : public TLcVtFlagContainer
    {
public:

    /**
    * Issues flag updation.
    */
    inline void Update();

private:

    /**
    * Called when flags needs to be updated (as a result to call Update()).
    */
    virtual void UpdateFlags() = 0;

    };

#include "tlcvtflagcontainer.inl"

#endif // T_LCVTFLAGCONTAINER_H
