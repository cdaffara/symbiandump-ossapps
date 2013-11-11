/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the interface required to be implemented by
*                table classes used by the mpxdbmanager.
*
*/

#ifndef MPXTABLE_H
#define MPXTABLE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RSqlDatabase;

// CLASS DECLARATION
class MMPXTable
{
    public:
    /**
    * Create table query in the database
    * @param aDatabase the database to create the table
    * @param aCorruptTable indicates that the table is created because it was corrupt
    * in the database
    */
    virtual void  CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable) = 0;

    /**
    * Drop table query in the database
    * @param aDatabase the database to drop the table
    */
    virtual void DropTableL(RSqlDatabase& aDatabase) = 0;

    /**
    * Check that table is valid in the database
    * @param aDatabase the database to check that the table exists and has correct fields
    */
    virtual TBool CheckTableL(RSqlDatabase& aDatabase) = 0;
};

#endif  // MPXTABLE_H
