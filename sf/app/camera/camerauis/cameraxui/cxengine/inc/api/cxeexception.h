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

#ifndef CXEEXCEPTION_H
#define CXEEXCEPTION_H

#include <exception>

/*!
* Exception class for engine internal errors.
* Contains one integer which can be used to carry error code,
* in normal cases which is of type CxeError::Id.
*/
class CxeException : public std::exception
{
public:
    /*!
    * Constructor.
    */
    explicit CxeException(int error) : mError(error) {};

    /*!
    * Destructor.
    */
    virtual ~CxeException() throw() {};

    /*!
    * Get the error code causing this exception.
    * @return The error code.
    */
    int error() const { return mError; };

    /*!
    * Helper method to throw exception if given status code is an error.
    * Everything but zero is considered error.
    * @param status The status code to check.
    */
    static void throwIfError(int status) { if (status) { throw new CxeException(status); } }

private:
    //! Error code for this exception.
    int mError;
};

#endif // CXEEXCEPTION_H
