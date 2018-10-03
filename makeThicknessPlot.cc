

double avgBackgroundEvents(TGraph* gr) {
 double a = gr->Eval(-95); 
 double b = gr->Eval(-90); 
 double c = gr->Eval(85); 
 double d = gr->Eval(90);
 
 return (a + b + c + d)/4.0;
  
}

void makeThicknessPlot(std::string plotName, std::string fname) {

  TFile* outf = new TFile(fname.c_str(), "UPDATE"); 

  TFile* f02 = new TFile("suprasilTPB.root", "READ"); 
  //TFile* f05 = new TFile("angularTPB05umHighStats.root", "READ"); 
  TFile* f22 = new TFile("noSuprasilTPB.root", "READ"); 

  TH1F* gr02 = f02->Get("angularDist"); 
  //TGraph* gr05 = f05->Get("No_Cylinder_Angular_SweepAt_0_degrees;1No_Cylinder_Angular_SweepAt_0_degrees"); 
  TH1F* gr22 = f22->Get("angularDist"); 
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
  gr02->Sumw2(kTRUE); 
  gr22->Sumw2(kTRUE);

  gr02->SetLineColor(kRed);
  gr22->SetLineColor(kBlue);
  
  gr02->SetMarkerColor(kRed);
  gr22->SetMarkerColor(kBlue);

  gr02->Scale(1e-6); 
  gr22->Scale(1e-6); 
  
  gr02->GetYaxis()->SetTitle("");  
  gr22->GetYaxis()->SetTitle("");  
  gr22->Draw("E1");
  gr02->Draw("E1 same"); 
   auto legend = new TLegend(0.1,0.7,0.48,0.9);
   legend->AddEntry(gr02,"Suprasil","l");
   legend->AddEntry(gr22,"No Suprasil","l");
   legend->Draw();
  /*mg->Draw("AP");
  mg->GetXaxis()->SetTitle("Angle from Normal / Degrees"); 
  mg->GetYaxis()->SetTitle("Photon Count"); 
  c->BuildLegend();  
  */
  outf->WriteTObject(c);   
}
