# include <iostream>
# include <sstream>
# include <TROOT.h>
# include <TFile.h>
# include <TTree.h>
# include <TH1D.h>
# include <TH2D.h>
# include <TVector3.h>
# include <TVectorD.h>
# include <TSystem.h>
# include <vector>
# include <algorithm>
# include <TCanvas.h>
# include <math.h>
# include <TMultiGraph.h>
# include <TLegend.h>
# include <TGraphErrors.h>
# include <RAT/DS/Root.hh>
# include <RAT/DS/MC.hh>
# include <RAT/DS/MCTrack.hh>
# include <RAT/DS/MCTrackStep.hh>
# include <TSystem.h>

const double PI = 3.14159265358979323846;

int strToInt(char* ch) {
    std::stringstream ss;
    ss << ch; 
    int num;
    ss >> num; 
    return num; 
}

TVector3 getNormal(double angle) {
  
  double x = 0.0; 
  double y = cos(angle * PI/180); 
  double z = sin(angle * PI/180); 
 
  return TVector3(x, y, z); 
}

double photonTracker(std::string fname, int normalAngle, int sampleAngle, TFile* diagnostic) {
  std::cout << fname << std::endl;
  if (gSystem->AccessPathName(fname.c_str())) {
    std::cout << "File " << fname << " does not exist, assuming 0 photons detected" << std::endl; 
    return 0.0;
  }
  TFile* file = new TFile(fname.c_str(), "READ");
  TTree* tr = (TTree*) file->Get("T");

  RAT::DS::Root* rds = new RAT::DS::Root();
  tr->SetBranchAddress("ds", &rds);
  
  char upstreamName[1000];
  sprintf(upstreamName, "upstreamAngle_%d_%d", sampleAngle, normalAngle);
  char downstreamName[1000];
  sprintf(downstreamName, "downstreamAngle_%d_%d", sampleAngle, normalAngle);
  char positionName[1000];
  sprintf(positionName, "photonPosition_%d_%d", sampleAngle, normalAngle);
  char wavelengthName[1000];
  sprintf(wavelengthName, "photonWavelength_%d_%d", sampleAngle, normalAngle);
  TH1F* upstreamAngle = new TH1F(upstreamName, "Upstream Angle", 100, -1.1, 1.1);
  TH1F* downstreamAngle = new TH1F(downstreamName, "Downstream Angle", 100, -1.1, 1.1);
  TH2F* yzPositions = new TH2F(positionName, "Y-Z Positions", 100, -500, 500, 100, -100, 100); 
  TH1F* wavelength = new TH1F(wavelengthName, "Wavelength", 100, 0, 400); 
  TH1F* zDist = new TH1F("zDist", "Ray z-position distribution", 50, -1.5, 1.5); 
  
  TVector3 hatX = TVector3(1.0,0,0);
  TVector3 hatY = TVector3(0,1.0,0);
  TVector3 hatZ = TVector3(0,0,1.0);
  TVector3 hat45 = TVector3(0, 1, 1);
  TVector3 hat60 = TVector3(0, 1, 1.73205080757);
  TVector3 photodiodeNormal = getNormal(normalAngle); 
  int numCube = 0;
  int numPhotodiode = 0;
 
  int nentries = tr->GetEntries();
  std::cout << "Num Entries: " << nentries << std::endl;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {

    if (iEntry % 1000 == 0) {
      std::cout << "At entry " << iEntry << std::endl;
    }

    tr->GetEntry(iEntry); 
    RAT::DS::MC* rmc = rds->GetMC(); 
    int numMCTracks = rmc->GetMCTrackCount(); 

    for (int iTrack = 0; iTrack < numMCTracks; iTrack++) {
      
      RAT::DS::MCTrack* photonTrack = rmc->GetMCTrack(iTrack); 
      int numSteps = photonTrack->GetMCTrackStepCount(); 

      for (int iStep = 0; iStep < numSteps; iStep++) {

        RAT::DS::MCTrackStep* photonStep = photonTrack->GetMCTrackStep(iStep);
        double yComponent = photonStep->GetMomentum().Dot(hatY); 

        double xPos = photonStep->GetEndpoint().x();
        double yPos = photonStep->GetEndpoint().y();
        double zPos = photonStep->GetEndpoint().z();
        yzPositions->Fill(yPos, zPos);
      
        double photonWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.; 
        wavelength->Fill(photonWavelength); 

       if (photonStep->GetVolume() == "testdisk" && yComponent >= 0.0) {
         numCube++; 
         double cos_theta = (photonStep->GetMomentum().Dot(hatY))/(photonStep->GetMomentum().Mag());
         upstreamAngle->Fill(cos_theta); 
         zDist->Fill(zPos); 
        } 

        if (photonStep->GetVolume() == "verticalSurface" || photonStep->GetVolume() == "photodiodetest") {
         numPhotodiode++; 
  //       TVector3 photoDiodeNormal = hatY;//getNormal(normalAngle); 
         RAT::DS::MCTrackStep* p = photonTrack->GetMCTrackStep(iStep-1);
         RAT::DS::MCTrackStep* pp = photonTrack->GetMCTrackStep(iStep-2);
         RAT::DS::MCTrackStep* ppp = photonTrack->GetMCTrackStep(iStep-3);
         double cos_theta = (photonStep->GetMomentum().Dot(hatY))/(photonStep->GetMomentum().Mag() * hatY.Mag());
         //std::cout << cos_theta << std::endl;
         downstreamAngle->Fill(cos_theta); 
       }

      }

    }
  }

  std::cout << "UPSTREAM NUMBER FOR ANGLE " << normalAngle << " " << numCube << std::endl;
  std::cout << "NUMBER FOR ANGLE " << normalAngle << " " << numPhotodiode << std::endl;
  diagnostic->WriteTObject(upstreamAngle);
  diagnostic->WriteTObject(downstreamAngle); 
  diagnostic->WriteTObject(yzPositions);
  diagnostic->WriteTObject(wavelength);
  diagnostic->WriteTObject(zDist); 
  return double(numPhotodiode); 

} 



