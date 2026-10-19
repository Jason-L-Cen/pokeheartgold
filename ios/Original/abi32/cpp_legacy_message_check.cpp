// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../lib/MSL_C/src/msl_vector_length_error.cpp"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern "C" unsigned CppLegacyControls(void);
extern "C" unsigned CppLegacyArraysDeleted(void);
extern "C" unsigned CppLegacyControlDeletes(void);
extern "C" unsigned CheckCppLegacyMessages(void) {
    CHECK(sizeof(std::tr1::shared_ptr<char>)==8 && sizeof(HGNativeMSL::logic_error)==12 && sizeof(HGNativeMSL::length_error)==12);
    unsigned arrays=CppLegacyArraysDeleted(),controls=CppLegacyControlDeletes();
    {
        HGNativeMSL::length_error first("original");
        SharedPtr *message=reinterpret_cast<SharedPtr *>(reinterpret_cast<unsigned char *>(&first)+4);
        CHECK(message->control->references==1 && CppLegacyControls()==1);
        HGNativeMSL::length_error second(first);
        CHECK(message->control->references==2 && CppLegacyControls()==1);
        HGNativeMSL::length_error third("replace");
        third=first; first=first;
        CHECK(message->control->references==3 && CppLegacyControls()==1);
        CHECK(!__builtin_strcmp(first.what(),"original") && !__builtin_strcmp(second.what(),"original") && !__builtin_strcmp(third.what(),"original"));
    }
    CHECK(!CppLegacyControls() && CppLegacyArraysDeleted()==arrays+2 && CppLegacyControlDeletes()==controls+2);
    bool caught=false;
    try { std::__vector_common<true>::throw_length_error(); }
    catch (const std::exception &error) { caught=!__builtin_strcmp(error.what(),"vector length error"); }
    CHECK(caught && !CppLegacyControls());
    caught=false;
    try { std::__vector_common<true>::throw_length_error(); }
    catch (HGNativeMSL::length_error error) { caught=!__builtin_strcmp(error.what(),"vector length error"); }
    CHECK(caught && !CppLegacyControls());
    return 0;
}
extern "C" void ThrowCppLegacyControlFailure(void) { HGNativeMSL::length_error error("rollback"); __builtin_trap(); }
