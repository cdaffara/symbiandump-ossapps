/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging macros
*
*/



#ifndef ATTRIBUTELOG_H_
#define ATTRIBUTELOG_H_

#include <glxlogger.h>
#include <mpxmediageneraldefs.h> // KMPXMediaIdGeneral attributes
#include <glxmediacollectioninternaldefs.h> // KGlxMediaIdCollectionInternal
#include <glxbackgroundtnmessagedefs.h> // KGlxMessageIdBackgroundThumbnail
#include <glxcollectiongeneraldefs.h> // KGlxMediaIdCollectionPluginSpecific
#include <glxcollectionmessagedefs.h> // KGlxCollectionMessageSpecific
#include <glxfiltergeneraldefs.h>  // KGlxFilterIdGeneral
#include <glxmediageneraldefs.h> // KGlxMediaIdGeneral
//#include <glxmediastaticitemdefs.h> // KGlxMediaIdStaticItem
#include <mpxmediadrmdefs.h> // KMPXMediaIdDrm

void LogAttribute(const TDesC8& aText, const TMPXAttribute& aAttribute)
	{
	TBuf8<0x50> attributeName;
	
	// KMPXMediaIdGeneral attributes
	if (aAttribute == KMPXMediaGeneralType)
		{
		attributeName.Append(_L8("KMPXMediaGeneralType"));
		}
	else if (aAttribute == KMPXMediaGeneralCategory)
		{
		attributeName.Append(_L8("KMPXMediaGeneralCategory"));
		}
	else if (aAttribute == KMPXMediaGeneralUri)
		{
		attributeName.Append(_L8("KMPXMediaGeneralUri"));
		}
	else if (aAttribute == KMPXMediaGeneralDrive)
		{
		attributeName.Append(_L8("KMPXMediaGeneralDrive"));
		}
	else if (aAttribute == KMPXMediaGeneralSize)
		{
		attributeName.Append(_L8("KMPXMediaGeneralSize"));
		}
	else if (aAttribute == KMPXMediaGeneralDuration)
		{
		attributeName.Append(_L8("KMPXMediaGeneralDuration"));
		}
	else if (aAttribute == KMPXMediaGeneralTitle)
		{
		attributeName.Append(_L8("KMPXMediaGeneralTitle"));
		}
	else if (aAttribute == KMPXMediaGeneralDate)
		{
		attributeName.Append(_L8("KMPXMediaGeneralDate"));
		}
	else if (aAttribute == KMPXMediaGeneralDate)
		{
		attributeName.Append(_L8("KMPXMediaGeneralDate"));
		}
	else if (aAttribute == KMPXMediaGeneralComment)
		{
		attributeName.Append(_L8("KMPXMediaGeneralComment"));
		}
	else if (aAttribute == KMPXMediaGeneralMimeType)
		{
		attributeName.Append(_L8("KMPXMediaGeneralMimeType"));
		}
	else if (aAttribute == KMPXMediaGeneralThumbnail1)
		{
		attributeName.Append(_L8("KMPXMediaGeneralThumbnail1"));
		}
	else if (aAttribute == KMPXMediaGeneralThumbnail2)
		{
		attributeName.Append(_L8("KMPXMediaGeneralThumbnail2"));
		}
	else if (aAttribute == KMPXMediaGeneralIcon)
		{
		attributeName.Append(_L8("KMPXMediaGeneralIcon"));
		}
	else if (aAttribute == KMPXMediaGeneralRemote)
		{
		attributeName.Append(_L8("KMPXMediaGeneralRemote"));
		}
	else if (aAttribute == KMPXMediaGeneralSynchronized)
		{
		attributeName.Append(_L8("KMPXMediaGeneralSynchronized"));
		}
	else if (aAttribute == KMPXMediaGeneralDeleted)
		{
		attributeName.Append(_L8("KMPXMediaGeneralDeleted"));
		}
	else if (aAttribute == KMPXMediaGeneralModified)
		{
		attributeName.Append(_L8("KMPXMediaGeneralModified"));
		}
	else if (aAttribute == KMPXMediaGeneralCount)
		{
		attributeName.Append(_L8("KMPXMediaGeneralCount"));
		}
	else if (aAttribute == KMPXMediaGeneralId)
		{
		attributeName.Append(_L8("KMPXMediaGeneralId"));
		}
	else if (aAttribute == KMPXMediaGeneralCollectionId)
		{
		attributeName.Append(_L8("KMPXMediaGeneralCollectionId"));
		}
	else if (aAttribute == KMPXMediaGeneralBrokenLink)
		{
		attributeName.Append(_L8("KMPXMediaGeneralBrokenLink"));
		}
	else if (aAttribute == KMPXMediaGeneralNonPermissibleActions)
		{
		attributeName.Append(_L8("KMPXMediaGeneralNonPermissibleActions"));
		}
	else if (aAttribute == KMPXMediaGeneralContainerId)
		{
		attributeName.Append(_L8("KMPXMediaGeneralContainerId"));
		}
	else if (aAttribute == KMPXMediaGeneralSubTitle)
		{
		attributeName.Append(_L8("KMPXMediaGeneralSubTitle"));
		}
	else if (aAttribute == KMPXMediaGeneralCopyright)
		{
		attributeName.Append(_L8("KMPXMediaGeneralCopyright"));
		}
	else if (aAttribute == KMPXMediaGeneralFlags)
		{
		attributeName.Append(_L8("KMPXMediaGeneralFlags"));
		}
	else if (aAttribute == KMPXMediaGeneralLastPlaybackPosition)
		{
		attributeName.Append(_L8("KMPXMediaGeneralLastPlaybackPosition"));
		}
	else if (aAttribute == KMPXMediaGeneralLastPlaybackTime)
		{
		attributeName.Append(_L8("KMPXMediaGeneralLastPlaybackTime"));
		}
	else if (aAttribute == KMPXMediaGeneralPlayCount)
		{
		attributeName.Append(_L8("KMPXMediaGeneralPlayCount"));
		}
	else if (aAttribute == KMPXMediaGeneralContainerPath)
		{
		attributeName.Append(_L8("KMPXMediaGeneralContainerPath"));
		}
	else if (aAttribute == KMPXMediaGeneralPath)
		{
		attributeName.Append(_L8("KMPXMediaGeneralPath"));
		}
	else if (aAttribute == KMPXMediaGeneralValue)
		{
		attributeName.Append(_L8("KMPXMediaGeneralValue"));
		}
	else if (aAttribute == KMPXMediaGeneralAll)
		{
		attributeName.Append(_L8("KMPXMediaGeneralAll"));
		}	
	//  KGlxMediaIdCollectionInternal attributes
	else if (aAttribute == KGlxMediaCollectionInternalStartDate)
		{
		attributeName.Append(_L8("KGlxMediaCollectionInternalStartDate"));
		}	
	else if (aAttribute == KGlxMediaCollectionInternalEndDate)
		{
		attributeName.Append(_L8("KGlxMediaCollectionInternalEndDate"));
		}
	else if (aAttribute == KGlxMediaCollectionInternalUsageCount)
		{
		attributeName.Append(_L8("KGlxMediaCollectionInternalUsageCount"));
		}
	else if (aAttribute == KGlxMediaCollectionInternalSystemItemType)
		{
		attributeName.Append(_L8("KGlxMediaCollectionInternalSystemItemType"));
		}
	// KGlxMessageIdBackgroundThumbnail attributes
	else if (aAttribute == KGlxBackgroundThumbnailMediaId)
		{
		attributeName.Append(_L8("KGlxBackgroundThumbnailMediaId"));
		}
	else if (aAttribute == KGlxBackgroundThumbnailSize)
		{
		attributeName.Append(_L8("KGlxBackgroundThumbnailSize"));
		}
	else if (aAttribute == KGlxBackgroundThumbnailError)
		{
		attributeName.Append(_L8("KGlxBackgroundThumbnailError"));
		}
	// KGlxMediaIdCollectionPluginSpecific attributes
	else if (aAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
		{
		attributeName.Append(_L8("KGlxMediaCollectionPluginSpecificSubTitle"));
		}
	else if (aAttribute == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
		{
		attributeName.Append(_L8("KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle"));
		}	
	else if (aAttribute == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
		{
		attributeName.Append(_L8("KGlxMediaCollectionPluginSpecificNewMediaItemTitle"));
		}	
	else if (aAttribute == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
		{
		attributeName.Append(_L8("KGlxMediaCollectionPluginSpecificDefaultMediaTitle"));
		}	
	// KGlxCollectionMessageSpecific attributes
	else if (aAttribute == KGlxCollectionMessageContainerId)
		{
		attributeName.Append(_L8("KGlxCollectionMessageContainerId"));
		}
	else if (aAttribute == KGlxCollectionMessageContainerCategory)
		{
		attributeName.Append(_L8("KGlxCollectionMessageContainerCategory"));
		}
	else if (aAttribute == KGlxCollectionMessageContainerCameraAlbum)
		{
		attributeName.Append(_L8("KGlxCollectionMessageContainerCameraAlbum"));
		}
	// KGlxFilterIdGeneral attributes
	else if (aAttribute == KGlxFilterGeneralSortType)
		{
		attributeName.Append(_L8("KGlxFilterGeneralSortType"));
		}
	else if (aAttribute == KGlxFilterGeneralSortDirection)
		{
		attributeName.Append(_L8("KGlxFilterGeneralSortDirection"));
		}
	else if (aAttribute == KGlxFilterGeneralIncludeCameraAlbum)
		{
		attributeName.Append(_L8("KGlxFilterGeneralIncludeCameraAlbum"));
		}
	else if (aAttribute == KGlxFilterGeneralMinCount)
		{
		attributeName.Append(_L8("KGlxFilterGeneralMinCount"));
		}
	else if (aAttribute == KGlxFilterGeneralItemId)
		{
		attributeName.Append(_L8("KGlxFilterGeneralItemId"));
		}
	else if (aAttribute == KGlxFilterGeneralExcludeAnimation)
		{
		attributeName.Append(_L8("KGlxFilterGeneralExcludeAnimation"));
		}
	else if (aAttribute == KGlxFilterGeneralLastCaptureDate)
		{
		attributeName.Append(_L8("KGlxFilterGeneralLastCaptureDate"));
		}
	else if (aAttribute == KGlxFilterGeneralThumbnailLoadability)
		{
		attributeName.Append(_L8("KGlxFilterGeneralThumbnailLoadability"));
		}
	else if (aAttribute == KGlxFilterGeneralMPXCollectionPath)
		{
		attributeName.Append(_L8("KGlxFilterGeneralMPXCollectionPath"));
		}
	else if (aAttribute == KGlxFilterGeneralItemType)
		{
		attributeName.Append(_L8("KGlxFilterGeneralItemType"));
		}
	else if (aAttribute == KGlxFilterGeneralUri)
		{
		attributeName.Append(_L8("KGlxFilterGeneralUri"));
		}
	else if (aAttribute == KGlxFilterGeneralSortOrderPromoteSystemItems)
		{
		attributeName.Append(_L8("KGlxFilterGeneralSortOrderPromoteSystemItems"));
		}
	else if (aAttribute == KGlxFilterGeneralOrigin)
		{
		attributeName.Append(_L8("KGlxFilterGeneralOrigin"));
		}
	else if (aAttribute == KGlxFilterGeneralNoDRM)
		{
		attributeName.Append(_L8("KGlxFilterGeneralNoDRM"));
		}
	else if (aAttribute == KGlxFilterGeneralNavigationalStateOnly)
		{
		attributeName.Append(_L8("KGlxFilterGeneralNavigationalStateOnly"));
		}
	// KGlxMediaIdGeneral attributes
	else if (aAttribute == KGlxMediaGeneralSystemItem)
		{
		attributeName.Append(_L8("KGlxMediaGeneralSystemItem"));
		}
	else if (aAttribute == KGlxMediaGeneralDimensions)
		{
		attributeName.Append(_L8("KGlxMediaGeneralDimensions"));
		}
	else if (aAttribute == KGlxMediaGeneralFramecount)
		{
		attributeName.Append(_L8("KGlxMediaGeneralFramecount"));
		}
	else if (aAttribute == KGlxMediaGeneralLocation)
		{
		attributeName.Append(_L8("KGlxMediaGeneralLocation"));
		}
	else if (aAttribute == KGlxMediaGeneralSlideshowableContent)
		{
		attributeName.Append(_L8("KGlxMediaGeneralSlideshowableContent"));
		}
	else if (aAttribute == KGlxMediaGeneralLastModifiedDate)
		{
		attributeName.Append(_L8("KGlxMediaGeneralLastModifiedDate"));
		}
	// KMPXMediaIdDrm attributes
	else if (aAttribute == KMPXMediaDrmType)
		{
		attributeName.Append(_L8("KMPXMediaDrmType"));
		}
	else if (aAttribute == KMPXMediaDrmProtected)
		{
		attributeName.Append(_L8("KMPXMediaDrmProtected"));
		}
	else if (aAttribute == KMPXMediaDrmRightsStatus)
		{
		attributeName.Append(_L8("KMPXMediaDrmRightsStatus"));
		}
	else if (aAttribute == KMPXMediaDrmRightsType)
		{
		attributeName.Append(_L8("KMPXMediaDrmRightsType"));
		}
	else if (aAttribute == KMPXMediaDrmCount)
		{
		attributeName.Append(_L8("KMPXMediaDrmCount"));
		}
	else if (aAttribute == KMPXMediaDrmStartTime)
		{
		attributeName.Append(_L8("KMPXMediaDrmStartTime"));
		}
	else if (aAttribute == KMPXMediaDrmEndTime)
		{
		attributeName.Append(_L8("KMPXMediaDrmEndTime"));
		}
	else if (aAttribute == KMPXMediaDrmInterval)
		{
		attributeName.Append(_L8("KMPXMediaDrmInterval"));
		}
	else if (aAttribute == KMPXMediaDrmIntervalStartTime)
		{
		attributeName.Append(_L8("KMPXMediaDrmIntervalStartTime"));
		}
	else if (aAttribute == KMPXMediaDrmAccumulatedTime)
		{
		attributeName.Append(_L8("KMPXMediaDrmAccumulatedTime"));
		}
	else if (aAttribute == KMPXMediaDrmSendingAllowed)
		{
		attributeName.Append(_L8("KMPXMediaDrmSendingAllowed"));
		}
	else if (aAttribute == KMPXMediaDrmCanSetAutomated)
		{
		attributeName.Append(_L8("KMPXMediaDrmCanSetAutomated"));
		}
	else if (aAttribute == KMPXMediaDrmHasInfoUrl)
		{
		attributeName.Append(_L8("KMPXMediaDrmHasInfoUrl"));
		}
	else if (aAttribute == KMPXMediaDrmHasPreviewUrl)
		{
		attributeName.Append(_L8("KMPXMediaDrmHasPreviewUrl"));
		}
	else if (aAttribute == KMPXMediaDrmAboutToExpire)
		{
		attributeName.Append(_L8("KMPXMediaDrmAboutToExpire"));
		}
	else if (aAttribute == KMPXMediaDrmAll)
		{
		attributeName.Append(_L8("KMPXMediaDrmAll"));
		}	
	else
		{	    
		attributeName.AppendFormat(_L8("context id: %d, attribute id %d"), aAttribute.ContentId(), aAttribute.AttributeId());
		}
	
	GlxLogger::WriteFormat(_L8("%S %S"), &aText, &attributeName);
	}


#endif /*ATTRIBUTELOG_H_*/
