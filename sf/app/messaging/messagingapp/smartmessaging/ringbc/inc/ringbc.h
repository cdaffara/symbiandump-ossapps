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
 * Description:  Handles the ringing tone bio control. 
 *     Finds whether ringing tone is valid and saves the ringing
 *     tone to the file system.
 *
 */

#ifndef MSGRINGBC_H
#define MSGRINGBC_H

#include <qobject.h>

#ifdef  RINGBC_DLL
#define RINGBC_DLL_EXPORT Q_DECL_EXPORT
#else
#define RINGBC_DLL_EXPORT Q_DECL_IMPORT
#endif

class RingBcPrivate;


class RINGBC_DLL_EXPORT RingBc
{
public:
    
    /**
     * Constructor.
     */
    RingBc();

    /**
     * Destructor
     */
    ~RingBc();

public: 
	   
	/**
	 * Saves the given riging tone. 
	 * @param path ringing tone binary file path.
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
     * Private implementation.
     * Owned.
     */
    RingBcPrivate* d_ptr;
};

    
#endif  // not defined MSGRINGBC_H
// End of file
