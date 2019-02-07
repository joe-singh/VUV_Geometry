#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLegend.h>

void makeReflectancePlots(std::string inf, std::string outf) {

  TFile* plots = new TFile(inf.c_str(), "READ"); 
  TFile* outfile = new TFile(outf.c_str(), "RECREATE"); 
  
  TH1F* h_0 = plots->Get("angularDist_0"); 
  TH1F* h_15 = plots->Get("angularDist_15"); 
  TH1F* h_30 = plots->Get("angularDist_30"); 
  TH1F* h_45 = plots->Get("angularDist_45"); 
  TH1F* h_60 = plots->Get("angularDist_60"); 
  TH1F* h_75 = plots->Get("angularDist_75"); 

  h_0->Sumw2(kTRUE);
  h_15->Sumw2(kTRUE); 
  h_30->Sumw2(kTRUE);
  h_45->Sumw2(kTRUE); 
  h_60->Sumw2(kTRUE);
  h_75->Sumw2(kTRUE); 

  h_0->Scale(1e-6); 
  h_15->Scale(1e-6); 
  h_30->Scale(1e-6); 
  h_45->Scale(1e-6); 
  h_60->Scale(1e-6);
  h_75->Scale(1e-6); 
   
  TCanvas* c_0 = new TCanvas("c_0", "c_0", 1000, 600); 
  h_0->Draw("E1");
  TCanvas* c_15 = new TCanvas("c_15", "c_15", 1000, 600); 
  h_15->Draw("E1");
  TCanvas* c_30 = new TCanvas("c_30", "c_30", 1000, 600); 
  h_30->Draw("E1");
  TCanvas* c_45 = new TCanvas("c_45", "c_45", 1000, 600); 
  h_45->Draw("E1");
  TCanvas* c_60 = new TCanvas("c_60", "c_60", 1000, 600); 
  h_60->Draw("E1");
  TCanvas* c_75 = new TCanvas("c_75", "c_75", 1000, 600); 
  h_75->Draw("E1");
  
  outfile->WriteTObject(c_0);
  outfile->WriteTObject(c_15);
  outfile->WriteTObject(c_30);
  outfile->WriteTObject(c_45);
  outfile->WriteTObject(c_60);
  outfile->WriteTObject(c_75);
  
  plots->Close(); 
  outfile->Close();
 
  
}
