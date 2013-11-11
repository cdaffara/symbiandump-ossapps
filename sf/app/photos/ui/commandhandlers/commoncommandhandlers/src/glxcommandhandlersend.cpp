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

#include "glxcommandhandlersend.h"
#include <QModelIndex>
#include <shareui.h>
#include <glxmodelparm.h>
#include <glxmediamodel.h>
#include <QAbstractItemModel>

GlxCommandHandlerSend::GlxCommandHandlerSend()
    {
     mSendUiDialog = new ShareUi();      
    }


GlxCommandHandlerSend::~GlxCommandHandlerSend()
    {
     delete mSendUiDialog;
     mSendUiDialog =NULL;
    }

void GlxCommandHandlerSend::doHandleUserAction(GlxMediaModel* model,QList<QModelIndex> indexList) const
{
  QStringList fileList;   
  QString imagePath ;
   //The Index list is NULL, when the item is selected to be sent from contect menu 
   // and fullscreen view,so that inturn is sending the focused item
    if(indexList.count() == 0)
        {
        imagePath = (model->data(model->index(model->data(model->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
        fileList.append(imagePath);
        }
    else
    {
    for ( int i = 0; i < indexList.count(); i++)
        {        
        imagePath = (model->data(indexList[i],GlxUriRole)).value<QString>();
        fileList.append(imagePath);
        } 
    }                              
    mSendUiDialog->send(fileList,true);    
}

