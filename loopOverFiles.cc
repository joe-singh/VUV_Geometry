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

double photonTracker(std::string fname, double normalAngle, double sampleAngle, TFile* wavelengthFile) {
  char wavelengthPlotName[1000];
  sprintf(wavelengthPlotName, "no_cylinder_angle_%d_%d", sampleAngle, normalAngle);
  char dotProductPlotName[1000];
  sprintf(dotProductPlotName,"downstreamDotProduct_%d_%d", sampleAngle, normalAngle);   
  char positionPlotName[1000]; 
  sprintf(positionPlotName, "photonDownstreamDistribution_%d_%d", sampleAngle, normalAngle);
  char frontOnPositionPlotName[1000]; 
  sprintf(frontOnPositionPlotName, "photonDownStreamFrontOnDistribution_%d_%d", sampleAngle, normalAngle); 
  
  TFile* file = new TFile(fname.c_str(), "READ"); 
  TTree* tr = (TTree*) file->Get("T"); 
  
  RAT::DS::Root* rds = new RAT::DS::Root(); 
  tr->SetBranchAddress("ds", &rds); 
  
  TH2F* yzHist = new TH2F("yzHist", "yzHist", 1000, -500, 500, 1000, -500, 500);  
  TH2F* xzHist = new TH2F("xzHist", "xzHist", 100, -500, 500, 100, -500, 500); 
  
  TH1F* outputSpectrum = new TH1F("wavelengths", "PhotodiodeWavelengths", 300, 200, 600); 
  TH1F* incidentSpectrum = new TH1F("incidentWavelength", "Incident Wavelengths", 300, 200, 600); 
  TH1F* producedInsideTPB = new TH1F("producedTPB", "Photons produced in TPB", 300, 200, 600);  
  TH1F* trackNumber = new TH1F("TrackNumber", "Track Number/Event", 10, 0, 10);
  TH1F* multiEventPhoton = new TH1F("multiEventPhoton", "Multi Photon Event Wavelength", 300, 200, 600);  
  TH1F* downstreamDotProduct = new TH1F(dotProductPlotName, "Downstream angle", 200, -1, 1);  
  TH1F* upstreamDotProduct = new TH1F("upstreamDotProduct", "Upstream angle", 200, -90, 90);   
  TH1F* angularDist = new TH1F("angularDist", "Angular Dist On Cylinder", 100, -1, 1); 
  
  TH1F* azimuthalDotProduct = new TH1F("azimuthalDistribution", "Azimuthal Distribution relative to x hat", 100, -1, 1); 
  int nentries = tr->GetEntries(); 
  
  TVector3 hatX = TVector3(1.0,0,0);
  TVector3 hatY = TVector3(0,1.0,0);
  TVector3 hatZ = TVector3(0,0,1.0);
  TVector3 hat45 = TVector3(0, 1, 1); 
  TVector3 hat60 = TVector3(0, 1, 1.73205080757);  
  TVector3 Normal = hatY; 
 
  int numLeftTPB = 0;
  int numStayedInTPB = 0; 
  int numCube = 0; 
  int numQuartz = 0; 
  int numPhotodiode = 0;
  int numEnteringTPB = 0;  
  bool continueTrackLoop = false; 

  std::cout << "Num Entries: " << nentries << std::endl;
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    
    if (iEntry % 1000 == 0) {
      std::cout << "At entry " << iEntry << std::endl;
    }
    
    tr->GetEntry(iEntry); 
    RAT::DS::MC* rmc = rds->GetMC();

    int numMCTracks = rmc->GetMCTrackCount(); 
    
    trackNumber->Fill(numMCTracks);  
    
    for (int iTrack = 0; iTrack < numMCTracks; iTrack++) {

    RAT::DS::MCTrack* photonTrack = rmc->GetMCTrack(iTrack); 
    int numSteps = photonTrack->GetMCTrackStepCount(); 
     
    RAT::DS::MCTrackStep* lastStep = photonTrack->GetMCTrackStep(numSteps - 1); 

    double xPos = lastStep->GetEndpoint().x(); 
    double yPos = lastStep->GetEndpoint().y(); 
    double zPos = lastStep->GetEndpoint().z(); 
    
    bool enteredTPB = false; 
    bool leftTPB = false; 
    bool enteredQuartzIncident = false; 
    bool goneThroughDownStream = false; 
    
    for (int iStep = 0; iStep < numSteps; iStep++) {
      RAT::DS::MCTrackStep* photonStep = photonTrack->GetMCTrackStep(iStep);

      double xPos = photonStep->GetEndpoint().x(); 
      double yPos = photonStep->GetEndpoint().y(); 
      double zPos = photonStep->GetEndpoint().z(); 
      yzHist->Fill(yPos, zPos); 
      double yComponent = photonStep->GetMomentum().Dot(hatY);     
     // Count all photons going through hole in positive direction, these enter the cube. 
     if (photonStep->GetVolume() == "test2" /*|| photonStep->GetVolume() == "test2" || photonStep->GetVolume() == "test1" || photonStep->GetVolume() == "test4"*/) {
          goneThroughDownStream = true;
          double photonWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
          double cos_theta = (photonStep->GetMomentum().Dot(Normal))/(photonStep->GetMomentum().Mag() * Normal.Mag());
          downstreamDotProduct->Fill(cos_theta);
          
          double azimuthal_cos = (photonStep->GetMomentum().Dot(hatX))/(photonStep->GetMomentum().Mag()); 

     } 
     
     if (photonStep->GetVolume() == "testdisk" && yComponent >= 0.0)  {
       double angle = acos(yComponent/(photonStep->GetMomentum().Mag()));
       upstreamDotProduct->Fill(angle * 180/PI); 
       numCube++;
     }

     // If we leave the tpb, check if hit the photodiode. If so, count it and end the step loop, 
     // we are done with this photon. 
     double photonWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
     if (photonStep->GetVolume() == "photodiodetest" /*&& photonWavelength > 400.0*/ /* yComponent > 0.0 && goneThroughDownStream */) { 
       RAT::DS::MCTrackStep* p = photonTrack->GetMCTrackStep(iStep-1); 
       RAT::DS::MCTrackStep* pp = photonTrack->GetMCTrackStep(iStep-2); 
       
//      std::cout << pp->GetVolume() << " -> " << p->GetVolume() << " -> " << photonStep->GetVolume() << std::endl;
      
         numPhotodiode++;
 
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       outputSpectrum->Fill(visibleWavelength); 
       //break; 
     }      
    /*  
     // Check if the incoming photon has entered the quartz before hitting the TPB. 
     if (photonStep->GetVolume() == "samplehousing" && !enteredTPB) {
       enteredQuartzIncident = true; 
     } 
     
     // Produced inside tpb
     if (iStep == 0 && photonStep->GetVolume() == "tpb3") {  
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       producedInsideTPB->Fill(visibleWavelength);
       enteredTPB = true;  
     } 

     // If we enter the tpb activate the flag saying we have entered. This is 
     // not considering photons produced inside the tpb, only those incident so
     // cannot be on the first step.  
     if (photonStep->GetVolume() == "tpb3" && !enteredTPB && iStep != 0) {
       enteredTPB = true; 
       numEnteringTPB++; 
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       incidentSpectrum->Fill(visibleWavelength); 
     } 
     
      // If we have entered but the volume has changed, it means the photon must have left the
     // tpb. 
     if (enteredTPB && photonStep->GetVolume() != "tpb3" && !leftTPB) {
       leftTPB = true; 
       numLeftTPB++;
     } 
    
     double yComponentAfterTPB = photonStep->GetMomentum().Dot(hatY); 
     
     // Check for photons leaving the tpb+acrylic and now coming out of the other side. 
     // XXX THIS IS HARD CODED FOR 0 DEGREES!!!!!!!
     if (photonStep->GetVolume() == "test3" && enteredTPB) {
        double photonWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;

          double cos_theta = (photonStep->GetMomentum().Dot(Normal))/(photonStep->GetMomentum().Mag() * Normal.Mag()); 
          double angle = acos(cos_theta) * 180/PI;
 
          double zComp = photonStep->GetMomentum().Dot(hatZ); 
          if (zComp < 0) {angle = -angle;}  
          downstreamDotProduct->Fill(angle); // convert to degrees 
     }  
     // If we leave the tpb, check if hit the photodiode. If so, count it and end the step loop, 
     // we are done with this photon. 
     if (leftTPB && photonStep->GetVolume() == "photodiodetest") {
       numPhotodiode++;
       continueTrackLoop = true;
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       outputSpectrum->Fill(visibleWavelength); 
       break; 
     } */ 
     
    } 
   
    }
   
   }
 
  TCanvas* wavelength = new TCanvas(wavelengthPlotName, wavelengthPlotName, 1000, 600); 
  incidentSpectrum->SetLineColor(kBlue); 
  outputSpectrum->SetLineColor(kRed); 
  incidentSpectrum->Draw(); 
  outputSpectrum->Draw("same"); 
