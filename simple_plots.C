////////////////////////////////////////////
// Simple code to plot a variable from a ROOT tree and apply simple cuts
// Run with command: root -b -l simple_plots.C
///////////////////////////////////////////// 

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"


void simple_plots(){

  gStyle->SetOptStat(0);


  //Open file and get the TTree containing the events
  TFile inFile("/Users/anavrera/MicroBooNE/data/prodgenie_bnb_nu_uboone_overlay_mcc9.1_v08_00_00_26_filter_run3_reco2_G_reco2.root");
  TTree *events = (TTree*) inFile->Get("nuselection/NeutrinoSelectionFilter");
  

  //Create the histograms that will be filled later
  TH1F h_nu_ene("h_nu_ene", "Neutrino energy", 100, 0, 10);
  TH1F h_nu_ene_nue("h_nu_ene_nue", "Neutrino energy (nue only)", 100, 0, 10);


  //Set the parameters of the histogram
  h_nu_ene.GetXaxis()->SetTitle("Neutrino energy [GeV]"); // x-axis title
  h_nu_ene_nue.SetLineColor(kRed); // Color
  
  //Create a TCanvas to display plots
  TCanvas c_ene("c_ene", "c_ene", 1);

  //Project nu energy variable (nue) into the h_nu_ene histogram with TTree:Draw().
  events->Draw("nu_e>>h_nu_ene", "", "e0");

  //Save TCanvas as a figure
  c_ene.SaveAs("neutrino_energy.png");
  

  //Create another canvas
  TCanvas c_ene_nue("c_ene_nue", "c_ene_nue", 1);

  //Set y axis to log scale
  c_ene_nue.SetLogy();

  //Plot previous histogram
  h_nu_ene.Draw("e0");

  //plot nu energy again, but request pdg code to be ±12 (nue or nuebar). Option "same" requeste the second histogram to be plotted on the same canvas
  events->Draw("nu_e>>h_nu_ene_nue","nu_pdg == 12 || nu_pdg == -12","e0 same");

  //Create a legend for the plot
  c_ene_nue.BuildLegend();

  //Save again
  c_ene_nue.SaveAs("neutrino_energy_nue.png");

}
