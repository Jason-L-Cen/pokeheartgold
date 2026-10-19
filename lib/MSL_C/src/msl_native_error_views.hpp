// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_MSL_NATIVE_ERROR_VIEWS_H
#define HEARTGOLD_MSL_NATIVE_ERROR_VIEWS_H
#include <nitro/types.h>
extern "C" {
#include "msl_shared_ptr_internal.h"
unsigned long strlen(const char *) throw();
char *strcpy(char *,const char *) throw();
}
namespace std {
class exception {
public:
    exception() throw() {}
    virtual ~exception() throw();
    virtual const char *what() const throw();
};
namespace detail { template<class T> struct default_delete {}; }
namespace tr1 {
template<class T> class shared_ptr {
    T *pointer; void *control;
public:
    template<class Y,class D> shared_ptr(Y *p,D d);
    shared_ptr(const shared_ptr &other) : pointer(other.pointer),control(other.control) {
        if (control) static_cast<SharedPtrDeleter *>(control)->references++;
    }
    shared_ptr &operator=(const shared_ptr &other) {
        if (this!=&other) {
            if (other.control) static_cast<SharedPtrDeleter *>(other.control)->references++;
            _ZNSt3tr110shared_ptrIcED1Ev(this);
            pointer=other.pointer; control=other.control;
        }
        return *this;
    }
    ~shared_ptr() throw();
    T *get() const { return pointer; }
};
template<> template<> inline shared_ptr<char>::shared_ptr(char *p,::std::detail::default_delete<char[]> d) {
    ::DefaultDelete deleter={0};
    try { _ZNSt3tr110shared_ptrIcEC1IcNSt6detail14default_deleteIA0_cEEEEPT_T0_(reinterpret_cast<SharedPtr *>(this),p,deleter); }
    catch (...) { _ZdaPv(p); throw; }
}
}
}
namespace HGNativeMSL {
class error_string {
    std::tr1::shared_ptr<char> str;
public:
    explicit error_string(const char *s) : str(new char[strlen(s)+1],std::detail::default_delete<char[]>()) { strcpy(str.get(),s); }
    const char *get() const { return str.get(); }
};
class logic_error : public std::exception {
    error_string message;
public:
    explicit logic_error(const char *s) : message(s) {}
    virtual ~logic_error() throw() {}
    virtual const char *what() const throw() { return message.get(); }
};
class length_error : public logic_error {
public:
    explicit length_error(const char *s) : logic_error(s) {}
    virtual ~length_error() throw() {}
};
}
#endif
