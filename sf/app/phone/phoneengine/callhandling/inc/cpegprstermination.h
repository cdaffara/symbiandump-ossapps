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
* Description:  GPRS termination active object definition.
*
*/



#ifndef CPEGPRSTERMINATION_H
#define CPEGPRSTERMINATION_H

//  INCLUDES
#include    <e32base.h>

 
// CLASS DECLARATION

/**
* Active object for terminating GPRS connections.
* @since 4.0
*/
class CPEGprsTermination 
    : public CActive
    {
    public: // Constructors and destructor.

        /**
        * Constructor.
        */
        CPEGprsTermination();

        /**
        * Destructor.
        */
        ~CPEGprsTermination();

    public: // New functions
        
        /**
        * Starts termination.
        */
        void StartL();

        /**
        * Checks if it is terminating connections 
        * (or just preparing for that).
        * @return ETrue if terminating.
        */
        TBool IsTerminating() const;

    private: // Functions from base classes

        /**
        * @see CActive::RunL.
        */
        void RunL();

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();

    private:

    };

#endif      // CPEGPRSTERMINATION_H   
            
// End of File
