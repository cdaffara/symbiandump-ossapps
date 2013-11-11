/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MPHONECOVERUIOBSERVER_H
#define MPHONECOVERUIOBSERVER_H

// Class declaration

/**
*  Cover UI observer interface.
*
*/
class MPhoneCoverUiObserver  
    {
    public:
        
        /**
        * This function is called when Mediator receives response to the sent
        * ShowMultiMediaRingingTone command.
        * @param aStatus - Standard Symbian error code indicating the
        *                  success of the command.
        */
        virtual void ShowMultimediaRingingToneResponseL( TInt aStatus ) = 0;
    };

#endif // MPHONECOVERUIOBSERVER_H   
            
// End of File
