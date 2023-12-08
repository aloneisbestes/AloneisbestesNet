#ifndef __AIB_EXCEPTION_H__
#define __AIB_EXCEPTION_H__

#include <string>
#include <cstdarg>
#include <cstring>
#include <exception>

namespace aib
{
    namespace aibexce
    {
        class AIBException : public std::exception
        {
            public:
                AIBException(const char *fmt, ...)
                {
                    std::memset(m_buffer, sizeof(m_buffer), 0);
                    va_list args;
                    va_start(args, fmt);
                    vsnprintf(m_buffer, sizeof(m_buffer), fmt, args);
                    va_end(args);
                }

                const char* what() const noexcept override {
                    return m_buffer;
                }

                AIBException(const AIBException &other) 
                {
                    std::memcpy(m_buffer, other.m_buffer, 10240);
                }

                void operator=(const AIBException &other)
                {
                    std::memcpy(m_buffer, other.m_buffer, 10240);
                }

            private:
                char m_buffer[10241];
        };
    }
}

#endif // __AIB_EXCEPTION_H__