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
* Description:  Offers message creation and sending services.
 *
*/




#ifndef SHAREUI_H_
#define SHAREUI_H_

#ifdef BUILD_SHAREUI_DLL
#define SHAREUI_EXPORT Q_DECL_EXPORT
#else
#define SHAREUI_EXPORT Q_DECL_IMPORT
#endif

// SYSTEM INCLUDES
#include <QStringList>
#include <qglobal.h>


// FORWARD DECLARATIONS
class ShareUiPrivate;

/**
 * This class offers message creation and sending services.
 * this class inturn launches HbDialog with list of services asynchronously
 * object should be alive till dialog handled completely.so client should create this class instance as a member variable
 */
class SHAREUI_EXPORT ShareUi  
    {
    
public:
    /**
     * Constructor
     */
    ShareUi();
    
    /**
     * Destructor
     */
    ~ShareUi();
    

    bool send(QStringList& fileList, bool embedded);
    
    /**
     * @deprecated
     */
    bool init(QList<QVariant>& fileList, bool embedded);
    
private:
    
    /**
     * ShareUiPrivate implementation
     * Owned
     */
    ShareUiPrivate* d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr,ShareUi)
    };




#endif /* SENDUI_DIALOG_API_H_ */
