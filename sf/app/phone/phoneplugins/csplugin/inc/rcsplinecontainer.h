/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Encapsulates all of the lines required by the CS Plugin.
*
*/


#ifndef RCSPLINECONTAINER_H
#define RCSPLINECONTAINER_H

// INCLUDES

#include    <etelmm.h>         // ETel
#include    <mccpcscall.h>
#include    <mccecall.h>

//  CLASS DEFINITIONS 

/**
*  Encapsulates all of the lines required by the phone
*  engine.
*
*  @lib phoneengine.lib
*  @since 1.2
*/
NONSHARABLE_CLASS(RCSPLineContainer)
    {
    public:
    
    enum TCSPLineId {
        ECSPLineSpeech = 0,
        ECSPLineAuxSpeech,
        ECSPLineData,
        ECSPLineFax
    };
    
    public: // Constructors

        // Default c++ constructor
        RCSPLineContainer();

    public: // new functions
        /**
        * Open voice, aux, data and fax lines. 
        * @since 3.2
        * @param aPhone open lines on this phone
        * @return Returns an error only if the primary 
        *         line cannot be opened, otherwise KErrNone.
        */
        TInt Open( RMobilePhone& aPhone,
                   TUint32 aServiceId );


        /**
        * @since 3.2
        * Close the lines. Frees all resources owned by
        * this container.
        */
        void Close();

        /**
        * Access a specific line
        * @since 1.2
        * @param aLine The type of line to return
        * @returns The line specified
        */
        RMobileLine& LineByType( TCSPLineId aLine );

        /**
        * Returns whether the specified line is open
        * @since 3.2
        * @param aLine line to check
        * @return ETrue if specified line is open.
        */
        TBool LineIsOpen( TCSPLineId aLine );
        
        /**
        * Resolve call information.
        * @param aLineId line id (input)
        * @param aServiceId service id (output)
        * @param aCallType call type (output)
        * @param aLineType line type (output)
        */
        TInt ResolveCallInfo( RCSPLineContainer::TCSPLineId aLineId, 
                              TUint32& aServiceId,
                              CCPCall::TCallType& aCallType,
                              CCCECallParameters::TCCELineType& aLineType ) const;

        /**
        * Resolves line id based on call parameters.
        * @param aCallParameters
        * @return line id
        */
        RCSPLineContainer::TCSPLineId ResolveLineIdL( 
                    const CCCECallParameters& aCallParameters ) const;


    private:

        // Prohibit copy constructor if not deriving from CBase.
        RCSPLineContainer( RCSPLineContainer& );
        // Prohibit assigment operator if not deriving from CBase.
        RCSPLineContainer& operator=( const RCSPLineContainer& );

    private:
        RMobileLine iPrimaryLine;
        RMobileLine iAls2Line;
        RMobileLine iDataLine;
        RMobileLine iFaxLine;
        
        TUint32 iServiceId;
    };

#endif // RCSPLINECONTAINER_H

// End of File
