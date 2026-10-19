// SPDX-License-Identifier: GPL-3.0-or-later
#include <cstdlib>
#include <exception>
#include <new>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
namespace std { new_handler __new_handler=nullptr; }
static unsigned failures,allocationCalls,handlerCalls,lastSize,guards,destroyed,unwinding;
extern "C" void *CppExceptionCheckMalloc(unsigned long size) {
    allocationCalls++; lastSize=size;
    if (failures) { failures--; return nullptr; }
    return std::malloc(size);
}
static void retry(void) { handlerCalls++; }
static void throwHandler(void) { handlerCalls++; throw std::bad_alloc(); }
struct Left { virtual ~Left() {} int left=11; };
struct Right { virtual ~Right() {} int right=22; };
struct Derived : Left,Right { int value=37; ~Derived() { destroyed++; } };
struct Guard { ~Guard() { guards++; unwinding+=std::uncaught_exceptions()>0; } };
extern "C" void CppExceptionCFrame(void (*function)(void));
static void throwObject(void) { Guard guard; throw Derived(); }
extern "C" unsigned CheckCppExceptionRuntime(void) {
    failures=allocationCalls=handlerCalls=lastSize=guards=destroyed=unwinding=0;
    std::__new_handler=nullptr;
    CHECK(sizeof(std::bad_alloc)==4 && !std::uncaught_exceptions());
    void *p=::operator new(0); CHECK(p && allocationCalls==1 && lastSize==4); ::operator delete(p);
    failures=2; allocationCalls=handlerCalls=0; std::__new_handler=retry;
    p=::operator new(32); CHECK(p && allocationCalls==3 && handlerCalls==2); ::operator delete(p);
    std::__new_handler=nullptr; failures=1;
    bool caught=false;
    try { p=::operator new(64); ::operator delete(p); }
    catch (const std::bad_alloc &) { caught=true; }
    CHECK(caught && !failures && !std::uncaught_exceptions());
    failures=1; CHECK(::operator new(64,std::nothrow)==nullptr && !failures);
    std::__new_handler=throwHandler; handlerCalls=0; failures=1;
    CHECK(::operator new(64,std::nothrow)==nullptr && handlerCalls==1);
    std::__new_handler=nullptr;
    std::exception_ptr held;
    try { CppExceptionCFrame(throwObject); }
    catch (Right &value) { CHECK(value.right==22); held=std::current_exception(); }
    CHECK(held && guards==1 && unwinding==1 && !destroyed && !std::uncaught_exceptions());
    caught=false;
    try { std::rethrow_exception(held); }
    catch (const Derived &value) { CHECK(value.left==11 && value.right==22 && value.value==37); caught=true; }
    CHECK(caught && !destroyed); held=nullptr; CHECK(destroyed==1);
    caught=false;
    try { try { throw 43; } catch (int) { throw; } }
    catch (int value) { caught=value==43; }
    CHECK(caught && !std::uncaught_exceptions());
    CHECK(std::getenv("HOME")==nullptr);
    return 0;
}
extern "C" void ExitCppExceptionRuntime(unsigned status) { std::_Exit(static_cast<int>(status)); }
static volatile unsigned recursionResult;
extern "C" __attribute__((noinline)) void RecurseCppExceptionRuntime(unsigned depth) {
    volatile unsigned local=depth;
    if (depth) RecurseCppExceptionRuntime(depth-1);
    recursionResult=local;
}
