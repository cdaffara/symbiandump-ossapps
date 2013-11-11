/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     inline functions for CMmsReceiveMessage
*
*/



inline CMsvEntrySelection& CMmsReceiveMessage::Received() const
    {
    return *iSuccessful;
    }

inline CMsvEntrySelection& CMmsReceiveMessage::Failed() const
    {
    return *iFailed;
    }

inline CMsvEntrySelection& CMmsReceiveMessage::BadNotifications() const
    {
    return *iBad;
    }

inline TBool CMmsReceiveMessage::InForeignNetwork() const
    {
    return ( iRegistrationStatus != 0 );
    }

// End of File
