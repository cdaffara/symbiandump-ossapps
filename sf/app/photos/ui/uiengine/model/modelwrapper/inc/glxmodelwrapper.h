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

#ifndef GLXMODELWRAPPER_H
#define GLXMODELWRAPPER_H

#include <QAbstractItemModel>

#ifdef BUILD_MODELWRAPPER
#define GLX_MODELWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_MODELWRAPPER_EXPORT Q_DECL_IMPORT
#endif


class GLX_MODELWRAPPER_EXPORT GlxModelWrapper : public QAbstractItemModel
{
Q_OBJECT
public :    
    
    GlxModelWrapper();
    ~GlxModelWrapper();
    void setModel(QAbstractItemModel *model);
    void setRoles(int convertrole, int originalrole = Qt::DecorationRole);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &idx, int role) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex basemodelindex(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    void scrollingStarted();
    void scrollingEnded();
    
    
private:
	void connectToModel();
	void disConnectFromModel();
    
protected slots:	  
    void modelDestroyed();
	void dataChangedinModel(QModelIndex startIndex, QModelIndex endIndex);
	void rowsAboutToBeInserted(const QModelIndex &parent,int start,int end);
    void rowsInserted(const QModelIndex &parent, int start, int end);
	void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    //These might be needed later.  Commenting now
    /*void columnsAboutToBeInserted(QModelIndex,int,int);
    void columnsInserted(QModelIndex,int,int);
    void columnsAboutToBeRemoved(QModelIndex,int,int);
    void columnsRemoved(QModelIndex,int,int);
    void rowsMoved(QModelIndex, int, int, QModelIndex, int);
    void layoutAboutToBeChanged();
    void layoutChanged();
    void modelReset();
    void destroyed();*/
    
private:
    
    void resetTheModel();
	
   QAbstractItemModel  *mModel;
   int                  mOriginalRole;
   int                  mConvertRole;
   bool                 mPaintPage;  // Flag to determine painting First page  Together
};

#endif /* GLXMODELWRAPPER_H */
