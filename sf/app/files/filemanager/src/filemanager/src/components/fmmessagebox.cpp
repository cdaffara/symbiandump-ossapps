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
 * 
 * Description:
 *      The source file of filemanager messagebox
 */

#include "fmmessagebox.h"
#include <hbmessagebox.h>

/*!
    This is a convenience function for showing an information dialog with \a informationText
*/
void FmMessageBox::information( const QString &informationText,
    HbMessageBox::StandardButtons buttons, bool isBlocking  )
{    
    HbMessageBox::information( informationText,
         this, SLOT(dialogClosed(HbAction*)), buttons );
    if ( isBlocking ) {
        mEventLoop.exec();
    }
    
}

/*!
    This is a convenience function for showing a question dialog with \a questionText
*/
bool FmMessageBox::question( const QString &questionText,
    HbMessageBox::StandardButtons buttons )
{
    HbMessageBox::question( questionText,
            this, SLOT(dialogClosed(HbAction*)), buttons  );
    mEventLoop.exec();
    return mRet;
}

/*!
    This is a convenience function for showing a warning dialog with \a warningText
*/
void FmMessageBox::warning( const QString &warningText,
        HbMessageBox::StandardButtons buttons, bool isBlocking )
{
    HbMessageBox::warning( warningText,
         this, SLOT(dialogClosed(HbAction*)), buttons );
    if ( isBlocking ) {
        mEventLoop.exec();
    }
}

void FmMessageBox::dialogClosed(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0)) 
    {
        mRet = true;
    }
    else
    {
        mRet = false;
    }
    mEventLoop.exit();
}
