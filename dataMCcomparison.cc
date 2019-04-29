#include <fstream>
#include <sstream> 
#include <vector> 
#include <string> 
#include <algorithm> 
#include <math>

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

double integralTGraph(int n_datapoints, double* angles, double* rates) {
 
  TGraph dummy(n_datapoints, angles, rates); 
  struct dataFunc {
    dataFunc(TGraph & g) : fGraph(&g) {} 
    double operator() (double *x, double *) {return fGraph->Eval(x[0]);} 
    TGraph * fGraph; 
  };
  dataFunc f(dummy); 
  TF1 dataF("dataF", &f,angles[0], angles[n_datapoints-1],0,"dataFunc"); // Data must be in decreasing order to be positive integral
  double norm_factor = abs(dataF.Integral(angles[n_datapoints-1], angles[0])); // Abs to take account of sign introduced by ordering of data
  return norm_factor;
}

std::vector<double> getFitParams(int n, double* x, double* y) {
  
  std::vector<double> result; 

  double y_max = *std::max_element(y, y + n);
  int max_index = 0; 

  for (int i = 0; i < n; i++) {
    if (y[i] == y_max) {
      max_index = i; 
      break; 
    }
  }
   
  double max_x_coord = x[max_index];
  double range_min = max_x_coord;
  double range_max = max_x_coord; 

  for (int i = 0; i < n; i++) {
    double x_val = x[i];
    double y_val = y[i]; 
      if (y_val > 0.6 * y_max) {
        range_min = x_val;   
        break;
   }
  }
    

  for (int i = n; i > 0; i -= 1) {
    double x_val = x[i-1];
    double y_val = y[i-1]; 
    if (y_val > 0.6 * y_max) {
      range_max = x_val;   
      break;
    } 
  } 

  if (range_max < range_min) {

    double temp = range_max; 
    range_max = range_min;  
    range_min = temp;

  }
  
  result.push_back(range_min); 
  result.push_back(range_max); 

  double mean = max_x_coord; 
  double sigma = 0.5 * abs(x[max_index - 2] - x[max_index + 2]);
  double amplitude = y_max; 

  result.push_back(mean); 
  result.push_back(sigma); 
  result.push_back(amplitude); 

  return result;  
}

std::vector<double> getFitLimits(TGraph* gr, TF1* f, double mean, double sigma) {

  double lower_bound; 
  double upper_bound; 
  double reduced_chisq = 1e12; 

  for (int i = 250; i > 0; i-=1 ) { 
    const double j = 0.01 * i;
    double lower = mean - j*sigma; 
    double upper = mean + j*sigma;
    gr->Fit(f, "RMQ","", lower, upper); 
    double chisq = gr->Chisquare(f); 
    double ndf = f->GetNDF(); 
    
    if (ndf == 0) break;
    double current_reduced_chisq = chisq/ndf;
    
    if (current_reduced_chisq < reduced_chisq) {
      reduced_chisq = current_reduced_chisq;
      lower_bound = lower;
      upper_bound = upper; 
    }
    //std::cout << "Upper " << upper << "Lower " << lower << std::endl;
    if (reduced_chisq <= 5.0 && reduced_chisq >= 1.0) {

     lower_bound = lower; 
     upper_bound = upper; 
     break;
    }  
  }

  //std::cout << "Mean " << mean << "Sigma " << sigma << std::endl;  
  std::cout << "Reduced ChSq " << reduced_chisq << std::endl;
  std::vector<double> fitlims; 
  fitlims.push_back(lower_bound);
  fitlims.push_back(upper_bound);
  return fitlims; 
}


