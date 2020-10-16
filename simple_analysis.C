////////////////////////////////////////////////////////////////////////////////////////
// Simple code to read variables from a ROOT tree and apply simple cuts event by event
// Run with: root -l simple_analysis.C+
// Try to add your own cuts and plots !
///////////////////////////////////////////////////////////////////////////////////////


//C++ includes
#include <iostream>

//ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

using std::cout;
using std::endl;


bool in_fv(float vtx_x, float vtx_y, float vtx_z)
{
  // Require the vertex is in the fiducial volume.

  float x1(10), x2(243), y1(-106.5), y2(106.5), z1(20), z2(986);

  if (vtx_x   >= x1 && vtx_x <= x2
      && vtx_y   >= y1 && vtx_y <= y2
      && vtx_z   >= z1 && vtx_z <= z2) {
    return true; // pass
  }

  return false; // fail
}

void simple_analysis()
{
  //Uncomment to have no plot shown interactively
  //gROOT->SetBatch(kTRUE);

  // Create histograms.

  // 1D histograms.
  TH1F h_ene_all("h_showers", "All events", 100, 0, 10);
  TH1F h_ene_cuts("h_ene_cuts", "Selected events", 100, 0, 10);

  // 2D histograms.
  TH2F h2d_vtx_yz("h2d_vtx_xy", "", 100, -150, 150, 100, -100, 1100);

  TFile inFile("/Users/anavrera/MicroBooNE/data/prodgenie_bnb_nu_uboone_overlay_mcc9.1_v08_00_00_26_filter_run3_reco2_G_reco2.root");

  // Create a TTreeReader for the tree, for instance by passing the
  TTreeReader myReader("nuselection/NeutrinoSelectionFilter", &inFile);

  // Access the values in the tree.
  // Be careful, the template type passed to the TTreeReaderValue has to match
  // the type of the variable in the TTree.

  // Truth information.
  TTreeReaderValue<Int_t> n_showers(myReader, "n_showers"); // number of showers in the event
  TTreeReaderValue<Int_t> nu_pdg(myReader, "nu_pdg"); // neutrino particle code

  TTreeReaderValue<Float_t> nu_e(myReader, "nu_e"); // true neutrino energy

  // Reconstructed information.
  TTreeReaderValue<Float_t> reco_e(myReader, "reco_e"); // Reconstructed neutrino energy

  // Vertex information.
  TTreeReaderValue<Float_t> reco_vtx_x(myReader, "reco_nu_vtx_sce_x"); // reconstructed vertex x-position
  TTreeReaderValue<Float_t> reco_vtx_y(myReader, "reco_nu_vtx_sce_y"); // reconstructed vertex y-position
  TTreeReaderValue<Float_t> reco_vtx_z(myReader, "reco_nu_vtx_sce_z"); // reconstructed vertex z-position

  // Shower information.
  TTreeReaderValue<Int_t> shr_pdg(myReader, "shr_bkt_pdg"); // PDG code of the particle associated with the leading shower
  TTreeReaderValue<Float_t> shr_energy(myReader, "shr_energy"); // energy of the leading shower

  // Total number of events in the file.
  const int nentries = myReader.GetEntries();

  int passed = 0;
  int true_nues = 0;
  int all_nues  = 0;

  while (myReader.Next()) { // Loop on the events of the tree

    // Access the tree variables as if they were iterators (note the '*' in front of them):
    h_ene_all.Fill(*nu_e);
    h2d_vtx_yz.Fill(*reco_vtx_y, *reco_vtx_z);

    if (*nu_pdg == 12 || *nu_pdg == -12) ++all_nues;

    // Apply slection cuts.
    if (*n_showers == 0) continue; // request at least 1 shower
    if (!in_fv(*reco_vtx_x, *reco_vtx_x, *reco_vtx_z)) continue; // request the vertex to be in fiducial volume

    // Fill the histograms with events that pass the cuts.
    h_ene_cuts.Fill(*nu_e);
    ++passed;

    // Request the neutrino PDG code the one for nue/nuebar to count nues.
    if (*nu_pdg == 12 || *nu_pdg == -12) ++true_nues;

    // Feel free to add your own cuts!
  }


  // Compute some numbers.
  float eff_nue = true_nues * 100. / all_nues;
  float purity  = true_nues * 100. / passed;


  // Print some information.
  cout << passed << " events passed the cuts (" << true_nues << " nues)" << endl;
  cout << "Efficiency: " << eff_nue <<"%" << endl;
  cout << "Purity: " << purity << "%" << endl;


  // Plot the histograms.

  TCanvas c_nu_ene;
  h_ene_all.Draw();

  h_ene_cuts.SetLineColor(2);
  h_ene_cuts.Draw("same");
  c_nu_ene.SaveAs("neutrino_energy.eps");

  TCanvas c2d_vtx;
  h2d_vtx_yz.GetXaxis()->SetTitle("Vertex y position [cm]");
  h2d_vtx_yz.GetYaxis()->SetTitle("Vertex z position [cm]");

  h2d_vtx_yz.Draw("colz");

  c2d_vtx.SaveAs("vertex_position_yz.eps");

}
