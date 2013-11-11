/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     The source file for messaging's audio fetcher model
 */

// SYSTEM INCLUDES
#include <xqutils.h>
#include <xqappmgr.h>

// USER INCLUDES
#include "msgaudiofetchermodel.h"
#include "msgaudioselectionengine.h"

MsgAudioFetcherModel::MsgAudioFetcherModel(QObject *parent) :
    QStandardItemModel(parent), mSelectionEngine(0)
{
    init();   
}

MsgAudioFetcherModel::~MsgAudioFetcherModel()
{
    delete mSelectionEngine;
}

void MsgAudioFetcherModel::init()
{
    mSelectionEngine = new MsgAudioSelectionEngine();

    connect(mSelectionEngine, SIGNAL(mdeSessionOpened()), this, SLOT(mdeSessionOpened()));
    connect(mSelectionEngine, SIGNAL(mdeSessionError(int)), this, SLOT(mdeSessionError(int)));
    connect(mSelectionEngine, SIGNAL(queryComplete(QStringList, QStringList)), this,
        SLOT(queryComplete(QStringList, QStringList)));
    connect(mSelectionEngine, SIGNAL(queryError(int)), this, SLOT(queryError(int)));

    connect(mSelectionEngine, SIGNAL(notifyObjectChanged()), this, SLOT(onObjectChanged()));
    mSelectionEngine->CreateMetaDataSession();
}

void MsgAudioFetcherModel::getRomToneFiles()
{
    // z:\data\sounds\digital
    QDir digitalsoundPath(XQUtils::romRootPath() + XQUtils::digitalSoundsPath());
    addToneFiles(digitalsoundPath);

    //z:\data\sounds\simple
    QDir simpleSoundPath(XQUtils::romRootPath() + XQUtils::simpleSoundsPath());
    addToneFiles(simpleSoundPath);

    //z:\data\sounds
    QDir soundPath(XQUtils::romRootPath() + XQUtils::soundsPath());
    addToneFiles(soundPath);
}

void MsgAudioFetcherModel::addToneFiles(QDir& toneDir)
{
    // List only xx.amr files
    QStringList filters;
    filters << "*.amr";
    toneDir.setNameFilters(filters);

    QStringList soundList = toneDir.entryList();
    for (int i = 0; i < soundList.size(); ++i) {
        QString fullPath = QDir::toNativeSeparators(toneDir.absolutePath());
        fullPath.append(QDir::separator());
        fullPath.append(soundList.at(i));
        addRow(fullPath);
    }
}

QVariant MsgAudioFetcherModel::data(const QModelIndex &index, int role) const
{
    return QStandardItemModel::data(index, role);
}

void MsgAudioFetcherModel::addRow(QString filepath)
{
    if(isDRM(filepath))
    {
        // don't insert DRM protected files, as sending itself
        // is not allowed in 'Restricted Mode'
        return;
    }

    QFileInfo fInfo(filepath);
    QStandardItem* item = new QStandardItem();
    QString filename = fInfo.baseName();
    item->setData(filename, Qt::DisplayRole);
    item->setData(filepath, Qt::UserRole);

    // get insert index
    int index = this->insertIndex(0, rowCount(), filename);
    insertRow(index, item);
}

int MsgAudioFetcherModel::insertIndex(int low, int high, QString value)
{
    if (low == high) {
        return low;
    }
    int middle = (low + high - 1) / 2;
    QModelIndex lowItemIndex = ((QStandardItemModel *) this)->index(low, 0);
    QModelIndex highItemIndex = ((QStandardItemModel *) this)->index(high - 1, 0);
    QModelIndex middleItemIndex = ((QStandardItemModel *) this)->index(middle, 0);
    QString lowString = data(lowItemIndex).toString();
    QString highString = data(highItemIndex).toString();
    QString middleString = data(middleItemIndex).toString();

    if (value >= highString) {
        return high;
    }
    if (value < lowString) {
        return low;
    }
    high = high - 1;
    while (low < high) {
        middle = (low + high) / 2;
        middleItemIndex = ((QStandardItemModel *) this)->index(middle, 0);
        middleString = data(middleItemIndex).toString();
        if (value >= middleString) {
            low = middle + 1;
        }
        else {
            high = middle;
        }
    }
    return low;
}

void MsgAudioFetcherModel::clearAll()
{    
    QStandardItemModel::clear();
}

void MsgAudioFetcherModel::mdeSessionOpened()
{
    mSelectionEngine->QueryTones();
    getRomToneFiles();
}

void MsgAudioFetcherModel::queryComplete(const QStringList &nameList, const QStringList &uriList)
{
    for (int i = 0; i < nameList.size(); ++i) {
        addRow(uriList.at(i));
    }
}

void MsgAudioFetcherModel::queryError(int error)
{
    Q_UNUSED(error);        
}

void MsgAudioFetcherModel::mdeSessionError(int error)
{
    Q_UNUSED(error);
}

void MsgAudioFetcherModel::onObjectChanged()
{
    clearAll();
    getRomToneFiles();
    mSelectionEngine->QueryTones();
}

bool MsgAudioFetcherModel::isDRM(QString filepath)
{
    QList<int> attrNames;
    attrNames.append(XQApplicationManager::IsProtected);
    
    QFile file(filepath);
    QVariantList attrValues;
    XQApplicationManager appmgr;
    bool ok = appmgr.getDrmAttributes(file, attrNames, attrValues);
    if(!ok)
    {
        return true;
    }
    return (0 < attrValues.at(0).toInt());
}

//End of File
