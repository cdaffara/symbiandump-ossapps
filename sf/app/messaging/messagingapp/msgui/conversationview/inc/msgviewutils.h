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

#ifndef MSGVIEWUTILS_H
#define MSGVIEWUTILS_H

// INCLUDES
#include <apgcli.h>
#include <apmstd.h>
#include <f32file.h>

#include <e32std.h>
#include <e32base.h>
#include <QString>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * MsgViewUtils
 *
 * This class is a utility for the conversationview.
 *
 */
class MsgViewUtils
{
public:

    /**
     * Constructor
     */
    MsgViewUtils();

    /**
     * Destructor.
     */
    ~MsgViewUtils();

    /*
     * get the mime type
     * @param fileName for which mime type to find
     */
    QString mimeTypeL(QString fileName);

private:
    // Data

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgViewUtils;
#endif

};

#endif // MSGVIEWUTILS_H
//EOF
