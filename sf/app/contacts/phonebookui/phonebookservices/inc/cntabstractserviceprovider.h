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
 * Description: Abstract interface for highway providers used in phonebookservices.
 *
 */

#ifndef CNTABSTRACTSERVICEPROVIDER_H
#define CNTABSTRACTSERVICEPROVIDER_H


QTM_USE_NAMESPACE

/*!
 * Abstract interface for highway providers used in phonebookservices.
 */
class CntAbstractServiceProvider
{
public:

    /*!
     Complete the Qt Highway service (through a XQServiceProvider instance).
     Closes the app after the completing is done (message sending to client is done).
     */
    virtual void CompleteServiceAndCloseApp(const QVariant& retValue) = 0;

    /*!
     If this method has been called, then the argument in a call to CompleteServiceAndCloseApp() 
     can be ignored by concrete provider.
     */
    virtual void overrideReturnValue(const QVariant& retValue) { Q_UNUSED(retValue) };
    
    /*!
     By default sub views are allowed to exit the service, override this and return false to
     restrict this functionality.
     */
    virtual bool allowSubViewsExit() { return true; };

};

#endif /* CNTABSTRACTSERVICEPROVIDER_H */
