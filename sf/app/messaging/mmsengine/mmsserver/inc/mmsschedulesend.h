/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     scheduled sending of MMS messages
*
*/



#ifndef MMSSCHEDULESEND_H
#define MMSSCHEDULESEND_H

//  INCLUDES
#include <msvschedulesend.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Scheduled sending and receiving.
*/
class CMmsScheduleSend :public CMsvScheduleSend
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * Note: Base class ConstructL is used
        * @param aServerEntry Initial Entry
        */
        static CMmsScheduleSend* NewL(CMsvServerEntry& aServerEntry);

        /**
        * Destructor.
        */
        virtual ~CMmsScheduleSend();

    public: // New functions
        
        /**
        * LoadSettingsL loads the schedule settings
        */
        void LoadSettingsL( TInt aCommand );

    public: // Functions from base classes

        /**
        * From CMsvScheduleSend get scheduled message.
        * @param aId Id of a message
        * @return pointer to restored message data
        */
        CMsvScheduledEntry* GetMessageL(const TMsvId aId) const;
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * C++ constructor.
        * @param aServerEntry Initial Entry
        */
        CMmsScheduleSend( CMsvServerEntry& aServerEntry );
        

    private:

        // By default, prohibit copy constructor
        CMmsScheduleSend( const CMmsScheduleSend& );
        // Prohibit assignment operator
        CMmsScheduleSend& operator= ( const CMmsScheduleSend& );

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSSCHEDULESEND_H   
            
// End of File
