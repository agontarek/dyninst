/*
 * Copyright (c) 1996-2007 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SYMTAB_H__
#define __SYMTAB_H__
 
#include "Symbol.h"
#include "Region.h"
#include "LineInformation.h"

#include "Annotatable.h"
#include "Serialization.h"


int symtab_printf(const char *format, ...);

typedef struct {} user_funcs_a;
typedef struct {} user_regions_a;
typedef struct {} user_types_a;
typedef struct {} user_symbols_a;

class MappedFile;

namespace Dyninst{
namespace SymtabAPI{

class Archive;
class builtInTypeCollection;

class ExceptionBlock;
class Object;
class relocationEntry;

class Symtab : public LookupInterface,
               public Serializable,
               public AnnotatableSparse
{

   friend class Archive;
   friend class Symbol;
   friend class Module;
   friend class Region;
   friend class emitElf;
   friend class emitElf64;

 public:
   typedef enum {
       mangledName = 1,
       prettyName = 2,
       typedName = 4,
       anyName = 7 } nameType_t;

   /***** Public Member Functions *****/
   public:
   DLLEXPORT Symtab(MappedFile *);

   DLLEXPORT Symtab();

   DLLEXPORT Symtab(const Symtab& obj);

   DLLEXPORT static bool openFile(Symtab *&obj, std::string filename);
   DLLEXPORT static bool openFile(Symtab *&obj,char *mem_image, size_t size);

   DLLEXPORT void serialize(SerializerBase *sb, const char *tag = "Symtab");
   static bool setup_module_up_ptrs(SerializerBase *,Symtab *st);
   static bool fixup_relocation_symbols(SerializerBase *,Symtab *st);

   DLLEXPORT bool exportXML(std::string filename);
   DLLEXPORT bool exportBin(std::string filename);
   static Symtab *importBin(std::string filename);


   /**************************************
    *** LOOKUP FUNCTIONS *****************
    **************************************/

   // Symbol

   DLLEXPORT virtual bool findSymbolByType(std::vector<Symbol *> &ret, 
                                           const std::string name,
                                           Symbol::SymbolType sType, 
                                           nameType_t nameType,
                                           bool isRegex = false, 
                                           bool checkCase = false);
   DLLEXPORT virtual bool findAllSymbols(std::vector<Symbol *> &ret);

   DLLEXPORT virtual bool getAllSymbolsByType(std::vector<Symbol *> &ret, 
         Symbol::SymbolType sType);

   // Return all undefined symbols in the binary. Currently used for finding
   // the .o's in a static archive that have definitions of these symbols
   DLLEXPORT bool getAllUndefinedSymbols(std::vector<Symbol *> &ret);

   // Function

   DLLEXPORT bool findFuncByEntryOffset(Function *&ret, const Offset offset);
   DLLEXPORT bool findFunctionsByName(std::vector<Function *> &ret, const std::string name,
                                      nameType_t nameType = anyName, 
                                      bool isRegex = false,
                                      bool checkCase = true);
   DLLEXPORT bool getAllFunctions(std::vector<Function *>&ret);

   // Variable
   DLLEXPORT bool findVariableByOffset(Variable *&ret, const Offset offset);
   DLLEXPORT bool findVariablesByName(std::vector<Variable *> &ret, const std::string name,
                                      nameType_t nameType = anyName, 
                                      bool isRegex = false, 
                                      bool checkCase = true);
   DLLEXPORT bool getAllVariables(std::vector<Variable *> &ret);

   // Module

   DLLEXPORT bool getAllModules(std::vector<Module *>&ret);
   DLLEXPORT bool findModuleByOffset(Module *&ret, Offset off);
   DLLEXPORT bool findModuleByName(Module *&ret, const std::string name);


   // Region

   DLLEXPORT bool getCodeRegions(std::vector<Region *>&ret);
   DLLEXPORT bool getDataRegions(std::vector<Region *>&ret);
   DLLEXPORT bool getAllRegions(std::vector<Region *>&ret);
   DLLEXPORT bool getAllNewRegions(std::vector<Region *>&ret);
   //  change me to use a hash
   DLLEXPORT bool findRegion(Region *&ret, std::string regname);
   DLLEXPORT bool findRegionByEntry(Region *&ret, const Offset offset);
   DLLEXPORT Region *findEnclosingRegion(const Offset offset);

   // Exceptions
   DLLEXPORT bool findException(ExceptionBlock &excp,Offset addr);
   DLLEXPORT bool getAllExceptions(std::vector<ExceptionBlock *> &exceptions);
   DLLEXPORT bool findCatchBlock(ExceptionBlock &excp, Offset addr, 
         unsigned size = 0);

   // Relocation entries
   DLLEXPORT bool getFuncBindingTable(std::vector<relocationEntry> &fbt) const;

   /**************************************
    *** SYMBOL ADDING FUNCS **************
    **************************************/

   DLLEXPORT bool addSymbol(Symbol *newsym, bool isDynamic = false);
   DLLEXPORT bool addSymbol(Symbol *newSym, Symbol *referringSymbol);


   /*****Query Functions*****/
   DLLEXPORT bool isExec() const;
   DLLEXPORT bool isStripped();
   DLLEXPORT ObjectType getObjectType() const;

   DLLEXPORT bool isCode(const Offset where) const;
   DLLEXPORT bool isData(const Offset where) const;
   DLLEXPORT bool isValidOffset(const Offset where) const;

   DLLEXPORT bool isNativeCompiler() const;
   DLLEXPORT bool getMappedRegions(std::vector<Region *> &mappedRegs) const;

   /***** Line Number Information *****/
   DLLEXPORT bool getAddressRanges(std::vector<std::pair<Offset, Offset> >&ranges,
         std::string lineSource, unsigned int LineNo);
   DLLEXPORT bool getSourceLines(std::vector<LineInformationImpl::LineNoTuple> &lines, 
         Offset addressInRange);
   DLLEXPORT bool addLine(std::string lineSource, unsigned int lineNo,
         unsigned int lineOffset, Offset lowInclAddr,
         Offset highExclAddr);
   DLLEXPORT bool addAddressRange(Offset lowInclAddr, Offset highExclAddr, std::string lineSource,
         unsigned int lineNo, unsigned int lineOffset = 0);

   /***** Type Information *****/
   DLLEXPORT virtual bool findType(Type *&type, std::string name);
   DLLEXPORT virtual bool findVariableType(Type *&type, std::string name);

   DLLEXPORT bool addType(Type *typ);

   DLLEXPORT static std::vector<Type *> *getAllstdTypes();
   DLLEXPORT static std::vector<Type *> *getAllbuiltInTypes();

   DLLEXPORT void parseTypesNow();

   /***** Local Variable Information *****/
   DLLEXPORT bool findLocalVariable(std::vector<localVar *>&vars, std::string name);

   /***** Relocation Sections *****/
   DLLEXPORT bool hasRel() const;
   DLLEXPORT bool hasRela() const;

   /***** Write Back binary functions *****/
   DLLEXPORT bool emitSymbols(Object *linkedFile, std::string filename, unsigned flag = 0);
   DLLEXPORT bool addRegion(Offset vaddr, void *data, unsigned int dataSize, 
         std::string name, Region::RegionType rType_, bool loadable = false);
   DLLEXPORT bool addRegion(Region *newreg);
   DLLEXPORT bool emit(std::string filename, unsigned flag = 0);

   DLLEXPORT void addDynLibSubstitution(std::string oldName, std::string newName);
   DLLEXPORT std::string getDynLibSubstitution(std::string name);

   DLLEXPORT bool getSegments(std::vector<Segment> &segs) const;
   DLLEXPORT bool updateCode(void *buffer, unsigned size);
   DLLEXPORT bool updateData(void *buffer, unsigned size);
   DLLEXPORT Offset getFreeOffset(unsigned size);

   /***** Data Member Access *****/
   DLLEXPORT std::string file() const;
   DLLEXPORT std::string name() const;

   DLLEXPORT char *mem_image() const;

   DLLEXPORT Offset imageOffset() const;
   DLLEXPORT Offset dataOffset() const;
   DLLEXPORT Offset dataLength() const;
   DLLEXPORT Offset imageLength() const;
   //   DLLEXPORT char*  image_ptr ()  const;
   //   DLLEXPORT char*  data_ptr ()  const;

   DLLEXPORT const char*  getInterpreterName() const;

   DLLEXPORT unsigned getAddressWidth() const;
   DLLEXPORT Offset getLoadOffset() const;
   DLLEXPORT Offset getEntryOffset() const;
   DLLEXPORT Offset getBaseOffset() const;
   DLLEXPORT Offset getTOCoffset() const;

   DLLEXPORT std::string getDefaultNamespacePrefix() const;

   DLLEXPORT unsigned getNumberofRegions() const;
   DLLEXPORT unsigned getNumberofSymbols() const;

   DLLEXPORT std::vector<std::string> &getDependencies();

   /***** Error Handling *****/
   DLLEXPORT static SymtabError getLastSymtabError();
   DLLEXPORT static std::string printError(SymtabError serr);

   DLLEXPORT ~Symtab();

   bool delSymbol(Symbol *sym); 

   static builtInTypeCollection *builtInTypes;
   static typeCollection *stdTypes;

   protected:
   Symtab(std::string filename, std::string member_name, Offset offset, bool &err, void *base = NULL);
   Symtab(char *img, size_t size, std::string member_name, Offset offset, bool &err, void *base = NULL);

   /***** Private Member Functions *****/
   private:
   DLLEXPORT Symtab(std::string filename, bool &err); 
   DLLEXPORT Symtab(char *mem_image, size_t image_size, bool &err);

   DLLEXPORT bool extractInfo(Object *linkedFile);

   // Parsing code

   bool extractSymbolsFromFile(Object *linkedFile, std::vector<Symbol *> &raw_syms);
   bool fixSymModules(std::vector<Symbol *> &raw_syms);
   bool demangleSymbols(std::vector<Symbol *> &rawsyms);
   bool createIndices(std::vector<Symbol *> &raw_syms);
   bool createAggregates();

   bool fixSymModule(Symbol *&sym);
   bool demangleSymbol(Symbol *&sym);
   bool addSymbolToIndices(Symbol *&sym);
   bool addSymbolToAggregates(Symbol *&sym);
   bool updateIndices(Symbol *sym, std::string newName, nameType_t nameType);


   void setModuleLanguages(dyn_hash_map<std::string, supportedLanguages> *mod_langs);

   void setupTypes();
   static void setupStdTypes();

   bool buildDemangledName( const std::string &mangled, 
         std::string &pretty,
         std::string &typed,
         bool nativeCompiler, 
         supportedLanguages lang );

   // Change the type of a symbol after the fact
   bool changeType(Symbol *sym, Symbol::SymbolType oldType);

   Module *getOrCreateModule(const std::string &modName, 
         const Offset modAddr);
   Module *newModule(const std::string &name, const Offset addr, supportedLanguages lang);
   
   //bool buildFunctionLists(std::vector <Symbol *> &raw_funcs);
   //void enterFunctionInTables(Symbol *func, bool wasSymtab);


   bool addSymtabVariables();

   void checkPPC64DescriptorSymbols(Object *linkedFile);


   void parseLineInformation();
   void parseTypes();
   bool setDefaultNamespacePrefix(std::string &str);


   bool addUserRegion(Region *newreg);
   bool addUserType(Type *newtypeg);





   /***** Private Data Members *****/
   private:
   std::string member_name_;
   Offset member_offset_;
   MappedFile *mf;
   MappedFile *mfForDebugInfo;

   Offset imageOffset_;
   unsigned imageLen_;
   Offset dataOffset_;
   unsigned dataLen_;

   bool is_a_out;
   Offset main_call_addr_; // address of call to main()

   bool nativeCompiler;

   unsigned address_width_;
   char *code_ptr_;
   char *data_ptr_;
   std::string interpreter_name_;
   Offset entry_address_;
   Offset base_address_;
   Offset load_address_;
   Offset toc_offset_;
   ObjectType object_type_;
   bool is_eel_;
   std::vector<Segment> segments_;
   //  make sure is_a_out is set before calling symbolsToFunctions

   // A std::vector of all Symtabs. Used to avoid duplicating
   // a Symtab that already exists.
   static std::vector<Symtab *> allSymtabs;
   std::string defaultNamespacePrefix;

   //sections
   unsigned no_of_sections;
   std::vector<Region *> regions_;
   std::vector<Region *> codeRegions_;
   std::vector<Region *> dataRegions_;
   dyn_hash_map <Offset, Region *> regionsByEntryAddr;

   //Point where new loadable sections will be inserted
   unsigned newSectionInsertPoint;

   //symbols
   unsigned no_of_symbols;

   // Indices

   std::vector<Symbol *> everyDefinedSymbol;
   // Subset of the above
   std::vector<Symbol *> userAddedSymbols;
   // hashtable for looking up undefined symbols in the dynamic symbol
   // tale. Entries are referred by the relocation table entries
   // NOT a subset of everyDefinedSymbol
   std::map <std::string, Symbol *> undefDynSyms;

   // Symbols by offsets in the symbol table
   dyn_hash_map <Offset, std::vector<Symbol *> > symsByOffset;

   // The raw name from the symbol table
   dyn_hash_map <std::string, std::vector<Symbol *> > symsByMangledName;

   // The name after we've run it through the demangler
   dyn_hash_map <std::string, std::vector<Symbol *> > symsByPrettyName;

   // The name after we've derived the parameter types
   dyn_hash_map <std::string, std::vector<Symbol *> > symsByTypedName;

   // We also need per-Aggregate indices
   std::vector<Function *> everyFunction;
   // Since Functions are unique by address we require this structure to
   // efficiently track them.
   dyn_hash_map <Offset, Function *> funcsByOffset;

   // Similar for Variables
   std::vector<Variable *> everyVariable;
   dyn_hash_map <Offset, Variable *> varsByOffset;

   // For now, skip the index-by-name structures. We can use the Symbol
   // ones instead. 
   /*
   dyn_hash_map <std::string, std::vector<Function *> *> funcsByMangledName;
   dyn_hash_map <std::string, std::vector<Function *> *> funcsByPrettyName;
   dyn_hash_map <std::string, std::vector<Function *> *> funcsByTypedName;
   */

   //dyn_hash_map <Offset, std::vector<Function *> > funcsByEntryAddr;
   // note, a prettyName is not unique, it may map to a function appearing
   // in several modules.  Also only contains instrumentable functions....
   //dyn_hash_map <std::string, std::vector<Function *>*> funcsByPretty;
   // Hash table holding functions by mangled name.
   // Should contain same functions as funcsByPretty....
   //dyn_hash_map <std::string, std::vector<Function *>*> funcsByMangled;
   // A way to iterate over all the functions efficiently
   //std::vector<Symbol *> everyUniqueFunction;
   //std::vector<Function *> allFunctions;
   // And the counterpart "ones that are there right away"
   //std::vector<Symbol *> exportedFunctions;

   //dyn_hash_map <Address, Function *> funcsByAddr;
   dyn_hash_map <std::string, Module *> modsByFileName;
   dyn_hash_map <std::string, Module *> modsByFullName;
   std::vector<Module *> _mods;

   // Variables indexed by pretty (non-mangled) name
   /*
   dyn_hash_map <std::string, std::vector <Symbol *> *> varsByPretty;
   dyn_hash_map <std::string, std::vector <Symbol *> *> varsByMangled;
   dyn_hash_map <Offset, Symbol *> varsByAddr;
   std::vector<Symbol *> everyUniqueVariable;
   */

   //dyn_hash_map <std::string, std::vector <Symbol *> *> modsByName;
   //std::vector<Module *> _mods;


   std::vector<relocationEntry > relocation_table_;
   std::vector<ExceptionBlock *> excpBlocks;

   std::vector<std::string> deps_;

   //Line Information valid flag;
   bool isLineInfoValid_;
   //type info valid flag
   bool isTypeInfoValid_;

   int nlines_;
   unsigned long fdptr_;
   char *lines_;
   char *stabstr_;
   int nstabs_;
   void *stabs_;
   char *stringpool_;

   //Relocation sections
   bool hasRel_;
   bool hasRela_;

   //Don't use obj_private, use getObject() instead.
   Object *getObject();
   Object *obj_private;

   // dynamic library name substitutions
   std::map <std::string, std::string> dynLibSubs;

   public:
   Type *type_Error;
   Type *type_Untyped;

 public:
   /********************************************************************/
   /**** DEPRECATED ****************************************************/
   /********************************************************************/
   dyn_hash_map <std::string, Module *> &getModsByFileName()
   {
      return modsByFileName;
   }
   dyn_hash_map <std::string, Module *> &getModsByFullName()
   {
      return modsByFullName;
   }
   
   DLLEXPORT bool findFuncByEntryOffset(std::vector<Symbol *>&ret, const Offset offset);
   DLLEXPORT virtual bool findSymbolByType(std::vector<Symbol *> &ret, 
                                           const std::string name,
                                           Symbol::SymbolType sType, 
                                           bool isMangled = false,
                                           bool isRegex = false, 
                                           bool checkCase = false);


};

