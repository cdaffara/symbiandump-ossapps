/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Music Player media key handler.
*              Helper class for sending response back to Remote Controller Framework.
*
*/

#ifndef MPMEDIAKEYREMCONKEYRESPONSE_H
#define MPMEDIAKEYREMCONKEYRESPONSE_H

#include <remconcoreapitarget.h>


NONSHARABLE_CLASS( MpMediaKeyRemConResponse ) : public CActive
{
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @param aRemConCoreApiTarget RemCon core target object.
     * @return Pointer to newly created object.
     */
    static MpMediaKeyRemConResponse* NewL( CRemConCoreApiTarget& aRemConCoreApiTarget );

    /**
     * Destructor.
     */
    virtual ~MpMediaKeyRemConResponse();

    /**
     * Send the any key response back to Remcon server
     *
     * @since 3.0
     * @param aOperationId RemCon operation Id.
     */
    void CompleteAnyKey( TRemConCoreApiOperationId aOperationId );

private:

    /**
     * C++ default constructor.
     */
    MpMediaKeyRemConResponse( CRemConCoreApiTarget& aRemConCoreApiTarget );

// from base class CActive

    /**
     * From CActive
     * Handles an active object's request completion event.
     */
    void RunL();

    /**
     * From CActive
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

private:    // Data

    // Response array.
    RArray<TRemConCoreApiOperationId> iResponseArray;   // Own
    CRemConCoreApiTarget& iRemConCoreApiTarget;         // Not own
};

#endif  // MPMEDIAKEYREMCONKEYRESPONSE_H

// End of File

