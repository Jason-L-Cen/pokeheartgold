// SPDX-License-Identifier: GPL-3.0-or-later
// Isolate constructor/lifetime ABI from the unsupported exception-throw path.
#define throw_length_error SharedPtrCheckUnusedThrow
#include "../../../lib/MSL_C/src/msl_vector_length_error.cpp"
#undef throw_length_error
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 controls,payloads,deletedControls,deletedArrays;
extern "C" {
void *SharedPtrCheckNew(u32 size) { void *p=malloc(size); if (!p) __builtin_trap(); controls++; return p; }
void SharedPtrCheckDelete(void *p) { if (p) { controls--; deletedControls++; free(p); } }
void SharedPtrCheckArrayDelete(void *p) { if (p) { payloads--; deletedArrays++; free(p); } }
SharedPtrVTable SharedPtrCheckBaseVTable={0};
SharedPtrVTable SharedPtrCheckVTable={
    _ZNSt3tr16detail18shared_ptr_deleterIcNSt6detail14default_deleteIA0_cEEED1Ev,
    _ZNSt3tr16detail18shared_ptr_deleterIcNSt6detail14default_deleteIA0_cEEED0Ev,
    NULL,_ZNSt3tr16detail18shared_ptr_deleterIcNSt6detail14default_deleteIA0_cEEE7disposeEv
};
u32 CheckOriginalSharedPtr(void) {
    CHECK(sizeof(SharedPtr)==8 && sizeof(SharedPtrDeleter)==16 && sizeof(SharedPtrVTable)==16);
    CHECK(sizeof(std::tr1::shared_ptr<char>)==8 && !controls && !payloads);
    u32 beforeControls=deletedControls,beforeArrays=deletedArrays;
    char *text=(char *)malloc(16); CHECK(text); payloads++; strcpy(text,"original");
    {
        std::tr1::shared_ptr<char> owned(text,std::detail::default_delete<char[]>());
        SharedPtr *raw=reinterpret_cast<SharedPtr *>(&owned);
        CHECK(owned.get()==text && !strcmp(owned.get(),"original") && controls==1 && payloads==1);
        CHECK(raw->control->references==1 && raw->control->weakReferences==1);
        SharedPtr alias=*raw; raw->control->references++;
        _ZNSt3tr110shared_ptrIcED1Ev(&alias);
        CHECK(raw->control->references==1 && controls==1 && payloads==1);
    }
    CHECK(!controls && !payloads && deletedControls==beforeControls+1 && deletedArrays==beforeArrays+1);
    SharedPtrDeleter *weak;
    text=(char *)malloc(16); CHECK(text); payloads++;
    {
        std::tr1::shared_ptr<char> owned(text,std::detail::default_delete<char[]>());
        weak=reinterpret_cast<SharedPtr *>(&owned)->control; weak->weakReferences++;
    }
    CHECK(!payloads && controls==1 && !weak->references && weak->weakReferences==1);
    _ZNSt3tr16detail25shared_ptr_deleter_common12release_weakEv(weak);
    CHECK(!controls && deletedControls==beforeControls+2 && deletedArrays==beforeArrays+2);
    { std::tr1::shared_ptr<char> empty((char *)NULL,std::detail::default_delete<char[]>()); CHECK(!empty.get()); }
    CHECK(!controls && !payloads && deletedControls==beforeControls+3 && deletedArrays==beforeArrays+2);
    return 0;
}
}
