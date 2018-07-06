#include <iostream>
# include <TFile.h>
# include <TTree.h>
# include <TH1D.h>
# include <TH2D.h>
# include <TVector3.h>
# include <TVectorD.h>
# include <TSystem.h>
# include <vector>

void photonTracker(std::string fname) {
  TFile* file = new TFile(fname.c_str(), "r"); 
  TTree* tr = (TTree*) file->Get("T"); 
  
  RAT::DS::Root* rds = new RAT::DS::Root(); 
  tr->SetBranchAddress("ds", &rds); 
  
  TH1F* xHist = new TH1F("xHist", "xHist", 100, -500,500); 
  TH1F* yHist = new TH1F("yHist", "yHist", 100, -500, 500); 
  TH1F* zHist = new TH1F("zHist", "zHist", 100, -500, 500);  
  TH2F* yzHist = new TH2F("yzHist", "yzHist", 10000, -500, 500, 10000, -500, 500);  
  TH2F* xzHist = new TH2F("xzHist", "xzHist", 10000, -500, 500, 10000, -500, 500); 
  TH1F* outputSpectrum = new TH1F("wavelengths", "PhotodiodeWavelengths", 300, 200, 600); 
  TH1F* incidentSpectrum = new TH1F("incidentWavelength", "Incident Wavelengths", 300, 200, 600); 
  TH1F* producedInsideTPB = new TH1F("producedTPB", "Photons produced in TPB", 300, 200, 600);  
  TH1F* trackNumber = new TH1F("TrackNumber", "Track Number/Event", 10, 0, 10);
  TH1F* multiEventPhoton = new TH1F("multiEventPhoton", "Multi Photon Event Wavelength", 300, 200, 600);  
 
  int nentries = tr->GetEntries(); 
  
  TVector3 hatX = TVector3(1,0,0);
  TVector3 hatY = TVector3(0,1,0);
  TVector3 hatZ = TVector3(0,0,1); 
 
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
    //if (numMCTracks == 1) continue; 
    
    for (int iTrack = 0; iTrack < numMCTracks; iTrack++) {

    RAT::DS::MCTrack* photonTrack = rmc->GetMCTrack(iTrack); 
    int numSteps = photonTrack->GetMCTrackStepCount(); 
     
    RAT::DS::MCTrackStep* lastStep = photonTrack->GetMCTrackStep(numSteps - 1); 

    double xPos = lastStep->GetEndpoint().x(); 
    double yPos = lastStep->GetEndpoint().y(); 
    double zPos = lastStep->GetEndpoint().z(); 
    double yComponent = lastStep->GetMomentum().Dot(hatY); 
    
    bool enteredTPB = false; 
    bool leftTPB = false; 
    bool enteredQuartzIncident = false; 
  
    if (yPos >= -12.000 * 25.4 || yComponent >= 0.0 && yPos < -12.0 * 25.4) {    
     xzHist->Fill(zPos, xPos);
     yzHist->Fill(yPos, zPos);
    } 
    
    for (int iStep = 0; iStep < numSteps; iStep++) {
      RAT::DS::MCTrackStep* photonStep = photonTrack->GetMCTrackStep(iStep);
 
      double xPos = photonStep->GetEndpoint().x(); 
      double yPos = photonStep->GetEndpoint().y(); 
      double zPos = photonStep->GetEndpoint().z(); 
      xHist->Fill(xPos); 
      yHist->Fill(yPos); 
      zHist->Fill(zPos);
     
//      if (numMCTracks >= 2 && iStep == 0 && iTrack >= 2) {
//        if (photonStep->GetVolume() != "tpb3") {
//          std::cout << "Photon from multi track event produced in " << photonStep->GetVolume() << std::endl; 
//        } else {
//          double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
//          multiEventPhoton->Fill(visibleWavelength); 
//        }
//      }      
      double yComponent = photonStep->GetMomentum().Dot(hatY);     
     // Count all photons going through hole in positive direction, these enter the cube. 
     if (photonStep->GetVolume() == "testdisk" && yComponent >= 0.0)  {numCube++;}     
      
     // Check if the incoming photon has entered the quartz before hitting the TPB. 
     if (photonStep->GetVolume() == "samplehousing" && !enteredTPB) {
       enteredQuartzIncident = true; 
     } 
     
     // Produced inside tpb
     if (iStep == 0 && photonStep->GetVolume() == "tpb3") {  
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       producedInsideTPB->Fill(visibleWavelength); 
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
 
     // Entered quartz but never got to tpb. numQuartz + numTPB = numCube ideally, since either
     // incoming photons stick in the quartz or go on to the tpb. 
     else if (enteredQuartzIncident && !enteredTPB && iStep == numSteps - 1) {
       numQuartz++; 
     }  
     
     // If we have entered but the volume has changed, it means the photon must have left the
     // tpb. 
     if (enteredTPB && photonStep->GetVolume() != "tpb3" && !leftTPB) {
       leftTPB = true; 
       numLeftTPB++;
       continue;  
     } 
    
     // If we leave the tpb, check if hit the photodiode. If so, count it and end the step loop, 
     // we are done with this photon. 
     if (leftTPB && photonStep->GetVolume() == "photodiodetest") {
       numPhotodiode++;
       continueTrackLoop = true;
       double visibleWavelength = 1.2398/(photonStep->GetKE()*pow(10,6))*1000.;
       outputSpectrum->Fill(visibleWavelength); 
       break; 
     } 
     
    } 
   
    }
   
   }
  
  TCanvas* c3 = new TCanvas("c3", "c", 1000, 600);
  xzHist->Draw(); 
  TCanvas* c4 = new TCanvas("c4", "c", 1000, 600); 
  yzHist->Draw(); 
  TCanvas* c5 = new TCanvas("c5", "c", 1000, 600); 
  incidentSpectrum->SetLineColor(kRed); 
  producedInsideTPB->SetLineColor(kBlue); 
  incidentSpectrum->Draw(); 
  outputSpectrum->Draw("same"); 
  producedInsideTPB->Draw("same"); 
  //multiEventPhoton->SetLineColor(kGreen);
  //multiEventPhoton->Draw("same");  
 
   auto legend = new TLegend(0.1,0.7,0.48,0.9);
   legend->AddEntry(incidentSpectrum,"Incident Spectrum (enterTPB == True)","l");
   legend->AddEntry(producedInsideTPB, "Spectrum of photons produced in TPB","l");
   legend->AddEntry(outputSpectrum, "Photodiode Spectrum","l");
   legend->Draw();
 
  TCanvas* c6 = new TCanvas("c6", "c", 1000, 600); 
  trackNumber->Draw(); 

  std::cout << "Number of photons entering cube " << numCube << std::endl;
  std::cout << "Number of photons stuck in quartz + Number of photons entering TPB = " << numQuartz + numEnteringTPB << std::endl;
  std::cout << "Number of photons entering TPB " << numEnteringTPB << std::endl;
  std::cout << "Number of photons leaving TPB " << numLeftTPB << std::endl;
  std::cout << "Total photon tracks on histogram " << yzHist->GetEntries() << std::endl;
  std::cout << "Ratio of photons incident on tpb to photons incident on photodiode " << double(numPhotodiode)/numEnteringTPB << std::endl;
}
