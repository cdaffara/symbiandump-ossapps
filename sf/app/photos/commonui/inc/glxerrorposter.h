/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Singleton that posts errors to the framework
*
*/



#ifndef GLXERRORPOSTER_H_
#define GLXERRORPOSTER_H_

#include <e32base.h>

/**
 * CGlxErrorPoster
 * Posts errors to the framework.
 * (Causes a Leave() to occur from a RunL())
 *
 * @author Alex Birkett
 */
NONSHARABLE_CLASS(CGlxErrorPoster) : public CActive
    {
public:
    /**
     * Provides instance of error poster and increments ref count
     */
    IMPORT_C static CGlxErrorPoster* InstanceL();

    /**
     * Removes instance of error poster and decrements ref count
     */
    IMPORT_C void Close();

    /**
     * Post an error
     * The error is stored in a member variable and providing the
     * active object is not already active, a call back to RunL()
     * is requested. When called by the active scheduler RunL()
     * leaves with the error code stored in the member variable. 
     * This causes a dialog displaying an error note to be invoked
     * by the framework.
     *
     * @param aError Error to post
     */
    IMPORT_C void PostError(TInt aError);

public: // from CActive
    /** See @ref CActive::RunL */
    void RunL();

    /** See @ref CActive::DoCancel */
    void DoCancel();

private:
    /**
     * Two phase constructor
     */
    static CGlxErrorPoster* NewL();

    /**
     * Constructor
     */
    CGlxErrorPoster();

    /**
     * Destructor
     */
  	~CGlxErrorPoster();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private:
    /**
     * latest error
     */
    TInt iLatestError;
    };

#endif /*GLXERRORPOSTER_H_*/
