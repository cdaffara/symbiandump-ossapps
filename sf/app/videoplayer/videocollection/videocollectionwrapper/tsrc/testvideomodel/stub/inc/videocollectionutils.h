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
* Description:   CVideoCollectionUtils class definition*
*/

#ifndef __VIDEOCOLLECTIONUTILS_H__
#define __VIDEOCOLLECTIONUTILS_H__


// INCLUDES
#include <QObject>
#include <QString>
#include <QStringList>
#include <mpxmedia.h>

class VideoCollectionUtils
{    
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoCollectionUtils) 

public: 

    
    /**
     * Returns singleton instance for this class.
     * 
     * @return The singleton instance.
     */
    static VideoCollectionUtils& instance();
    
    /**
     * Gets QString value from provided media -object based on attribute provided
     * 
     * @param CMPXMedia* pointer to media object from where to get data
     * @param TMPXAttributeData& attribute to look for
     * @param QString& wanted value
     * 
     * @return bool true data value gotten ok
     */
    template<class T>
    bool mediaValue(const CMPXMedia *media, const TMPXAttributeData& attribute, QString& result)
    {
        bool status = false;
        if( media && media->IsSupported(attribute))
        {
            HBufC* valueText = NULL;        
            valueText = media->ValueText( attribute ).Alloc();
            if (valueText && valueText->Length() > 0)
            {
                // temp QString needed to make sure string is initialized correctly (Error  #424)
                QString text((QChar*)valueText->Des().Ptr(),valueText->Length());
                result = text;
                status = true;
            }
            delete valueText;
        }
        return status;
    }
    
    /**
     * Gets typed value from provided media -object based on attribute provided
     * 
     * @param CMPXMedia* pointer to media object from where to get data
     * @param TMPXAttributeData& attribute to look for
     * @param T& wanted value
     * 
     * @return bool true data value gotten ok
     */
    template<class T>
    bool mediaValue(const CMPXMedia *media, const TMPXAttributeData& attribute, T& result)
    {
        bool status = false;
        if(media && media->IsSupported(attribute))
        {
            T *pointer = 0;
            pointer = media->Value<T>(attribute);
            if(pointer)
            {
                result = *pointer;
                status = true;
            }
        }          
        return status;
    }
    
    /**
     * Gets typed pointer from provided media -object based on attribute provided
     * 
     * @param CMPXMedia* pointer to media object from where to get data
     * @param TMPXAttributeData& attribute to look for
     * 
     * @return T* pointer to wanted data (NULL if does not succeed)
     */
    template<class T>
    T* mediaValuePtr(const CMPXMedia *media, const TMPXAttributeData& attribute)
    {
        T *pointer = 0;
        if(media && media->IsSupported(attribute))
        {
            pointer = media->Value<T>(attribute);
        }          
        return pointer;
    }
    
    /**
     * Prepares length string from given value.
     * 
     * @param length Length value in seconds.
     * @return Length as string (for example "1min 45sec").
     */
    QString prepareLengthString(quint32 length);
    
    /**
     * Prepares size string from the given value.
     * 
     * @param size Size value in bytes
     * @return Size as string (for example "450 kB").
     */
    QString prepareSizeString(quint32 size);

public:
    static QString mPrepareLengthStringReturnValue;
    static QString mPrepareSizeStringReturnValue;

private: 

	/**
	 * Default constructor
     */
    VideoCollectionUtils();	
	
	/**
     * Destructor
     */
	~VideoCollectionUtils();	
	
 
    
};
#endif  // __VIDEOCOLLECTIONUTILS_H__

// End of file
    