/**
 * Used to represent something like a C++ try/catch block.  
 * Currently only used on Linux/x86
 **/
class ExceptionBlock : public Serializable, public AnnotatableSparse {

   public:
      DLLEXPORT void serialize(SerializerBase *sb, const char *tag = "exceptionBlock");
      DLLEXPORT ExceptionBlock(Offset tStart, unsigned tSize, Offset cStart);
      DLLEXPORT ExceptionBlock(Offset cStart);
      DLLEXPORT ExceptionBlock(const ExceptionBlock &eb);
      DLLEXPORT ~ExceptionBlock();
      DLLEXPORT ExceptionBlock();

      DLLEXPORT bool hasTry() const;
      DLLEXPORT Offset tryStart() const;
      DLLEXPORT Offset tryEnd() const;
      DLLEXPORT Offset trySize() const;
      DLLEXPORT Offset catchStart() const;
      DLLEXPORT bool contains(Offset a) const;

   private:
      Offset tryStart_;
      unsigned trySize_;
      Offset catchStart_;
      bool hasTry_;
};

// relocation information for calls to functions not in this image
// on sparc-solaris: target_addr_ = rel_addr_ = PLT entry addr
// on x86-solaris: target_addr_ = PLT entry addr
//		   rel_addr_ =  GOT entry addr  corr. to PLT_entry

