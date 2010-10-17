/*
   Source for x86 emulator IdaPro plugin
   Copyright (c) 2005-2010 Chris Eagle
   
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

#ifndef __PE_UTILS_H
#define __PE_UTILS_H

#include <stdio.h>

#include "buffer.h"

struct _IMAGE_NT_HEADERS;
struct _IMAGE_SECTION_HEADER;
struct _IMAGE_EXPORT_DIRECTORY;

struct thunk_rec {
   char *dll_name;
   unsigned int iat_base;  //base VA for iat
   unsigned int iat_size;
   unsigned int *iat;
//   char **names;
   thunk_rec *next;
};

class PETables {
public:
   PETables();
   ~PETables();
   unsigned int rvaToFileOffset(unsigned int rva);
   void setBase(unsigned int b) {base = b;};
   void setNtHeaders(_IMAGE_NT_HEADERS *inth);
   void setSectionHeaders(unsigned int nsecs, _IMAGE_SECTION_HEADER *ish);
   void buildThunks(FILE *f);
   void destroy();
   void loadTables(Buffer &b);
   void saveTables(Buffer &b);
   
   unsigned int valid;
   unsigned int base;
   _IMAGE_NT_HEADERS *nt;
   _IMAGE_SECTION_HEADER *sections;
   unsigned short num_sections;
   thunk_rec *imports;
};

struct DllList {
   char *dllName;
   unsigned int handle;
   unsigned int id;
   unsigned int maxAddr;
   _IMAGE_NT_HEADERS *nt;
   _IMAGE_SECTION_HEADER *sections;
   _IMAGE_EXPORT_DIRECTORY *exportdir;
   unsigned int NoF;  //NumberOfFunctions
   unsigned int NoN;  //NumberOfNames
   unsigned int *eat; // AddressOfFunctions  export address table
   unsigned int *ent; // AddressOfNames      export name table
   unsigned short *eot;  // AddressOfNameOrdinals  export ordinal table
   DllList *next;
};

unsigned int loadIntoIdb(FILE *dll);
void applyPEHeaderTemplates(unsigned int mz_addr);
void createSegment(unsigned int start, unsigned int size, unsigned char *content, 
                   unsigned int clen = 0, const char *name = NULL);

#endif
