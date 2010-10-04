/*
   Source for x86 emulator IdaPro plugin
   Copyright (c) 2005, 2006 Chris Eagle
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option) 
   any later version.
   
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
   more details.
   
   You should have received a copy of the GNU General Public License along with 
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
   Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef USE_DANGEROUS_FUNCTIONS
#define USE_DANGEROUS_FUNCTIONS 1
#endif

#ifndef USE_STANDARD_FILE_FUNCTIONS
#define USE_STANDARD_FILE_FUNCTIONS 1
#endif

#include <windows.h>
#include <winnt.h>

#ifdef PACKED
#undef PACKED
#endif

#include <pro.h>
#include <kernwin.hpp>
#include <segment.hpp>
#include <bytes.hpp>
#include <typeinf.hpp>
#include "peutils.h"
#include "sdk_versions.h"

extern til_t *ti;

static char *stringFromFile(FILE *f) {
   char *n = NULL;
   char *p = NULL;
   unsigned int len = 0;
   while (1) {
      p = n;
      n = (char*)realloc(n, len + 1);
      if (n == NULL) {
         free(p);
         return NULL;
      }
      if ((n[len] = fgetc(f)) == EOF) {
         free(n);
         return NULL;
      }
      len++;
      if (n[len - 1] == 0) break;
   }
   return n;
}

void applyPEHeaderTemplates(unsigned int mz_addr) {
#if (IDA_SDK_VERSION < 520)
   tid_t idh = til2idb(-1, "IMAGE_DOS_HEADER");
   tid_t inth = til2idb(-1, "IMAGE_NT_HEADERS");
   tid_t ish = til2idb(-1, "IMAGE_SECTION_HEADER");
#else
   tid_t idh = import_type(ti, -1, "IMAGE_DOS_HEADER");
   tid_t inth = import_type(ti, -1, "IMAGE_NT_HEADERS");
   tid_t ish = import_type(ti, -1, "IMAGE_SECTION_HEADER");
#endif
   
   doStruct(mz_addr, sizeof(IMAGE_DOS_HEADER), idh);
   unsigned short e_lfanew = get_word(mz_addr + 0x3C);
   mz_addr += e_lfanew;

   if (doStruct(mz_addr, sizeof(IMAGE_NT_HEADERS), inth) == 0) {
      do_unknown(mz_addr, 0);
      set_cmt(mz_addr - e_lfanew, "!!Warning, MZ Header overlaps PE header!!", 0);
      doStruct(mz_addr, sizeof(IMAGE_NT_HEADERS), inth);
   }

   unsigned short num_sects = get_word(mz_addr + 6);

   mz_addr += sizeof(IMAGE_NT_HEADERS);
   
   for (unsigned short i = 0; i < num_sects; i++) {
      doStruct(mz_addr + i * sizeof(IMAGE_SECTION_HEADER), sizeof(IMAGE_SECTION_HEADER), ish);
   }
}

void createSegment(unsigned int start, unsigned int size, unsigned char *content) {
   segment_t s;
   //create ida segment to hold headers
   memset(&s, 0, sizeof(s));
   s.startEA = start;
   s.endEA = start + size;
   s.align = saRelPara;
   s.comb = scPub;
   s.perm = SEGPERM_WRITE | SEGPERM_READ;
   s.bitness = 1;
   s.type = SEG_DATA;
   s.color = DEFCOLOR;
   if (add_segm_ex(&s, NULL, "DATA", ADDSEG_QUIET | ADDSEG_NOSREG)) {
      //zero out the newly created segment
      for (ea_t ea = s.startEA; ea < s.endEA; ea++) {
         patch_byte(ea, 0);
      }
      if (content) {
         patch_many_bytes(s.startEA, content, size);
      }
//      msg("segment created %x-%x\n", s.startEA, s.endEA);
   }
   else {
//      msg("seg create failed\n");
   }
}

PETables::PETables() {
   valid = 0;
   base = 0;
   nt = (IMAGE_NT_HEADERS*)malloc(sizeof(IMAGE_NT_HEADERS));
   sections = NULL;
   num_sections = 0;
   imports = NULL;
}

PETables::~PETables() {
   destroy();
}

static unsigned int rvaToFileOffset(_IMAGE_SECTION_HEADER *sect, unsigned int n_sect, unsigned int rva) {
   unsigned int minOffset = 0xFFFFFFFF;
   for (unsigned int i = 0; i < n_sect; i++) {
      unsigned int sectOffset = sect[i].PointerToRawData;
      if (sectOffset && (sectOffset < minOffset)) {
         minOffset = sectOffset;
      }
      unsigned int max = sect[i].VirtualAddress + sect[i].Misc.VirtualSize;
      if (rva >= sect[i].VirtualAddress && rva < max) {
         return sect[i].PointerToRawData + (rva - sect[i].VirtualAddress);
      }
   }
   if (rva < minOffset) {
      return rva;
   }
   return 0xFFFFFFFF;
}

unsigned int PETables::rvaToFileOffset(unsigned int rva) {
   return ::rvaToFileOffset(sections, num_sections, rva);
/*
   int i;
   if (valid) {
      return 
      for (i = 0; i < num_sections; i++) {
         unsigned int max = sections[i].VirtualAddress + sections[i].Misc.VirtualSize;
         if (rva >= sections[i].VirtualAddress && rva < max) {
            return sections[i].PointerToRawData + (rva - sections[i].VirtualAddress);
         }
      }
   }
   return 0xFFFFFFFF;
*/
}

