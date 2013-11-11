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
* Description:  Base class for data types that can be identified by an
*                identifier (extension id).
*
*/



#ifndef TVTENGTTYPE_H
#define TVTENGTTYPE_H

//  INCLUDES
#include <e32std.h>

// DATA TYPES

enum TVtEngTypeId
    {
    EVtEngTypeV1 = 0,
    EVtEngTypeDtmf
    };

// CLASS DECLARATION

/**
*  Base class for data types
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngtType
    {
    public: // constructor

        inline TInt TypeId() const;

    protected: // constructors
        /**
        * C++ constructor that only for descendant classes may call.
        */
        inline TVtEngtType( TVtEngTypeId aTypeId );

        /**
        * copy constructor
        */
        inline TVtEngtType( const TVtEngtType& aType );

    protected: // operators

        inline TVtEngtType& operator=( const TVtEngtType& aType );

    protected:
        // Extension id
        TInt  iTypeId;
    };

#include "TVtEngType.inl"

#endif      // TVTENGTTYPE_H
            
// End of File
