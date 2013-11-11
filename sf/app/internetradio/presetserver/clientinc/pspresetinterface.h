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
* Description:  Common interface for preset handling
*
*/


#ifndef C_CPSPRESETINTERFACE_H
#define C_CPSPRESETINTERFACE_H

#include "pspresetobserver.h"
#include "pstransaction.h"

const TUid KPSPresetInterface = { 0x10281CB6 }; /**< The ECOM interface UID for preset plugins. */

class CPSPresetNotifier;
class CPSPresetTransaction;
class RReadStream;
class RPSServ;
class RPSServSession;
class RWriteStream;

/**
 * Common interface for preset handling.
 *
 * All preset plugins must inherit from this class, and implement the InternalizeL( RReadStream& aStream )
 * and ExternalizeL( RWriteStream& aStream ) methods.
 *
 */
class CPSPresetInterface : public CBase, public MPSPresetObserver, public MPSTransactionEntity
    {

public:

    /**
     * Destructor
     */
    IMPORT_C ~CPSPresetInterface();
    
    /**
     * Creates and starts an explicit transaction.
     *
     * If a transaction is not active when using the setters, they will panic with <code>KErrNotReady</code>.
     *
     * Creating a transaction leaves with <code>KErrLocked</code> if another transaction for the same preset is active.
	 *
     * Transactions are completed with either CommitL or RollbackL.
     */
    IMPORT_C CPSTransaction* CreateTransactionL();

    /**
     * Creates and starts explicit transaction leaving it on the cleanup stack.
     *
     * @see CPSPresetTramsaction::CreateTransactionL()
     */
    IMPORT_C CPSTransaction* CreateTransactionLC();

    /**
     * Sets the name of the preset.
     *
     * @param   aName       The name of the preset. 
     */
    IMPORT_C void SetNameL( const TDesC& aName );
    
    /**
     * Returns the name of the preset.
     *
     * @return  The name of the preset.
     */
    IMPORT_C const TDesC& Name() const;

    /**
     * Sets the index of the preset.
     *
     * @param   aIndex      The index of the preset.
     */
    IMPORT_C void SetIndex( TInt aIndex );
    
    /**
     * Returns the index of the preset.
     *
     * @return  The index of the preset.
     */
    IMPORT_C TInt Index() const;

    /**
     * Returns the id of the preset.
     *
     * @return  The id of the preset.
     */
    IMPORT_C TInt Id() const;

    /**
     * Returns the data handler of the preset.
     *
     * @return  The data handler of the preset.
     */
    IMPORT_C TUid DataHandler() const;

protected:

    /**
     * Completes the construction of the preset interface.
     *
     * All classes deriving from this class must call BaseConstructL( TAny* aParams ) before
     * trying to use any of the functionality provided.
     *
     * @param   aParams     The ECOM construction parameters, as supplied to the plugin's
     *                      <code>NewL( TAny* aParams )</code> by the ECOM framework.
     */
    IMPORT_C void BaseConstructL( TAny* aParams );

    /**
     * Validates the preset for data change.
     *
     * Implementing plugins must call this whenever any of their functions that modify the external
     * status of the preset are called before making any changes to the preset's values.
     *
     * Checks if there is a transaction open for this preset, and if not, panics the application as it
     * indicates a programming error.
     */
    IMPORT_C void ValidateDataChange();

    /**
     * Internalizes the preset.
     *
     * This is called when the preset's data is changed, and needs to be updated.
     *
     * @param   aStream     Read stream containing the preset's data.
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;

    /**
     * Externalizes the preset.
     *
     * Called when the preset is about to be committed, and the data provided by the plugin 
     * needs to be externalized.
     *
     * @param   aStream     Write stream to write the preset's data to.
     */
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;

private:

// from base class MPSPresetObserver

    /**
     * @see MPSPresetObserver::HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
     */
    IMPORT_C void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );

// from base class MPSTransactionEntity
    
    /**
     * @see MPSTransactionEntity::BeginTransactionL()
     */
    IMPORT_C void BeginTransactionL();

    /**
     * @see MPSTransactionEntity::CommitL()
     */
    IMPORT_C void CommitL();
    
    /**
     * @see MPSTransactionEntity::Rollback()
     */
    IMPORT_C void Rollback();

public:

    /**
     * Creates a new preset interface object.
     * 
     * @param   aServ           reference to session, used to communicate to server
     * @param   aId             id of the preset 
     * @param   aDataHandler    datahandler uid, used in preset creation
     */
    static CPSPresetInterface* CreateL( RPSServ& aServ, TInt aId, TUid aDataHandler );

private:

    /**
     * Updates the preset.
     */
    void UpdatePresetL();

private:

    /** The preset change notifier. */
    CPSPresetNotifier* iNotifier;
    /** Session to the preset server. */
    RPSServSession* iSession;
    /** <code>ETrue</code> if the preset is in transaction, <code>EFalse</code> otherwise. */
    TBool iTransaction;
    /** Name of the preset. */
    HBufC* iName;
    /** Index of the preset. */
    TInt iIndex;
    /** Identifier of the preset. */
    TInt iId;
    /** Data handler of the preset. */
    TUid iDataHandler;
    /** Destructor id. */
    TUid iDtor;
    /** Extension. */
    TAny* iExtension;

    };

#endif // C_CPSPRESETINTERFACE_H
