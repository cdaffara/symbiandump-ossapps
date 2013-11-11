/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CVTUSERINPUT_H
#define CVTUSERINPUT_H
NONSHARABLE_CLASS( CVTUserInputDtmf ) : public CBase, 
                                        public MVTUserInput
    {
    public:
        /**
         * ConstructL.
         */
        void ConstructL(TUint8 aTone);
        /**
         * NewL.
         */
        static CVTUserInputDtmf* NewL(TUint8 aTone);
        
        /**
         * Destructor.
         */
        ~CVTUserInputDtmf();

        /**
        * Virtual function to return the user input type
        **/
        virtual TUserInputType GetType();
        CPVUserInputDtmfSymbian* iUII;
    };

NONSHARABLE_CLASS( CVTUserInputAlphanumeric ) : public CBase, 
                                                public MVTUserInput
    {
    public:
        /**
         * ConstructL.
         */
        void ConstructL(TUint8 aTone);
        /**
         * NewL.
         */
        static CVTUserInputAlphanumeric* NewL(TUint8 aTone);
        
        /**
         * Destructor.
         */
        ~CVTUserInputAlphanumeric();

        /**
        * Virtual function to return the user input type
        **/
        virtual TUserInputType GetType();
        CPVUserInputAlphanumericSymbian* iUII;
    };
#endif
//  End of File
 