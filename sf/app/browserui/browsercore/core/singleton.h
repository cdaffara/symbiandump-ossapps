/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*  This defines both Meyers Singleton and Gamma Singleton template.
*/


#ifndef __SINGLETON_H
#define __SINGLETON_H

/* 
 * This is how a Gamma Singleton would instantiate its object.
 */
template <class T> struct CreateGamma {
    static T* Create() { return new T; }
};

/* 
 * This is how a Meyers Singleton would instantiate its object.
 */
template <class T> struct CreateMeyers {
    static T* Create() {
        static T _instance;
        return &_instance;
    }
};

/*
 * This Singleton class accepts different creation policies.
 */
template <class T, template<class> class CreationPolicy=CreateMeyers>
class Singleton {
public:
    static T& Instance() {
        if (!m_pInstance)
            m_pInstance=CreationPolicy<T>::Create();
        return *m_pInstance;
}

private:
    Singleton();           // hide constrcutor
    ~Singleton();          // hide desctructor
    Singleton(Singleton const&);    // hide copy constructor
    Singleton& operator=(Singleton const&);  // hide assignment operator

    static T* m_pInstance;
};

template <class T, template<class> class C>
T* Singleton<T,C>::m_pInstance=0;

#endif // __SINGLETON_H