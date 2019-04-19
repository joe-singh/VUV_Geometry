double* addGraphToMultiGraph(TFile* f, std::string fname, TMultiGraph* mg, int color, TCanvas* c) {
  TCanvas* activeCanv = c;
  TGraphErrors* g = (TGraphErrors*) f->Get("peakHeightVsSampleAngle"); 

  double* y_vals = g->GetY(); 
  int len = g->GetN(); 

  mg->Add(g); 
  g->SetLineColor(color); 
  g->SetMarkerColor(color); 
  g->SetFillColor(0);
  g->SetTitle(fname.c_str());
  TF1* fn = g->GetListOfFunctions()->FindObject("fitFunction"); 
  fn->SetLineColor(color);
  double amplitude = fn->GetParameter(0); 
  double y_threshold = 0.9*amplitude;
  double x_threshold = fn->GetX(y_threshold, 220, 240); 
  
  double coord[4] = {x_threshold,g->GetYaxis()->GetXmin(), x_threshold, y_threshold};
  return coord;
}


void getSystematicDifferences(std::vector<std::string> fnames, std::string title, bool isBack, bool hasDirection) {

  TFile* default_config_file = new TFile(fnames[0].c_str(), "READ"); 
  TFile* plus_config_file = new TFile(fnames[1].c_str(), "READ");
  TFile* minus_config_file;
 
  TGraphErrors* nominal = (TGraphErrors*) default_config_file->Get("peakHeightVsSampleAngle");
  TGraphErrors* plus = (TGraphErrors*) plus_config_file->Get("peakHeightVsSampleAngle");
  TGraphErrors* minus;
  int n = nominal->GetN();

  double* angles = nominal->GetX();    

  double* nominal_intensity = nominal->GetY(); 
  double* plus_intensity = plus->GetY(); 
  double* minus_intensity;  

  if (hasDirection) {
  
   minus_config_file = new TFile(fnames[2].c_str(), "READ"); 
   minus = (TGraphErrors*) minus_config_file->Get("peakHeightVsSampleAngle");
   minus_intensity = minus->GetY(); 
  } 
  
  std::cout << "Systematics for " << title << std::endl;
  for (int i = 0; i < n ; i++) {
    
    double angle = angles[i];
    double plus_syst = plus_intensity[i] - nominal_intensity[i];
    double minus_syst;
    if (hasDirection)  minus_syst = minus_intensity[i] - nominal_intensity[i];
    double nom = nominal_intensity[i];  
    if (isBack) angle -= 180; 
    
    if (hasDirection) { std::cout << "Angle: " << angle << ", Plus: " << plus_syst*100/nom << "\%, Minus: " << minus_syst*100/nom << "\% " << std::endl;}
    else {std::cout << "Angle: " << angle << ", On: " << plus_syst*100/nom << "\%" << std::endl;}

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

void makeDropoffPlot() {


  std::vector<std::string> aperture_front; 
  aperture_front.push_back("peak_analysis_mirror_aperture_front_nominal.root"); 
  aperture_front.push_back("peak_analysis_mirror_aperture_front_plus.root"); 
  aperture_front.push_back("peak_analysis_mirror_aperture_front_minus.root");
  makeDropoffPlotMain(aperture_front, "Aperture Front Systematic"); 
  getSystematicDifferences(aperture_front, "Aperture Front", false, true);  

  std::vector<std::string> laser_shift;
  laser_shift.push_back("peak_analysis_mirror_laser_axis_back_nominal.root"); 
  laser_shift.push_back("peak_analysis_mirror_laser_axis_back_plus.root"); 
  //aperture_width.push_back("peak_analysis_mirror_laser_axis_back_minus.root");
  makeDropoffPlotMain(laser_shift, "Laser Shift Systematic"); 
  getSystematicDifferences(laser_shift, "Laser Shift", true, false); 

    
  std::vector<std::string> reflection;
  reflection.push_back("peak_analysis_mirror_reflection_back_nominal.root"); 
  reflection.push_back("peak_analysis_mirror_reflection_back_plus.root"); 
  //aperture_width.push_back("peak_analysis_mirror_laser_axis_back_minus.root");
  makeDropoffPlotMain(reflection, "Reflection Systematic"); 
  getSystematicDifferences(reflection, "Reflection", true, false); 

  std::vector<std::string> reflection_front;
  reflection_front.push_back("peak_analysis_mirror_reflection_front_nominal.root"); 
  reflection_front.push_back("peak_analysis_mirror_reflection_front_plus.root");
  makeDropoffPlotMain(reflection_front, "Reflection Front Systematic"); 
  getSystematicDifferences(reflection_front, "Reflection Front", false, false);

  std::vector<std::string> rotation;
  rotation.push_back("peak_analysis_mirror_rotation_axis_back_nominal.root"); 
  rotation.push_back("peak_analysis_mirror_rotation_axis_back_plus.root"); 
  makeDropoffPlotMain(rotation, "Rotation Systematic"); 
   getSystematicDifferences(rotation, "Rotation Axis Offset", true, false); 

  std::vector<std::string> rotation_front;
  rotation_front.push_back("peak_analysis_mirror_rotation_axis_front_nominal.root");
  rotation_front.push_back("peak_analysis_mirror_rotation_axis_front_plus.root");
  makeDropoffPlotMain(rotation_front, "Rotation Front Systematic");
  getSystematicDifferences(rotation_front, "Rotation Axis Offset Front", false, false);

  std::vector<std::string> distance_front;
  distance_front.push_back("peak_analysis_mirror_distance_front_nominal.root"); 
  distance_front.push_back("peak_analysis_mirror_distance_front_plus.root"); 
  distance_front.push_back("peak_analysis_mirror_distance_front_minus.root");
  makeDropoffPlotMain(distance_front, "Distance Systematic Front");
  getSystematicDifferences(distance_front, "Distance Front", false, true); 
  
  std::vector<std::string> laser_axis_front; 
  laser_axis_front.push_back("peak_analysis_mirror_laser_axis_front_nominal.root");    
  laser_axis_front.push_back("peak_analysis_mirror_laser_axis_front_plus.root");   
  makeDropoffPlotMain(laser_axis_front, "Laser Axis Front Systematic"); 
  getSystematicDifferences(laser_axis_front, "Laser Axis Front", false, false);  
  
  std::vector<std::string> pmt_x_back;
  pmt_x_back.push_back("peak_analysis_mirror_pmt_x_back_nominal.root"); 
  pmt_x_back.push_back("peak_analysis_mirror_pmt_x_back_plus.root"); 
  pmt_x_back.push_back("peak_analysis_mirror_pmt_x_back_minus.root");
  makeDropoffPlotMain(pmt_x_back, "PMT X Back"); 
  getSystematicDifferences(pmt_x_back, "PMT X Back", true, true); 

  std::vector<std::string> pmt_y_back;
  pmt_y_back.push_back("peak_analysis_mirror_pmt_y_back_nominal.root"); 
  pmt_y_back.push_back("peak_analysis_mirror_pmt_y_back_plus.root"); 
  pmt_y_back.push_back("peak_analysis_mirror_pmt_y_back_minus.root");
  makeDropoffPlotMain(pmt_y_back, "PMT Y Back"); 
  getSystematicDifferences(pmt_y_back, "PMT Y Back", true, true); 
}
