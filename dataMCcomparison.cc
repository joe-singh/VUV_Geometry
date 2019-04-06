#include <fstream>
#include <sstream> 
#include <vector> 
#include <string> 

double stringToDouble(std::string num) {
  std::stringstream ss(num);
  double x = 0.0;
  ss >> x;
  return x;
}

std::vector<double> parseCommaString(std::string str) {
  std::vector<double> result;
  std::stringstream ss(str);
  while (ss.good()) {
    std::string substr; 
    getline(ss, substr, ',');
    result.push_back(stringToDouble(substr));
  }
  return result;
}

std::vector<std::vector<double>> readDataFile(std::string fname) {
 
  std::vector<std::vector<double>> data; // x,y,ey,ex vectors in that order
  std::string line; 
  std::ifstream myfile (fname.c_str());
  int counter = 0; 
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
       line.erase(0,3); 
       std::vector<double> vec = parseCommaString(line); 
       data.push_back(vec); 
    }
  }
 return data;
}




double avgFirstNVals(double* arr, int N) {
  double total = 0.0; 
  for (int i = 0; i < N; i++) {
    total += arr[i];
  }
 
 return total/double(N); 
}


void normalise(double* arr, int len, double factor) {
  for (int i = 0; i < len; i++) {
    arr[i] = double(arr[i])*factor; 
  }
}


TMultiGraph* makeMergedPlot(int n_datapoints, double* angles, double* rates, double* angle_err, double* rate_err,  
                            bool isBackFace, TGraphErrors* mc, std::string title) {
 
 double data_factor = avgFirstNVals(rates, 4); 
 normalise(rate_err, n_datapoints, 1/data_factor); 
 normalise(rates, n_datapoints, 1/data_factor);
 TGraphErrors* data = new TGraphErrors(n_datapoints, angles, rates, angle_err, rate_err);
 data->SetMarkerStyle(8); 
 data->SetMarkerColor(kRed);
 data->SetLineColor(kRed); 
 data->SetTitle("Data"); 
 
 int n_mc = mc->GetN(); 
 double* mc_x = mc->GetX(); 
 double* mc_y = mc->GetY(); 
 double* mc_ex = mc->GetEX(); 
 double* mc_ey = mc->GetEY(); 
 
 double mc_factor = avgFirstNVals(mc_y, 10); 
 normalise(mc_ey, n_mc, 1/mc_factor); 
 normalise(mc_y, n_mc, 1/mc_factor);
 if (isBackFace) {
 
  for (int i = 0; i < n_mc; i++) {
    mc_x[i] -= 180.0;   
  }
 }
 
 mc = new TGraphErrors(n_mc, mc_x, mc_y, mc_ex, mc_ey);

 mc->SetMarkerStyle(8);
 mc->SetMarkerColor(kBlack); 
 mc->SetLineColor(kBlack); 
 mc->SetTitle("MC"); 

 TMultiGraph* comparison = new TMultiGraph();
 comparison->SetTitle(title.c_str()); 
 comparison->Add(data);
 comparison->Add(mc);
 return comparison;
}

/*void dataMCcomparison(int n_datapoints, double* angles, double* rates, double* angle_err, double* rate_err,  
                      bool isBackFace, std::string title, std::string mcFileName) {

 TFile* mcFile = new TFile(mcFileName.c_str(), "READ"); 
 TGraphErrors* mcGraph = (TGraphErrors*) mcFile->Get("peakHeightVsSampleAngle"); 
 TMultiGraph* plot = makeMergedPlot(n_datapoints, angles, rates, angle_err, rate_err, isBackFace, mcGraph, title);
 TCanvas* canvas = new TCanvas(title.c_str(), title.c_str(), 1000, 600); 
 plot->Draw("AP"); 
 plot->GetXaxis()->SetTitle("Angle from Normal / Deg");
 plot->GetYaxis()->SetTitle("Intensity / arb units"); 
 canvas->Modified();
 gPad->BuildLegend(0.75,0.75,0.95,0.95,""); 
 
 readDataFile("data.txt"); 
 return; 

} */

void dataMCcomparison(std::string datafile, std::string mcFileName, std::string title, bool isBackFace) {

  std::vector<std::vector<double>> data = readDataFile(datafile); 
  
  int n_datapoints = data[0].size();
  std::cout << n_datapoints << std::endl; 
  std::cout << data[0].size() << ' ' << data[1].size() << ' ' << data[2].size() << ' ' << data[3].size() << std::endl;
  double* angles = new double[n_datapoints];
  double* rates = new double[n_datapoints];
  double* angle_err = new double[n_datapoints];
  double* rate_err = new double[n_datapoints];

  for (int i = 0; i < n_datapoints; i++) {
 
    angles[i] = data[0][i];
    rates[i] = data[1][i];
    angle_err[i] = data[2][i];
    rate_err[i] = data[3][i];
  } 

 TFile* mcFile = new TFile(mcFileName.c_str(), "READ"); 
 TGraphErrors* mcGraph = (TGraphErrors*) mcFile->Get("peakHeightVsSampleAngle"); 
 TMultiGraph* plot = makeMergedPlot(n_datapoints, angles, rates, angle_err, rate_err, isBackFace, mcGraph, title);
 TCanvas* canvas = new TCanvas(title.c_str(), title.c_str(), 1000, 600); 
 plot->Draw("AP"); 
 plot->GetXaxis()->SetTitle("Angle from Normal / Deg");
 plot->GetYaxis()->SetTitle("Intensity / arb units"); 
 canvas->Modified();
 gPad->BuildLegend(0.75,0.75,0.95,0.95,""); 
 return;
}
