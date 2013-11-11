/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   operation for sending a read report
*
*/



#ifndef MMSREADREPORT_H
#define MMSREADREPORT_H

#include "mmsbaseoperation.h"

class CMmsHeaders;

/**
 *  Operation to send a read report.
 *
 *  @lib mmssrv.lib
 *  @since v3.1
 */
NONSHARABLE_CLASS( CMmsReadReport ): public CMmsBaseOperation
    {

public:

    static CMmsReadReport* NewL( RFs& aFs, CMmsSettings* aMmsSettings );

    virtual ~CMmsReadReport();

    /**
     * Failed
     * @return selection of failed entries
     */
    CMsvEntrySelection& Failed() const;

// from base class CMmsBaseOperation

    /**
     * From CMmsBaseOperation
     * Start the state machine.
     *
     * @since v3.1
     * @param aSelection list of message entry ids to be handled.
     * @param aServerEntry CMsvServerEntry pointer from CMmsServer
     * @param aSettings MMSC settings (entry points)
     * @param aService current MMS service id
     * @param aStatus TRequestStatus of the calling active object
     */
    void StartL(
        CMsvEntrySelection& aSelection,
        CMsvServerEntry& aServerEntry,
        TMsvId aService,
        TRequestStatus& aStatus );
        
protected:

// from base class CMsgActive
    /**
     * From CMsgActive
     * Active object cancellation.
     *
     * @since v3.1
     */
    void DoCancel();
    
    /**
     * From CMsgActive: Complete current operation.
     * Do whatever cleanup is possible. (Delete incomplete entry etc.)
     * @param aError Error code received by RunL
     */
    void DoComplete( TInt& aError );

    
// from base class CMmsBaseOperation

    /**
     * From CMmsBaseOperation
     * Encode one read report request
     *
     * @since v3.1
     */
    void EncodePDUL();
    
    /**
     * From CMmsBaseOperation
     * Delete the handled entry
     *
     * @since v3.1
     */
    void MoveEntryL();



private:

    CMmsReadReport( RFs& aFs );

    void ConstructL( CMmsSettings* aMmsSettings );

private: // data

    /**
     * mms headers - contain the read report data to be sent
     * Own.
     */
     CMmsHeaders* iMmsHeaders;

    };


#endif // MMSREADREPORT_H
