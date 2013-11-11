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
* Description:  Defines a transaction
*
*/


#ifndef C_CPSTRANSACTION_H
#define C_CPSTRANSACTION_H

#include <e32base.h>

/**
 * Pure virtual base class for transactions.
 */
NONSHARABLE_CLASS( MPSTransactionEntity )
    {

public:

    /**
     * Begins an explicit transaction.
     */
    virtual void BeginTransactionL() = 0;
    
    /**
     * Commits changes done during the transaction.
     */
    virtual void CommitL() = 0;
    
    /**
     * Reverts changes done during the transaction.
     */
    virtual void Rollback() = 0;

    };

/**
 * Defines a transaction.
 *
 * Transactions must be created whenever any value of the transaction is to be modified. They provide
 * means to synchronize transaction item updates so that they cannot be updated simultaneously by two clients.
 *
 * Transactions cannot be created as is, as they must be created through transactable entities.
 *
 * To create a transaction with a preset, use CPSPresetInterface::CreateTransactionL() or CPSPresetInterface::CreateTransactionLC().
 *
 * When a transaction is started, the transaction entity remains in locked state until it is either committed or rollbacked.
 */
NONSHARABLE_CLASS( CPSTransaction ) : public CBase
    {

public:

    /**
     * Destructor
     */
    IMPORT_C ~CPSTransaction();
    
    /**
     * Completes the transaction, committing all changes done to the preset.
     */
    IMPORT_C void CommitL();

    /**
     * Completes the transaction, reverting all changes done to the preset.
     */
    IMPORT_C void Rollback();

public:

    /**
     * Creates a new preset transaction object.
     * 
     * @param   aEntity     The transaction entity to start the transaction with.
     */
    static CPSTransaction* NewL( MPSTransactionEntity& aEntity );

private:

    /**
     * Constructor.
     * 
     * @param   aEntity     The transaction entity to start the transaction with.
     */
    CPSTransaction( MPSTransactionEntity& aEntity );

    /** 
     * Second-phase constructor.
     */
    void ConstructL();     
           
private:
    
    /** The transaction entity. */
    MPSTransactionEntity& iEntity;
    /** Extension. */
    TAny* iExtension;

    };

#endif // C_CPSTRANSACTION_H
