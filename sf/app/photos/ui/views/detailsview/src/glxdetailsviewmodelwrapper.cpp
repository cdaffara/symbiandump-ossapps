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
 * Description:   ?Description
 *
 */

//Includes
#include <QDebug>
#include <QModelIndex>
#include <qdatetime.h>
#include <hbparameterlengthlimiter.h>

//User Includes
#include "glxviewids.h"
#include "glxmodelroles.h"  //Contains the declaration of the roles used to retrieve the data from model
#include "glxlocalisationstrings.h" //contains the localisation strings
#include "glxdetailsviewmodelwrapper.h" //contains the declaration of this file/class
//Constants
const int KBytesInKB = 1024;
const int KBytesInMB = 1024 * 1024;
const int KBytesInGB = 1024 * 1024 * 1024;
const int TOTAL_NO_FIELDS_LIST = 5;

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsViewModelWrapper
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsViewModelWrapper::GlxDetailsViewModelWrapper(QAbstractItemModel *model) :
    mModel(model)
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsViewModelWrapper
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsViewModelWrapper::~GlxDetailsViewModelWrapper()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//rowCount
//--------------------------------------------------------------------------------------------------------------------------------------------
int GlxDetailsViewModelWrapper::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int substate = 0;

    if (mModel) {
        QVariant variant = mModel->data(mModel->index(0, 0), GlxSubStateRole);

        if (variant.isValid() && variant.canConvert<int> ()) {
            substate = variant.value<int> ();
        }
    }
    //If photos app, then show the Description field, from imageviewer app - hide description field
    return (substate != IMAGEVIEWER_DETAIL_S)?TOTAL_NO_FIELDS_LIST:(TOTAL_NO_FIELDS_LIST - 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//columnCount
//--------------------------------------------------------------------------------------------------------------------------------------------
int GlxDetailsViewModelWrapper::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//parent
//--------------------------------------------------------------------------------------------------------------------------------------------
QModelIndex GlxDetailsViewModelWrapper::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    //returns always invalid model index
    return QModelIndex();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//data
//--------------------------------------------------------------------------------------------------------------------------------------------
QVariant GlxDetailsViewModelWrapper::data(const QModelIndex &index, int role) const
{
    int rowIndex = index.row();

    qDebug
        ("GlxDetailsViewModelWrapper::data index %d role %d", rowIndex, role);

    if ((!index.isValid()) || (rowIndex >= rowCount())) {
        return QVariant();
    }

    switch (role) {

    case GlxUriRole:
    {
        QString imageName = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
            GlxFocusIndexRole).toInt(), 0), GlxImageTitle)).toString();
        return imageName;
    }

    case GlxDateRole:
    {
        QString datestring;
        QString dateFormat("dd.MM.yyyy");
        QDate date = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
            GlxFocusIndexRole).toInt(), 0), GlxDateRole)).value<QDate> ();

        if (date.isNull() == FALSE) {

            QString dateStringValue = date.toString(dateFormat);
            datestring = hbTrId(GLX_DETAILS_DATE).arg(dateStringValue);
        }
        return datestring;
    }

    case GlxTimeRole:
    {
        QString timestring;
        QString timeFormat("h:m ap");
        QTime timevalue = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
            GlxFocusIndexRole).toInt(), 0), GlxTimeRole)).value<QTime> ();

        if (timevalue.isNull() == FALSE) {
            QString timeStringValue = timevalue.toString(timeFormat);
            timestring = hbTrId(GLX_DETAILS_TIME).arg(timeStringValue);
        }
        return timestring;
    }

    case GlxSizeRole:
    {
        int size = 0;
        QString sizeString;
        size = (mModel->data(mModel->index(
            mModel->data(mModel->index(0, 0), GlxFocusIndexRole).toInt(), 0), GlxSizeRole)).toInt();

        sizeString = sizeinStrings(size);

        return sizeString;
    }

    case GlxDescRole:
    {
        QString description = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
            GlxFocusIndexRole).toInt(), 0), GlxDescRole)).toString();

        return description;
    }

    case GlxSubStateRole:
    {
        int substate = 0;

        if (mModel) {
            QVariant variant = mModel->data(mModel->index(0, 0), GlxSubStateRole);

            if (variant.isValid() && variant.canConvert<int> ()) {
                substate = variant.value<int> ();
            }
        }
        return substate;
    }

    default:
        return QVariant();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//index
//--------------------------------------------------------------------------------------------------------------------------------------------
QModelIndex GlxDetailsViewModelWrapper::index(int row, int column, const QModelIndex & /*parent*/) const
{
    //Q_UNUSED(parent)
    qDebug
        (" GlxDetailsViewModelWrapper::index row index %d column index %d", row, column);

    if ((row < 0) || (row >= rowCount()) || (column < 0) || (column >= columnCount())) {
        return QModelIndex();
    }

    return QAbstractItemModel::createIndex(row, column);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//refreshCustomWidget
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsViewModelWrapper::refreshCustomWidget()
{
    emit updateCustomWidgets();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//sizeinStrings
//--------------------------------------------------------------------------------------------------------------------------------------------
QString GlxDetailsViewModelWrapper::sizeinStrings(int size) const
{
    QString sizeString;
    if (size >= KBytesInGB) {
        int gbSize = size / KBytesInGB; // Size in GB
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_GB, gbSize);
    }
    else if (size >= KBytesInMB) {
        int mbSize = size / KBytesInMB; // Size in MB
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_MB, mbSize);
    }
    else if (size >= KBytesInKB) {
        int kbSize = size / KBytesInKB; // Size in KB
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_KB, kbSize);
    }
    else {
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_BYTES, size);
    }
    return sizeString;
}
