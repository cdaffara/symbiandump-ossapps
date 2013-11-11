/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CREATORCONTACTELEMENT_H_
#define CREATORCONTACTELEMENT_H_

#include "creator_scriptelement.h"

namespace creatorcontact
{ 
    _LIT(KContact, "contact");
    _LIT(KContactSet, "contact-set");
    _LIT(KContactGroup, "contactgroup");
    _LIT(KMembers, "members");
    _LIT(KName, "name");
    
    // Contact field elements:
    _LIT(KFirstname, "firstname");
    _LIT(KLastname, "lastname");
    _LIT(KFirstnameReading, "firstnamereading");
    _LIT(KLastnameReading, "lastnamereading");
    _LIT(KPrefix, "prefix");
    _LIT(KSuffix, "suffix");
    _LIT(KSecondname, "secondname");
    _LIT(KLandphoneHome, "landphonehome");
    
    _LIT(KMobilephoneHome, "mobilephonehome");
    _LIT(KVideonumberHome, "videonumberhome");
    _LIT(KFaxnumberHome, "faxnumberhome");
    _LIT(KVoipHome, "voiphome");
    _LIT(KEmailHome, "emailhome");
    _LIT(KUrlHome, "urlhome");
    _LIT(KAddrlabelHome, "addrlabelhome");
    _LIT(KAddrpoHome, "addrpohome");
    
    _LIT(KAddrextHome, "addrexthome");
    _LIT(KAddrstreetHome, "addrstreethome");
    _LIT(KAddrlocalHome, "addrlocalhome");
    _LIT(KAddrregionHome, "addrregionhome");
    _LIT(KAddrpostcodeHome, "addrpostcodehome");
    _LIT(KAddrcountryHome, "addrcountryhome");
    _LIT(KJobtitle, "jobtitle");
    _LIT(KCompanyname, "company");
    
    _LIT(KLandphoneWork, "landphonework");
    _LIT(KMobilephoneWork, "mobilephonework");
    _LIT(KVideonumberWork, "videonumberwork");
    _LIT(KFaxnumberWork, "faxnumberwork");
    _LIT(KVoipWork, "voipwork");
    _LIT(KEmailWork, "emailwork");
    _LIT(KUrlWork, "urlwork");
    _LIT(KAddrlabelWork, "addrlabelwork");
    
    _LIT(KAddrpoWork, "addrpowork");
    _LIT(KAddrextWork, "addrextwork");
    _LIT(KAddrstreetWork, "addrstreetwork");
    _LIT(KAddrlocalWork, "addrlocalwork");
    _LIT(KAddrregionWork, "addrregionwork");
    _LIT(KAddrpostcodeWork, "addrpostcodework");
    _LIT(KAddrcountryWork, "addrcountrywork");
    _LIT(KLandphoneGen, "landphonegen");
    
    _LIT(KMobilephoneGen, "mobilephonegen");
    _LIT(KVideonumberGen, "videonumbergen");
    _LIT(KFaxnumberGen, "faxnumbergen");
    _LIT(KVoipGen, "voipgen");
    _LIT(KPoc, "poc");
    _LIT(KSwis, "swis");
    _LIT(KSip, "sip");
    _LIT(KEmailGen, "emailgen");
    
    _LIT(KUrlGen, "urlgen");
    _LIT(KAddrlabelGen, "addrlabelgen");
    _LIT(KAddrpoGen, "addrpogen");
    _LIT(KAddrextGen, "addrextgen");
    _LIT(KAddrstreetGen, "addrstreetgen");
    _LIT(KAddrlocalGen, "addrlocalgen");
    _LIT(KAddrregionGen, "addrregiongen");
    _LIT(KAddrpostcodeGen, "addrpostcodegen");
    
    _LIT(KAddrcountryGen, "addrcountrygen");
    _LIT(KPagerNumber, "pagernumber");
    _LIT(KDtmfString, "dtmfstring");
    _LIT(KWvAddress, "wvaddress");
    _LIT(KDate, "date");
    _LIT(KNote, "note");
    _LIT(KThumbnailPath, "thumbnailpath");
    _LIT(KThumbnailId, "thumbnailid");
    _LIT(KRingTone, "ringtonepath");
    _LIT(KRingToneId, "ringtoneid");
    
    _LIT(KCallerobjImg, "callerobjimg");
    _LIT(KCallerobjText, "callerobjtext");
    _LIT(KMiddlename, "middlename");
    _LIT(KDepartment, "department");
    _LIT(KAsstname, "asstname");
    _LIT(KSpouse, "spouse");
    _LIT(KChildren, "children");
    _LIT(KAsstphone, "asstphone");
    
    _LIT(KCarphone, "caphone");
    _LIT(KAnniversary, "anniversary");
    _LIT(KSyncclass, "synchronization");
    _LIT(KLocPrivacy, "locprivacy");
    _LIT(KGenlabel, "genlabel");
}
/**
 * Contact elements
 */

/**
 * Base class for contact elements
 */
class CCreatorContactElementBase : public CCreatorScriptElement
{
public:
    static CCreatorContactElementBase* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
protected:
    CCreatorContactElementBase(CCreatorEngine* aEngine);
};

/**
 * Contact element
 */
class CCreatorContactElement : public CCreatorContactElementBase
{
public:
    static CCreatorContactElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
protected:
    CCreatorContactElement(CCreatorEngine* aEngine);
};

/**
 * Contact-set element
 */
class CCreatorContactSetElement : public CCreatorContactElementBase
{
public:
    static CCreatorContactSetElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    virtual void AddToCacheL();
protected:
    CCreatorContactSetElement(CCreatorEngine* aEngine);
};

/**
 * Contact group element
 */
class CCreatorContactGroupElement : public CCreatorContactElementBase
{
public:
    static CCreatorContactGroupElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
protected:
    CCreatorContactGroupElement(CCreatorEngine* aEngine);
};

/**
 * Contact field element
 */
class CCreatorContactFieldElement : public CCreatorScriptElement
{
public:
    static CCreatorContactFieldElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
protected:
    CCreatorContactFieldElement(CCreatorEngine* aEngine);
};

#endif /*CREATORCONTACTELEMENT_H_*/
