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

#include <glxmodelwrapper.h>
#include <glxmodelparm.h>
#include <QDebug>

GlxModelWrapper::GlxModelWrapper(): mModel ( NULL),
    mOriginalRole(Qt::DecorationRole),
    mConvertRole(Qt::DecorationRole),
    mPaintPage(false)
    {

    }

void GlxModelWrapper::setModel(QAbstractItemModel *model)
    {
    if(model && mModel != model)
        {
        disConnectFromModel();
        mModel = model;
        QVariant variant = mModel->data(mModel->index(0,0),GlxPaintPageFlag);
        if (variant.isValid() &&  variant.canConvert<bool> () )
            {
            mPaintPage = variant.value<bool>();
            }
        connectToModel();
        resetTheModel();
        }	
    }


void GlxModelWrapper::setRoles(int convertrole, int originalrole)
    {
	mConvertRole  =   convertrole;
	mOriginalRole =   originalrole;
	}

void GlxModelWrapper::scrollingStarted()
    {
 
    }

void GlxModelWrapper::scrollingEnded()
    {

    }

void GlxModelWrapper::connectToModel()
	{
	connect(mModel, SIGNAL(destroyed()),this, SLOT(modelDestroyed()));
	connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataChangedinModel(QModelIndex,QModelIndex)));
	connect(mModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),this,SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)),this,  SLOT(rowsInserted(QModelIndex,int,int)));
    connect(mModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(rowsRemoved(QModelIndex,int,int)));
	}
	
void GlxModelWrapper::disConnectFromModel()
	{
	if(mModel)
	    {
	    disconnect(mModel, SIGNAL(destroyed()),this, SLOT(modelDestroyed()));
		disconnect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataChangedinModel(QModelIndex,QModelIndex)));
		disconnect(mModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),this,SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
	    disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)),this,  SLOT(rowsInserted(QModelIndex,int,int)));
	    disconnect(mModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));
	    disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(rowsRemoved(QModelIndex,int,int)));	
		}
	}
	

GlxModelWrapper::~GlxModelWrapper()
    {
	disConnectFromModel();
    }

int GlxModelWrapper::rowCount(const QModelIndex &parent ) const
    {
	Q_UNUSED(parent);
	if(mModel)
		return (mModel->rowCount());
	else
		return 0;
    }

int GlxModelWrapper::columnCount(const QModelIndex &parent ) const
    {
    Q_UNUSED(parent);
	return 1; 
    }

QModelIndex GlxModelWrapper::parent(const QModelIndex &child) const
    {
    Q_UNUSED(child);
    return QModelIndex();  // No need to Check mModel ,Should i Set mModel As parent
    }

QVariant GlxModelWrapper::data(const QModelIndex &idx, int role) const
    {
	if(mModel)
		{
		if(mOriginalRole == role)
			{
			return (mModel->data(idx,mConvertRole));
			}
		else
			{
			return (mModel->data(idx,role));		
			}
		}
	else
		{
		return QVariant();	
		}
    }

bool GlxModelWrapper::setData ( const QModelIndex & idx, const QVariant & value, int role )
    {
    if(mModel)
    	{
    	return (mModel->setData(mModel->index(idx.row(),0),value,role));
    	}
    else
    	{
    	return FALSE;
    	}
    }

QModelIndex GlxModelWrapper::index(int row, int column, const QModelIndex &parent) const
    {
    Q_UNUSED(parent);
    if ( ( row < 0 ) || ( row >= rowCount() )  || ( column < 0 )  || ( column >= columnCount() ) ) 
        {
        return QModelIndex();
        }
    return QAbstractItemModel::createIndex(row, column); 
    }

QModelIndex GlxModelWrapper::basemodelindex(int row, int column, const QModelIndex &parent) const
    {
    Q_UNUSED(parent);
    if(mModel)
        {
        return (mModel->index(row,column));
        }
    else
        {
        return QModelIndex();
        }
    }

void GlxModelWrapper::modelDestroyed()
    {
    disConnectFromModel();
    mModel = NULL;
    resetTheModel();
    }

void GlxModelWrapper::resetTheModel()
    {
    beginResetModel();
    endResetModel();
    }

void GlxModelWrapper::dataChangedinModel(QModelIndex startIndex, QModelIndex endIndex)
    {
    int aStartRow = startIndex.row();

    if(mPaintPage)
        {
    if((aStartRow  == 14) || (aStartRow+1 == rowCount()))
        {
        emit dataChanged(index(0,0),index(endIndex.row(),0));
            mPaintPage = false;
        }
    else if(aStartRow  >= 15)
        {
        emit dataChanged(index(aStartRow,0),index(endIndex.row(),0));
        }
        }
    else
        {
        emit dataChanged(index(aStartRow,0),index(endIndex.row(),0));
        }
    }

void GlxModelWrapper::rowsAboutToBeInserted(const QModelIndex &parent,int start,int end)
    {
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    }

void GlxModelWrapper::rowsInserted(const QModelIndex &parent, int start, int end)
    {
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), start, end);
    endInsertRows();
    }

void GlxModelWrapper::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
    {
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    }

void GlxModelWrapper::rowsRemoved(const QModelIndex &parent, int start, int end)
    {
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), start, end);
    endRemoveRows();
    }



