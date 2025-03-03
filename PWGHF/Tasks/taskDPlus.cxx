// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file taskDPlus.cxx
/// \brief D± analysis task
/// \note Extended from taskD0
///
/// \author Fabio Catalano <fabio.catalano@cern.ch>, Politecnico and INFN Torino
/// \author Vít Kučera <vit.kucera@cern.ch>, CERN

#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "PWGHF/DataModel/HFSecondaryVertex.h"
#include "PWGHF/DataModel/HFCandidateSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;
using namespace o2::aod::hf_cand;
using namespace o2::aod::hf_cand_prong3;

#include "Framework/runDataProcessing.h"

/// D± analysis task
struct TaskDPlus {
  HistogramRegistry registry{
    "registry",
    {{"hPt", "3-prong candidates;candidate #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hPtProng0", "3-prong candidates;prong 0 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hPtProng1", "3-prong candidates;prong 1 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hPtProng2", "3-prong candidates;prong 2 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hCPARecSig", "3-prong candidates (matched);cosine of pointing angle;entries", {HistType::kTH1F, {{110, -1.1, 1.1}}}},
     {"hCPARecBg", "3-prong candidates (unmatched);cosine of pointing angle;entries", {HistType::kTH1F, {{110, -1.1, 1.1}}}},
     {"hEtaRecSig", "3-prong candidates (matched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}},
     {"hEtaRecBg", "3-prong candidates (unmatched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}},
     {"hEtaGen", "MC particles (matched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}}}};

  Configurable<int> d_selectionFlagDPlus{"d_selectionFlagDPlus", 7, "Selection Flag for DPlus"}; // 7 corresponds to topo+PID cuts
  Configurable<double> cutYCandMax{"cutYCandMax", -1., "max. cand. rapidity"};
  Configurable<std::vector<double>> bins{"pTBins", std::vector<double>{hf_cuts_dplus_topikpi::pTBins_v}, "pT bin limits"};

  void init(o2::framework::InitContext&)
  {
    auto vbins = (std::vector<double>)bins;
    registry.add("hMass", "3-prong candidates;inv. mass (#pi K #pi) (GeV/#it{c}^{2});entries", {HistType::kTH2F, {{350, 1.7, 2.05}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hEta", "3-prong candidates;candidate #it{#eta};entries", {HistType::kTH2F, {{100, -2., 2.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hCt", "3-prong candidates;proper lifetime (D^{#pm}) * #it{c} (cm);entries", {HistType::kTH2F, {{120, -20., 100.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hDecayLength", "3-prong candidates;decay length (cm);entries", {HistType::kTH2F, {{200, 0., 2.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hDecayLengthXY", "3-prong candidates;decay length xy (cm);entries", {HistType::kTH2F, {{200, 0., 2.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hNormalisedDecayLengthXY", "3-prong candidates;norm. decay length xy;entries", {HistType::kTH2F, {{80, 0., 80.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hCPA", "3-prong candidates;cos. pointing angle;entries", {HistType::kTH2F, {{110, -1.1, 1.1}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hCPAxy", "3-prong candidates;cos. pointing angle xy;entries", {HistType::kTH2F, {{110, -1.1, 1.1}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hImpactParameterXY", "3-prong candidates;impact parameter xy (cm);entries", {HistType::kTH2F, {{200, -1., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hMaxNormalisedDeltaIP", "3-prong candidates;norm. IP;entries", {HistType::kTH2F, {{200, -20., 20.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hImpactParameterProngSqSum", "3-prong candidates;squared sum of prong imp. par. (cm^{2});entries", {HistType::kTH2F, {{100, 0., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hDecayLengthError", "3-prong candidates;decay length error (cm);entries", {HistType::kTH2F, {{100, 0., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hDecayLengthXYError", "3-prong candidates;decay length xy error (cm);entries", {HistType::kTH2F, {{100, 0., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hImpactParameterError", "3-prong candidates;impact parameter error (cm);entries", {HistType::kTH2F, {{100, 0., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hd0Prong0", "3-prong candidates;prong 0 DCAxy to prim. vertex (cm);entries", {HistType::kTH2F, {{100, -1., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hd0Prong1", "3-prong candidates;prong 1 DCAxy to prim. vertex (cm);entries", {HistType::kTH2F, {{100, -1., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hd0Prong2", "3-prong candidates;prong 2 DCAxy to prim. vertex (cm);entries", {HistType::kTH2F, {{100, -1., 1.}, {vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtRecSig", "3-prong candidates (matched);#it{p}_{T}^{rec.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtRecSigPrompt", "3-prong candidates (matched, prompt);#it{p}_{T}^{rec.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtRecSigNonPrompt", "3-prong candidates (matched, non-prompt);#it{p}_{T}^{rec.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtRecBg", "3-prong candidates (unmatched);#it{p}_{T}^{rec.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtGen", "MC particles (matched);#it{p}_{T}^{gen.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtGenSig", "MC particles (matched);#it{p}_{T}^{gen.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtGenPrompt", "MC particles (matched, prompt);#it{p}_{T}^{gen.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtGenNonPrompt", "MC particles (matched, non-prompt);#it{p}_{T}^{gen.} (GeV/#it{c});entries", {HistType::kTH1F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}}});
    registry.add("hPtvsYRecSig_RecoPID", "3-prong candidates (RecoPID - matched);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigPrompt_RecoPID", "3-prong candidates (RecoPID - matched, prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigNonPrompt_RecoPID", "3-prong candidates (RecoPID - matched, non-prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSig_RecoTopol", "3-prong candidates (RecoTopol - matched);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigPrompt_RecoTopol", "3-prong candidates (RecoTopol - matched, prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigNonPrompt_RecoTopol", "3-prong candidates (RecoTopol - matched, non-prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSig_RecoSkim", "3-prong candidates (RecoSkim - matched);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigPrompt_RecoSkim", "3-prong candidates (RecoSkim - matched, prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYRecSigNonPrompt_RecoSkim", "3-prong candidates (RecoSkim - matched, non-prompt);#it{p}_{T}^{rec.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYGen", "MC particles (matched);#it{p}_{T}^{gen.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYGenPrompt", "MC particles (matched, prompt);#it{p}_{T}^{gen.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
    registry.add("hPtvsYGenNonPrompt", "MC particles (matched, non-prompt);#it{p}_{T}^{gen.}; #it{y}", {HistType::kTH2F, {{vbins, "#it{p}_{T} (GeV/#it{c})"}, {100, -5., 5.}}});
  }

  Partition<soa::Join<aod::HfCandProng3, aod::HFSelDplusToPiKPiCandidate>> selectedDPlusCandidates = aod::hf_selcandidate_dplus::isSelDplusToPiKPi >= d_selectionFlagDPlus;

  void process(soa::Join<aod::HfCandProng3, aod::HFSelDplusToPiKPiCandidate> const& candidates)
  {
    for (auto& candidate : selectedDPlusCandidates) {
      //not possible in Filter since expressions do not support binary operators
      if (!(candidate.hfflag() & 1 << DecayType::DPlusToPiKPi)) {
        continue;
      }
      if (cutYCandMax >= 0. && std::abs(YDPlus(candidate)) > cutYCandMax) {
        continue;
      }
      registry.fill(HIST("hMass"), InvMassDPlus(candidate), candidate.pt());
      registry.fill(HIST("hPt"), candidate.pt());
      registry.fill(HIST("hEta"), candidate.eta(), candidate.pt());
      registry.fill(HIST("hCt"), CtDPlus(candidate), candidate.pt());
      registry.fill(HIST("hDecayLength"), candidate.decayLength(), candidate.pt());
      registry.fill(HIST("hDecayLengthXY"), candidate.decayLengthXY(), candidate.pt());
      registry.fill(HIST("hNormalisedDecayLengthXY"), candidate.decayLengthXYNormalised(), candidate.pt());
      registry.fill(HIST("hCPA"), candidate.cpa(), candidate.pt());
      registry.fill(HIST("hCPAxy"), candidate.cpaXY(), candidate.pt());
      registry.fill(HIST("hImpactParameterXY"), candidate.impactParameterXY(), candidate.pt());
      registry.fill(HIST("hMaxNormalisedDeltaIP"), candidate.maxNormalisedDeltaIP(), candidate.pt());
      registry.fill(HIST("hImpactParameterProngSqSum"), candidate.impactParameterProngSqSum(), candidate.pt());
      registry.fill(HIST("hDecayLengthError"), candidate.errorDecayLength(), candidate.pt());
      registry.fill(HIST("hDecayLengthXYError"), candidate.errorDecayLengthXY(), candidate.pt());
      registry.fill(HIST("hImpactParameterError"), candidate.errorImpactParameter0(), candidate.pt());
      registry.fill(HIST("hImpactParameterError"), candidate.errorImpactParameter1(), candidate.pt());
      registry.fill(HIST("hImpactParameterError"), candidate.errorImpactParameter2(), candidate.pt());
      registry.fill(HIST("hPtProng0"), candidate.ptProng0());
      registry.fill(HIST("hPtProng1"), candidate.ptProng1());
      registry.fill(HIST("hPtProng2"), candidate.ptProng2());
      registry.fill(HIST("hd0Prong0"), candidate.impactParameter0(), candidate.pt());
      registry.fill(HIST("hd0Prong1"), candidate.impactParameter1(), candidate.pt());
      registry.fill(HIST("hd0Prong2"), candidate.impactParameter2(), candidate.pt());
    }
  }

  Partition<soa::Join<aod::HfCandProng3, aod::HFSelDplusToPiKPiCandidate, aod::HfCandProng3MCRec>> recoFlagDPlusCandidates = aod::hf_selcandidate_dplus::isSelDplusToPiKPi > 0;

  void processMC(soa::Join<aod::HfCandProng3, aod::HFSelDplusToPiKPiCandidate, aod::HfCandProng3MCRec> const& candidates,
                 soa::Join<aod::McParticles, aod::HfCandProng3MCGen> const& particlesMC, aod::BigTracksMC const& tracks)
  {
    // MC rec.
    //Printf("MC Candidates: %d", candidates.size());
    for (auto& candidate : recoFlagDPlusCandidates) {
      //not possible in Filter since expressions do not support binary operators
      if (!(candidate.hfflag() & 1 << DecayType::DPlusToPiKPi)) {
        continue;
      }
      if (cutYCandMax >= 0. && std::abs(YDPlus(candidate)) > cutYCandMax) {
        continue;
      }
      if (std::abs(candidate.flagMCMatchRec()) == 1 << DecayType::DPlusToPiKPi) {
        // Get the corresponding MC particle.
        auto indexMother = RecoDecay::getMother(particlesMC, candidate.index0_as<aod::BigTracksMC>().mcParticle_as<soa::Join<aod::McParticles, aod::HfCandProng3MCGen>>(), pdg::Code::kDPlus, true);
        auto particleMother = particlesMC.rawIteratorAt(indexMother);
        registry.fill(HIST("hPtGenSig"), particleMother.pt()); // gen. level pT
        auto ptRec = candidate.pt();
        auto yRec = YDPlus(candidate);
        registry.fill(HIST("hPtvsYRecSig_RecoSkim"), ptRec, yRec);
        if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoTopol)) {
          registry.fill(HIST("hPtvsYRecSig_RecoTopol"), ptRec, yRec);
        }
        if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoPID)) {
          registry.fill(HIST("hPtvsYRecSig_RecoPID"), ptRec, yRec);
        }
        if (candidate.isSelDplusToPiKPi() >= d_selectionFlagDPlus) {
          registry.fill(HIST("hPtRecSig"), ptRec); // rec. level pT
        }
        if (candidate.originMCRec() == OriginType::Prompt) {
          registry.fill(HIST("hPtvsYRecSigPrompt_RecoSkim"), ptRec, yRec);
          if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoTopol)) {
            registry.fill(HIST("hPtvsYRecSigPrompt_RecoTopol"), ptRec, yRec);
          }
          if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoPID)) {
            registry.fill(HIST("hPtvsYRecSigPrompt_RecoPID"), ptRec, yRec);
          }
          if (candidate.isSelDplusToPiKPi() >= d_selectionFlagDPlus) {
            registry.fill(HIST("hPtRecSigPrompt"), ptRec); // rec. level pT, prompt
          }
        } else {
          registry.fill(HIST("hPtvsYRecSigNonPrompt_RecoSkim"), ptRec, yRec);
          if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoTopol)) {
            registry.fill(HIST("hPtvsYRecSigNonPrompt_RecoTopol"), ptRec, yRec);
          }
          if (TESTBIT(candidate.isSelDplusToPiKPi(), aod::SelectionStep::RecoPID)) {
            registry.fill(HIST("hPtvsYRecSigNonPrompt_RecoPID"), ptRec, yRec);
          }
          if (candidate.isSelDplusToPiKPi() >= d_selectionFlagDPlus) {
            registry.fill(HIST("hPtRecSigNonPrompt"), ptRec); // rec. level pT, non-prompt
          }
        }
        registry.fill(HIST("hCPARecSig"), candidate.cpa());
        registry.fill(HIST("hEtaRecSig"), candidate.eta());
      } else {
        registry.fill(HIST("hPtRecBg"), candidate.pt());
        registry.fill(HIST("hCPARecBg"), candidate.cpa());
        registry.fill(HIST("hEtaRecBg"), candidate.eta());
      }
    }
    // MC gen.
    //Printf("MC Particles: %d", particlesMC.size());
    for (auto& particle : particlesMC) {
      if (std::abs(particle.flagMCMatchGen()) == 1 << DecayType::DPlusToPiKPi) {
        auto ptGen = particle.pt();
        auto yGen = RecoDecay::Y(array{particle.px(), particle.py(), particle.pz()}, RecoDecay::getMassPDG(particle.pdgCode()));
        if (cutYCandMax >= 0. && std::abs(yGen) > cutYCandMax) {
          continue;
        }
        registry.fill(HIST("hPtGen"), ptGen);
        registry.fill(HIST("hPtvsYGen"), ptGen, yGen);
        if (particle.originMCGen() == OriginType::Prompt) {
          registry.fill(HIST("hPtGenPrompt"), ptGen);
          registry.fill(HIST("hPtvsYGenPrompt"), ptGen, yGen);
        } else {
          registry.fill(HIST("hPtGenNonPrompt"), ptGen);
          registry.fill(HIST("hPtvsYGenNonPrompt"), ptGen, yGen);
        }
        registry.fill(HIST("hEtaGen"), particle.eta());
      }
    }
  }
  PROCESS_SWITCH(TaskDPlus, processMC, "Process MC", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<TaskDPlus>(cfgc, TaskName{"hf-task-dplus"})};
}
