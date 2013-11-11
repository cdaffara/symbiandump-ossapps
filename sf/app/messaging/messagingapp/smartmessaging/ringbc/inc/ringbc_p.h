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
 * Description:  Private implementation of ringing tone bio control. 
 *     Finds whether ringing tone is valid and saves the ringing
 *     tone to the file system.
 *
 */

#ifndef RINGBC_PRIVATE_H
#define RINGBC_PRIVATE_H

#include <QString>
#include <e32base.h>

class RingBCNSMConverter;

class RingBcPrivate :public CBase
    {
public:
    /**
     * Constructor
     */
    RingBcPrivate();

    /**
     * Destructor
     */
    ~RingBcPrivate();

    /**
     * Saves the given riging tone. 
     * @param path ringing tone file path.
     */  
    void saveTone(const QString &path);

    /**
     * Returns title for the given ringing tone.
     * @param path ringing tone binary file path.
     * @return QString title of the ringing tone.
     * 
     * returns null string if any error occurs in parsing
     * or unsupported type.
     */
    QString toneTitle(const QString &path);
    
private:

    /**
     * Saves the given riging tone. 
     * @param path ringing tone file path.
     * @return error code 
     */  
    void saveToneL(const QString& aFileName);
    
    /**
     * Returns title for the given ringing tone.
     * @param path ringing tone binary file path.
     * @return QString title of the ringing tone.
     * 
     * returns null string if any error occurs in parsing
     * or unsupported type.
     */
    QString titleL(const TDesC& aFileName);

    /**
     * Does all the initializations. 
     */
    void initL();

private:
    /**
     * Converter object.
     * Owned.
     */
    RingBCNSMConverter* mConverter;
    };

#endif  // not defined RINGBC_PRIVATE_H
// End of file
