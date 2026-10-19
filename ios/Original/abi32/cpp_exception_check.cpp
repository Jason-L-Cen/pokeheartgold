// SPDX-License-Identifier: GPL-3.0-or-later
#include <cstdlib>
#include <exception>
#include <new>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static unsigned failures,allocationCalls,handlerCalls,lastSize,guards,destroyed,unwinding;
static unsigned legacyControls,legacyArraysDeleted,legacyControlDeletes;
static bool failLegacyControl;
struct ControlFailure : std::bad_alloc { int marker=117; };
extern "C" unsigned CheckCppLegacyMessages(void);
extern "C" void ThrowCppLegacyControlFailure(void);
extern "C" unsigned CppLegacyControls(void) { return legacyControls; }
extern "C" unsigned CppLegacyArraysDeleted(void) { return legacyArraysDeleted; }
extern "C" unsigned CppLegacyControlDeletes(void) { return legacyControlDeletes; }
extern "C" void *CppLegacyControlNew(unsigned size) {
    if (failLegacyControl) { failLegacyControl=false; throw ControlFailure(); }
    void *p=::operator new(size); legacyControls++; return p;
}
extern "C" void CppLegacyControlDelete(void *p) { if (p) { legacyControls--; legacyControlDeletes++; } ::operator delete(p); }
extern "C" void CppLegacyArrayDelete(void *p) { if (p) legacyArraysDeleted++; ::operator delete[](p); }
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
    std::set_new_handler(nullptr);
    void *p=nullptr;
    CHECK(sizeof(std::bad_alloc)==4 && !std::uncaught_exceptions());
    CHECK(std::malloc(0)==nullptr); std::free(nullptr);
    unsigned char *bytes=static_cast<unsigned char *>(std::malloc(33)); CHECK(bytes);
    for (unsigned i=0;i<33;i++) bytes[i]=static_cast<unsigned char>(i+3);
    bytes=static_cast<unsigned char *>(std::realloc(bytes,97)); CHECK(bytes);
    for (unsigned i=0;i<33;i++) CHECK(bytes[i]==i+3);
    CHECK(std::realloc(bytes,0xFFFFFFF0u)==nullptr);
    for (unsigned i=0;i<33;i++) CHECK(bytes[i]==i+3);
    std::free(bytes);
    bytes=static_cast<unsigned char *>(std::calloc(33,3)); CHECK(bytes);
    for (unsigned i=0;i<99;i++) CHECK(!bytes[i]);
    std::free(bytes);
    void *aligned=nullptr; CHECK(!::posix_memalign(&aligned,64,128) && aligned);
    CHECK(reinterpret_cast<unsigned long>(aligned)%64==0); std::free(aligned);
    p=std::realloc(nullptr,48); CHECK(p); std::free(p);
    p=::operator new(0); CHECK(p && allocationCalls==1 && lastSize==4); ::operator delete(p);
    failures=2; allocationCalls=handlerCalls=0; std::set_new_handler(retry);
    p=::operator new(32); CHECK(p && allocationCalls==3 && handlerCalls==2); ::operator delete(p);
    std::set_new_handler(nullptr); failures=1;
    bool caught=false;
    try { p=::operator new(64); ::operator delete(p); }
    catch (const std::bad_alloc &) { caught=true; }
    CHECK(caught && !failures && !std::uncaught_exceptions());
    failures=1; CHECK(::operator new(64,std::nothrow)==nullptr && !failures);
    std::set_new_handler(throwHandler); handlerCalls=0; failures=1;
    CHECK(::operator new(64,std::nothrow)==nullptr && handlerCalls==1);
    std::set_new_handler(nullptr);
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
    CHECK(!CheckCppLegacyMessages() && !legacyControls);
    unsigned before=legacyArraysDeleted; failLegacyControl=true; caught=false;
    try { ThrowCppLegacyControlFailure(); }
    catch (const ControlFailure &failure) { caught=failure.marker==117; }
    CHECK(caught && !failLegacyControl && !legacyControls && legacyArraysDeleted==before+1);
    return 0;
}
extern "C" void ExitCppExceptionRuntime(unsigned status) { std::_Exit(static_cast<int>(status)); }
static volatile unsigned recursionResult;
extern "C" __attribute__((noinline)) void RecurseCppExceptionRuntime(unsigned depth) {
    volatile unsigned local=depth;
    if (depth) RecurseCppExceptionRuntime(depth-1);
    recursionResult=local;
}
