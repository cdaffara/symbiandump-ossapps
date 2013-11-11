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
* Description:  Utility class
 *
*/


#ifndef _C_CS_MSG_PLUGIN_UTILITY_H_
#define _C_CS_MSG_PLUGIN_UTILITY_H_

// SYSTEM INCLUDES
#include <msvapi.h>
#include <smuthdr.h>                    // CSmsHeader
#include <txtetext.h>
#include <msvids.h>
#include <ccsdefs.h>
#include <ccsconversationentry.h>

// FORWARD DECLARATION
class CCsConversationEntry;

/**
 *  Message plugin Utility Class
 *  This class provides some utility methods
 */
class CCsMsgPluginUtility: public CBase
    {

public:

    /**
     * Two phase construction
     */
    static CCsMsgPluginUtility* NewL();

    /**
     * Destructor
     */
    virtual ~CCsMsgPluginUtility();

    /**
     * Creates CCsConversationEntry using input parameters
     *
     * @since S60
     * @param aContact Conatct details
     * @param aEnryId Unique Id 
     * @param aTimeStamp time stamp details
     * @param aDir TMsvId of Parent
     * @param aDescription description of the body message
     * @param aSendState TCsSendState enum value
     * @param aMsgAttribs TCsAttribute enum value
     * @return  CCsConversationEntry object
     */
    CCsConversationEntry* CreateConversationEntryL(
            const HBufC* aContact,
            TMsvId aEnryId,
            TInt64 aTimeStamp,
            TMsvId aDir,
            const HBufC* aDescription,
            TCsSendState aSendState,
            TCsAttribute aMsgAttribs,
            TCsType aCsType) const ;

    /**
     * Map the call direction to TCsDirection.
     *
     * @since S60
     * @param aDir The direction string which needs to be mapped
     * @return TCsDirection enum value
     */
    TCsDirection MapDirection( TMsvId aDir ) const;

    /**
     * Returns the message attributes as a bitmask of TCsAttribute values.
     *
     * @param aEntry Message entry.
     * @return A bitmask of TCsAttribute values.
     * @see TCsAttribute
     */
    TUint16 GetMsgAttributes( const TMsvEntry& aEntry );

    /**
     * Returns the send status.
     *
     * @param aContext Message entry.
     * @return send status like failed, sent, suspended etc.
     * @see TCsSendState
     */
    TCsSendState GetSendState( const TMsvEntry& aContext );

    /**
     * Create the Contact (From/To) of the message
     *
     * @param aSession Message Server session
     * @param aEntry Message entry
     * @param addressList List from extracted from/to addresses
     */
    void CreateContactL( CMsvSession* aSession,
                         const TMsvEntry& aEntry,
                         RPointerArray<HBufC>& addressList);

    /**
     * Compare previous entry with new entry
     *
     * @param aOldContext previous Message entry
     * @param aNewContext new Message entry
     * @param aDir entry folder
     * @return  ETrue/EFalse for same/different
     */
    TBool CompareEntry( const TMsvEntry& aOldContext,
                        const TMsvEntry& aNewContext, 
                        TMsvId aDir ) const;

    /**
     * Extracts name and address from the aMsvAddress
     * which is of the form name<address>
     * @param aMsvAddress which is of the form name<address>
     * @param aName  extracted name
     * @param aAddress extracted address
     */
    void NameAndAddress( const TPtrC& aMsvAddress, 
                         TPtrC& aName, 
                         TPtrC& aAddress );

    /**
     * Extract the address part for MMS ignoring the alias
     */
    TPtrC PureAddress(const TDesC& aAddress);

private:
    /**
     * Constructor
     */
    CCsMsgPluginUtility();

    };

#endif // _C_CS_MSG_PLUGIN_UTILITY_H_
