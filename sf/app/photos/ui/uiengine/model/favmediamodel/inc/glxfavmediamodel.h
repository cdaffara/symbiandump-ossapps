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



#ifndef GLXFAVMEDIAMODEL_H 
#define GLXFAVMEDIAMODEL_H 

#include <QAbstractItemModel>
#include <glxmlwrapper.h>
#include <glxuistd.h>



#ifdef BUILD_FAVMEDIAMODEL
#define GLX_MEDIAMODEL_EXPORT Q_DECL_EXPORT
#else
#define GLX_MEDIAMODEL_EXPORT Q_DECL_IMPORT
#endif

//forward declarations
class GlxModelParm;


class GLX_MEDIAMODEL_EXPORT GlxFavMediaModel : public QAbstractItemModel
{
Q_OBJECT
public :	
	
    GlxFavMediaModel(GlxModelParm & modelParm);
    ~GlxFavMediaModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

public slots:
	
	void itemsAdded(int startIndex, int endIndex);
	void itemsRemoved(int startIndex, int endIndex);
	
private:
	GlxMLWrapper* mMLWrapper;	
	GlxContextMode mContextMode;	
};


#endif /* GLXFAVMEDIAMODEL_H */
