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

#ifndef M_RADIOENGINELOGGER_H
#define M_RADIOENGINELOGGER_H

#include <e32std.h>

_LIT( KMarkerEngine, "RadioEngine:" );
_LIT( KMarkerUi,     "RadioUI:    " );

/**
 * Common interface for logger types.
 */
NONSHARABLE_CLASS( MRadioEngineLogger )
    {

public:

    IMPORT_C static MRadioEngineLogger* Logger();

    /**
     * Clear the log.
     */
    virtual MRadioEngineLogger& ClearLog() = 0;

    /**
     * Adds a 8-bit string to log line
     */
    virtual MRadioEngineLogger& Add( const TDesC8& aMsg ) = 0;

    /**
     * Adds a 16-bit string to log line
     */
    virtual MRadioEngineLogger& Add( const TDesC& aMsg ) = 0;

    /**
     * Adds a TInt to log line
     */
    virtual MRadioEngineLogger& Add( TInt aInt ) = 0;

    /**
     * Adds a TReal to log line
     */
    virtual MRadioEngineLogger& Add( const TReal& aReal ) = 0;

    /**
     * Adds a c-style string to log line
     */
    virtual MRadioEngineLogger& Add( const char* aText ) = 0;

    /**
     * Adds a pointer value to log line
     */
    virtual MRadioEngineLogger& Add( const TAny* aPtr ) = 0;

    /**
     * Adds a timestamp of current time to log line
     */
    virtual MRadioEngineLogger& Add( const TTime& aTime ) = 0;

    /**
     * Adds a timestamp of current time to log line
     */
    virtual MRadioEngineLogger& AddTimestamp() = 0;

    /**
     * Adds a formatted string to log line
     */
    virtual MRadioEngineLogger& AddFormat( TRefByValue<const TDesC> aFmt, ... ) = 0;

    /**
     * Adds the line indentation to log line
     */
    virtual MRadioEngineLogger& AddIndent( const TDesC& aMarker ) = 0;

    /**
     * Adds the line indentation to log line
     */
    virtual MRadioEngineLogger& AddIndentClear( const TDesC& aMarker ) = 0;

    /**
     * Increment indentation
     */
    virtual MRadioEngineLogger& IncIndent() = 0;

    /**
     * Decrement indentation
     */
    virtual MRadioEngineLogger& DecIndent() = 0;

    /**
     * Commits the log line to file and RDebug and resets internal buffer
     * @param aNewLine ETrue if newline is to be added, EFalse if not
     */
    virtual MRadioEngineLogger& Commit( TBool aNewLine = ETrue ) = 0;

    };

#endif // M_RADIOENGINELOGGER_H
