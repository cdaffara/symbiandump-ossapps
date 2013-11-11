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
 * Description:Utility class having static helper methods.
 *
 */

#ifndef MSGUTILS_H_
#define MSGUTILS_H_

//FORWARD DECLARATIONS.
class QDateTime;
class QString;

/**
 * Utility class having static utility functions.
 */
class MsgUtils
{
public:
    // DATA
    /**
     * Character support
     */
    enum MsgCharSupport
    {
        msgCharSupportFull = 0,
        msgCharSupportReduced
    };
private:
    //constructor and destructor made private to avoid instantiation of this class.
    /**
     * Constructor
     */
    MsgUtils(){};
    
    /**
     * Destructor
     */
    ~MsgUtils(){};
    
public:

    /**
     * Converts QDateTime to formatted string.
     * @param dateTime QDateTime object to be converted to formatted string.
     * @param format determines the format of the result string.
     * @return formatted string representation of dateTime specified by format.
     * if date is invalid empty string is returned.
     * if format string is invalid, format string itself is returned.
     */
    static QString dateTimeToString(const QDateTime& dateTime,
        const QString& format);

};

#endif /* MSGUTILS_H_ */

// EOF
