/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmframeworkadapterheaders.h"


/*!
    \class NmMailboxSearchObserver
    \brief A Qt wrapper class for MFSMailBoxSearchObserver.
*/


/*!
    Class constructor.
*/
NmMailboxSearchObserver::NmMailboxSearchObserver()
{
    NM_FUNCTION;
}


/*!
    Class destructor.
*/
NmMailboxSearchObserver::~NmMailboxSearchObserver()
{
    NM_FUNCTION;
}


/*!
    From MFSMailBoxSearchObserver.
    Converts the ID of the received message into NmId and emits it with the
    signal notifying that a match was found.

    \param aMatchMessage A message that matches the search criteria. This class
                         takes the ownership of the given instance.
*/
void NmMailboxSearchObserver::MatchFoundL(CFSMailMessage *aMatchMessage)
{
    NM_FUNCTION;
    
    if (aMatchMessage) {
        NmMessage *message = aMatchMessage->GetNmMessage();

        if (message) {
            NmMessageEnvelope messageEnvelope = message->envelope();
            emit matchFound(messageEnvelope.messageId(), messageEnvelope.folderId());
            delete message;
        }
    delete aMatchMessage;
    aMatchMessage = NULL;
    }
}


/*!
    From MFSMailBoxSearchObserver.
    Emits a signal to notify that the search is complete.
*/
void NmMailboxSearchObserver::SearchCompletedL()
{
    NM_FUNCTION;
    
    emit searchComplete();
}


/*!
    From MFSMailBoxSearchObserver.
    TODO Implement if necessary.
*/
void NmMailboxSearchObserver::ClientRequiredSearchPriority(
    TInt *apRequiredSearchPriority)
{
    NM_FUNCTION;
    
    Q_UNUSED(apRequiredSearchPriority);
}


// End of file.
