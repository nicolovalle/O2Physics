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

/// \author Nicolo' Jacazio <nicolo.jacazio@cern.ch>, CERN
/// \author Alexander Kalweit <alexander.kalweit@cern.ch>, CERN

// O2 includes
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "AnalysisCore/MC.h"
#include "TLorentzVector.h"
#include "Framework/HistogramRegistry.h"
#include "ReconstructionDataFormats/PID.h"
#include "AnalysisCore/RecoDecay.h"
#include "DetectorsVertexing/DCAFitterN.h"
#include "AnalysisCore/trackUtilities.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct Alice3CDeuteron {
  Configurable<float> magField{"magField", 0.5, "Magnetic field"};
  Configurable<float> minRadius{"minRadius", -100, "Minimum decay radius"};
  Configurable<float> maxRadius{"maxRadius", 100, "Maximum decay radius"};
  Configurable<float> minMomPt{"minMomPt", -100, "Minimum pT of the mother"};
  Configurable<float> minKaonPt{"minKaonPt", -100, "Minimum pT of the pion daughter"};
  Configurable<float> minPionPt{"minPionPt", -100, "Minimum pT of the kaon daughter"};
  Configurable<float> minVtxContrib{"minVtxContrib", 3, "Minimum number of contributors to the primary vertex"};
  Configurable<float> minDca{"minDca", -100, "Minimum track DCA to the primary vertex"};
  Configurable<float> minDcaPion{"minDcaPion", -100, "Minimum DCA of the pion to the primary vertex"};
  Configurable<float> maxDca{"maxDca", 100, "Maximum track DCA to the primary vertex"};
  Configurable<float> minCpa{"minCpa", 0, "Minimum CPA"};
  HistogramRegistry histos{"Histos", {}, OutputObjHandlingPolicy::AnalysisObject};
  o2::vertexing::DCAFitterN<3> fitter;

  void init(InitContext&)
  {

    fitter.setBz(magField);
    fitter.setPropagateToPCA(true);
    fitter.setMaxR(1.);
    fitter.setMinParamChange(1e-3);
    fitter.setMinRelChi2Change(0.9);
    fitter.setMaxDZIni(1e9);
    fitter.setMaxChi2(1e9);
    fitter.setUseAbsDCA(true);

    const AxisSpec axisInvMass{100, 2.5, 4, "Inv. Mass_{c-d}"};
    const AxisSpec axisDecayRadius{2000, 0, 0.1, "Decay radius"};
    const AxisSpec axisDecayRadiusReso{2000, -0.01, 0.01, "Decay radius resolution"};
    const AxisSpec axisPionProdRadiusXY{2000, 0, 0.01, "Pion production radius in xy"};
    const AxisSpec axisDca{5000, -0.01, 0.01, "DCA to secondary"};
    const AxisSpec axisDcaXY{5000, -0.05, 0.05, "DCA_{xy}"};
    const AxisSpec axisDcaXYProd{5000, -5e-6, 5e-6, "DCA_{xy} product"};
    const AxisSpec axisDcaZ{5000, -0.05, 0.05, "DCA_{z}"};
    const AxisSpec axisDcaZProd{5000, -5e-6, 5e-6, "DCA_{z} product"};
    const AxisSpec axisPt{100, 0, 10, "#it{p}_{T} (GeV/#it{c})"};
    const AxisSpec axisVtxX{100, -0.1, 0.1, "Vtx_{X}"};
    const AxisSpec axisVtxY{100, -0.1, 0.1, "Vtx_{Y}"};
    const AxisSpec axisVtxZ{100, -0.1, 0.1, "Vtx_{Z}"};
    const AxisSpec axisCPA{4000, -1.1, 1.1, "CPA"};
    const TString tit = Form(" [%.6f, %.6f] R [%.6f, %.6f] DCA ",
                             minRadius.value, maxRadius.value,
                             minDca.value, maxDca.value);

    histos.add("event/candcuts", "cuts", kTH1D, {{10, 0, 10}});
    auto hcut = histos.get<TH1>(HIST("event/candcuts"));
    hcut->GetXaxis()->SetBinLabel(1, "magField");
    hcut->GetXaxis()->SetBinLabel(2, "minRadius");
    hcut->GetXaxis()->SetBinLabel(3, "maxRadius");
    hcut->GetXaxis()->SetBinLabel(4, "minMomPt");
    hcut->GetXaxis()->SetBinLabel(5, "minKaonPt");
    hcut->GetXaxis()->SetBinLabel(6, "minPionPt");
    hcut->GetXaxis()->SetBinLabel(7, "minVtxContrib");
    hcut->GetXaxis()->SetBinLabel(8, "minDca");
    hcut->GetXaxis()->SetBinLabel(9, "maxDca");

    histos.add("event/cuts", "cuts", kTH1D, {{10, 0, 10}});
    hcut = histos.get<TH1>(HIST("event/cuts"));
    hcut->GetXaxis()->SetBinLabel(1, "magField");
    hcut->GetXaxis()->SetBinLabel(2, "minRadius");
    hcut->GetXaxis()->SetBinLabel(3, "maxRadius");
    hcut->GetXaxis()->SetBinLabel(4, "minMomPt");
    hcut->GetXaxis()->SetBinLabel(5, "minKaonPt");
    hcut->GetXaxis()->SetBinLabel(6, "minPionPt");
    hcut->GetXaxis()->SetBinLabel(7, "minVtxContrib");
    hcut->GetXaxis()->SetBinLabel(8, "minDca");
    hcut->GetXaxis()->SetBinLabel(9, "maxDca");

    hcut->SetBinContent(1, magField);
    hcut->SetBinContent(2, minRadius);
    hcut->SetBinContent(3, maxRadius);
    hcut->SetBinContent(4, minMomPt);
    hcut->SetBinContent(5, minKaonPt);
    hcut->SetBinContent(6, minPionPt);
    hcut->SetBinContent(7, minVtxContrib);
    hcut->SetBinContent(8, minDca);
    hcut->SetBinContent(9, maxDca);

    histos.add("event/vtxX", "vtxX", kTH1D, {axisVtxX});
    histos.add("event/vtxY", "vtxY", kTH1D, {axisVtxY});
    histos.add("event/vtxZ", "vtxZ", kTH1D, {axisVtxZ});
    histos.add("event/mcvtxX", "mcvtxX", kTH1D, {axisVtxX});
    histos.add("event/mcvtxY", "mcvtxY", kTH1D, {axisVtxY});
    histos.add("event/mcvtxZ", "mcvtxZ", kTH1D, {axisVtxZ});
    histos.add("event/candperdeuteron", "candperdeuteron", kTH1D, {{1000, 0, 10000}});
    histos.add("event/particles", "particles", kTH1D, {{3, 0.5, 3.5}});
    hcut = histos.get<TH1>(HIST("event/particles"));
    hcut->GetXaxis()->SetBinLabel(1, "d");
    hcut->GetXaxis()->SetBinLabel(2, "K");
    hcut->GetXaxis()->SetBinLabel(3, "#pi");
    histos.add("event/multiplicity", "multiplicity", kTH1D, {{1000, 0, 10000}});

#define MakeHistos(tag)                                                                        \
  histos.add(tag "/cpa", "cpa" + tit, kTH1D, {axisCPA});                                       \
  histos.add(tag "/invmass", "invmass" + tit, kTH1D, {axisInvMass});                           \
  histos.add(tag "/decayradius", "decayradius" + tit, kTH1D, {axisDecayRadius});               \
  histos.add(tag "/decayradiusResoX", "decayradiusResoX" + tit, kTH1D, {axisDecayRadiusReso}); \
  histos.add(tag "/decayradiusResoY", "decayradiusResoY" + tit, kTH1D, {axisDecayRadiusReso}); \
  histos.add(tag "/decayradiusResoZ", "decayradiusResoZ" + tit, kTH1D, {axisDecayRadiusReso}); \
  histos.add(tag "/decayradiusReso", "decayradiusReso" + tit, kTH1D, {axisDecayRadiusReso});   \
  histos.add(tag "/radius3xy", "radius3xy" + tit, kTH1D, {axisPionProdRadiusXY});              \
  histos.add(tag "/decaydca0", "decaydca0" + tit, kTH1D, {axisDca});                           \
  histos.add(tag "/decaydca1", "decaydca1" + tit, kTH1D, {axisDca});                           \
  histos.add(tag "/dcaxy1", "dcaxy1 Deuteron" + tit, kTH1D, {axisDcaXY});                      \
  histos.add(tag "/dcaxy2", "dcaxy2 Kaon" + tit, kTH1D, {axisDcaXY});                          \
  histos.add(tag "/dcaxy3", "dcaxy3 Pion" + tit, kTH1D, {axisDcaXY});                          \
  histos.add(tag "/dcaxy1xdcaxy2", "dcaxy1xdcaxy2" + tit, kTH1D, {axisDcaXYProd});             \
  histos.add(tag "/dcaxy3xdcaxy2", "dcaxy3xdcaxy2" + tit, kTH1D, {axisDcaXYProd});             \
  histos.add(tag "/dcaz1", "dcaz1 Deuteron" + tit, kTH1D, {axisDcaZ});                         \
  histos.add(tag "/dcaz2", "dcaz2 Kaon" + tit, kTH1D, {axisDcaZ});                             \
  histos.add(tag "/dcaz3", "dcaz3 Pion" + tit, kTH1D, {axisDcaZ});                             \
  histos.add(tag "/dcaz1xdcaz2", "dcaz1xdcaz2" + tit, kTH1D, {axisDcaZProd});                  \
  histos.add(tag "/dcaz3xdcaz2", "dcaz3xdcaz2" + tit, kTH1D, {axisDcaZProd});                  \
  histos.add(tag "/pt1", "pt1 Deuteron" + tit, kTH1D, {axisPt});                               \
  histos.add(tag "/pt2", "pt2 Kaon" + tit, kTH1D, {axisPt});                                   \
  histos.add(tag "/pt3", "pt3 Pion" + tit, kTH1D, {axisPt});                                   \
  histos.add(tag "/ptmom", "ptmom" + tit, kTH1D, {axisPt});                                    \
  histos.add(tag "/pmom", "pmom" + tit, kTH1D, {axisPt});

    MakeHistos("sig");
    MakeHistos("bkg");
    MakeHistos("signocut");
    MakeHistos("bkgnocut");
    MakeHistos("sigcut");
    MakeHistos("bkgcut");

#undef MakeHistos
  }

  void process(const soa::Join<o2::aod::Collisions, o2::aod::McCollisionLabels>::iterator& coll,
               const o2::aod::McCollisions& Mccoll,
               const soa::Join<o2::aod::Tracks, o2::aod::McTrackLabels, o2::aod::TracksExtra, o2::aod::TracksCov>& tracks,
               const aod::McParticles& mcParticles)
  {

    histos.fill(HIST("event/vtxX"), coll.posX());
    histos.fill(HIST("event/vtxY"), coll.posY());
    histos.fill(HIST("event/vtxZ"), coll.posZ());
    histos.fill(HIST("event/mcvtxX"), coll.mcCollision().posX());
    histos.fill(HIST("event/mcvtxY"), coll.mcCollision().posY());
    histos.fill(HIST("event/mcvtxZ"), coll.mcCollision().posZ());
    const math_utils::Point3D<float> collPos{coll.mcCollision().posX(),
                                             coll.mcCollision().posY(),
                                             coll.mcCollision().posZ()};
    // const math_utils::Point3D<float> collPos{coll.posX(),
    //                                          coll.posY(),
    //                                          coll.posZ()};
    // for (const auto& mcParticle : mcParticles) {
    //   // ParticlesOfInterest.push_back(mcParticle.globalIndex());
    // }
    int ntrks = 0;
    for (const auto& t : tracks) {
      if (t.mcParticle().pdgCode() == 1000010020) {
        histos.fill(HIST("event/particles"), 1);
      } else if (t.mcParticle().pdgCode() == -321) {
        histos.fill(HIST("event/particles"), 2);
      } else if (t.mcParticle().pdgCode() == 211) {
        histos.fill(HIST("event/particles"), 3);
      }
      ntrks++;
    }
    histos.fill(HIST("event/multiplicity"), ntrks);

    std::array<float, 2> dca1{1e10f, 1e10f};
    std::array<float, 2> dca2{1e10f, 1e10f};
    std::array<float, 2> dca3{1e10f, 1e10f};
    for (const auto& track1 : tracks) {
      const auto index1 = track1.globalIndex();
      int ncand = 0;
      if (track1.mcParticle().pdgCode() != 1000010020) {
        continue;
      }
      if (!getTrackPar(track1).propagateParamToDCA(collPos,
                                                   magField * 10.f, &dca1, 100.)) {
        continue;
      }

      for (const auto& track2 : tracks) {
        const auto index2 = track2.globalIndex();
        if (index1 == index2) {
          continue;
        }
        if (track2.mcParticle().pdgCode() != -321) {
          continue;
        }

        if (!getTrackPar(track2).propagateParamToDCA(collPos,
                                                     magField * 10.f, &dca2, 100.)) {
          continue;
        }

        for (const auto& track3 : tracks) {

          const auto index3 = track3.globalIndex();
          if (index2 == index3) {
            continue;
          }
          if (index1 == index3) {
            continue;
          }
          if (track3.mcParticle().pdgCode() != 211) {
            continue;
          }
          bool iscut = false;
          if (abs(dca1[0]) < minDca || abs(dca1[1]) < minDca) {
            iscut = true;
          }
          if (abs(dca1[0]) > maxDca || abs(dca1[1]) > maxDca) {
            iscut = true;
          }

          if (abs(dca2[0]) < minDca || abs(dca2[1]) < minDca) {
            iscut = true;
          }
          if (abs(dca2[0]) > maxDca || abs(dca2[1]) > maxDca) {
            iscut = true;
          }
          if (abs(dca2[0]) < minDcaPion || abs(dca2[1]) < minDcaPion) {
            iscut = true;
          }
          if (track2.pt() < minKaonPt) {
            iscut = true;
          }
          if (track3.pt() < minPionPt) {
            iscut = true;
          }
          if (!getTrackPar(track3).propagateParamToDCA(collPos,
                                                       magField * 10.f, &dca3, 100.)) {
            continue;
          }

          if (abs(dca3[0]) < minDca || abs(dca3[1]) < minDca) {
            iscut = true;
          }
          if (abs(dca3[0]) > maxDca || abs(dca3[1]) > maxDca) {
            iscut = true;
          }

          const auto mother1 = track1.mcParticle().mother0_as<aod::McParticles>();
          const auto mother2 = track2.mcParticle().mother0_as<aod::McParticles>();
          const auto mother3 = track3.mcParticle().mother0_as<aod::McParticles>();
          bool issig = true;
          if (mother1 != mother2) {
            issig = false;
          }
          if (mother1 != mother3) {
            issig = false;
          }

          auto pc1 = getTrackParCov(track1);
          auto pc2 = getTrackParCov(track2);
          auto pc3 = getTrackParCov(track3);
          if (pc1.getSigmaY2() * pc1.getSigmaZ2() - pc1.getSigmaZY() * pc1.getSigmaZY() < 0.) {
            Printf("Track 1 has issues");
            continue;
          }
          if (pc2.getSigmaY2() * pc2.getSigmaZ2() - pc2.getSigmaZY() * pc2.getSigmaZY() < 0.) {
            Printf("Track 2 has issues");
            continue;
          }
          if (pc3.getSigmaY2() * pc3.getSigmaZ2() - pc3.getSigmaZY() * pc3.getSigmaZY() < 0.) {
            Printf("Track 3 has issues");
            continue;
          }
          const int status = fitter.process(pc1, pc2, pc3);
          if (status == 0) {
            continue;
          }

          TLorentzVector v1{};
          v1.SetPtEtaPhiM(track1.pt(), track1.eta(), track1.phi(), 1.8756129);

          TLorentzVector v2{};
          v2.SetPtEtaPhiM(track2.pt(), track2.eta(), track2.phi(), 0.493677);

          TLorentzVector v3{};
          v3.SetPtEtaPhiM(track3.pt(), track3.eta(), track3.phi(), 0.139570);
          v1 += v2;
          v1 += v3;
          if (v1.Pt() < minMomPt) {
            iscut = true;
          }

          // fitter.propagateTracksToVertex();
          const auto& secVtx = fitter.getPCACandidate();
          const float decay_radius = sqrt(secVtx[0] * secVtx[0] + secVtx[1] * secVtx[1] + secVtx[2] * secVtx[2]);
          if (decay_radius < minRadius) {
            iscut = true;
          }
          if (decay_radius > maxRadius) {
            iscut = true;
          }

          const float magMom = sqrt(v1.Px() * v1.Px() + v1.Py() * v1.Py() + v1.Pz() * v1.Pz());
          const float CPA = (v1.Px() * secVtx[0] + v1.Py() * secVtx[1] + v1.Pz() * secVtx[2]) / (decay_radius * magMom);
          if (abs(CPA) < minCpa) {
            iscut = true;
          }

          const float vx = mother1.vx();
          const float vy = mother1.vy();
          const float vz = mother1.vz();
          const float rmc = sqrt((secVtx[0] - vx) * (secVtx[0] - vx) + (secVtx[1] - vy) * (secVtx[1] - vy) + (secVtx[2] - vz) * (secVtx[2] - vz));
          ncand++;
          const float radius3xy = sqrt((track3.mcParticle().vx() - coll.mcCollision().posX()) * (track3.mcParticle().vx() - coll.mcCollision().posX()) +
                                       (track3.mcParticle().vy() - coll.mcCollision().posY()) * (track3.mcParticle().vy() - coll.mcCollision().posY()));

#define FillHistos(tag)                                                              \
  histos.fill(HIST(tag "/cpa"), CPA);                                                \
  histos.fill(HIST(tag "/invmass"), v1.M());                                         \
  histos.fill(HIST(tag "/decayradius"), decay_radius);                               \
  histos.fill(HIST(tag "/decayradiusResoX"), secVtx[0] - vx);                        \
  histos.fill(HIST(tag "/decayradiusResoY"), secVtx[1] - vy);                        \
  histos.fill(HIST(tag "/decayradiusResoZ"), secVtx[2] - vz);                        \
  histos.fill(HIST(tag "/radius3xy"), radius3xy);                                    \
  histos.fill(HIST(tag "/decayradiusReso"), rmc);                                    \
  histos.fill(HIST(tag "/decaydca0"), TMath::Sqrt(fitter.getChi2AtPCACandidate(0))); \
  histos.fill(HIST(tag "/decaydca1"), TMath::Sqrt(fitter.getChi2AtPCACandidate(1))); \
  histos.fill(HIST(tag "/dcaxy1"), dca1[0]);                                         \
  histos.fill(HIST(tag "/dcaz1"), dca1[1]);                                          \
  histos.fill(HIST(tag "/dcaxy2"), dca2[0]);                                         \
  histos.fill(HIST(tag "/dcaz2"), dca2[1]);                                          \
  histos.fill(HIST(tag "/dcaxy3"), dca3[0]);                                         \
  histos.fill(HIST(tag "/dcaz3"), dca3[1]);                                          \
  histos.fill(HIST(tag "/dcaxy1xdcaxy2"), dca1[0] * dca2[0]);                        \
  histos.fill(HIST(tag "/dcaz1xdcaz2"), dca1[1] * dca2[1]);                          \
  histos.fill(HIST(tag "/dcaxy3xdcaxy2"), dca3[0] * dca2[0]);                        \
  histos.fill(HIST(tag "/dcaz3xdcaz2"), dca3[1] * dca2[1]);                          \
  histos.fill(HIST(tag "/pt1"), track1.pt());                                        \
  histos.fill(HIST(tag "/pt2"), track2.pt());                                        \
  histos.fill(HIST(tag "/pt3"), track3.pt());                                        \
  histos.fill(HIST(tag "/ptmom"), v1.Pt());                                          \
  histos.fill(HIST(tag "/pmom"), v1.P());

          if (issig) {
            FillHistos("signocut");
          } else {
            FillHistos("bkgnocut");
          }
          if (iscut) {
            if (issig) {
              FillHistos("sigcut");
            } else {
              FillHistos("bkgcut");
            }
            continue;
          }

          if (issig) {
            FillHistos("sig");
          } else {
            FillHistos("bkg");
          }
#undef FillHistos

          // fitterCasc.getTrack(1).getPxPyPzGlo(pvecbach);
        } // End loop on pions
      }   // End loop on kaons
      histos.fill(HIST("event/candperdeuteron"), ncand);
    } // End loop on deuterons
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<Alice3CDeuteron>(cfgc)};
}