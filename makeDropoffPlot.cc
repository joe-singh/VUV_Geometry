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


void getSystematicDifferences(std::vector<std::string> fnames, std::string title, bool isBack) {

  TFile* default_config_file = new TFile(fnames[0].c_str(), "READ"); 
  TFile* plus_config_file = new TFile(fnames[1].c_str(), "READ");
  TFile* minus_config_file = new TFile(fnames[2].c_str(), "READ"); 
 
  TGraphErrors* nominal = (TGraphErrors*) default_config_file->Get("peakHeightVsSampleAngle");
  TGraphErrors* plus = (TGraphErrors*) plus_config_file->Get("peakHeightVsSampleAngle");
  TGraphErrors* minus = (TGraphErrors*) minus_config_file->Get("peakHeightVsSampleAngle");
  int n = nominal->GetN(); 
  double* angles = nominal->GetX();    

/*  TF1* f_nominal =  nominal->GetListOfFunctions()->FindObject("fitFunction"); 
  TF1* f_plus = plus->GetListOfFunctions()->FindObject("fitFunction"); 
  TF1* f_minus = minus->GetListOfFunctions()->FindObject("fitFunction");

  std::cout << "Systematics for " << title << std::endl;
  for (int i = 0; i < n; i++) {

    double angle = angles[i];
    double nom = f_nominal->Eval(angle); 
    double high = f_plus->Eval(angle);   
    double low = f_minus->Eval(angle);
    
    double plus_syst = abs(high - nom); 
    double minus_syst = abs(nom - low);    
 
    if (isBack) angle -= 180; 
    
    std::cout << "Angle: " << angle << ", Plus: " << plus_syst*100/nom << "\%, Minus: " << minus_syst*100/nom << "\% " << std::endl;

  }
  std::cout << std::endl; 
  */

  double* nominal_intensity = nominal->GetY(); 
  double* plus_intensity = plus->GetY(); 
  double* minus_intensity = minus->GetY(); 
  
  std::cout << "Systematics for " << title << std::endl;
  for (int i = 0; i < n ; i++) {
    
    double angle = angles[i];
    double plus_syst = abs(plus_intensity[i] - nominal_intensity[i]);
    double minus_syst = abs(minus_intensity[i] - nominal_intensity[i]);
    double nom = nominal_intensity[i];  
    if (isBack) angle -= 180; 
   
    std::cout << "Angle: " << angle << ", Plus: " << plus_syst*100/nom << "\%, Minus: " << minus_syst*100/nom << "\% " << std::endl;
  }
  std::cout << std::endl;
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
  mg->Draw("AP");
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

void makeDropoffPlot(bool isBack) {
/*
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
*/
  std::vector<std::string> aperture_width;
  aperture_width.push_back("peak_analysis_mirror_aperture_back_nominal.root"); 
  aperture_width.push_back("peak_analysis_mirror_aperture_back_plus.root"); 
  aperture_width.push_back("peak_analysis_mirror_aperture_back_minus.root");
  makeDropoffPlotMain(aperture_width, "Aperture Width Systematic"); 
  getSystematicDifferences(aperture_width, "Aperture Width", isBack); 
  
/*  std::vector<std::string> pmt_distance;
  pmt_distance.push_back("systematic_mirror/default.root"); 
  pmt_distance.push_back("systematic_mirror/distance_plus.root"); 
  pmt_distance.push_back("systematic_mirror/distance_minus.root"); 
  makeDropoffPlotMain(pmt_distance, "PMT Distance Systematic");
  getSystematicDifferences(pmt_distance, "PMT Distance", isBack); 

  std::vector<std::string> reflection;
  reflection.push_back("systematic_mirror/default.root"); 
  reflection.push_back("systematic_mirror/no_reflection.root"); 
  reflection.push_back("systematic_mirror/no_reflection.root"); 
  makeDropoffPlotMain(reflection,"Sample Holder Reflection Systematic"); 
  getSystematicDifferences(reflection, "Sample Holder Reflection", isBack); 

  std::vector<std::string> wavelength;
  wavelength.push_back("systematic_mirror/default.root"); 
  wavelength.push_back("systematic_mirror/wavelength_plus.root"); 
  wavelength.push_back("systematic_mirror/wavelength_minus.root"); 
  makeDropoffPlotMain(wavelength,"Wavelength Systematic"); 
  getSystematicDifferences(wavelength, "Wavelength", isBack);*/ 
}
