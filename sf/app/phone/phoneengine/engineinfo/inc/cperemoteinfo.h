/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPERemoteInfo object
*
*/


#ifndef CPEREMOTEINFO_H
#define CPEREMOTEINFO_H

#include <e32base.h>
#include "pevirtualengine.h"

class CPhCntContactId;

/**
 *  Remote info definition
 *
 *  This class defines the call remote end information
 *  used for contact matching and logging
 *
 *  @lib PhoneEngine.lib
 *  @since S60 v4.0
 */
NONSHARABLE_CLASS( CPERemoteInfo ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPERemoteInfo* NewL();

    /**
     * Destructor
     */
    virtual ~CPERemoteInfo();

    /**
     * Get call id associated with this information
     * @return call identification number
     */
    TInt CallId() const;

    /**
     * Get contact link or id
     * @return contact link or id
     */
    const CPhCntContactId& ContactId() const;

    /**
     * Get contact link.
     * @return Virtual phonebook contact link
     */
    const TDesC8& ContactLink() const;
    
    /**
     * Get remote end phone number
     * @return phone number
     */
    const TPEPhoneNumber& PhoneNumber() const;

    /**
     * Get remote end connected phone number
     * @return phone number
     */
    const TPEPhoneNumber& ColpNumber() const;

    /**
     * Get contact name
     * Depending on user selected language may be in order:
     * first + last name OR last + first name
     * @return contact name
     */
    const TPEContactName& Name() const;

    /**
     * Get contact company name
     * @return company name
     */
    const TPEContactCompany& CompanyName() const;

    /**
     * Get ringing tone
     * Returns a ringing tone defined for one of the user groups
     * if personal tone is not found
     * @return ringing tone file path
     */
    const TPEContactFileName& PersonalRingingTone() const;

    /**
     * Get remote end phone number type
     * @return phone number type
     */
    const TPEPhoneNumberIdType& PhoneNumberType() const;

    /**
    * Get text to speech text for this contact. Used in
    * text to speech ringing tone.
    * @return descriptor, ownership passed.
    */
    const TPtrC TextToSpeechText() const;

    /**
     * Get user group ids this contact is assigned to
     * @return array for group ids
     */
    const TArray< TContactItemId > ContactGroups() const;

    /**
     * Get caller text
     * @return caller text
     */
    const TPtrC CallerText() const;

    /**
     * Get caller image
     * @return caller image
     */
    const TPtrC CallerImage() const;

    /**
     * Get caller thumbnail
     * @return pointer to thumbnail data
     */
    CFbsBitmap* CallerThumbnail() const;

    /**
     * Get caller thumbnail status
     * @return status of caller thumbnail
     */
    TBool HasCallerThumbnail() const;

    /**
     * Get predefined dtmf strings
     * @return reference to string array
     */
    CDesCArray& PredefinedDtmfStrings();
     
    /**
     * Set call association for this information
     * @param aContactId pointer to contact id
     */
    void SetCallId( const TInt aCallId );

    /**
     * Set contact link or id
     * @param aContactId pointer to contact id
     */
    void SetContactId( CPhCntContactId* aContactId );

    /**
     * Set remote end phone number
     * @param aPhoneNumber phone number
     */
    void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber );

    /**
     * Set remote end connected phone number
     * @param aColpNumber phone number
     */
    void SetColpNumber( const TPEPhoneNumber& aColpNumber );

    /**
     * Set contact name
     * Depending on user selected language may be in order:
     * first + last name OR last + first name
     * @param aContactName contact name
     */
    void SetName( const TPEContactName& aContactName );

    /**
     * Set contact company name
     * @param aCompanyName company name
     */
    void SetCompanyName( const TPEContactName& aCompanyName );

    /**
     * Set ringing tone
     * @param aPersonalRingingTone ringing tone file path
     */
    void SetPersonalRingingTone(
        const TPEContactFileName& aPersonalRingingTone );

    /**
     * Set remote end phone number type
     * @param aPhoneNumberType phone number id
     */
    void SetPhoneNumberType( const TPEPhoneNumberIdType aPhoneNumberType );

    /**
     * Set remote end Text To Speech Text
     * @param aText Text To Speech Text
     */
    void SetTextToSpeechText( HBufC* aText );

    /**
     * Set user group ids this contact is assigned to
     * Array is reset if the operation fails.
     * @param aGroupIdArray array of group ids
     */
    void SetContactGroups( const TArray< TContactItemId >& aGroupIdArray );

    /**
     * Set caller text
     * @param aCallerText caller text
     */
    void SetCallerText( const TPtrC aCallerText );

    /**
     * Set caller image
     * @param aCallerImage caller image
     */
    void SetCallerImage( const TPtrC aCallerImage );

    /**
     * Set caller thumbnail
     * @param aCallerThumbnail  caller thumbnail
     */
    void SetCallerThumbnail( CFbsBitmap* aCallerThumbnail );

    /**
     * Set caller thumbnail status
     * @param aStatus  caller thumbnail status
     */
    void SetHasCallerThumbnail( TBool aStatus );

    /**
     * Set predefined dtmf strings
     * @param aArray, is reference to sring array 
     */
    void SetPredefinedDtmfStrings( 
            const CDesCArray& aArray );           
    /**
     * Reset remote information
     * Resets all member data
     */
    void ResetRemoteInfo();

private:

    /**
     * Constructor
     */
    CPERemoteInfo();

private: // data

    /**
     * Call id association
     */
    TInt iCallId;

    /**
     * Contact link or id
     * Own.
     */
    CPhCntContactId* iContactId;

    /**
     * Contact link
     * Own.
     */
    HBufC8* iContactLink;
    
    /**
     * Remote end phone number
     */
    TPEPhoneNumber iPhoneNumber;

    /**
     * Remote end phone number if the mobile originated call was forwarded
     */
    TPEPhoneNumber iColpNumber;

    /**
     * First name + Last name
     */
    TPEContactName iName;

    /**
     * Remote end company name
     */
    TPEContactCompany iCompanyName;

    /**
     * Remote end personal or group ringing tone
     */
    TPEContactFileName iPersonalRingingTone;

    /**
     * Remote end phone number type
     */
    TPEPhoneNumberIdType iPhoneNumberType;

    /**
     * Remote end Text To Speech text
     */
    HBufC* iTextToSpeechText;

    /**
     * Array for caller group ids
     * Own.
     */
    RArray< TContactItemId > iGroupId;

    /**
     * Caller text
     */
    HBufC* iCallerText;

    /**
     * Caller image
     */
    HBufC* iCallerImage;

    /**
     * Caller thumbnail image
     * Own.
     */
    CFbsBitmap* iCallerThumbnail;
    
    /**
     * Caller thumbnail image status
     * Own.
     */
    TBool iHasCallerThumbnail;    
    
    /**
     * Dtmf array
     *  
     */
    CDesCArray* iPredefinedDtmfStrings;
    
    
    };

#endif // CPEREMOTEINFO_H
