#ifndef __AIB_OBJECT_H__
#define __AIB_OBJECT_H__

#include "AIBTypes.h"

#include <string>
#include <mutex>

namespace aib
{
    /**
     * @brief AIBOBject: The base class of AIB class
     */
    class AIBObject
    {
        public:
            AIBObject(){}
            virtual ~AIBObject() {}

            inline std::string     errstr() { return m_errstr; }

        protected:
            void            setErrorString(const std::string &error_string) 
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_errstr=error_string; 
            }

        private:
            std::mutex      m_mutex;
            std::string     m_errstr;
    };

    /**
     * @brief AIBCopyable: Classes that inherit from that class can be copied
     */
    class AIBCopyable
    {
        protected:
            AIBCopyable()=default;
            virtual ~AIBCopyable()=default;
    };

    /**
     * @brief AIBNoncopyable: Classes that inherit from this class cannot be copied
     */
    class AIBNoncopyable
    {
        protected:
            AIBNoncopyable()=default;
            virtual ~AIBNoncopyable()=default;

        public:
            AIBNoncopyable(const AIBNoncopyable &)=delete;
            void operator=(const AIBNoncopyable &)=delete;
    };
}


#endif // __AIB_OBJECT_H__