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
 * Description: native mms message's utility class. Provides utility
 * functions for extracting data from SMIL and/or Attachment manager.
 */

#ifndef CONVERSATIONS_ENGINE_UTILITY_H
#define CONVERSATIONS_ENGINE_UTILITY_H

#include <ccsdefs.h>

// CONSTANTS
_LIT(KUnixEpoch, "19700000:000000.000000");

/**
 * Utility class for conversationengine.
 */
class ConversationsEngineUtility
    {

public:

    /**
     * Map message type to ConvergedMessage::MessageType.
     * @param value, TCsType
     * @return ConvergedMessage::MessageType.
     */
    static int messageType(TCsType value);

    /**
     * Map message type to ConvergedMessage::MessageType.
     * @param value
     * @return ConvergedMessage::MessageType.
     */
    static int messageType(TInt32 value);

    /**
     * Map message type to ConvergedMessage::MessageSubType.
     * @param value, TCsType
     * @return ConvergedMessage::MessageSubType.
     */
    static int messageSubType(TCsType value);

    };

#endif //CONVERSATIONS_ENGINE_UTILITY_H
