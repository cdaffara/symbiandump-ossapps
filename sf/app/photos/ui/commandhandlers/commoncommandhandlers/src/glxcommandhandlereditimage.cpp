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
* Description: Handles command related to mediaeditors in fullscreen 
* for image Rotate, Crop & Set as Wallpaper
*
*/



#include <glxcommandfactory.h>


#ifdef FF_IMAGE_EDITOR
    #include <photoeditor_highway.hrh>
#endif //FF_IMAGE_EDITOR

#include "glxcommandhandlereditimage.h"
#include <glxcommandhandlers.hrh>
#include <glxmodelparm.h>
#include <glxmediamodel.h>
#include <XQServiceRequest.h>
#include <XQAiwRequest.h>

GlxCommandHandlerEditImage::GlxCommandHandlerEditImage() :
    mReq(NULL), mMediaModel(NULL)
    {
    //Nothing to do here
    }

GlxCommandHandlerEditImage::~GlxCommandHandlerEditImage()
    {
    delete mReq;
    mReq = NULL;
    clearMediaModel();
    }

void GlxCommandHandlerEditImage::executeCommand(int commandId,
        int collectionId, QList<QModelIndex> /*indexList*/)
    {
    const QString service = QLatin1String("PhotoEditor");
    const QString interface = QLatin1String("com.nokia.symbian.imageeditor");
    const QString operation = QLatin1String("view(QString,int)");
    
    //Connect to service provider
    if(mReq == NULL)
        {
        mReq = mAppmgr.create(service, interface, operation, true);
        mReq->setEmbedded(true);
        mReq->setSynchronous(false);
        }
    
    if(mReq == NULL)
        {
        return;
        } 
    
    connect(mReq, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
    connect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    GlxModelParm modelParm(collectionId, 0);
    clearMediaModel();
    mMediaModel = new GlxMediaModel(modelParm);
    if (!mMediaModel)
        {
        return;
        }

    connect( mMediaModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( storeItems( QModelIndex, int, int ) ) );

    //Get the file path for the item selected
    QString imagePath = (mMediaModel->data(
            mMediaModel->index(mMediaModel->data(mMediaModel->index(0, 0),
                    GlxFocusIndexRole).value<int> (), 0), GlxUriRole)).value<
            QString> ();
    //delete mediaModel;

    QList<QVariant> args;
    args << imagePath;
#ifdef FF_IMAGE_EDITOR    
    if(EGlxCmdSetWallpaper == commandId)
        {
        args << EEditorHighwayWallpaperCrop;
        }
    else if(EGlxCmdRotateImgCrop == commandId)
        {
        args << EEditorHighwayFreeCrop;
        }
    else if(EGlxCmdRotateImgCW == commandId)
        {
        args << EEditorHighwayRotateCW;
        }
    else // if(EGlxCmdRotateImgCCW == aCommandId)

        {
        args << EEditorHighwayRotateCCW;
        }
    mReq->setArguments(args);
#endif //FF_IMAGE_EDITOR
    // Send the request
    bool res = mReq->send();
    if (!res)
        {
        // Request failed. 
        }
    }

void GlxCommandHandlerEditImage::doHandleUserAction(GlxMediaModel* /*model*/,
        QList<QModelIndex> /*indexList*/) const
    {
    //Dummy, to keepup with compiler error
    }

void GlxCommandHandlerEditImage::handleOk(const QVariant& result)
    {
    //Nothing to do for - Needs further implementation to refine setting 
    //default image in fullscreen view
    }

void GlxCommandHandlerEditImage::handleError(int errorCode,
        const QString& errorMessage)
    {
    clearMediaModel();
    }

void GlxCommandHandlerEditImage::storeItems(const QModelIndex &parent,
        int aStartRow, int aEndRow)
    {
    //This implementation assumes that we will be getting only one new 
    //image creation notification. Needs refined implementation 
    if (mMediaModel)
        {
        QModelIndex modelIndex = mMediaModel->index(aStartRow, 0);
        mMediaModel->setData(modelIndex, modelIndex.row(), GlxFocusIndexRole);
        }
    }

void GlxCommandHandlerEditImage::clearMediaModel()
    {
    if (mMediaModel)
        {
        disconnect(mMediaModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(storeItems(QModelIndex,int,int)));
        delete mMediaModel;
        mMediaModel = NULL;
        }
    }