class relocationEntry : public Serializable, public AnnotatableSparse {
   public:

      DLLEXPORT relocationEntry();
      DLLEXPORT relocationEntry(Offset ta, Offset ra, Offset add, std::string n, Symbol *dynref = NULL, unsigned long relType = 0);
      DLLEXPORT relocationEntry(Offset ta, Offset ra, std::string n, Symbol *dynref = NULL, unsigned long relType = 0);
      DLLEXPORT relocationEntry(Offset ra, std::string n, Symbol *dynref = NULL, unsigned long relType = 0, Region::RegionType rtype = Region::RT_REL);

      DLLEXPORT relocationEntry(const relocationEntry& ra);

      DLLEXPORT const relocationEntry& operator= (const relocationEntry &ra);
      DLLEXPORT void serialize(SerializerBase *sb, const char *tag = "relocationEntry");

      DLLEXPORT Offset target_addr() const;
      DLLEXPORT Offset rel_addr() const;
      DLLEXPORT Offset addend() const;
      DLLEXPORT Region::RegionType regionType() const;
      DLLEXPORT void setAddend(const Offset);
      DLLEXPORT void setRegionType(const Region::RegionType);
      DLLEXPORT const std::string &name() const;
      DLLEXPORT Symbol *getDynSym() const;
      DLLEXPORT bool addDynSym(Symbol *dynref);
      DLLEXPORT unsigned long getRelType() const;

      // dump output.  Currently setup as a debugging aid, not really
      //  for object persistance....
      DLLEXPORT std::ostream & operator<<(std::ostream &s) const;
      friend std::ostream &operator<<(std::ostream &os, relocationEntry &q);

      enum {pltrel = 1, dynrel = 2} relocationType;

   private:
      Offset target_addr_;	// target address of call instruction 
      Offset rel_addr_;		// address of corresponding relocation entry 
      Offset addend_;       // addend (from RELA entries)
      Region::RegionType rtype_;        // RT_REL vs. RT_RELA
      std::string  name_;
      Symbol *dynref_;
      unsigned long relType_;
};

}//namespace SymtabAPI

}//namespace Dyninst
#endif
