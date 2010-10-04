
#include "cpu.h"
#include "seh.h"

static int seh_enable = 0;
static WIN_CONTEXT ctx;

typedef struct _VehNode {
   dword handler;
   struct _VehNode *next;
} VehNode;

static VehNode *vehList;

struct WIN_CONTEXT *getContext() {
   return &ctx;
}

int usingSEH() {
   return seh_enable;
}

VehNode *findVehHandler(dword handler) {
   for (VehNode *h = vehList; h; h = h->next) {
      if (h->handler == handler) {
         return h;
      }
   }
   return NULL;
}

void saveSEHState(Buffer &b) {
   int dummy;
   b.write(&dummy, sizeof(dummy));
   b.write(&seh_enable, sizeof(seh_enable));
   b.write(&ctx, sizeof(ctx));
}

void loadSEHState(Buffer &b) {
   dword dummy;
   b.read(&dummy, sizeof(dummy));
   b.read(&seh_enable, sizeof(seh_enable));
   b.read(&ctx, sizeof(ctx));
}

void saveVEHState(Buffer &b) {
   for (VehNode *v = vehList; v; v = v->next) {
      b.write(&v->handler, sizeof(dword));
   }
}

void loadVEHState(Buffer &b) {
   dword dummy;
   while (b.read(&dummy, sizeof(dummy)) == 0) {
      addVectoredExceptionHandler(0, dummy);
   }
   b.reset_error();
}

//Copy current CPU state into CONTEXT structure for Windows Exception Handling
//Note that the global ctx struct is the only place that Debug and Floating
//point registers are currently defined
void cpuToContext() {
   regsToContext(&cpu, &ctx);
   ctx.Eip = cpu.initial_eip;  //use address at which exception occurred
}

//Copy from CONTEXT structure into CPU state for Windows Exception Handling
//Note that the global ctx struct is the only place that Debug and Floating
//point registers are currently defined
void contextToCpu() {
   contextToRegs(&ctx, &cpu);
}

void initContext() {
   initContext(&ctx);
}

void popContext() {
   byte *ptr = (byte*) &ctx;
   dword addr, i;
   dword ctx_size = (sizeof(WIN_CONTEXT) + 3) & ~3;  //round up to next dword
   addr = esp;
   for (i = 0; i < sizeof(WIN_CONTEXT); i++) {
      *ptr++ = (byte) readMem(addr++, SIZE_BYTE);
   }
   esp += ctx_size;
   contextToCpu();
}

void getContextToMem(dword addr) {
   byte *ptr = (byte*) &ctx;
   cpuToContext();
   copyContextToMem(&ctx, addr);
}

dword pushContext() {
   dword ctx_size = (sizeof(WIN_CONTEXT) + 3) & ~3;  //round up to next dword
   dword addr = esp - ctx_size;
   getContextToMem(addr);
   esp = addr;
   return esp;
}

void popExceptionRecord(EXCEPTION_RECORD *rec) {
   byte *ptr = (byte*) &rec;
   dword addr, i;
   dword rec_size = (sizeof(EXCEPTION_RECORD) + 3) & ~3;  //round up to next dword
   addr = esp;
   for (i = 0; i < sizeof(EXCEPTION_RECORD); i++) {
      *ptr++ = (byte) readMem(addr++, SIZE_BYTE);
   }
   esp += rec_size;
}

dword pushExceptionRecord(EXCEPTION_RECORD *rec) {
   byte *ptr = (byte*) rec;
   dword addr, i;
   dword rec_size = (sizeof(EXCEPTION_RECORD) + 3) & ~3;  //round up to next dword
   addr = esp -= rec_size;
   for (i = 0; i < sizeof(EXCEPTION_RECORD); i++) {
      writeMem(addr++, *ptr++, SIZE_BYTE);
   }
   return esp;
}

void doSehException(EXCEPTION_RECORD *rec) {
   dword err_ptr = readMem(fsBase, SIZE_DWORD);
   dword handler = readMem(err_ptr + 4, SIZE_DWORD);  //err->handler
   
   //do sanity checks on handler here?
   
   cpuToContext();
   dword ctx_ptr = pushContext();
   dword rec_ptr = pushExceptionRecord(rec);
   
   push(ctx_ptr, SIZE_DWORD);
   push(err_ptr, SIZE_DWORD);       //err_ptr == fsBase??
   push(rec_ptr, SIZE_DWORD);
   push(SEH_MAGIC, SIZE_DWORD);             //handler return address
//need to execute exception handler here setup flag to trap ret
//set eip to start of exception handler and resume fetching
   cpu.eip = handler;
}

static dword currentVehHandler;

