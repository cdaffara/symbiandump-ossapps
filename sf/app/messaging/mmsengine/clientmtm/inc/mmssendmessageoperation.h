/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Operation class for SendAs API send support
*
*/




#ifndef MMSSENDMESSAGEOPERATION_H
#define MMSSENDMESSAGEOPERATION_H

//  INCLUDES
#include <msvapi.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class to support SendAs API send operation.
*  @since 3.0
*/
NONSHARABLE_CLASS( CMmsSendMessageOperation ) : public CMsvSendOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aMsvSession message server session
        * @param aSelection entries to be sent
        * @param aParameter send parameters
        * @ObserverRequestStatus status of calling active object
        */
        static CMmsSendMessageOperation* NewL( CMsvSession& aMsvSession,
                                             const CMsvEntrySelection& aSelection,
                                             const TDesC8& aParameter,
                                             TRequestStatus& aObserverRequestStatus );
        
        /**
        * Destructor.
        */
        virtual ~CMmsSendMessageOperation();

    public: // New functions

    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        * @param aMsvSession message server session
        * @param aObserverRequestStatus status of calling active object
        */
        CMmsSendMessageOperation( CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aSelection entries to be sent
        * @param aParameter send parameter pack
        */
        void ConstructL( const CMsvEntrySelection& aSelection, const TDesC8& aParameter );
        
        /**
        * From CMsvSendOperation
        * @param aProgress send operation progress
        * @return Standard progress structure CMsvSendOperation::TSendOperationProgress
        */
        virtual const TDesC8& TranslateProgress(const TDesC8& aProgress);

    public:     // Data
    
    protected:  // Data

    private:    // Data
         
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSSENDMESSAGEOPERATION_H
            
// End of File