void newLoopOverFiles(int sampleMin, int sampleMax, int sampleDelta, 
                      int normalMin, int normalMax, int normalDelta) {
  
  gROOT->SetBatch(kTRUE); 
  TFile* outf = new TFile("angular_sweeps.root", "RECREATE"); 
  TMultiGraph* mg = new TMultiGraph();
  auto leg = new TLegend(0.1,0.7,0.2,0.9); 
  TFile* diagnosticAngle = new TFile("angularDiagnostics.root", "RECREATE");   
 
  for (int sampleAngle = sampleMin; sampleAngle < sampleMax + sampleDelta; sampleAngle += sampleDelta) {
    
    int numDataPoints = (normalMax - normalMin)/normalDelta + 1;
    std::cout << numDataPoints << std::endl;
    std::vector<double> normalAngles; 
    std::vector<double> intensities; 
    std::vector<double> errors; 
    
    char histname[1000];
    sprintf(histname, "angularDist_%d", sampleAngle); 
    TH1F* hist = new TH1F(histname, "Photon Angular Distribution;Angle From Normal / Degrees;Number of Photons", 500, normalMin, normalMax + normalDelta); 
    hist->SetLineWidth(2); 
    for (int normalAngle = normalMin; normalAngle < normalMax + normalDelta; normalAngle += normalDelta) {

      char filename[1000];
      sprintf(filename, "./angle_sweep/angle_%d/no_cylinder_angle_%d.%d.root", sampleAngle, sampleAngle,normalAngle);
      double photonCount = photonTracker(filename, normalAngle, sampleAngle, diagnosticAngle);
  
      normalAngles.push_back(normalAngle); 
      intensities.push_back(photonCount); 
      errors.push_back(sqrt(photonCount));      

      hist->Fill(normalAngle, photonCount); 
  }
  
  double* normalArray = &normalAngles[0]; 
  double* intensityArray = &intensities[0]; 
  double* errorArray = &errors[0]; 
  
  TGraphErrors* gr = new TGraphErrors(normalAngles.size(), normalArray, intensityArray, 0, errorArray); 
  gr->SetMarkerStyle(21);
  gr->SetLineWidth(2);
  gr->SetLineStyle(9);
  int colour = sampleAngle - sampleMin - 2;
  if (colour == 10) colour = 28; // Account for white 
  gr->SetLineColor(colour);
  gr->SetMarkerColor(colour); 
  gr->GetXaxis()->SetTitle("Angle from Normal / degrees");
  gr->GetYaxis()->SetTitle("Number Of Photons");
   
  gr->SetTitle(histname); 
  gr->SetName(histname);  
  //outf->WriteTObject(gr); 
  outf->WriteTObject(hist); 
  mg->Add(gr);

  char graphname[1000];
  sprintf(graphname, "%d", sampleAngle); 
  gr->SetTitle(graphname); 
  leg->AddEntry(gr);  
 } 
 
 std::cout << "LOOP COMPLETE" << std::endl;
 mg->SetName("distribution_multigraph");  
 outf->WriteTObject(mg); 
 TCanvas* canv = new TCanvas("c", "c", 1000, 600);
 mg->Draw("apc");
 leg->Draw();
 mg->GetXaxis()->SetTitle("Angle from Normal / degrees"); 
 mg->GetYaxis()->SetTitle("Number of Photons");
 outf->WriteTObject(canv);

 std::cout << "COMPLETE!" << std::endl;

}

int main(int argc, char* argv[]) {
   int samplemin = strToInt(argv[1]);
   int samplemax = strToInt(argv[2]); 
   int sampledelta = strToInt(argv[3]); 
   int normalmin = strToInt(argv[4]);
   int normalmax = strToInt(argv[5]); 
   int normaldelta = strToInt(argv[6]);  
   newLoopOverFiles(samplemin, samplemax, sampledelta, normalmin, normalmax, normaldelta); 
   return 0; 
}
