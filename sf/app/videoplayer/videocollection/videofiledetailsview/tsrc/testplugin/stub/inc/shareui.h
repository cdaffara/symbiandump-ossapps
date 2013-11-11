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
* Description:  shareui dialog stub.
 *
*/

#ifndef SHAREUI_H_
#define SHAREUI_H_

// SYSTEM INCLUDES
#include <QStringList>

/**
 * This class offers message creation and sending services.
 */
class ShareUi  
    {
    
public:
    /**
     * Constructor
     */
    ShareUi() {}
    
    /**
     * Destructor
     */
    ~ShareUi() {}
    

    bool send(QStringList& fileList, bool embedded)
    {
        mFileList = fileList;
        mEmbedded = embedded;
        return true;
    }
    
public:
    
    static QStringList mFileList;
    static bool mEmbedded;
    };

#endif
