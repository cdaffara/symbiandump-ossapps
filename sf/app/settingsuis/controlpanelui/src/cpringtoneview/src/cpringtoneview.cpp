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
#include "cpringtoneview.h"
#include <HbListWidget>
#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <QList>
#include <QPair>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStandardItem>
#include <xqaiwrequest.h>
#include <cplogger.h>
#include <hbstyleloader.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataform.h>
#include <cpsettingformentryitemdata.h>
#include <QDesktopServices>
//ovi ringtong address.
const QString oviUrl("http://lr.ovi.mobi/store/ringtones");

CpRingToneView::CpRingToneView( QGraphicsItem *parent ):
                              CpBaseSettingView(0, parent),
                              mToneTypeList( new HbListWidget(this) ),
                              mReq(0), mProcessing(false)
{
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem_color.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.widgetml");
    
    HbDataForm *form = qobject_cast<HbDataForm*> ( widget() );
    form->setHeading(hbTrId("txt_cp_subhead_select_tone_type"));   
    
    HbDataFormModel *model = new HbDataFormModel();  
    QList< QPair<QString,QString> > tonesTypeList;
    tonesTypeList << qMakePair( QString("qtg_large_tone_off"), hbTrId("txt_cp_list_no_tone") )
                 << qMakePair( QString("qtg_large_tone"), hbTrId("txt_cp_list_tone") )
                 << qMakePair( QString("qtg_large_music"), hbTrId("txt_cp_list_music") )                 
                 << qMakePair( QString("qtg_large_ovistore"), hbTrId("txt_cp_list_get_more_tones") );
       
    for (int i = 0; i < tonesTypeList.count(); ++i) {
        HbDataFormModelItem *itemData = new HbDataFormModelItem();
        itemData->setType ( static_cast<HbDataFormModelItem::DataItemType> (CpSettingFormEntryItemData::ListEntryItem) );
        itemData->setLabel(tonesTypeList.at(i).second);
        itemData->setIcon(tonesTypeList.at(i).first);
        model->appendDataFormItem(itemData, model->invisibleRootItem());
    }
    connect(form, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
    form->setModel(model);
}
CpRingToneView::~CpRingToneView()
{
    if (mReq) {
        delete mReq;
    }    
}

void CpRingToneView::itemActivated( const QModelIndex &index )
{
    //avoid responding to the second or later consecutive click
	if (mProcessing) {
		return;
	}
	mProcessing = true;
    int nRow = index.row();        
    switch(nRow) {
        case 0:         //no tone, set default no sound
            emit selOK(QString(""));
            emit aboutToClose();
            break;
        case 1: {        //tone
            launchMediaFetcher( "com.nokia.symbian.IToneFetch", "fetch()" );
            break;
        }
        case 2: {        //music            
            XQRequestInfo requestInfo;            
            requestInfo.setInfo("WindowTitle", QVariant(hbTrId("txt_cp_title_control_panel")));
            launchMediaFetcher("com.nokia.symbian.IMusicFetch", "fetch()", QList<QVariant>(), requestInfo );
            break;
        }
        case 3: {        //get more tones
            // Launch the URL in the browser and            
            QDesktopServices::openUrl(QUrl(oviUrl, QUrl::TolerantMode));
            break;
        }
	    default:
            break;
	 }
}
void CpRingToneView::handleOk(const QVariant &result)
{
    mProcessing = false;
    CPFW_LOG( "CpRingToneView::handleOk" );
    if (!result.canConvert<QString>() || result.toString().length() == 0 )  //error result
    {
        return;
    }
    hide();
    emit selOK( result.value<QString>() );
    emit aboutToClose();
}


void CpRingToneView::handleError(int errorCode, const QString &errorMessage)
{
    mProcessing = false;
    emit(selError( errorCode, errorMessage ));
}

/*!
    Launch media fetcher service.
    \a strService, the service interface name.
    \a srItface, the service operation name.
    \a arguments, the request arguments, preserved.
    \a info, the request info.
*/
void CpRingToneView::launchMediaFetcher( const QString &strService, const QString &strItface,\
                                        const QList<QVariant> &arguments, const XQRequestInfo &info )
{
    CPFW_LOG("CpRingToneView::launchMediaFetcher, START");
    if(mReq)
    {
        delete mReq;
        mReq = 0;
    }
        //launch media fetcher
    mReq = mAppMgr.create(strService, strItface, true);
    mReq->setSynchronous(false);
    if (!mReq)
    {
      CPFW_LOG("CpRingToneView::launchMediaFetcher, Mediafetcher start failed");
      return;
    }
    else
    {   //use QueuedConnection so that requestError will not be emitted when selecting one tone
        connect(mReq, SIGNAL(requestOk(QVariant)), SLOT( handleOk(QVariant)), Qt::QueuedConnection);
        connect(mReq, SIGNAL(requestError(int, QString)), SLOT(handleError(int, QString)));
    }
    if (!arguments.isEmpty()) {
        mReq->setArguments(arguments);
    }
    if (info.isValid()) {
        mReq->setInfo(info);
    }
    // Make the request
    if (!mReq->send())
    {
        CPFW_LOG("CpRingToneView::launchMediaFetcher, Mediafetcher calling failed");
    }
    CPFW_LOG("CpRingToneView::launchMediaFetcher, END");
}

