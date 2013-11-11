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


#ifndef LOADGEN_EDITORS_H
#define LOADGEN_EDITORS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AknForm.h> 

#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS
class CAknQueryValueText;
class CAknQueryValueTextArray;

// CLASS DECLARATIONS

class CLoadGenLoadTypeEditorBase : public CAknForm
    {
protected: // Constructors
    CLoadGenLoadTypeEditorBase(TBool aEditingExisting);
    void ConstructL(const TDesC& aTitleText);        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);

protected:  // New methods
    virtual TBool DoRunQueryLD(TInt aResource);
    
public:  // New methods
    virtual TBool RunQueryLD();

protected:  // New methods
    void InsertFieldAfterL(TInt aResourceId, TInt aControlId, TInt aPrevControlId);
    void UpdateFormL();

protected: // Data
    TBool iEditingExisting;
    };




class CLoadGenCPULoadEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenCPULoadEditor* NewL(TCPULoadAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenCPULoadEditor();

private: // Constructors
    CLoadGenCPULoadEditor(TCPULoadAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    void LoadCPUsL();
    
private: // Data
    TCPULoadAttributes&         iAttributes;
    CAknQueryValueText*         iCpuQueryValText;
    CAknQueryValueTextArray*    iCpuTextArray;
    CDesCArray*                 iCPUsArray;
    };




class CLoadGenMemoryEatEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenMemoryEatEditor* NewL(TMemoryEatAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenMemoryEatEditor();

private: // Constructors
    CLoadGenMemoryEatEditor(TMemoryEatAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    void UpdateAvailableMemoryL();

private: // Data
    TMemoryEatAttributes&              iAttributes;
    }; 



class CLoadGenPhoneCallEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenPhoneCallEditor* NewL(TPhoneCallAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenPhoneCallEditor();

private: // Constructors
    CLoadGenPhoneCallEditor(TPhoneCallAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    
private: // Data
    TPhoneCallAttributes&              iAttributes;
    };



class CLoadGenNetConnEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenNetConnEditor* NewL(TNetConnAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenNetConnEditor();

private: // Constructors
    CLoadGenNetConnEditor(TNetConnAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    
private: // Data
    TNetConnAttributes&              iAttributes;
    };
    
 
 

class CLoadGenKeyPressEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenKeyPressEditor* NewL(TKeyPressAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenKeyPressEditor();

private: // Constructors
    CLoadGenKeyPressEditor(TKeyPressAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    
private: // Data
    TKeyPressAttributes&              iAttributes;
    };

class CLoadGenPointerEventEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenPointerEventEditor* NewL(TPointerEventAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenPointerEventEditor();

private: // Constructors
    CLoadGenPointerEventEditor(TPointerEventAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    
private: // Data
    TPointerEventAttributes&              iAttributes;
    };



class CLoadGenMessagesEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenMessagesEditor* NewL( TMessageAttributes& aAttributes, 
                                         TBool aEditingExisting = EFalse );
    virtual ~CLoadGenMessagesEditor();

private: // Constructors
    CLoadGenMessagesEditor( TMessageAttributes& aAttributes, 
                            TBool aEditingExisting = EFalse );
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL( TInt aButtonId );    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL( TInt aControlId );
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL( TBool aFormInit = EFalse );
    
private: // Data
    TMessageAttributes&              iAttributes;
    };

class CLoadGenApplicationsEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenApplicationsEditor* NewL( TApplicationsAttributes& aAttributes, 
                                         TBool aEditingExisting = EFalse );
    virtual ~CLoadGenApplicationsEditor();

private: // Constructors
    CLoadGenApplicationsEditor( TApplicationsAttributes& aAttributes, 
                            TBool aEditingExisting = EFalse );
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL( TInt aButtonId );    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL( TInt aControlId );
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL( TBool aFormInit = EFalse );
    
private: // Data
    TApplicationsAttributes&              iAttributes;
    };

class CLoadGenPhotoCaptureEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenPhotoCaptureEditor* NewL(TPhotoCaptureAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenPhotoCaptureEditor();

private: // Constructors
    CLoadGenPhotoCaptureEditor(TPhotoCaptureAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    void LoadCamerasL();
    
private: // Data
    TPhotoCaptureAttributes&    iAttributes;
    CAknQueryValueText*         iCameraQueryValText;
    CAknQueryValueTextArray*    iCameraTextArray;
    CDesCArray*                 iCamerasArray;
    };

class CLoadGenBluetoothEditor : public CLoadGenLoadTypeEditorBase
    {
public:
    static CLoadGenBluetoothEditor* NewL(TBluetoothAttributes& aAttributes, TBool aEditingExisting=EFalse);
    virtual ~CLoadGenBluetoothEditor();

private: // Constructors
    CLoadGenBluetoothEditor(TBluetoothAttributes& aAttributes, TBool aEditingExisting=EFalse);
    void ConstructL();        

protected:  // From CEikDialog
    TBool OkToExitL(TInt aButtonId);    
    void PreLayoutDynInitL();
    void HandleControlStateChangeL(TInt aControlId);
        
public:  // New methods
    virtual TBool RunQueryLD();

private:  // New methods
    void UpdateVisibilitiesOfFieldsL(TBool aFormInit=EFalse);
    
private: // Data
    TBluetoothAttributes&              iAttributes;
    };

#endif
