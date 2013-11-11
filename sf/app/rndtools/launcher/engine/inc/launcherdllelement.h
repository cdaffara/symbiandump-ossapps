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

#ifndef LAUNCHERDLLELEMENT_H_
#define LAUNCHERDLLELEMENT_H_

#include <e32base.h>

typedef TBuf8<8> TDllIdValue;

class CLauncherDLLElement : public CBase
    {
public:
    
    enum TDifference
        {
        EDifference_UID1,      // UID1 is different
        EDifference_UID2,      // UID1 is different
        EDifference_UID3,      // UID1 is different
        EDifference_SID,       // SID is different
        EDifference_Capability // Capabilities are different
        };
    
    virtual ~CLauncherDLLElement();
    static CLauncherDLLElement* NewL();
    static CLauncherDLLElement* NewLC(); 
    
    /*
     * Compares DLL elements.
     * @return ETrue if differencies exist, EFalse otherwise.     
     * @param aReferenceElement Reference DLL-element for comparison
     * @param aDifferenceList Differencies are added to this list.
     */
    TBool CompareL(const CLauncherDLLElement& aReferenceElement, RArray<TDifference>& aDifferenceList) const;
    
    /*
     * Returns DLL name
     * @return DLL name
     */
    TFileName Name() const;    
    /*
     * Sets DLL name
     * @param aName DLL name
     */
    void SetNameL(const TFileName& aName);
    
    /*
     * Returns DLL UID1
     * @return UID1 object
     */
    TUid UID1() const;
    
    /*
     * Sets DLL UID1 
     * @param aUID1 UID1 object
     */
    void SetUID1L(TUid aUID1);
    
    /*
     * Returns DLL UID2
     * @return UID2 object
     */
    TUid UID2() const;
        
    /*
     * Sets DLL UID2 
     * @param aUID2 UID2 object
     */
    void SetUID2L(TUid aUID2);
        
    /*
     * Returns DLL UID3
     * @return UID3 object
     */
    TUid UID3() const;
    
    /*
     * Sets DLL UID3
     * @param aUID3 UID3 object
     */
    void SetUID3L(TUid aUID3);
    
    /*
     * Returns DLL SID
     * @return SID object
     */
    TUid SID() const;
    
    /*
     * Sets DLL SID
     * @param aSID SID object
     */
    void SetSIDL(TUid aSID);
    
    /*
     * Returns DLL capability
     * @return capability value
     */    
    TUint32 Capability() const;
    
    /*
     * Sets DLL capability
     * @param aCapability capability value
     */
    void SetCapabilityL(TUint32 aCapability);
    
protected:
    CLauncherDLLElement();
    virtual void ConstructL();
    
private:
    TFileName   iName;
    TUid iUID1;
    TUid iUID2;
    TUid iUID3;
    TUid iSID;
    TUint32 iCapability;
    };

#endif /* LAUNCHERDLLELEMENT_H_ */
