/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#include "cpmainview.h"
#include "cpmainmodel.h"
#include <QList>
#include <hbmainwindow.h>
#include <hbdataform.h>
#include <cpitemdatahelper.h>
#include <cplogger.h>
#include <hbactivitymanager.h>
#include <hbapplication.h>
#include <hbinstance.h>
#include <QPixMap>
#include <QMetaType>
#include "cpcategorysettingformitemdata.h"

//CpMainView implementation
CpMainView::CpMainView(HbMainWindow *mainWindow/*= 0*/)
: CpBaseSettingView(0,0), 
  mMainModel(0), 
  mItemDataHelper(0), 
  mMainWindow(mainWindow),
  mActivityManager(0)
{ 
    //delay loading
    //connect(mMainWindow,SIGNAL(viewReady()),this,SLOT(initializeMainModel()));
    // AUTO SAVE ACTIVITY OF CONTROLPANEL
    HbApplication *app= qobject_cast<HbApplication *>(qApp);
    mActivityManager = app->activityManager();
    initializeMainModel();    
    connect(this, SIGNAL(aboutToClose()),this, SLOT(saveActivity()));
}

CpMainView::~CpMainView()
{
    delete mMainModel;
    delete mItemDataHelper;
}

bool CpMainView::event(QEvent *e)
{
    if (e->type() == QEvent::Show || e->type() == QEvent::ShowToParent) {
        CPPERF_LOG("CpMainView shown.");
    }
    return CpBaseSettingView::event(e);
}

void CpMainView::initializeMainModel()
{
    if (HbDataForm *form = qobject_cast<HbDataForm *>(widget())) {
        if (!mMainModel) {  
            mItemDataHelper = new CpItemDataHelper(form);
            mMainModel = new CpMainModel;
            mMainModel->initialize(*mItemDataHelper);
            form->setModel(mMainModel);
            connect(form, SIGNAL(activated(QModelIndex)), this, SLOT(onDataFormItemActivated(QModelIndex)));
            
            connect(mMainWindow, SIGNAL(aboutToChangeView(HbView *, HbView *)), this, SLOT(onAboutToChangeView(HbView *, HbView *)));
        }
    }   
    loadActivity();
}

void CpMainView::loadActivity()
{
    if (mMainModel == 0) {
        return;            
    } 
    QVariant data = mActivityManager->activityData(QString("ControlPanelView"));
    if (data.canConvert<QList<QVariant> >())
    {
        QList<QVariant> isExpanded = qvariant_cast<QList<QVariant> >(data);
        HbDataFormModelItem *root = mMainModel->invisibleRootItem();
        int groupCount = 0;
        
        if (root) {
         groupCount = root->childCount();
        }
        
        if (groupCount == isExpanded.count()) {
            for (int i = 0; i < groupCount; i++) {
                HbDataFormModelItem *groupItem = root->childAt(i);
                QModelIndex index = mMainModel->indexFromItem(groupItem);
                HbDataForm *form = qobject_cast<HbDataForm *>(widget());
                if (form) {
                    if (isExpanded.at(i).toBool()) {
                        onDataFormItemActivated(index);
                    }
                    form->setExpanded(index,isExpanded.at(i).toBool());
                }
            }
        }
        
        mActivityManager->removeActivity("ControlPanelView");
    }
}

void CpMainView::saveActivity()
{
    HbDataFormModelItem *root = mMainModel->invisibleRootItem();
    int groupCount = 0;
    QList<QVariant> isExpanded;
    if (root) {
        groupCount = root->childCount();
    }
    for (int i = 0; i < groupCount; i++) {
        HbDataFormModelItem *groupItem = root->childAt(i);
        QModelIndex index = mMainModel->indexFromItem(groupItem);
        HbDataForm *form = qobject_cast<HbDataForm *>(widget());    
        isExpanded << form->isExpanded(index);
    }
    
    HbMainWindow *mainWindow = hbInstance->allMainWindows().first();
    HbView *view = mainWindow->currentView();
    if(this == view)
        {
        mScreenshot = QPixmap::grabWidget(mainWindow, mainWindow->rect());
        }
    
    QVariantHash metadata;
    metadata.insert("screenshot", mScreenshot);
    QVariant data(isExpanded);

    mActivityManager->addActivity("ControlPanelView", data, metadata);
}

void CpMainView::onDataFormItemActivated(const QModelIndex &index)
{
    CPFW_LOG("CpMainView::onDataFormItemActivated");
    HbDataFormModelItem *modelItem = mMainModel->itemFromIndex(index);
    if (modelItem != 0 && modelItem->type() == HbDataFormModelItem::GroupItem) {
        CpSettingFormItemData *itemData = static_cast<CpSettingFormItemData *>(modelItem);
        if (CpCategorySettingFormItemData *categoryItemData = qobject_cast<CpCategorySettingFormItemData *>(itemData)) {
            categoryItemData->initialize(*mItemDataHelper);
        }    
    }
        
}
void CpMainView::onAboutToChangeView(HbView * oldView, HbView *newView)
{
    Q_UNUSED(newView);
    if (this == oldView) {
        mScreenshot = QPixmap::grabWidget(mMainWindow, mMainWindow->rect());
        }
    }

// End of File