//  wavelengthFile->WriteTObject(wavelength); 
  wavelengthFile->WriteTObject(downstreamDotProduct); 
  wavelengthFile->WriteTObject(upstreamDotProduct);
  TCanvas* positions = new TCanvas(positionPlotName, "positions", 1000, 600); 
  yzHist->Draw(); 
//  TCanvas* frontOnPosition = new TCanvas(frontOnPositionPlotName, "frontOnView", 1000, 600); 
//  xzHist->Draw("COLZ"); 
//  wavelengthFile->WriteTObject(frontOnPosition);  
  wavelengthFile->WriteTObject(positions); 
  wavelengthFile->WriteTObject(angularDist); 
  wavelengthFile->WriteTObject(azimuthalDotProduct); 
  file->Close();

  std::cout << "UPSTREAM NUMBER FOR ANGLE " << normalAngle << " " << numCube << std::endl;  
  std::cout << "NUMBER FOR ANGLE " << normalAngle << " " << numPhotodiode << std::endl;  
  return double(numPhotodiode); 
}


void loopOverFiles() {
  gROOT->SetBatch(kTRUE);
  TFile* outf = new TFile("angularTPBHighStats.root", "RECREATE");   
  TMultiGraph* mg = new TMultiGraph();
  TFile* wavelengthFile = new TFile("wavelength_zero_sweep.root", "RECREATE"); 

  for (int sampleAngle = 0; sampleAngle < 5; sampleAngle += 5) {
    
    double normalAngles[38]; 
    double intensities[38]; 
    double errors[38]; 
    
    TH1F* angularDist = new TH1F("angularDist", "angularDist", 10, -20, 20); 

    for (int normal = -95; normal < 100; normal += 5) {
  
      char filename[1000];
      sprintf(filename, "./zero_angle_sweep_highstats/no_cylinder_angle_0_%d.root", normal);
      double ratio = photonTracker(filename, normal, sampleAngle, wavelengthFile); 
      int index = normal/5 + 19;     
      normalAngles[index] = normal; 
      intensities[index] = ratio;
      errors[index] = sqrt(ratio); 
      angularDist->Fill(normal, ratio);  
    }   

    char graphTitle[1000]; 
    char graphName[1000]; 
    sprintf(graphTitle, "No_Cylinder_Angular_SweepAt_%d_degrees", sampleAngle);
    sprintf(graphName, "No_Cylinder_Angular_SweepAt_%d_degrees", sampleAngle); 
 
    TGraphErrors* gr = new TGraphErrors(39, normalAngles, intensities, 0, errors); 
    gr->SetTitle(graphTitle);
    gr->SetName(graphName); 
    gr->SetMarkerColor(int(sampleAngle/5)+1);
    gr->SetMarkerStyle(21);  
    gr->SetLineWidth(2); 
    gr->SetLineColor(int(sampleAngle/5)+1);
    gr->SetLineStyle(9);  
    gr->GetXaxis()->SetTitle("Angle from Normal / degrees"); 
    gr->GetYaxis()->SetTitle("Number Of Photons");  
    //TCanvas* c = new TCanvas("c", "c", 1000, 600);
    //c->SetName(graphName); 
    //gr->Draw("AP*");
    outf->WriteTObject(gr);  
    outf->WriteTObject(angularDist);
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

