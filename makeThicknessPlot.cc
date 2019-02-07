#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLegend.h>

double avgBackgroundEvents(TGraph* gr) {
 double a = gr->Eval(-95); 
 double b = gr->Eval(-90); 
 double c = gr->Eval(85); 
 double d = gr->Eval(90);
 
 return (a + b + c + d)/4.0;
  
}

void makeThicknessPlot(std::string plotName, std::string fname) {
  
  gStyle->SetOptStat(0); 
  TFile* outf = new TFile(fname.c_str(), "UPDATE"); 

//  TFile* f02 = new TFile("incident_60/sweep_0.2_.root", "READ"); 
//  TFile* f05 = new TFile("incident_60/sweep_0.5_.root", "READ"); 
  TFile* f22 = new TFile("sweep_2.2_.root", "READ"); 
  TFile* f10 = new TFile("sweep_10.0_.root", "READ"); 

  //TH1F* gr02 = (TH1F*) f02->Get("angularDist_0"); 
  //TH1F* gr05 = (TH1F*) f05->Get("angularDist_0"); 
  TH1F* gr22 = (TH1F*) f22->Get("angularDist_0"); 
  TH1F* gr10 = (TH1F*) f10->Get("angularDist_0"); 
  //TGraph* gr10 = f10->Get("No_Cylinder_Angular_SweepAt_0_degrees;1No_Cylinder_Angular_SweepAt_0_degrees");
   
  
  
  /*gr02->SetMarkerColor(kRed); 
  gr02->SetLineColor(kRed); 
  gr02->SetTitle("1.25 in Photodiode");
  gr02->SetMarkerStyle(21);    
  //std::cout << "Average Background 0.2um " << avgBackgroundEvents(gr02) << std:: endl;  

  gr05->SetMarkerColor(kRed); 
  gr05->SetLineColor(kRed); 
  gr05->SetTitle("0.5um"); 
  gr05->SetMarkerStyle(21);  
  std::cout << "Average Background 0.5um " << avgBackgroundEvents(gr05) << std::endl; 

  gr22->SetMarkerColor(kBlue);
  gr22->SetLineColor(kBlue);
  gr22->SetTitle("0.625 in Photodiode"); 
  gr22->SetMarkerStyle(21);  
  //std::cout << "Average Background 2.2um " << avgBackgroundEvents(gr22) << std::endl; 


  gr10->SetMarkerColor(kOrange);
  gr10->SetLineColor(kOrange);
  gr10->SetTitle("10um"); 
  gr10->SetMarkerStyle(21);  
  std::cout << "Average Background 10um " << avgBackgroundEvents(gr10) << std::endl; 

  TMultiGraph* mg = new TMultiGraph(plotName.c_str(), plotName.c_str()); 
  mg->Add(gr02); 
  //mg->Add(gr05); 
  mg->Add(gr22); 
  //mg->Add(gr10); 
*/
  TCanvas* c = new TCanvas(plotName.c_str(), plotName.c_str(), 1000, 600); 
  //gr02->Sumw2(kTRUE); 
  gr22->Sumw2(kTRUE);
 // gr05->Sumw2(kTRUE); 
  gr10->Sumw2(kTRUE);

  //gr02->SetLineColor(kBlack);
  gr22->SetLineColor(kBlue);
  //gr05->SetLineColor(kRed); 
  gr10->SetLineColor(kViolet); 

  //gr02->SetMarkerColor(kBlack);
  gr22->SetMarkerColor(kBlue);
 // gr05->SetMarkerColor(kRed); 
  gr10->SetMarkerColor(kViolet); 

  //gr02->Scale(1e-6); 
  gr22->Scale(1e-6);
  //gr05->Scale(1e-6);
  gr10->Scale(1e-6);  
  
  //gr02->SetTitle("");  
  gr22->SetTitle("");  
  //gr05->SetTitle(""); 
  gr10->SetTitle(""); 
  
  //gr02->GetYaxis()->SetTitle("");  
  gr22->GetYaxis()->SetTitle("");  
  //gr05->GetYaxis()->SetTitle(""); 
  gr10->GetYaxis()->SetTitle(""); 

  //gr02->Draw("E1"); 
  //gr05->Draw("E1 same"); 
  gr22->Draw("E1"); 
  gr10->Draw("E1 same");  
 
  auto legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->SetFillStyle(0); 
  legend->SetBorderSize(0); 
 // legend->AddEntry(gr02,"0.2um","l");
 // legend->AddEntry(gr05,"0.5um", "l");  
  legend->AddEntry(gr22,"2.2um","l");
  legend->AddEntry(gr10, "10um", "l"); 
  legend->Draw();
  /*mg->Draw("AP");
  mg->GetXaxis()->SetTitle("Angle from Normal / Degrees"); 
  mg->GetYaxis()->SetTitle("Photon Count"); 
  c->BuildLegend();  
  */
  outf->WriteTObject(c);   
}
