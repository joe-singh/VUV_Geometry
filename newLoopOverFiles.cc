# include <iostream>
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

const double PI = 3.14159265358979323846;

TVector3 getNormal(double angle) {
  
  double x = 0.0; 
  double y = cos(angle * PI/180); 
  double z = sin(angle * PI/180); 
 
  return TVector3(x, y, z); 
}

double photonTracker(std::string fname, int normalAngle, int sampleAngle, TFile* diagnostic) {
  std::cout << fname << std::endl;
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
        } 

        if (photonStep->GetVolume() == "photodiodetest") {
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
  return double(numPhotodiode); 

} 



void newLoopOverFiles(int sampleMin, int sampleMax, int sampleDelta, 
                      int normalMin, int normalMax, int normalDelta) {
  
  gROOT->SetBatch(kTRUE); 
  TFile* outf = new TFile("angularTPB.root", "RECREATE"); 
  TMultiGraph* mg = new TMultiGraph(); 
  TFile* diagnosticAngle = new TFile("angularDiagnostics.root", "RECREATE");   
 
  for (int sampleAngle = sampleMin; sampleAngle < sampleMax + sampleDelta; sampleAngle += sampleDelta) {

    int numDataPoints = (normalMax - normalMin)/normalDelta + 1;
    std::cout << numDataPoints << std::endl;
    std::vector<double> normalAngles; 
    std::vector<double> intensities; 
    std::vector<double> errors; 
  
    for (int normalAngle = normalMin; normalAngle < normalMax + normalDelta; normalAngle += normalDelta) {

      char filename[1000];
      sprintf(filename, "./zero_angle_sweep_highstats/angle_%d/no_cylinder_angle_0_%d.root", normalAngle, normalAngle);
      double photonCount = photonTracker(filename, normalAngle, sampleAngle, diagnosticAngle);
  
      normalAngles.push_back(normalAngle); 
      intensities.push_back(photonCount); 
      errors.push_back(sqrt(photonCount));      

  }
  
  double* normalArray = &normalAngles[0]; 
  double* intensityArray = &intensities[0]; 
  double* errorArray = &errors[0]; 
 
  TGraphErrors* gr = new TGraphErrors(normalAngles.size(), normalArray, intensityArray, 0, errorArray); 
  gr->SetMarkerStyle(21);
  gr->SetLineWidth(2);
  gr->SetLineStyle(9);
  gr->GetXaxis()->SetTitle("Angle from Normal / degrees");
  gr->GetYaxis()->SetTitle("Number Of Photons");
  
  outf->WriteTObject(gr); 
  mg->Add(gr); 

 } 
 
 std::cout << "LOOP COMPLETE" << std::endl;
 
 outf->WriteTObject(mg); 
 TCanvas* canv = new TCanvas("c", "c", 1000, 600);
 //mg->GetXaxis()->SetTitle("Angle from Normal / degrees"); 
 //mg->GetYaxis()->SetTitle("Normalised Intensity"); 
 mg->Draw("apc");
 mg->GetXaxis()->SetTitle("Angle from Normal / Degrees");
 mg->GetYaxis()->SetTitle("Number of Photons");
 canv->BuildLegend();
 outf->WriteTObject(canv);

 std::cout << "COMPLETE!" << std::endl;

}
