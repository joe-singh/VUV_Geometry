

double avgBackgroundEvents(TGraph* gr) {
 double a = gr->Eval(-95); 
 double b = gr->Eval(-90); 
 double c = gr->Eval(85); 
 double d = gr->Eval(90);
 
 return (a + b + c + d)/4.0;
  
}

void makeThicknessPlot(std::string plotName, std::string fname) {

  TFile* outf = new TFile(fname.c_str(), "UPDATE"); 

  TFile* f02 = new TFile("angularTPBSuprasil.root", "READ"); 
  //TFile* f05 = new TFile("angularTPB05umHighStats.root", "READ"); 
  TFile* f22 = new TFile("angularTPB22umHighStats.root", "READ"); 
  //TFile* f10 = new TFile("angularTPB10umHighStats.root", "READ"); 

  TGraph* gr02 = f02->Get("Graph"); 
  //TGraph* gr05 = f05->Get("No_Cylinder_Angular_SweepAt_0_degrees;1No_Cylinder_Angular_SweepAt_0_degrees"); 
  TGraph* gr22 = f22->Get("Graph"); 
  //TGraph* gr10 = f10->Get("No_Cylinder_Angular_SweepAt_0_degrees;1No_Cylinder_Angular_SweepAt_0_degrees");
   

  gr02->SetMarkerColor(kRed); 
  gr02->SetLineColor(kRed); 
  gr02->SetTitle("2.2 um Suprasil");
  gr02->SetMarkerStyle(21);    
  //std::cout << "Average Background 0.2um " << avgBackgroundEvents(gr02) << std:: endl;  
/*
  gr05->SetMarkerColor(kRed); 
  gr05->SetLineColor(kRed); 
  gr05->SetTitle("0.5um"); 
  gr05->SetMarkerStyle(21);  
  std::cout << "Average Background 0.5um " << avgBackgroundEvents(gr05) << std::endl; 
*/
  gr22->SetMarkerColor(kBlue);
  gr22->SetLineColor(kBlue);
  gr22->SetTitle("2.2um Vacuum"); 
  gr22->SetMarkerStyle(21);  
  //std::cout << "Average Background 2.2um " << avgBackgroundEvents(gr22) << std::endl; 

/*
  gr10->SetMarkerColor(kOrange);
  gr10->SetLineColor(kOrange);
  gr10->SetTitle("10um"); 
  gr10->SetMarkerStyle(21);  
  std::cout << "Average Background 10um " << avgBackgroundEvents(gr10) << std::endl; 
*/
  TMultiGraph* mg = new TMultiGraph(plotName.c_str(), plotName.c_str()); 
  mg->Add(gr02); 
  //mg->Add(gr05); 
  mg->Add(gr22); 
  //mg->Add(gr10); 

  TCanvas* c = new TCanvas(plotName.c_str(), plotName.c_str(), 1000, 600); 
  mg->Draw("AP");
  mg->GetXaxis()->SetTitle("Angle from Normal / Degrees"); 
  mg->GetYaxis()->SetTitle("Photon Count"); 
  c->BuildLegend();  
  
  outf->WriteTObject(c);   
}
