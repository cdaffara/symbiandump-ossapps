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
* Description:
*
*/


#ifndef C_RADIOENGINETLS_H
#define C_RADIOENGINETLS_H

#include <e32base.h>

class MRadioEngineLogger;
class CCoeEnv;
class RFs;
class CRadioEngineLogger;

/**
 * TLS handler
 */
class CRadioEngineTls : public CBase
    {

friend class RadioEngineUtils;

public:

    /**
     * Returns the CRadioEngineTls instance from TLS
     *
     * @since Live TV UI v1.0
     * @return Reference to CRadioEngineTls instance
     */
    static CRadioEngineTls& Instance();

    /**
     * Return the logger storage if logging is enabled
     *
     * @since Live TV UI v1.0
     */
    static MRadioEngineLogger* Logger();

    /**
     * Returns the file server session
     *
     * @since Live TV UI v1.0
     * @return Reference to file server session
     */
    static RFs& FsSession();

    /**
     * Increase reference count
     */
    void AddRef();

    /**
     * Decrease reference count
     * Deletes self when ref count reaches zero.
     */
    void Release();

private:

    static void InitializeL( RFs* aFs = NULL );

    CRadioEngineTls( RFs* aFs );

    ~CRadioEngineTls();

    void ConstructL();

#ifdef LOGGING_ENABLED
    /**
     * File logger
     * Own.
     */
    CRadioEngineLogger* iLogger;
#endif

    /**
     * Flag to indicate whether or not the file server session is owned
     */
    TBool       iFsOwned;

    /**
     * File server session.
     * Own status determined by iFsOwned member variable
     */
    RFs*        iFs;

    /**
     * Reference count
     */
    TInt        iRefs;

    };

#endif // C_RADIOENGINETLS_H
