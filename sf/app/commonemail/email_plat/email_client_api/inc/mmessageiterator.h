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
* Description: Message iterator interface
*
*/

#ifndef MMESSAGEITERATOR_H
#define MMESSAGEITERATOR_H

#include <emailapidefs.h>

namespace EmailInterface {

class MEmailMessage;

/**
 * Iterator for email messages. 
 * @since S60 v5.0
 */
class MMessageIterator : public MEmailInterface
{
public:
    /**
    * Returns next message. When all messages are iterated, returns NULL
    * @return next message. Ownership is not transferred.
    */
    virtual MEmailMessage* NextL() = 0;
    
    /**
    * Returns previous message. When first message is reached, returns NULL
    * @return previous message. Ownership not is transferred.
    */
    virtual MEmailMessage* PreviousL() = 0;
    
    /**
    * Returns amount of messages accessible from the iterator.
    * @return message count
    */
    virtual TUint Count() const = 0;
    
};

} // EmailInterface

#endif // MMESSAGEITERATOR_H
