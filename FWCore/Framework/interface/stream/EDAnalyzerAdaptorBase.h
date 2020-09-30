#ifndef FWCore_Framework_stream_EDAnalyzerAdaptorBase_h
#define FWCore_Framework_stream_EDAnalyzerAdaptorBase_h
// -*- C++ -*-
//
// Package:     FWCore/Framework
// Class  :     EDAnalyzerAdaptorBase
//
/**\class edm::stream::EDAnalyzerAdaptorBase EDAnalyzerAdaptorBase.h "FWCore/Framework/interface/stream/EDAnalyzerAdaptorBase.h"

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Fri, 02 Aug 2013 18:09:15 GMT
//

// system include files
#include <map>
#include <string>
#include <vector>

// user include files
#include "DataFormats/Provenance/interface/BranchType.h"
#include "FWCore/Utilities/interface/ProductResolverIndex.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"
#include "FWCore/ServiceRegistry/interface/ConsumesInfo.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/RunIndex.h"
#include "FWCore/Utilities/interface/LuminosityBlockIndex.h"
#include "FWCore/Utilities/interface/ESIndices.h"
#include "FWCore/Utilities/interface/Transition.h"

// forward declarations

namespace edm {
  class ModuleCallingContext;
  class ProductResolverIndexHelper;
  class EDConsumerBase;
  class PreallocationConfiguration;
  class ProductResolverIndexAndSkipBit;
  class ActivityRegistry;
  class ThinnedAssociationsHelper;
  class WaitingTask;

  namespace maker {
    template <typename T>
    class ModuleHolderT;
  }

  namespace eventsetup {
    class ESRecordsToProxyIndices;
  }

  namespace stream {
    class EDAnalyzerBase;

    class EDAnalyzerAdaptorBase {
    public:
      template <typename T>
      friend class edm::WorkerT;
      template <typename T>
      friend class edm::maker::ModuleHolderT;

      EDAnalyzerAdaptorBase();
      virtual ~EDAnalyzerAdaptorBase();

      // ---------- const member functions ---------------------

      // ---------- static member functions --------------------

      // ---------- member functions ---------------------------
      const ModuleDescription& moduleDescription() const { return moduleDescription_; }

      virtual bool wantsProcessBlocks() const = 0;
      virtual bool wantsInputProcessBlocks() const = 0;
      virtual bool wantsGlobalRuns() const = 0;
      virtual bool wantsGlobalLuminosityBlocks() const = 0;
      bool wantsStreamRuns() const { return true; }
      bool wantsStreamLuminosityBlocks() const { return true; }

      std::string workerType() const { return "WorkerT<EDAnalyzerAdaptorBase>"; }
      void registerProductsAndCallbacks(EDAnalyzerAdaptorBase const*, ProductRegistry* reg);

    protected:
      template <typename T>
      void createStreamModules(T iFunc) {
        for (auto& m : m_streamModules) {
          m = iFunc();
          setModuleDescriptionPtr(m);
        }
      }

      //Same interface as EDConsumerBase
      void itemsToGet(BranchType, std::vector<ProductResolverIndexAndSkipBit>&) const;
      void itemsMayGet(BranchType, std::vector<ProductResolverIndexAndSkipBit>&) const;
      std::vector<ProductResolverIndexAndSkipBit> const& itemsToGetFrom(BranchType) const;

      std::vector<ESProxyIndex> const& esGetTokenIndicesVector(edm::Transition iTrans) const;
      std::vector<ESRecordIndex> const& esGetTokenRecordIndicesVector(edm::Transition iTrans) const;

      void updateLookup(BranchType iBranchType, ProductResolverIndexHelper const&, bool iPrefetchMayGet);
      void updateLookup(eventsetup::ESRecordsToProxyIndices const&);

      const EDConsumerBase* consumer() const;

      void modulesWhoseProductsAreConsumed(std::vector<ModuleDescription const*>& modules,
                                           ProductRegistry const& preg,
                                           std::map<std::string, ModuleDescription const*> const& labelsToDesc,
                                           std::string const& processName) const;

      void convertCurrentProcessAlias(std::string const& processName);

      std::vector<ConsumesInfo> consumesInfo() const;

    private:
      EDAnalyzerAdaptorBase(const EDAnalyzerAdaptorBase&) = delete;  // stop default

      const EDAnalyzerAdaptorBase& operator=(const EDAnalyzerAdaptorBase&) = delete;  // stop default

      bool doEvent(EventTransitionInfo const&, ActivityRegistry*, ModuleCallingContext const*);
      void doPreallocate(PreallocationConfiguration const&);
      virtual void preallocLumis(unsigned int) {}

      //For now this is a placeholder
      /*virtual*/ void preActionBeforeRunEventAsync(WaitingTask*, ModuleCallingContext const&, Principal const&) const {
      }

      virtual void setupStreamModules() = 0;
      virtual void doBeginJob() = 0;
      virtual void doEndJob() = 0;

      void doBeginStream(StreamID id);
      void doEndStream(StreamID id);
      void doStreamBeginRun(StreamID, RunTransitionInfo const&, ModuleCallingContext const*);
      virtual void setupRun(EDAnalyzerBase*, RunIndex) = 0;
      void doStreamEndRun(StreamID, RunTransitionInfo const&, ModuleCallingContext const*);
      virtual void streamEndRunSummary(EDAnalyzerBase*, edm::Run const&, edm::EventSetup const&) = 0;

      void doStreamBeginLuminosityBlock(StreamID, LumiTransitionInfo const&, ModuleCallingContext const*);
      virtual void setupLuminosityBlock(EDAnalyzerBase*, LuminosityBlockIndex) = 0;
      void doStreamEndLuminosityBlock(StreamID, LumiTransitionInfo const&, ModuleCallingContext const*);
      virtual void streamEndLuminosityBlockSummary(EDAnalyzerBase*,
                                                   edm::LuminosityBlock const&,
                                                   edm::EventSetup const&) = 0;

      virtual void doBeginProcessBlock(ProcessBlockPrincipal const&, ModuleCallingContext const*) = 0;
      virtual void doAccessInputProcessBlock(ProcessBlockPrincipal const&, ModuleCallingContext const*) = 0;
      virtual void doEndProcessBlock(ProcessBlockPrincipal const&, ModuleCallingContext const*) = 0;
      virtual void doBeginRun(RunTransitionInfo const&, ModuleCallingContext const*) = 0;
      virtual void doEndRun(RunTransitionInfo const&, ModuleCallingContext const*) = 0;
      virtual void doBeginLuminosityBlock(LumiTransitionInfo const&, ModuleCallingContext const*) = 0;
      virtual void doEndLuminosityBlock(LumiTransitionInfo const&, ModuleCallingContext const*) = 0;

      //For now, the following are just dummy implemenations with no ability for users to override
      void doRespondToOpenInputFile(FileBlock const& fb);
      void doRespondToCloseInputFile(FileBlock const& fb);
      void doRegisterThinnedAssociations(ProductRegistry const&, ThinnedAssociationsHelper&) {}

      bool hasAcquire() const { return false; }
      bool hasAccumulator() const { return false; }

      // ---------- member data --------------------------------
      void setModuleDescriptionPtr(EDAnalyzerBase* m);
      void setModuleDescription(ModuleDescription const& md) { moduleDescription_ = md; }
      ModuleDescription moduleDescription_;

      std::vector<EDAnalyzerBase*> m_streamModules;
    };
  }  // namespace stream
}  // namespace edm
#endif
