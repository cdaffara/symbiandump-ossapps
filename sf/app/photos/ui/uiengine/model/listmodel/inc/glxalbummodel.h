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

#ifndef GLXALBUMMODEL_H
#define GLXALBUMMODEL_H

#include <QAbstractItemModel>
#include <QCache>
#include <glxuistd.h>

#ifdef BUILD_LISTMODEL
#define GLX_LISTMODEL_EXPORT Q_DECL_EXPORT
#else
#define GLX_LISTMODEL_EXPORT Q_DECL_IMPORT
#endif

struct GlxAlbumListParams  
{
   QString title;
   QString subTitle;
   QPixmap* image; 
};

typedef QList<GlxAlbumListParams> GlxAlbumListData;

class HbIcon;
class GlxModelParm;
class GlxMLWrapper;

class GLX_LISTMODEL_EXPORT GlxAlbumModel : public QAbstractItemModel
{
Q_OBJECT
public :    
    
    GlxAlbumModel(GlxModelParm & modelParm);
    ~GlxAlbumModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

private :    
    /**
     * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
     */     
    void setContextMode(GlxContextMode contextMode);
    void setFocusIndex(const QModelIndex &index);
    void setSelectedIndex(const QModelIndex &index);
    QModelIndex getFocusIndex() const;
    HbIcon* GetPreviewIconItem(int itemIndex, GlxTBContextType tbContextType) const;
    HbIcon *getCorruptDefaultIcon ( const QModelIndex &index ) const;
    
signals :
    void iconAvailable(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType) const;
    void listPopulated();
public slots:
    void itemUpdated1(int mlIndex, GlxTBContextType tbContextType);
    void itemsAdded(int startIndex, int endIndex);
    void itemsRemoved(int startIndex, int endIndex);
    void modelPopulated();
protected:
    
private slots:
     void updateItemIcon(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType); 
private:
    GlxMLWrapper* mMLWrapper;
    GlxContextMode mContextMode;
    HbIcon* mDefaultIcon;
    QCache<int, HbIcon> itemIconCache;
    int mTempVisibleWindowIndex;
    HbIcon* m_CorruptIcon;
    int mSubState;
};

#endif /* GLXALBUMMODEL_H */
