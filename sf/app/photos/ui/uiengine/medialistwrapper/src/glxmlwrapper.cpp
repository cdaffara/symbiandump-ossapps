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

/*glxmlwrapper.cpp*/

//internal includes 
#include "glxmlwrapper.h"
#include "glxmlwrapper_p.h"
#include <qimage.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// constructor.
// ---------------------------------------------------------------------------
//
GlxMLWrapper::GlxMLWrapper(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType,QString uri,GlxFetcherFilterType fetcherFilterType)
{
    mMLWrapperPrivate = GlxMLWrapperPrivate::Instance(this, aCollectionId, aHierarchyId, aFilterType,uri,fetcherFilterType);
}

// ---------------------------------------------------------------------------
// destructor.
// ---------------------------------------------------------------------------
//
GlxMLWrapper::~GlxMLWrapper()
{
	delete mMLWrapperPrivate;
}

// ---------------------------------------------------------------------------
// setContextMode.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setContextMode(GlxContextMode contextMode)
{
	mMLWrapperPrivate->SetContextMode(contextMode);
}

// ---------------------------------------------------------------------------
// removeContextMode.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::removeContextMode(GlxContextMode contextMode)
{
    mMLWrapperPrivate->RemoveContextMode(contextMode);
}



// ---------------------------------------------------------------------------
// getItemCount.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getItemCount()
{
	return (mMLWrapperPrivate->GetItemCount());
}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
HbIcon* GlxMLWrapper::retrieveItemIcon(int index, GlxTBContextType aTBContextType)
{
	return (mMLWrapperPrivate->RetrieveItemIcon(index,aTBContextType));

}

QImage GlxMLWrapper::retrieveItemImage(int index, GlxTBContextType aTBContextType)
{
    return (mMLWrapperPrivate->RetrieveItemImage(index,aTBContextType));
}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveListTitle(int index)
{
	return (mMLWrapperPrivate->RetrieveListTitle(index));
}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveListSubTitle(int index)
{
	return (mMLWrapperPrivate->RetrieveListSubTitle(index));
}

// ---------------------------------------------------------------------------
// retrieveListDesc.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveListDesc(int index)
{
    return (mMLWrapperPrivate->RetrieveListDesc(index));
}

// ---------------------------------------------------------------------------
// retrieveItemUri.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveItemUri(int index)
{
	return (mMLWrapperPrivate->RetrieveItemUri(index));
}
// ---------------------------------------------------------------------------
// retrieveItemDimension.
// ---------------------------------------------------------------------------
//
QSize GlxMLWrapper::retrieveItemDimension(int index)
{
	return (mMLWrapperPrivate->RetrieveItemDimension(index));
}

// ---------------------------------------------------------------------------
// retrieveItemSize.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::retrieveItemSize(int index)
{
    return (mMLWrapperPrivate->RetrieveItemSize(index));
}

// ---------------------------------------------------------------------------
// retrieveItemDate.
// ---------------------------------------------------------------------------
//
QDate GlxMLWrapper::retrieveItemDate(int index)
{
	return (mMLWrapperPrivate->RetrieveItemDate(index));
}

// ---------------------------------------------------------------------------
// retrieveItemTime.
// ---------------------------------------------------------------------------
//
QTime GlxMLWrapper::retrieveItemTime(int index)
{
    return (mMLWrapperPrivate->RetrieveItemTime(index));
}


int GlxMLWrapper::retrieveItemFrameCount(int index)
{
    return (mMLWrapperPrivate->RetrieveItemFrameCount(index));
}

QString GlxMLWrapper::retrieveViewTitle()
{
    return (mMLWrapperPrivate->RetrieveViewTitle());
}

void GlxMLWrapper::handlepopulated()
{
    emit populated();
}

bool GlxMLWrapper::IsPopulated()
{
    return mMLWrapperPrivate->IsPopulated();
}

bool GlxMLWrapper::IsDrmProtected(int index)
    {
    return mMLWrapperPrivate->IsDrmProtected(index);
    }
bool GlxMLWrapper::IsDrmValid(int index)
	{
	return mMLWrapperPrivate->IsDrmValid(index);
	}

void GlxMLWrapper::setDrmValid(int index,bool valid)
	{
	mMLWrapperPrivate->setDrmValid(index,valid);
	}

QVariant GlxMLWrapper::RetrieveBitmap(int index)
{
    QVariant var;
    var.setValue(mMLWrapperPrivate->RetrieveBitmap(index));
    return var;
}

int GlxMLWrapper::retrieveListItemCount(int index)
{
    return mMLWrapperPrivate->RetrieveListItemCount( index ) ;
}

bool GlxMLWrapper::isSystemItem( int aItemIndex )
{
    return mMLWrapperPrivate->isSystemItem( aItemIndex );   
}

// ---------------------------------------------------------------------------
// getFocusIndex.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getFocusIndex() const
{
    return (mMLWrapperPrivate->GetFocusIndex());
}

// ---------------------------------------------------------------------------
// setFocusIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setFocusIndex(int itemIndex)
{
    mMLWrapperPrivate->SetFocusIndex(itemIndex);
}

// ---------------------------------------------------------------------------
// SetSelectedIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setSelectedIndex(int itemIndex)
{
    mMLWrapperPrivate->SetSelectedIndex(itemIndex);
}

// ---------------------------------------------------------------------------
// getVisibleWindowIndex.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getVisibleWindowIndex()
{
    return (mMLWrapperPrivate->GetVisibleWindowIndex());
}

// ---------------------------------------------------------------------------
// setVisibleWindowIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setVisibleWindowIndex(int itemIndex)
{
    mMLWrapperPrivate->SetVisibleWindowIndex(itemIndex);
}


// ---------------------------------------------------------------------------
// itemsAdded.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::itemsAdded(int startIndex,int endIndex)
{
	emit insertItems(startIndex,endIndex);
}

// ---------------------------------------------------------------------------
// itemsRemoved.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::itemsRemoved(int startIndex,int endIndex)
{
	emit removeItems(startIndex,endIndex);
}

// ---------------------------------------------------------------------------
// handleReceivedIcon.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleReceivedIcon(int itemIndex, GlxTBContextType tbContextType)
{
	emit updateItem(itemIndex, tbContextType);
}


// ---------------------------------------------------------------------------
// handleIconCorrupt.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleIconCorrupt(int itemIndex)
{
	emit itemCorrupted(itemIndex);
}

// ---------------------------------------------------------------------------
// handleListItemAvailable.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleListItemAvailable(int itemIndex)
{
	emit updateItem(itemIndex, GlxTBContextNone);
}


// ---------------------------------------------------------------------------
// handleDetailsItemAvailable.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleDetailsItemAvailable(int itemIndex)
{
    Q_UNUSED( itemIndex )
    emit updateDetails();
}

// ---------------------------------------------------------------------------
// handleGeneralError.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleGeneralError(int aError)
{
	Q_UNUSED(aError);
}

// ---------------------------------------------------------------------------
// handleTitleAvailable.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleTitleAvailable(QString aTitle)
{
    emit updateAlbumTitle(aTitle);
}

bool GlxMLWrapper::isCorruptedImage( int index )
{
    return mMLWrapperPrivate->IsCorruptedImage( index );
}