void PETables::setNtHeaders(IMAGE_NT_HEADERS *inth) {
   memcpy(nt, inth, sizeof(IMAGE_NT_HEADERS));
   base = nt->OptionalHeader.ImageBase;
}

void PETables::setSectionHeaders(unsigned int nsecs, IMAGE_SECTION_HEADER *ish) {
   num_sections = nsecs;
   sections = (IMAGE_SECTION_HEADER*)malloc(num_sections * sizeof(IMAGE_SECTION_HEADER));
   if (sections == NULL) return;
   memcpy(sections, ish, num_sections * sizeof(IMAGE_SECTION_HEADER));
   //bss type segments are zero filled by operating system loader
   for (unsigned short i = 0; i < num_sections; i++) {
      if (sections[i].SizeOfRawData < sections[i].Misc.VirtualSize) {
         ea_t sbase = sections[i].VirtualAddress + base;
         segment_t *seg = getseg(sbase);
         if (seg) {
            ea_t ea;
            for (ea = seg->startEA + sections[i].SizeOfRawData; ea < (seg->endEA - 3); ea += 4) {
               patch_long(ea, 0);
            }
            while (ea < seg->endEA) {
               patch_byte(ea++, 0);
            }
         }
      }
   }
   valid = 1;
}

void PETables::buildThunks(FILE *f) {
   unsigned int import_rva, min_rva = 0xFFFFFFFF, max_rva = 0;
   unsigned int min_iat = 0xFFFFFFFF, max_iat = 0;
   IMAGE_IMPORT_DESCRIPTOR desc;

   import_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
   if (import_rva) {
      msg("import_rva = %x, image_base = %x\n", import_rva, nt->OptionalHeader.ImageBase);
      import_rva = rvaToFileOffset(import_rva);
      imports = NULL;

      while (1) {
//         msg("iat seeking to %x\n", import_rva);
         if (fseek(f, import_rva, SEEK_SET)) {
//            msg("Could not seek to import table: %x\n", import_rva);
            destroy();
            return;
         }
         if (fread(&desc, sizeof(desc), 1, f) != 1)  {
//            msg("Failed to read import table\n");
            destroy();
            return;
         }
         unsigned int iat_base = desc.FirstThunk;
//         msg("iat_base = %x\n", iat_base);
         if (iat_base == 0) break;   //end of import descriptor array
         unsigned int name_table = desc.OriginalFirstThunk;
         unsigned int name = desc.Name;  //rva of dll name string
         unsigned int iat = rvaToFileOffset(iat_base);
         if (name_table == 0) {
            name_table = iat;
         }
         else {
            name_table = rvaToFileOffset(name_table);
         }

         import_rva = ftell(f);
         name = rvaToFileOffset(name);
         thunk_rec *tr = (thunk_rec*)calloc(1, sizeof(thunk_rec));
         if (tr == NULL)  {
//            msg("Failed to alloc thunk record\n");
            destroy();
            return;
         }
         tr->iat_base = iat_base;
         if (iat_base < min_iat) min_iat = iat_base;
         
         tr->next = imports;
         imports = tr;
         if (fseek(f, name, SEEK_SET))  {
//            msg("Could not seek to name %x\n", name);
            destroy();
            return;
         }
         tr->dll_name = stringFromFile(f);
         if (tr->dll_name == NULL) {
//            msg("dll_name was null\n");
            destroy();
            return;
         }
//         msg("thunk dll: %s\n", tr->dll_name);
         if (fseek(f, name_table, SEEK_SET)) {
//         if (fseek(f, iat, SEEK_SET)) {
            msg("Could not seek to iat\n");
            destroy();
            return;
         }
         if (desc.Name < min_rva) min_rva = desc.Name;
         if (desc.Name > max_rva) max_rva = desc.Name + strlen(tr->dll_name) + 1;
         while (1) {
            tr->iat = (unsigned int*)realloc(tr->iat, (tr->iat_size + 1) * sizeof(unsigned int));
            if (tr->iat == NULL) {
               msg("failed to realloc iat\n");
               destroy();
               return;
            }
            if (fread(&tr->iat[tr->iat_size], sizeof(unsigned int), 1, f) != 1) {
               msg("Failed to read iat\n");
               destroy();
               return;
            }
            tr->iat_size++;
            if (tr->iat[tr->iat_size - 1] == 0) break;
         }
         unsigned int end_iat = iat_base + 4 * tr->iat_size;
         if (end_iat > max_iat) max_iat = end_iat;
         
//         tr->names = (char**)calloc(tr->iat_size, sizeof(char*));
         for (int i = 0; tr->iat[i]; i++) {
            unsigned int name_rva = tr->iat[i];
            if (name_rva & 0x80000000) continue;  //import by ordinal
            if (fseek(f, rvaToFileOffset(name_rva + 2), SEEK_SET)) {
               msg("Could not seek to name_rva (by ordinal)\n");
               destroy();
               return;
            }
//            tr->names[i] = stringFromFile(f);
            char *n = stringFromFile(f);
            msg("read import name %s\n", n);
            if (name_rva < min_rva) min_rva = name_rva;
            if (name_rva > max_rva) max_rva = name_rva + strlen(n) + 1;
            free(n);
         }
      }
      if (isEnabled(base + min_rva) && isEnabled(base + max_rva - 1)) {
      }
      else {
         unsigned int sz = max_rva - min_rva + 2;
         unsigned char *strtable = (unsigned char *)malloc(sz);
         if (fseek(f, rvaToFileOffset(min_rva), SEEK_SET)) {
            free(strtable);
//            destroy();
            return;
         }
         if (fread(strtable, sz, 1, f) != 1)  {
            free(strtable);
//            destroy();
            return;
         }
         createSegment(base + min_rva, sz, strtable);
         free(strtable);
      }
      // Make sure there is a segment to hold the import table
      if (!isEnabled(base + min_iat) && !isEnabled(base + max_iat - 1)) {
         createSegment(base + min_iat, max_iat - min_iat, NULL);
      }
   }
}

