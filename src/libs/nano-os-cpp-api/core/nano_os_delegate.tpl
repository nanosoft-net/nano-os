/*
Copyright(c) 2017 Cedric Jimenez

This file is part of Nano-OS.

Nano-OS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nano-OS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Nano-OS.  If not, see <http://www.gnu.org/licenses/>.
*/


/** \brief Macro to join 2 tokens into 1 */
#define JOIN(a, b) DO_JOIN1(a, b)
#define DO_JOIN1(a, b) DO_JOIN2(a, b)
#define DO_JOIN2(a, b) a##b


/** \brief Macro to create the class name */
#define DELEGATE_CLASS_NAME JOIN(NanoOsDelegate, DELEGATE_PARAM_COUNT)


/** \brief Class representing a delegate = generic method pointer */
template <typename ReturnType DELEGATE_PARAM_SEP DELEGATE_TEMPLATE_PARAM_DEF_LIST>
class DELEGATE_CLASS_NAME
{
    public:
    
        /** \brief Constructor */
        DELEGATE_CLASS_NAME()
        : m_obj(NULL)
        , m_method(NULL)
        {}

        /** \brief Create a delegate */
        template <typename ObjType, ReturnType(ObjType::*MethodType)(DELEGATE_TEMPLATE_PARAM_TYPES_LIST)>
        static DELEGATE_CLASS_NAME create(ObjType& obj)
        {
            DELEGATE_CLASS_NAME deleg(&obj, &callMethod<ObjType, MethodType>);
            return deleg;
        }

        /** \brief Invoke the pointed method */
        ReturnType invoke(DELEGATE_METHOD_PARAM_TYPES_LIST) const
        {
            return (*m_method)(m_obj DELEGATE_PARAM_SEP DELEGATE_METHOD_PARAM_LIST);
        }
        
        /** \brief Invoke the pointed method */
        ReturnType operator() (DELEGATE_METHOD_PARAM_TYPES_LIST) const
        {
            return (*m_method)(m_obj DELEGATE_PARAM_SEP DELEGATE_METHOD_PARAM_LIST);
        }

        /** \brief Comparison operator */
        bool operator == (DELEGATE_CLASS_NAME<ReturnType DELEGATE_PARAM_SEP DELEGATE_TEMPLATE_PARAM_TYPES_LIST>& compareDelegate) const
        {
            const bool ret = ((m_obj == compareDelegate.m_obj) && (m_method == compareDelegate.m_method));
            return ret;
        }

        /** \brief Comparison operator */
        bool operator != (DELEGATE_CLASS_NAME<ReturnType DELEGATE_PARAM_SEP DELEGATE_TEMPLATE_PARAM_TYPES_LIST>& compareDelegate) const
        {
            const bool ret = !((m_obj == compareDelegate.m_obj) && (m_method == compareDelegate.m_method));
            return ret;
        }
        
        /** \brief Assignment operator */
        DELEGATE_CLASS_NAME<ReturnType DELEGATE_PARAM_SEP DELEGATE_TEMPLATE_PARAM_TYPES_LIST>& operator = (const DELEGATE_CLASS_NAME<ReturnType DELEGATE_PARAM_SEP DELEGATE_TEMPLATE_PARAM_TYPES_LIST>& copy)
        {
            m_obj = copy.m_obj;
            m_method = copy.m_method;
			return (*this);
        }

		/** \brief Indicate if the delegate points to a valid member function or object */
		bool isNull() const
		{
			return ((m_obj == NULL) || (m_method == NULL));
		}

		/** \brief Make the delagate invalid */
		void nullify()
		{
			m_obj = NULL;
			m_method = NULL;
		}

    private:

        /** \brief Caller method type */
        typedef ReturnType(*fpCall)(void* const obj DELEGATE_PARAM_SEP DELEGATE_METHOD_PARAM_TYPES_LIST);

        /** \brief Object to call */
        void* m_obj;

        /** \brief Method to call */
        fpCall m_method;


        /** \brief Constructor */
        DELEGATE_CLASS_NAME(void* const obj, const fpCall method)
        : m_obj(obj)
        , m_method(method)
        {}

        /** \brief Caller method */
        template <typename ObjType, ReturnType(ObjType::*MethodType)(DELEGATE_TEMPLATE_PARAM_TYPES_LIST)>
        static ReturnType callMethod(void* const obj DELEGATE_PARAM_SEP DELEGATE_METHOD_PARAM_TYPES_LIST)
        {
            ObjType* const obj_call = static_cast<ObjType*>(obj);
            return (obj_call->*MethodType)(DELEGATE_METHOD_PARAM_LIST);
        }

};


