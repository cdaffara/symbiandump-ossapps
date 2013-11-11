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

#include "hbdocumentloader.h"
#include "hbmainwindow.h"
#include "hbmenu.h"
#include "hbview.h"
#include "hbstackedwidget.h"
#include "hblabel.h"
#include "hbwidget.h"
#include "hbcheckbox.h"
#include "hbpushbutton.h"
#include "hblabel.h"
#include "hbgroupbox.h"
#include "videocollectionuiloader.h"
#include "videocollectionuiloaderdef.h"
#include "videolistwidget.h"
#include "videolistview.h"
#include "videolistselectiondialog.h"

bool HbDocumentLoader::mFindWidgetFails = false;
bool HbDocumentLoader::mFindObjectFails = false;
bool HbDocumentLoader::mCreateObjectFails = false;
bool HbDocumentLoader::mVideoListWidgetFailure = false;
bool HbDocumentLoader::mCollectionWidgetFailure = false;
bool HbDocumentLoader::mCollectionContentWidgetFailure = false;
bool HbDocumentLoader::mLoadFails = false;
QObjectList HbDocumentLoader::mLoadReturns;

void HbDocumentLoader::cleanup() {
    mFindWidgetFails = false;
    mFindObjectFails = false;
    mCreateObjectFails = false;
    mVideoListWidgetFailure = false;
    mCollectionWidgetFailure = false;
    mCollectionContentWidgetFailure = false;
    mLoadFails = false;
    mLoadReturns.clear();
}

HbDocumentLoader::HbDocumentLoader() : mCreatingObject(false)
{

}

HbDocumentLoader::HbDocumentLoader(const HbMainWindow *window) : mCreatingObject(false)
{
    Q_UNUSED(window);
    // Not stubbed.
}

HbDocumentLoader::~HbDocumentLoader()
{
    cleanup();
    reset();
}

QObjectList HbDocumentLoader::load( const QString &fileName, const QString &section , bool *ok)
{
    Q_UNUSED(fileName);
    Q_UNUSED(section);

    *ok = true;
    if(mLoadFails)
    {
        *ok = false;
    }
    
    return mLoadReturns;
}

QObjectList HbDocumentLoader::load( const QString &fileName, bool *ok)
{
    Q_UNUSED(fileName);
    Q_UNUSED(ok);

    *ok = true;
    if(mLoadFails)
    {
        *ok = false;
    }
    
    return mLoadReturns;
}
    
QGraphicsWidget *HbDocumentLoader::findWidget(const QString &name)
{   
    if(mFindWidgetFails)
    {
        return 0;
    }
    
    if(mVideoListWidgetFailure && name == DOCML_NAME_VC_VIDEOLISTWIDGET)
    {
        return 0;
    }
    
    if(mCollectionWidgetFailure && name == DOCML_NAME_VC_COLLECTIONWIDGET)
    {
        return 0;
    }
    
    if(mCollectionContentWidgetFailure && name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET)
    {
        return 0;
    }

    QObject *obj = 0;
    
    for(int i = 0; i < mObjects.count(); i++)
    {
        if(mObjects[i]->mName == name)
        {
            obj = mObjects[i]->mObject;
            break;
        }
    }
    
    if(!obj && !mCreatingObject)
    {
        mCreatingObject = true;
        obj = createObject(QString(), name);
        if(obj)
        {   
            mObjects.append(new ObjectData(obj, name));
        }
        mCreatingObject = false;
    }
    
    if(obj)
    {
        return qobject_cast<QGraphicsWidget *>(obj);
    }
    
    return 0;
}

QObject *HbDocumentLoader::findObject(const QString &name)
{
    if(mFindObjectFails)
    {
        return 0;
    }
    
    QObject *obj = 0;
    for(int i = 0; i < mObjects.count(); i++)
    {
        if(mObjects[i]->mName == name)
        {
            obj = mObjects[i]->mObject;
            break;
        }
    }

    if(!obj && !mCreatingObject)
    {
        mCreatingObject = true;
        obj = createObject(QString(), name);
        if(obj)
        {   
            mObjects.append(new ObjectData(obj, name));
        }
        mCreatingObject = false;
    }    
    
    return obj;
}

void HbDocumentLoader::reset()
{
    while(!mObjects.isEmpty())
    {
        ObjectData *o = mObjects.takeFirst();
        delete o;
    }
    mObjects.clear();
}

QObject *HbDocumentLoader::createObject(const QString& type, const QString &name)
{
    Q_UNUSED(type);
    
    QObject *obj = 0;
    
    if(mCreateObjectFails) 
    {
        return 0;
    }
    
    obj = doCreateObject(name);
    return obj;
}

QObject *HbDocumentLoader::doCreateObject(const QString &name)
{
    QObject *obj = 0;
    if(name == DOCML_NAME_VIEW)
    {
        obj = new VideoListView(0, 0);
    } 
    else if(name == DOCML_NAME_VC_HEADINGBANNER)
    {
        obj = new HbGroupBox();
    }
    else if(name == DOCML_NAME_VC_COLLECTIONWIDGET)
    {
        obj = new VideoListWidget(0, 0);
    }
    else if(name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET)
    {
        obj = new VideoListWidget(0, 0);
    }
    else if(name == DOCML_NAME_VC_VIDEOLISTWIDGET)
    {
        obj = new VideoListWidget(0, 0);
    }
    else if(name == DOCML_NAME_OPTIONS_MENU)
    {
        obj = new HbMenu();
    }
    else if(name == DOCML_NAME_SORT_MENU)
    {
        obj = new HbMenu();
    }
    else if(name == DOCML_NAME_SORT_BY_DATE)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_SORT_BY_NAME)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_SORT_BY_SIZE)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_ADD_TO_COLLECTION)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_CREATE_COLLECTION)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_DELETE_MULTIPLE)
    {
        obj = new HbAction();
    }
    else if(name == DOCML_NAME_NO_CONTENT_LABEL)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_VIDEOSELECTIONDIALOG_FILE)
    {
        obj = new VideoListSelectionDialog(0, 0);
    }
    else if(name == DOCML_NAME_DIALOG)
    {
        obj = new VideoListSelectionDialog(0, 0);
    }
    else if(name == DOCML_NAME_DLG_HEADINGLBL)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_NAME_CHECK_CONTAINER)
    {
        obj = new HbWidget();
    }
    else if(name == DOCML_NAME_MARKALL)
    {
        obj = new HbCheckBox();
    }
    else if(name == DOCML_NAME_LBL_SELECTION)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_NAME_LIST_CONTAINER)
    {
        obj = new HbStackedWidget();
    }
    else if(name == DOCML_NAME_LBL_MARKALL)
    {
        obj = new HbLabel();
    }
    return obj;
}
