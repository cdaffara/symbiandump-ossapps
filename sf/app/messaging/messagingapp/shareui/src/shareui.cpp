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


#include <shareui.h>
#include "shareuiprivate.h"
#include <QDir>

/**
 * Default Constructor.
 * 
 */

ShareUi::ShareUi() : d_ptr(NULL)
    {    
    }


/**
 * Destructor.
 */
ShareUi::~ShareUi()
    {
    // free allocated memory
    if(d_ptr)
        {
        delete d_ptr;
        }
    }

/**
 * send(QList<QVariant>& fileList, bool embedded)
 * 
 * method for passing file list to the sending services
 * @param fileList list of paths to files.eg: c:\images\sunset.jpg
 * @param embedded true if sendui dialog is embedded in the launched application
 * 
 * @return bool true if dialog initialization successfull otherwise false
 */
bool ShareUi::send(QStringList& fileList, bool embedded)
    {
    if((d_ptr = new ShareUiPrivate) == NULL)
        {
        return false;
        }
    return d_ptr->init(fileList, embedded);
    }

/**
 * Dialog initialization
 * 
 * Initialize the Dialog with a list of files to be sent.
 * @param fileList List of files to send via sendui_dialog_api.
 * @return bool successful initialization (true) or failure  (false)
 * @deprecated use send() instead.
 */

bool ShareUi::init(QList<QVariant>& fileList, bool embedded)
    {
    if((d_ptr = new ShareUiPrivate) == NULL)
        {
        return false;
        }
    
    QStringList aFileList;
    for(int i = 0; i < fileList.count(); i++)
        {
        aFileList.append(QDir::toNativeSeparators(fileList[i].toString()));
        }
    return d_ptr->init(aFileList, embedded);
    }


