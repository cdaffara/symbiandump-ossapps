/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description :
 *
 */

#ifndef SNSRDISPLAYCONTROLCLIENT_H
#define SNSRDISPLAYCONTROLCLIENT_H

// INCLUDES
#include <e32base.h>

//FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Client-side interface to Screensaver Display Control Server
 *
 * 
 *
 */
class RSnsrDisplayControlClient: public RSessionBase
    {
public:

    /**
     * Construct the object.
     */
    IMPORT_C RSnsrDisplayControlClient();

    /**
     * Open session
     */
    IMPORT_C TInt Open();
    
    /**
     * Closes session
     */
    IMPORT_C void Close();

    /**
     * Return version information
     */
    IMPORT_C TVersion Version() const;

    IMPORT_C void SetDisplayFullPower();
    
    IMPORT_C void SetDisplayLowPower( TInt aStartRow, TInt aEndRow );
    
    IMPORT_C void SetDisplayOff();
    
private:

    TInt iError;
    
    };

#endif // SNSRDISPLAYCONTROLCLIENT_H
// End of File