void doVehException(EXCEPTION_RECORD *rec, dword handler) {      
   cpuToContext();
   dword ctx_ptr = pushContext();
   dword rec_ptr = pushExceptionRecord(rec);
   
   push(ctx_ptr, SIZE_DWORD);
   push(rec_ptr, SIZE_DWORD);
   push(esp, SIZE_DWORD);
   push(VEH_MAGIC, SIZE_DWORD);             //handler return address
//need to execute exception handler here setup flag to trap ret
//set eip to start of exception handler and resume fetching
   cpu.eip = handler;
}

void doException(EXCEPTION_RECORD *rec) {
   if (vehList) {
      if (currentVehHandler == 0) {
         currentVehHandler = vehList->handler;
         doVehException(rec, currentVehHandler);
      }
      else {
         VehNode *v = findVehHandler(currentVehHandler);
         if (v) {
            v = v->next;
         }
         if (v) {
            currentVehHandler = v->handler;
            doVehException(rec, currentVehHandler);
         }
         else {
            currentVehHandler = 0xffffffff;
         }
      }
   }
   else {
      currentVehHandler = 0xffffffff;
   }   
   if (currentVehHandler == 0xffffffff) {
      doSehException(rec);
   }   
}

void sehReturn() {
   EXCEPTION_RECORD rec;
   
   //need to check eax here to see if exception was handled
   //or if it needs to be kicked up to next SEH handler
   
   esp += 3 * SIZE_DWORD;  //clear off exception pointers
   
   popExceptionRecord(&rec);

   popContext();
   contextToCpu();
   //eip is now restored to pre exception location
   
   //need to fake an iret here
   doInterruptReturn();  //this clobbers EIP, CS, EFLAGS
   //so restore them here from ctx values
   cpu.eip = ctx.Eip;
   cpu.eflags = ctx.EFlags;
   cs = ctx.SegCs;
   msg("Performing SEH return\n");
   currentVehHandler = 0;
}

void vehReturn() {
   EXCEPTION_RECORD rec;
   
   //need to check eax here to see if exception was handled
   //or if it needs to be kicked up to next SEH handler
   dword res = eax;
   
   esp += 3 * SIZE_DWORD;  //clear off exception pointers
   
   popExceptionRecord(&rec);

   popContext();
   contextToCpu();
   //eip is now restored to pre exception location
   
   //need to fake an iret here
   doInterruptReturn();  //this clobbers EIP, CS, EFLAGS
   //so restore them here from ctx values
   cpu.eip = ctx.Eip;
   cpu.eflags = ctx.EFlags;
   cs = ctx.SegCs;
   msg("Performing VEH return\n");

   if (res == EXCEPTION_CONTINUE_EXECUTION) {
      currentVehHandler = 0;
   }
   else {  //res == EXCEPTION_CONTINUE_SEARCH
      doException(&rec);
   }
}

void generateException(dword code) {
   if (seh_enable) {
      EXCEPTION_RECORD rec;
      rec.exceptionCode = code;
      rec.exceptionFlags = CONTINUABLE;   //nothing sophisticated here
      rec.exceptionRecord = 0;   //NULL
      rec.exceptionAddress = cpu.initial_eip;
      rec.numberParameters = 0;
      doException(&rec);
   }
}

void breakpointException() {
   generateException(BREAKPOINT_EXCEPTION);
}

void debugException() {
   generateException(DEBUG_EXCEPTION);
}

void divzeroException() {
   generateException(DIV_ZERO_EXCEPTION);
}

void memoryAccessException() {
   generateException(MEM_ACCESS);
}

void IllegalOpcodeException() {
   generateException(UNDEFINED_OPCODE_EXCEPTION);
}

void enableSEH() {
   initContext();
   seh_enable = 1;
}

void sehBegin(dword interrupt_number) {
   msg("Initiating SEH processing of INT %d\n", interrupt_number);
   switch (interrupt_number) {
   case 0:
      generateException(DIV_ZERO_EXCEPTION);
      break;   
   case 1:
      generateException(DEBUG_EXCEPTION);
      break;   
   case 3:
      generateException(BREAKPOINT_EXCEPTION);
      break;   
   case 6:
      generateException(UNDEFINED_OPCODE_EXCEPTION);
      break;   
   }
}

void addVectoredExceptionHandler(bool first, dword handler) {
   VehNode *n = (VehNode*)malloc(sizeof(VehNode));
   n->handler = handler;
   if (first) {
      n->next = vehList;
      vehList = n;
   }
   else {
      n->next = NULL;
      if (vehList) {
         VehNode *h;
         for (h = vehList; h->next; h = h->next);
         h->next = n;
      }
      else {
         vehList = n;
      }
   }
}

void removeVectoredExceptionHandler(dword handler) {
   VehNode *p = NULL;
   for (VehNode *h = vehList; h->next; h = h->next) {
      if (h->handler == handler) {
         if (p) {
            p->next = h->next;
         }
         else {
            vehList = p->next;
         }
         free(h);
         break;
      }
      p = h;
   }
}