void PETables::loadTables(Buffer &b) {
   b.read(&valid, sizeof(valid));
   if (b.has_error()) {
      valid = 0;
      return;
   }
   b.read(&base, sizeof(base));
   if (b.has_error()) {
      valid = 0;
      return;
   }
   b.read(&num_sections, sizeof(num_sections));
   if (b.has_error()) {
      valid = 0;
      return;
   }
   b.read(nt, sizeof(IMAGE_NT_HEADERS));
   if (b.has_error()) {
      valid = 0;
      return;
   }
   sections = (IMAGE_SECTION_HEADER*)malloc(num_sections * sizeof(IMAGE_SECTION_HEADER));
   b.read(sections, sizeof(IMAGE_SECTION_HEADER) * num_sections);
   if (b.has_error()) {
      valid = 0;
      return;
   }
   unsigned int num_imports;
   b.read(&num_imports, sizeof(num_imports));
   if (b.has_error()) {
      valid = 0;
      return;
   }
   thunk_rec *p = NULL, *n = NULL;
   for (unsigned int i = 0; i < num_imports; i++) {
      p = (thunk_rec*)malloc(sizeof(thunk_rec));
      p->next = n;
      n = p;
      b.readString(&p->dll_name);
      if (b.has_error()) {
         valid = 0;
         return;
      }
      b.read(&p->iat_base, sizeof(p->iat_base));
      if (b.has_error()) {
         valid = 0;
         return;
      }
      b.read(&p->iat_size, sizeof(p->iat_size));
      if (b.has_error()) {
         valid = 0;
         return;
      }
      p->iat = (unsigned int*) malloc(p->iat_size * sizeof(unsigned int));
      b.read(p->iat, p->iat_size * sizeof(unsigned int));
      if (b.has_error()) {
         valid = 0;
         return;
      }
/*
      p->names = (char**)calloc(p->iat_size, sizeof(char*));
      for (int i = 0; p->iat[i]; i++) {
         if (p->iat[i] & 0x80000000) continue;  //import by ordinal
         b.readString(&p->names[i]);
      }
*/
   }
   imports = p;
}

