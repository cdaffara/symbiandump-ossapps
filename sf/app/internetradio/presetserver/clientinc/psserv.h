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
* Description:  Header file for RPSServ
*
*/


#ifndef R_RPSSERV_H
#define R_RPSSERV_H

#include <s32mem.h>

#include "pscommon.h"

class CPSPresetInterface;

/**
 * Represents a session with the preset server.
 *
 * All communication with the server is done via this class.
 */
NONSHARABLE_CLASS( RPSServ ) : public RSessionBase
    {

    friend class RPSServNotifier;
    friend class RPSServSession;

public:

    /**
     * Constructor.
     */
    IMPORT_C RPSServ();

    /**
     * Connects the session to the preset server.
     *
     * @return  Any of the standard system error codes.
     */
    IMPORT_C TInt Connect();
    
    /**
     * Closes the session.
     */
    IMPORT_C void Close();
    
    /**
     * Creates a preset.
     *
     * If a preset with the supplied index and data handler exists, the call will leave with <code>KErrAlreadyExists</code>.
     *
     * @param   aIndex          Index of the preset.
     * @param   aDataHandler    Data handler of the preset.
     * @return  The newly created preset, ownership is transferred to the caller.
     */
    IMPORT_C CPSPresetInterface* CreatePresetL( TInt aIndex, TUid aDataHandler );
    
    /**
     * Opens a preset.
     *
     * If a preset with the specified id does not exist, the call will leave with <code>KErrNotFound</code>.
     *
     * @param   aId     Id of the preset to open.
     * @return  The opened preset, ownership is transferred to the caller.
     */
    IMPORT_C CPSPresetInterface* OpenPresetL( TInt aId );            

    /**
     * Deletes a preset.
     *
     * @param   aId     Id of the preset to delete.
     */
    IMPORT_C void DeletePresetL( TInt aId );
    
    /**
     * Moves a preset.
     *
     * If a preset with the specified id does not exist, the call will leave with <code>KErrNotFound</code>.
     *
     * @param   aId                 Id of the preset to move.
     * @param   aDestinationIndex   Index to move the preset to.
     */
    IMPORT_C void MovePresetL( TInt aId, TInt aDestinationIndex );

    /**
     * Gets a list of all presets.
     *
     * The presets can then be casted to their respective types by using the CPSPresetInterface::DataHandler() method.
     *
     * @param   aPresets    On return, will contain a list of all presets.
     */
    IMPORT_C void GetPresetsL( RPointerArray<CPSPresetInterface>& aPresets );
    
    /**
     * Gets a list of all presets matching the specified data handler.
     *
     * @param   aPresets        On return, will contain a list of all presets matching the specified data handler.
     * @param   aDataHandler    Data handler of the presets.
     */
    template <typename T>
    void GetPresetsL( RPointerArray<T>& aPresets, TUid aDataHandler );

private:

    /** Reserved. */
    TInt iReserved[2];

    };

#include <psserv.inl>

#endif // R_RPSSERV_H
