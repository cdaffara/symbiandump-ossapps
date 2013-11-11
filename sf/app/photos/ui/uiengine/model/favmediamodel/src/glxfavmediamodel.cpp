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



#include <glxfavmediamodel.h>
#include <glxmodelparm.h>
#include <glxtracer.h>
#include <glxfiltergeneraldefs.h>


GlxFavMediaModel::GlxFavMediaModel(GlxModelParm & modelParm)
{
    TRACER("GlxFavMediaModel::GlxFavMediaModel");
    mMLWrapper = new GlxMLWrapper(modelParm.collection(),0,EGlxFilterImage,modelParm.path());
    mMLWrapper->setContextMode( modelParm.contextMode() );      
    int err = connect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
        err = connect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));   
}

GlxFavMediaModel::~GlxFavMediaModel()
{
    TRACER("GlxFavMediaModel::~GlxFavMediaModel");
    int err =  disconnect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
    err = disconnect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));
    delete mMLWrapper;
}


int GlxFavMediaModel::rowCount(const QModelIndex &parent ) const
{
    TRACER("GlxFavMediaModel::rowCount");
    Q_UNUSED(parent);
    return (mMLWrapper->getItemCount());
}

int GlxFavMediaModel::columnCount(const QModelIndex &parent ) const
{
    TRACER("GlxFavMediaModel::columnCount");
    Q_UNUSED(parent);
    return 1;
}

QModelIndex GlxFavMediaModel::parent(const QModelIndex &child) const
{
    TRACER("GlxFavMediaModel::parent");
    Q_UNUSED(child);
    //returns always invalid model index
    return QModelIndex();
}


QVariant GlxFavMediaModel::data(const QModelIndex &index, int role) const
{
    TRACER("GlxFavMediaModel::data");
  
    if ((!index.isValid()) ) {
    return QVariant();
    }

    if (role == GlxFavorites){
    return rowCount();
    }
 
  return QVariant();
}


void GlxFavMediaModel::itemsAdded(int startIndex, int endIndex)
{
    TRACER("GlxFavMediaModel::itemsAdded");
    beginInsertRows(QModelIndex(), startIndex, endIndex);
    emit dataChanged(index(startIndex,0),index(startIndex,0));
    endInsertRows();
}

void GlxFavMediaModel::itemsRemoved(int startIndex, int endIndex)
{
    TRACER("GlxFavMediaModel::itemsRemoved");
    beginRemoveRows(QModelIndex(), startIndex, endIndex);
    emit dataChanged(index(startIndex,0),index(startIndex,0));
    endRemoveRows();    
}


QModelIndex GlxFavMediaModel::index(int row, int column, const QModelIndex &parent) const
{
    TRACER("GlxFavMediaModel::index");
    Q_UNUSED(parent)        
    if ( ( row < 0 )
        || ( row > rowCount() )
        || ( column < 0 )
        || ( column >= columnCount() ) ) {
        return QModelIndex();
    }
    
    return QAbstractItemModel::createIndex(row, column);           
}