void PETables::saveTables(Buffer &b) {
   b.write(&valid, sizeof(valid));
   b.write(&base, sizeof(base));
   b.write(&num_sections, sizeof(num_sections));
   b.write(nt, sizeof(IMAGE_NT_HEADERS));
   b.write(sections, sizeof(IMAGE_SECTION_HEADER) * num_sections);
   unsigned int num_imports = 0;
   thunk_rec *p;
   for (p = imports; p; p = p->next) num_imports++;
   b.write(&num_imports, sizeof(num_imports));
   for (p = imports; p; p = p->next) {
      b.writeString(p->dll_name);
      b.write(&p->iat_base, sizeof(p->iat_base));
      b.write(&p->iat_size, sizeof(p->iat_size));
      b.write(p->iat, p->iat_size * sizeof(unsigned int));
/*
      for (int i = 0; p->iat[i]; i++) {
         if (p->iat[i] & 0x80000000) continue;  //import by ordinal
         b.writeString(p->names[i]);
      }
*/
   }
}

void PETables::destroy() {
   free(sections);
   free(nt);
   thunk_rec *p;
   for (p = imports; p; p = imports) {
      imports = imports->next;
      free(p->dll_name);
      free(p->iat);
/*
      for (unsigned int i = 0; i < p->iat_size; i++) {
         free(p->names[i]);
      }

      free(p->names);
*/
      free(p);
   }
   valid = 0;
}

unsigned int loadIntoIdb(FILE *dll) {
   _IMAGE_DOS_HEADER dos, *pdos;
   _IMAGE_NT_HEADERS nt, *pnt;
   _IMAGE_SECTION_HEADER sect, *psect;
   unsigned int exp_size, exp_rva, exp_fileoff;
   _IMAGE_EXPORT_DIRECTORY *expdir;
   unsigned int len, handle;
   
   if (fread(&dos, sizeof(_IMAGE_DOS_HEADER), 1, dll) != 1) {
      return 0xFFFFFFFF;
   }
   if (dos.e_magic != 0x5A4D || fseek(dll, dos.e_lfanew, SEEK_SET)) {
      return 0xFFFFFFFF;
   }
   if (fread(&nt, sizeof(_IMAGE_NT_HEADERS), 1, dll) != 1) {
      return 0xFFFFFFFF;
   }
   if (nt.Signature != 0x4550) {
      return 0xFFFFFFFF;
   }
   if (fread(&sect, sizeof(_IMAGE_SECTION_HEADER), 1, dll) != 1) {
      return 0xFFFFFFFF;
   }
   //read all header bytes into buff
   len = sect.PointerToRawData;
   unsigned char *dat = (unsigned char*)malloc(len);
   if (dat == NULL || fseek(dll, 0, SEEK_SET) || fread(dat, len, 1, dll) != 1) {
      free(dat);
      return 0xFFFFFFFF;
   }
   pdos = (_IMAGE_DOS_HEADER*)dat;
   pnt = (_IMAGE_NT_HEADERS*)(dat + pdos->e_lfanew);
   handle = pnt->OptionalHeader.ImageBase;
   psect = (_IMAGE_SECTION_HEADER*)(pnt + 1);
   
   createSegment(handle, len, dat);

   applyPEHeaderTemplates(handle);

   exp_rva = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
   exp_size = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
   exp_fileoff = rvaToFileOffset(psect, nt.FileHeader.NumberOfSections, exp_rva);
   expdir = (_IMAGE_EXPORT_DIRECTORY*)malloc(exp_size);

   if (expdir == NULL || fseek(dll, exp_fileoff, SEEK_SET) || fread(expdir, exp_size, 1, dll) != 1) {
      free(dat);
      free(expdir);
      return 0xFFFFFFFF;
   }
   
   createSegment(handle + exp_rva, exp_size, (unsigned char*)expdir);

   if (expdir->AddressOfFunctions < exp_rva || expdir->AddressOfFunctions >= (exp_rva + exp_size)) {
      //EAT lies outside directory bounds
      msg("EAT lies outside directory bounds\n");
   }
   if (expdir->AddressOfNames < exp_rva || expdir->AddressOfNames >= (exp_rva + exp_size)) {
      //ENT lies outside directory bounds
      msg("ENT lies outside directory bounds\n");
   }
   if (expdir->AddressOfNameOrdinals < exp_rva || expdir->AddressOfNameOrdinals >= (exp_rva + exp_size)) {
      //EOT lies outside directory bounds
      msg("EOT lies outside directory bounds\n");
   }

   free(dat);
   free(expdir);
   return handle;
}
