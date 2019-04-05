double* addGraphToMultiGraph(TFile* f, std::string fname, TMultiGraph* mg, int color, TCanvas* c) {
  TCanvas* activeCanv = c;
  TGraphErrors* g = (TGraphErrors*) f->Get("peakHeightVsSampleAngle"); 

  double* y_vals = g->GetY(); 
  int len = g->GetN(); 

  mg->Add(g); 
  g->SetLineColor(color); 
  g->SetMarkerColor(color); 
  g->SetTitle(fname.c_str());
  TF1* fn = g->GetListOfFunctions()->FindObject("fitFunction"); 
  fn->SetLineColor(color);
  double amplitude = fn->GetParameter(0); 
  double y_threshold = 0.9*amplitude;
  double x_threshold = fn->GetX(y_threshold, 220, 240); 
  
  double coord[4] = {x_threshold,g->GetYaxis()->GetXmin(), x_threshold, y_threshold};
  return coord;
}



TMultiGraph* makeDropoffPlotMain(std::vector<std::string> fnames, std::string title) {

  TMultiGraph* mg = new TMultiGraph();
  TCanvas* c = new TCanvas(title.c_str(), title.c_str(), 1000, 600); 
  
  for (int i = 0; i < fnames.size(); i++) {
   
   std::string fname = fnames[i];
   TFile* f = new TFile(fname.c_str(), "READ"); 
   double coords[4] = addGraphToMultiGraph(f, fname, mg, i+1, c); 
   TLine threshold_vertical(coords[0],coords[1], coords[0], coords[2]);
   threshold_vertical.SetLineStyle(2); 
   threshold_vertical.SetLineColor(i+1);
//   threshold_vertical.Draw("same"); 
  } 
  
  mg->SetTitle(title.c_str());
  mg->Draw("ACP");
  mg->GetXaxis()->SetTitle("Incidence Angle / degrees"); 
  mg->GetYaxis()->SetTitle("Number of Photons"); 
  c->Modified();
  gPad->BuildLegend(0.75,0.75,0.95,0.95,""); 
  
  return mg;
}

double* normalise(double* array,  const int len) {
  static const int l = len;
  double normalised[l];  
  double normalisation = array[0];
  std::cout << len << std::endl;
  for (int i = 0; i < l; i++) {
    normalised[i] = array[i]/normalisation;
    std::cout << normalised[i] << std::endl;
  }
 
 return normalised;
}

void makeDropoffPlot() {

  std::vector<std::string> sampleholder_thickness;
  sampleholder_thickness.push_back("dropoff_problem/sweep_al_sampleholder.root"); 
  sampleholder_thickness.push_back("dropoff_problem/sweep_thickness_0.315.root"); 
  sampleholder_thickness.push_back("dropoff_problem/sweep_thickness_0.298.root"); 
  sampleholder_thickness.push_back("dropoff_problem/sweep_thickness_0.28.root"); 
  sampleholder_thickness.push_back("dropoff_problem/sweep_thickness_0.245.root"); 
  sampleholder_thickness.push_back("dropoff_problem/sweep_thickness_0.21.root"); 
  makeDropoffPlotMain(sampleholder_thickness, "Varying Sampleholder Thickness");


  std::vector<std::string> mirror_thickness;
  mirror_thickness.push_back("dropoff_problem/sweep_al_sampleholder.root");
  mirror_thickness.push_back("dropoff_problem/mirror_0.23.root");
  mirror_thickness.push_back("dropoff_problem/mirror_0.138.root");
  mirror_thickness.push_back("dropoff_problem/mirror_0.092.root");
  makeDropoffPlotMain(mirror_thickness, "Varying Mirror Thickness (default = 0.046)");

  std::vector<std::string> mirror_shift; 
  mirror_shift.push_back("dropoff_problem/sweep_al_sampleholder.root"); 
  mirror_shift.push_back("dropoff_problem/mirror_shift_0.08.root"); 
  mirror_shift.push_back("dropoff_problem/mirror_shift_0.12.root");
  makeDropoffPlotMain(mirror_shift, "Varying Mirror Position (default = 0.04)"); 


  double angles[15] = {15.0, 25.0, 39.0, 41.0, 45.0, 46.0, 48.0, 49.0, 54.0, 53.0, 57.0, 54.0, 57.0, 60.0, 63.0};
  double rates[15] = {228099.963163, 227136.641226, 231651.301358, 235690.28072, 233832.978309, 229868.805668, 208936.824357, 183473.15137, 132243.030543, 121204.836469, 40230.21131, 23463.665956, 27536.63494, 28064.915391, 27562.349646};
  
  
  double normal_rates[15] = {0.0};
  for (int i = 0; i < sizeof(angles)/sizeof(angles[0]); i++) {

   normal_rates[i] = 10000*double(rates[i])/double(rates[0]);
   std::cout <<normal_rates[i] << std::endl;
  }
  
  TGraphErrors* data = new TGraphErrors(15, angles, normal_rates);
  data->SetTitle("Data"); 
  data->SetMarkerStyle(8);
  data->SetLineColor(kRed); 
  data->SetMarkerColor(kRed); 
  std::vector<std::string> data_mc_comparison;
  data_mc_comparison.push_back("dropoff_problem/sweep_al_sampleholder.root");
  TMultiGraph* mg = makeDropoffPlotMain(data_mc_comparison, "Data MC Comparison"); 
  mg->Add(data); 
  mg->Draw("AP");
  
}
