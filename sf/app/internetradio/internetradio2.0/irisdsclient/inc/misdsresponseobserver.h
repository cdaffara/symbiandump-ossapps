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
* Description:  The implementation for presentation elements.
*
*/


#ifndef MISDSRESPONSEOBSERVER
#define MISDSRESPONSEOBSERVER

#include <e32def.h>

class CIRBrowseCatagoryItems;
class CIRBrowseChannelItems;
class CIRIsdsPreset;
class CIROTAUpdate;

/**
 * MIsdsResponseObserver
 * passes events and responses body data recived from isds server with this interface.
 * An instance of this class must be provided for construction of 
 * CIRIsdsClient.
 */
class MIsdsResponseObserver
    {
public:
    /**
     * MIsdsResponseObserver::IsdsErrorL()
     * Used to Indiacte error in retrieving data from Isds
     * @param aErrCode- contains error code
     */
    virtual void IsdsErrorL(TInt aErrCode) = 0;

    /**
     * MIsdsResponseObserver::IsdsCatogoryDataReceivedL()
     * Called when catogory information is recieved from ISDS
     * @param aParsedStructure - contains catogory info
     */
	virtual void IsdsCatogoryDataReceivedL(
		CArrayPtrFlat<CIRBrowseCatagoryItems> & aParsedStructure) = 0;

    /**
     * MIsdsResponseObserver::IsdsChannelDataReceivedL()
     * Called when channel information is recieved from ISDS
     * @param aParsedStructure - contains Channel info
     */
    virtual void IsdsChannelDataReceivedL(
            CArrayPtrFlat<CIRBrowseChannelItems> & aParsedStructure) = 0;

    /**
     * MIsdsResponseObserver::IsdsPresetDataReceivedL()
     * Called when preset information is recieved from ISDS
     * @param aParsedStructure - contains Preset info
     */
    virtual void IsdsPresetDataReceivedL(
            CArrayPtrFlat<CIRIsdsPreset> & aParsedStructure) = 0;

    /**
     * MIsdsResponseObserver::IsdsIRIDRecieved()
     * Called to provide IRID
     * @param aIRID  - contains IRID recived from Isds
     */
    virtual void IsdsIRIDRecieved(const TDesC& aIRID)=0;

    /**
     * MIsdsResponseObserver::IsdsOtaInfoRecieved()
     * called back when OTA info is recieved and Ui needs to be intimated
     * @param CIROTAUpdate&,the ota information 
     */
    virtual void IsdsOtaInfoRecieved(CIROTAUpdate &aOtaData)=0;

    /**
     * MIsdsResponseObserver::IsdsPresetRemovedL()
     * called back when the preset that is synced was removed from isds
     * @param TInt,the preset id
     */
    virtual void IsdsPresetRemovedL(TInt aId)=0;

    /**
     * MIsdsResponseObserver::IsdsPresetChangedL()
     * called back when the preset that is synced has been changed in the isds
     * @param CIRIsdsPreset&,the new preset data
     */
    virtual void IsdsPresetChangedL(CIRIsdsPreset& aPreset)=0;

    /**
     * MIsdsResponseObserver::IsdsPresetNoChangeL()
     * called back when the preset that is synced has not been changed
     */
    virtual void IsdsPresetNoChangeL()=0;

    };

#endif // MISDSRESPONSEOBSERVER
