#include <cstring>
#include <algorithm>
#include "RelocElfReader.h"

Elf::Elf(const string &filename, ios::openmode openmode) {
    open(filename, openmode);
}

void Elf::open(const string &filename, ios::openmode openmode) {
    handle.open(filename, openmode);
    ELF_ASSERT(handle.good());
    handle.seekg(0, ios::end);
    fileSize = handle.tellg();
    ELF_ASSERT(fileSize >= sizeof(Elf32_Ehdr));
    handle.seekg(0);
    handle.read((char *)&ehdr, sizeof(Elf32_Ehdr));
    ELF_ASSERT(handle.good());
    ELF_ASSERT(memcmp(ehdr.e_ident, ELFMAG, SELFMAG) == 0);
    ELF_ASSERT(ehdr.e_ident[EI_CLASS] == ELFCLASS32);
    ELF_ASSERT(ehdr.e_ident[EI_DATA] == ELFDATA2LSB);
    ELF_ASSERT(ehdr.e_ident[EI_VERSION] == EV_CURRENT);
    ELF_ASSERT(ehdr.e_ehsize == sizeof(Elf32_Ehdr));
    ELF_ASSERT(ehdr.e_shentsize == sizeof(Elf32_Shdr) && ehdr.e_shnum && ehdr.e_shstrndx < ehdr.e_shnum);
    ELF_ASSERT(ehdr.e_shoff <= fileSize && ehdr.e_shnum * sizeof(Elf32_Shdr) <= fileSize - ehdr.e_shoff);
    shdr.resize(ehdr.e_shnum);
    handle.seekg(ehdr.e_shoff);
    handle.read((char *)shdr.data(), static_cast<streamsize>(ehdr.e_shnum * sizeof(Elf32_Shdr)));
    ELF_ASSERT(handle.good());
    ELF_ASSERT(shdr[ehdr.e_shstrndx].sh_type == SHT_STRTAB);
    shstrtab = ReadSectionData<char>(shdr[ehdr.e_shstrndx]);
    ELF_ASSERT(shstrtab && shstrtab[shdr[ehdr.e_shstrndx].sh_size - 1] == 0);
    for (const auto & sec : shdr) {
        ELF_ASSERT(sec.sh_name < shdr[ehdr.e_shstrndx].sh_size);
        if (sec.sh_type == SHT_SYMTAB) {
            ELF_ASSERT(sym.empty());
            ELF_ASSERT(sec.sh_entsize == sizeof(Elf32_Sym) && sec.sh_size % sizeof(Elf32_Sym) == 0);
            ELF_ASSERT(sec.sh_link < shdr.size() && shdr[sec.sh_link].sh_type == SHT_STRTAB);
            // LLVM may share one table for symbol names and section names.
            strtab = ReadSectionData<char>(shdr[sec.sh_link]);
            ELF_ASSERT(strtab && strtab[shdr[sec.sh_link].sh_size - 1] == 0);
            sym.resize(sec.sh_size / sizeof(Elf32_Sym));
            ReadSectionData(sec, sym.data());
            for (const auto &symbol : sym) ELF_ASSERT(symbol.st_name < shdr[sec.sh_link].sh_size);
        }
    }
    ELF_ASSERT(!sym.empty() && strtab);
}

Elf::~Elf() {
    delete[] strtab;
    delete[] shstrtab;
}

Elf32_Shdr &Elf::GetSectionHeader(const char *name) {
    auto found = find_if(shdr.begin(), shdr.end(), [=](const Elf32_Shdr &sec) {
        return strcmp(shstrtab + sec.sh_name, name) == 0;
    } );
    ELF_ASSERT(found != shdr.end());
    return *found;
}

bool Elf::HasSection(const char *name) {
    return any_of(shdr.begin(), shdr.end(), [=](const Elf32_Shdr &sec) {
        return strcmp(shstrtab + sec.sh_name, name) == 0;
    } );
}

Elf32_Sym &Elf::GetSymbol(const char *name) {
    auto found = find_if(sym.begin(), sym.end(), [=](const Elf32_Sym &symbol) {
        return strcmp(strtab + symbol.st_name, name) == 0;
    });
    ELF_ASSERT(found != sym.end());
    return *found;
}

bool Elf::HasSymbol(const char *name) {
    return any_of(sym.begin(), sym.end(), [=](const Elf32_Sym &symbol) {
        return strcmp(strtab + symbol.st_name, name) == 0;
    });
}