TCanvas* makeMergedPlot(int n_datapoints, double* angles, double* rates, double* angle_err, double* rate_err,  
                            bool isBackFace, TGraphErrors* mc, std::string title, bool iscore, double mcPeak, double dataPeak) {
 gStyle->SetOptFit(1111); 
 // Leave off X uncertainty for now
 for (int i = 0; i < n_datapoints; i++) {
  angle_err[i] = 0.0;
 }

 
 double data_factor;   
 if (!iscore) {
  data_factor = avgFirstNVals(rates, 4); 
 } else {
  data_factor = integralTGraph(n_datapoints, angles, rates);  
 }
  std::cout << "Data Factor " << data_factor << std::endl;
  normalise(rate_err, n_datapoints, 1/data_factor); // Divide by integral
  normalise(rates, n_datapoints, 1/data_factor);
 
 TGraphErrors* data = new TGraphErrors(n_datapoints, angles, rates, angle_err, rate_err);
 data->SetMarkerStyle(8); 
 data->SetMarkerColor(kRed);
 data->SetLineColor(kRed);
 data->SetFillColor(kWhite); 
 data->SetTitle("Data");
  
 if (iscore) {
   
   std::vector<double> data_Fit_params = getFitParams(n_datapoints, angles, rates); 
   double mean = data_Fit_params[2]; 
   double sigma = data_Fit_params[3]; 
   double amplitude = data_Fit_params[4]; 
    
   TF1* fitFunc = new TF1("fitFunc", "gaus(0)", -10.0, 10.0);
   fitFunc->SetParameter(0, amplitude);
   fitFunc->SetParameter(1, mean); 
   fitFunc->SetParameter(2, sigma);
   fitFunc->SetLineColor(kRed); 
   std::vector<double> fit_lims = getFitLimits(data, fitFunc, mean, sigma); 
   double fitLow = fit_lims[0]; 
   double fitHigh = fit_lims[1];  
   data->Fit(fitFunc, "RM", "", fitLow, fitHigh);
   std::cout << "GRAPH CHISQUARE " << data->Chisquare(fitFunc) << std::endl;
 } 
 int n_mc = mc->GetN(); 
 double* mc_x = mc->GetX(); 
 double* mc_y = mc->GetY(); 
 double* mc_ex = mc->GetEX(); 
 double* mc_ey = mc->GetEY(); 
 
 //double mc_factor = avgFirstNVals(mc_y, 10); 
 //normalise(mc_ey, n_mc, 1/mc_factor); 
 //normalise(mc_y, n_mc, 1/mc_factor);
 if (isBackFace) {
  for (int i = 0; i < n_mc; i++) {
    mc_x[i] -= 180.0;   
  }
 }
  
 // Systematics for angles 200-250 on the back (20-60 degrees), 41 points (0-19 and 61-85 not assigned yet)
 double mirror_back_systematics_positive[]  = {0,0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  119.1049957,     92.09234496,   116.18089344,   110.94593278,   112.9822995,
                                                    103.46013725,   119.1049957,     92.28217596,   101.20276676,   128.14444974,
                                                    107.57787877,    76.24303247,    91.86402996,    99.47361459,    96.62815325,
                                                    82.06095296,   122.9024003,     88.74119675,    88.65100112,  111.65124272,
                                                    115.81882403,    91.0659102,     87.10338685,    83.53442404,    86.23224455,
                                                    136.7442869,    139.47401192,   245.50356413,   261.23743989,   323.11917306,
                                                    389.42136562,   439.98977261,   517.53743826,   677.46365216,   783.21261481,
                                                    966.43209798,   924.80971016,  1227.83142165,  1306.74672374,  1792.91522387,
                                                    1809.88369792, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                    0, 0, 0};
 
 double mirror_back_systematics_negative[] = {0,0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 276.55198426,  292.,          273.14831136,  273.66037346,  237.77510383,
                                                              287.,          254.25774324,  284.12849206,  251.04979586,  235.21266973,
                                                              272.26641365,  310.12900542,  260.04807248,  293.,          296.4641631,
                                                              297.16325479,  298.01510029,  280.,          319.,          259.40123361,
                                                              301.,          277.35356497,  289.26458477,  268.36542251,  220.,
                                                              263.92612603,  219.,          202.,          144.20124826,   93.,
                                                              136.00367642,  108.40664186,  154.46682492,  153.71727294,  239.2822601,
                                                              267.85817143,  188.50198938,  347.88072669,  542.75316673,  559.,
                                                              775.24318765, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
 // Systematics for angles 10-80 on front (71 points) (systematics for 81-85, 0-10 not done yet) 
 double mirror_front_systematics_positive[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98.84331035,   96.11971702,   97.82126558,  131.8370206,    83.22259309,
  113.28283189,  113.85517116,  101.12368664,  124.57126474,  120.35779991,
  125.00399994,  117.32007501,  122.14745188,  120.10412149,  132.07195009,
   95.40440241,   99.28242543,  127.37346662,  113.21660656,  111.17103939,
  127.95311641,  105.26157894,  106.75673281,   83.108363,    132.12872511,
   99.72462083,  105.63143472,  108.34666585,  100.24968828,  117.38824473,
   94.76286192,   86.66025617,  100.60318086,   89.5656184,   112.45888137,
  112.05802069,   97.67804257,   92.24424101,  116.73045875,   97.25224933,
  120.66482503,  127.27922061,   99.,          102.1763182,    85.93020424,
  100.00499988,   98.26494797,  114.,           96.60745313,  112.47221879,
  109.80892496,   92.54188241,  101.86756108,   96.51424765,   70.15696687,
   89.,           59.24525297,   53.,           98.08159868,   68.43244844,
   98.48857802,   79.9812478,    65.77233461,   56.68333088,  146.64583185,
  127.54607011,   28.,          186.64940396,   88.64536085,  105.65509926,
   88.95504483, 0, 0, 0, 0, 0};

 
 double mirror_front_systematics_negative[] = {0,0,0,0,0,0,0,0,0,0,276.17385828,  254.0354306,   262.00763348,  257.00194552 , 315.98417682,
  245.38133588,  253.43046384,  283.1148177,   216.01388844,  254.95293683,
  278.51391348,  268.00746258,  253.6513355,   224.90442415,  241.05186164,
  282.08686605,  250.12796725,  268.30765923,  242.90533135,  230.0021739,
  249.72384748,  270.4403816,   268.05409902, 320.99532707,  223.09190931,
  244.90610446,  290.64411227,  234.26907606,  256.59501164,  270.86897201,
  242.1012185,   320.41067398,  271.44796923,  314.43282271,  245.84547993,
  289.78095175,  295.23888633,  235.50159235,  249.56161564,  259.23734299,
  322.21731797,  253.46202871,  285.39971969,  273.60372804,  290.04482412,
  257.61017061,  236.15672762,  274.47586415,  279.62296043,  281.13342028,
  272.70863573,  284.17952073,  274.25353234,  267.89923479,  251.88687937,
  229.05894438,  236.58190971,  234.40776438,  197.10657016,  205.01219476,
  210.05237442,  214.69513269,  244.09629247,  205.08047201,  279.12362852,
  256.19523805,  300.26654825,  341.70894047,  410.20238907,  328.39610229,
  388.097925790, 0, 0, 0, 0, 0};
 //int n_err = 71;

 //if (isBackFace) n_err = 41; 
  int n_err = 41;

 double err_up[100];
 double err_down[100];
  
 double mc_factor;
 if (!iscore) { 
  mc_factor = avgFirstNVals(mc_y, 10);
 } else {
  mc_factor = integralTGraph(n_mc, mc_x, mc_y); //*std::max_element(mc_y, mc_y + n_mc); 
 }
  std::cout << "MC Factor " << mc_factor << std::endl;

 for (int i = 0; i < n_mc; i++) {
   
   double intensity = mc_y[i];
   int systematic_index = mc_x[i] - 0; // -10 for front, -20 for back.  
   
   if (isBackFace) {
     err_up[i] = mirror_back_systematics_positive[systematic_index];
     err_down[i] = mirror_back_systematics_negative[systematic_index]; 
   } else {
     err_up[i] = mirror_front_systematics_positive[systematic_index]; //* intensity; 
     err_down[i] = mirror_front_systematics_negative[systematic_index]; //* intensity; // Account for eventual normalisation 
   }
 
    double sqrt_N_error = sqrt(intensity); 
    err_up[i] = sqrt(pow(err_up[i], 2) + pow(sqrt_N_error,2));   
    err_down[i] = sqrt(pow(err_down[i], 2) + pow(sqrt_N_error,2));   
  }  
  
 
 normalise(mc_y, n_mc, 1/mc_factor);
 normalise(err_up, n_mc, 1/mc_factor);
 normalise(err_down, n_mc, 1/mc_factor);
 
 TGraphAsymmErrors* asymmc = new TGraphAsymmErrors(n_mc, mc_x, mc_y, mc_ex, mc_ex, err_down, err_up); 

 asymmc->SetMarkerStyle(8);
 asymmc->SetMarkerColor(kBlack); 
 asymmc->SetLineColor(kBlack);
 asymmc->SetFillColor(kWhite); 
 asymmc->SetTitle("MC");
 if (iscore) {
   std::vector<double> MC_fit_params = getFitParams(n_mc, mc_x, mc_y); 
   double fitMin = MC_fit_params[0]; 
   double fitMax = MC_fit_params[1]; 
   double mean = MC_fit_params[2]; 
   double sigma = MC_fit_params[3]; 
   double amplitude = MC_fit_params[4]; 
 
   TF1* fitFunc = new TF1("fitFunc", "gaus(0)", mcPeak - 2.0, mcPeak + 2.0);
   fitFunc->SetParameter(0, amplitude);
   fitFunc->SetParameter(1, mean); 
   fitFunc->SetParameter(2, sigma);
   asymmc->Fit(fitFunc, "RM", "", mcPeak - 2.0, mcPeak + 1.0);
 } 

 TMultiGraph* comparison = new TMultiGraph();
 comparison->SetTitle(title.c_str()); 
 comparison->Add(data);
 comparison->Add(asymmc);


 TCanvas* canvas = new TCanvas(title.c_str(), title.c_str(), 1000, 600); 
 //comparison->SetName(mcPlotName.c_str()); 
 comparison->Draw("AP"); 
 comparison->GetXaxis()->SetTitle("Angle from Normal / Deg");
 comparison->GetYaxis()->SetTitle("Intensity / arb units"); 
 comparison->GetHistogram()->SetMaximum(0.4);
 canvas->Update(); 
 
 TPaveStats* stats_data = (TPaveStats*) data->GetListOfFunctions()->FindObject("stats");
 TPaveStats* stats_asymmc = (TPaveStats*) asymmc->GetListOfFunctions()->FindObject("stats");
 stats_data->SetX1NDC(0.72); stats_data->SetX2NDC(0.92); stats_data->SetY1NDC(0.78);  
 stats_asymmc->SetX1NDC(0.72); stats_asymmc->SetX2NDC(0.92); stats_asymmc->SetY1NDC(0.58); stats_asymmc->SetY2NDC(0.78); 
 stats_data->SetTextColor(kRed);
 
 canvas->Modified();
 // gPad->BuildLegend(0.75,0.75,0.95,0.95,""); 
 gPad->BuildLegend(0.1, 0.7, 0.3, 0.9, ""); 
 return canvas;
}


void dataMCcomparison(std::string datafile, std::string mcFileName, std::string mcPlotName, std::string title, std::string outfile, bool isBackFace, bool isCorePlot, 
                      double MCpeak) {

  std::vector<std::vector<double>> data = readDataFile(datafile); 
  TFile* outf = new TFile(outfile.c_str(), "RECREATE"); 
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
 TGraphErrors* mcGraph = (TGraphErrors*) mcFile->Get(mcPlotName.c_str());
 TCanvas* plot = makeMergedPlot(n_datapoints, angles, rates, angle_err, rate_err, isBackFace, mcGraph, title, isCorePlot, MCpeak, MCpeak);
 
 outf->WriteTObject(plot);
 return;
}
/*
void dataMCcomparisonMain() {
  gROOT->SetBatch(kTRUE); 
  double front_nominal[] = {20, 30, 45, 47, 50, 52, 55, 57, 60, 62, 65, 67, 70, 72, 75}; 
  double front_data[] = {17.3, 30.8, 46.4, 48.8, 51.0, 52.9, 56.5, 58.6, 60.8, 63.3, 67.8, 69.3, 72.4, 73.7, 76.7};
  TFile* front = new TFile("MC_mirror_front.root", "RECREATE"); 

  double back_nominal[15];
  double back_data[] = {16.5, 25.7, 40.0, 46.1, 44.3, 45.5, 49.0, 49.7, 53.1, 53.7, 56.7, 53.9, 57.2, 59.0, 62.9}; 
  TFile* back = new TFile("MC_mirror_back.root", "RECREATE"); 

  for (int i = 0; i < 15; i++) {
    back_nominal[i] = front_nominal[i] + 180;
    back_data[i] += 180;
  } 

   
  for (int i = 0; i < 15; i++) {

    double angle_fn = front_nominal[i];
    double angle_fd = front_data[i]; 
    char datafile[100]; 
    sprintf(datafile, "./data_front/data_front_%d_deg.txt", angle_fn); 
    std::cout << datafile << std::endl;
    char title[100]; 
    sprintf(title,  "Front Nominal Peak %.1f Actual Peak %.1f", angle_fn, angle_fd);
    char plotname[100];
    sprintf(plotname, "tgraph_%.1f", angle_fd);  
    // MC simulated at data peaks 
    dataMCcomparison(datafile, "./core_dists/core_dist_front_data.root", plotname, title, front, false, true, angle_fd); 

    double angle_bn = back_nominal[i];
    double angle_bd = back_data[i]; 
    char datafile_back[100]; 
    sprintf(datafile_back, "./data_back/data_back_%d_deg.txt", angle_bn-180); 
    std::cout << datafile_back << std::endl;
    char title_back[100]; 
    sprintf(title_back,  "Back Nominal Peak %.1f Actual Peak %.1f", angle_bn-180, angle_bd-180);
    char plotname_back[100];
    sprintf(plotname_back, "tgraph_%.1f", angle_bd);  
    // MC simulated at data peaks 
    dataMCcomparison(datafile_back, "./core_dists/core_dist_back_data.root", plotname_back, title_back, back, true, true, angle_bd-180); 
  }

}
  */ 
