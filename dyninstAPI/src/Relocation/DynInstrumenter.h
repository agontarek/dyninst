#ifndef _DYNINSTRUMENTER_H_
#define _DYNINSTRUMENTER_H_


#include "Instrumenter.h"
#include "DynAddrSpace.h"
#include "Command.h"

#include "dyninstAPI/src/instPoint.h"
#include "dyninstAPI/src/miniTramp.h"


using Dyninst::PatchAPI::DynAddrSpace;
using Dyninst::PatchAPI::DynAddrSpacePtr;
using Dyninst::PatchAPI::Command;

class BPatch_point;
class BPatchSnippetHandle;

namespace Dyninst {
namespace PatchAPI {

class DynInstrumenter : public Dyninst::PatchAPI::Instrumenter {
  public:
    DynInstrumenter() {}
    virtual ~DynInstrumenter() {}
    virtual bool run();
    virtual bool undo();
};
typedef dyn_detail::boost::shared_ptr<DynInstrumenter> DynInstrumenterPtr;

/* Dyninst-specific Insert Snippet Command  */
class DynInsertSnipCommand : public Command {
  public:
    typedef dyn_detail::boost::shared_ptr<DynInsertSnipCommand> Ptr;
    DynInsertSnipCommand(instPoint* pt, callOrder order,
                         AstNodePtr ast, bool recursive);
    static Ptr create(instPoint* pt, callOrder order,
                      AstNodePtr ast, bool recursive);
    virtual ~DynInsertSnipCommand() {}

    miniTramp* mini() { return mini_; }

    virtual bool run();
    virtual bool undo();

  protected:
    miniTramp* mini_;
};

/* Dyninst-specific Remove Snippet Command  */
class DynRemoveSnipCommand : public Command {
  public:
    typedef dyn_detail::boost::shared_ptr<DynRemoveSnipCommand> Ptr;
    DynRemoveSnipCommand(miniTramp* mini);
    static Ptr create(miniTramp* mini);
    virtual ~DynRemoveSnipCommand() {}

    virtual bool run();
    virtual bool undo();

  protected:
    miniTramp* mini_;
};

/* Dyninst-specific Function Replacement */
class DynReplaceFuncCommand : public Command {
  public:
    typedef dyn_detail::boost::shared_ptr<DynReplaceFuncCommand> Ptr;
    DynReplaceFuncCommand(AddressSpace* as,
                          func_instance* old_func,
                          func_instance* new_func);
    static Ptr create(AddressSpace* as,
                      func_instance* old_func,
                      func_instance* new_func);
    virtual ~DynReplaceFuncCommand() {}

    virtual bool run();
    virtual bool undo();

  protected:
    AddressSpace* as_;
    func_instance *old_func_;
    func_instance *new_func_;
};

/* Dyninst-specific Modify Function call */
class DynModifyCallCommand : public Command {
  public:
    typedef dyn_detail::boost::shared_ptr<DynModifyCallCommand> Ptr;
    DynModifyCallCommand(AddressSpace* as,
                         block_instance* block,
                         func_instance* new_func,
                         func_instance* context);
    static Ptr create(AddressSpace* as,
                      block_instance* block,
                      func_instance* new_func,
                      func_instance* context);
    virtual ~DynModifyCallCommand() {}

    virtual bool run();
    virtual bool undo();

  protected:
    AddressSpace* as_;
    block_instance *block_;
    func_instance *new_func_;
    func_instance *context_;
};

/* Dyninst-specific Remove Function call */
class DynRemoveCallCommand : public Command {
  public:
    typedef dyn_detail::boost::shared_ptr<DynRemoveCallCommand> Ptr;
    DynRemoveCallCommand(AddressSpace* as,
                         block_instance* block,
                         func_instance* context);
    static Ptr create(AddressSpace* as,
                      block_instance* block,
                      func_instance* context);
    virtual ~DynRemoveCallCommand() {}

    virtual bool run();
    virtual bool undo();

  protected:
    AddressSpace* as_;
    block_instance *block_;
    func_instance *context_;
};


}
}

#endif /* _DYNINSTRUMENTER_H_ */
