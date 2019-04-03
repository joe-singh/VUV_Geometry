# include <iostream>
# include <sstream>
# include <algorithm>
# include <math.h>
# include <vector>
# include <algorithm>
# include <TROOT.h>
# include <TFile.h>
# include <TTree.h>
# include <TH1D.h>
# include <TH2D.h>
# include <TVector3.h>
# include <TVectorD.h>
# include <TSystem.h>
# include <TCanvas.h>
# include <TMultiGraph.h>
# include <TGraphErrors.h>
# include <TMath.h>
# include <TLine.h>
# include <RAT/DS/Root.hh>
# include <RAT/DS/MC.hh>
# include <RAT/DS/MCTrack.hh>
# include <RAT/DS/MCTrackStep.hh>

const double PI = 3.14159265358979323846;

double sigmoid(double *x, double *params) {
	return params[0]/(1+ TMath::Exp(-params[2]*(x[0]-params[1])));
}

double logistic_general(double *x, double *params) {
	return params[0]/TMath::Power(1 + params[3]*TMath::Exp(-params[2]*(x[0]-params[1])), 1/params[4]);
}

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
	//std::cout << fname << std::endl;
	TFile* file = new TFile(fname.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "File " << fname << " is zombie, will skip it." << std::endl;
    throw std::bad_alloc();
  }
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
	//std::cout << "Num Entries: " << nentries << std::endl;

	//for (int iEntry = 0; iEntry < nentries; iEntry++) {
	for (int iEntry = 0; iEntry < 10000; iEntry++) {

		//if (iEntry % 1000 == 0) {
		//	std::cout << "At entry " << iEntry << std::endl;
		//}

		tr->GetEntry(iEntry); 
		RAT::DS::MC* rmc = rds->GetMC(); 
		int numMCTracks = rmc->GetMCTrackCount(); 

		for (int iTrack = 0; iTrack < numMCTracks; iTrack++) {

			RAT::DS::MCTrack* photonTrack = rmc->GetMCTrack(iTrack); 
			int numSteps = photonTrack->GetMCTrackStepCount(); 

			for (int iStep = 0; iStep < numSteps; iStep++) {

				RAT::DS::MCTrackStep* photonStep = photonTrack->GetMCTrackStep(iStep);
				double yComponent = photonStep->GetMomentum().Dot(hatY); 

				//double xPos = photonStep->GetEndpoint().x();
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

				if (photonStep->GetVolume() == "verticalSurface" || photonStep->GetVolume() == "photodiodetest") {
					numPhotodiode++; 
					//       TVector3 photoDiodeNormal = hatY;//getNormal(normalAngle); 
					//RAT::DS::MCTrackStep* p = photonTrack->GetMCTrackStep(iStep-1);
					//RAT::DS::MCTrackStep* pp = photonTrack->GetMCTrackStep(iStep-2);
					//RAT::DS::MCTrackStep* ppp = photonTrack->GetMCTrackStep(iStep-3);
					double cos_theta = (photonStep->GetMomentum().Dot(hatY))/(photonStep->GetMomentum().Mag() * hatY.Mag());
					//std::cout << cos_theta << std::endl;
					downstreamAngle->Fill(cos_theta); 
				}

			}

		}
	}
  delete rds;

	//std::cout << "UPSTREAM NUMBER FOR ANGLE " << normalAngle << " " << numCube << std::endl;
	//std::cout << "NUMBER FOR ANGLE " << normalAngle << " " << numPhotodiode << std::endl;
	diagnostic->WriteTObject(upstreamAngle);
	diagnostic->WriteTObject(downstreamAngle); 
	diagnostic->WriteTObject(yzPositions);
	diagnostic->WriteTObject(wavelength); 

  delete upstreamAngle;
  delete downstreamAngle;
  delete yzPositions;
  delete wavelength;

	return double(numPhotodiode); 

} 

void loopOverFiles(int sampleMin, int sampleMax, int sampleDelta, 
				   int normalMin, int normalMax, int normalDelta) {
	TGraphErrors* peakHeightVsSampleAngle = NULL;
	//TFile* fin = new TFile("peakHeightVsIncidenceAngle_plots.root", "Read"); // try retrieve graph from pre-existing file
	//fin->GetObject("peakHeightVsSampleAngle", peakHeightVsSampleAngle);
	////std::cout << "peakHeightVsSampleAngle" << peakHeightVsSampleAngle << ";" << std::endl;
	//fin->Close();
	//delete fin;

	if (!peakHeightVsSampleAngle) { // if graph does not exist, create it
		gROOT->SetBatch(kTRUE); 
		TFile* diagnosticAngle = new TFile("peakHeightVsIncidenceAngle_angularDiagnostics.root", "RECREATE");   

		std::vector<double> intensityPeakHeights;
		std::vector<double> intensityPeakHeights_err;
		std::vector<double> sampleAngles;

		for (int sampleAngle = sampleMin; sampleAngle < sampleMax + sampleDelta; sampleAngle += sampleDelta) {
      // temporary hack to prioritize speed
      normalMin = sampleAngle - 5;
      normalMax = sampleAngle + 5;

			int numDataPoints = (normalMax - normalMin)/normalDelta + 1;
			std::cout << "numDataPoints: " << numDataPoints << std::endl;
			std::vector<double> normalAngles; 
			std::vector<double> intensities; 

			char histname[1000];
			sprintf(histname, "angularDist_%d", sampleAngle); 
			for (int normalAngle = normalMin; normalAngle < normalMax + normalDelta; normalAngle += normalDelta) {
				char filename[1000];
				//sprintf(filename, "./angle_sweep/angle_%d/no_cylinder_angle_%d.%d.root", sampleAngle, sampleAngle,normalAngle);
				sprintf(filename, "/data/snoplus/home/joesingh/VUV/VUV_Geometry/angle_sweep/angle_%d/no_cylinder_angle_%d.%d.root", sampleAngle, sampleAngle,normalAngle);
				double photonCount = -1;
				try {
					photonCount = photonTracker(filename, normalAngle, sampleAngle, diagnosticAngle);
				} catch (const std::exception& exc) {
					std::cout << "Exception raised: " << exc.what() << std::endl;
					TFile* file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(filename)); // clean up zombie ROOT file that was created in photonTracker() method
					delete file;
					continue;
				}

				normalAngles.push_back(normalAngle - 180); 
				intensities.push_back(photonCount); 
			}

      const double& maxIntensity = *std::max_element(intensities.begin(), intensities.end());
			intensityPeakHeights.push_back(maxIntensity);
      intensityPeakHeights_err.push_back(sqrt(maxIntensity));
			sampleAngles.push_back(sampleAngle);
		} 

		std::cout << "LOOP COMPLETE" << std::endl;

		assert(intensityPeakHeights.size() == sampleAngles.size());
		peakHeightVsSampleAngle = new TGraphErrors(intensityPeakHeights.size(), &sampleAngles[0], &intensityPeakHeights[0], NULL, &intensityPeakHeights_err[0]);
		peakHeightVsSampleAngle->SetTitle("peakHeightVsSampleAngle");
		peakHeightVsSampleAngle->SetName("peakHeightVsSampleAngle");
		TFile* plotsFile = new TFile("peakHeightVsIncidenceAngle_plots.root", "RECREATE"); 
		plotsFile->WriteTObject(peakHeightVsSampleAngle);
		plotsFile->Close();
		delete plotsFile;
		//peakHeightVsSampleAngle->Draw("A:L:P");
		//gPad->SaveAs("peakHeightVsIncidenceAngle_plots.root");

    delete diagnosticAngle;
	}

	//TF1 fitFunction("fitFunction", &sigmoid, sampleMin, sampleMax, 3);
	//fitFunction.SetParNames("amplitude", "location", "scale");
	//double amplitude_init;
	//double junk;
	////std::cout << "peakHeightVsSampleAngle" << peakHeightVsSampleAngle << ";" << std::endl;
	//peakHeightVsSampleAngle->GetPoint(0, junk, amplitude_init);
	//std::cout << "amplitude_init" << amplitude_init << ";" << std::endl;
	//std::cout << "sampleMin" << sampleMin << ";" << std::endl;
	//std::cout << "sampleMax" << sampleMax << ";" << std::endl;
	//fitFunction.SetParameters(amplitude_init, 235, -0.3);
	//fitFunction.SetParLimits(0, 0, 10*amplitude_init);
	//fitFunction.SetParLimits(1, sampleMin, sampleMax);
	//fitFunction.SetParLimits(2, -50, 0);

	// Prepare fit
	TF1 fitFunction("fitFunction", &logistic_general, sampleMin, sampleMax, 5);
	fitFunction.SetParNames("amplitude", "location", "scale", "Q", "nu");
	double amplitude_init;
	double junk;
	peakHeightVsSampleAngle->GetPoint(0, junk, amplitude_init);
	fitFunction.SetParameters(amplitude_init, 235, -0.3, 1, 1);

	// Draw
	TCanvas canvas("peakHeightVsSampleAngle_canvas", "peakHeightVsSampleAngle_canvas");
	canvas.Draw();
	canvas.cd();
	peakHeightVsSampleAngle->SetMarkerStyle(8);
	peakHeightVsSampleAngle->Draw("AP");
	peakHeightVsSampleAngle->GetXaxis()->SetTitle("Nominal beam incidence angle [degree]");
	peakHeightVsSampleAngle->GetYaxis()->SetTitle("Number of photons");

	peakHeightVsSampleAngle->Fit("fitFunction", "R"); // do fit

	// Find threshold at 90 % of maximum intensity
	double amplitude = fitFunction.GetParameter(0);
	double y_threshold = 0.9*amplitude;
	double x_threshold = fitFunction.GetX(y_threshold, sampleMin, sampleMax);
	std::cout << "x_threshold: " << x_threshold - 180 << ";" << std::endl;
	TLine threshold_horizontal(peakHeightVsSampleAngle->GetXaxis()->GetXmin(), y_threshold, x_threshold, y_threshold);
	threshold_horizontal.SetLineStyle(2);
	threshold_horizontal.Draw("Same");
	TLine threshold_vertical(x_threshold, peakHeightVsSampleAngle->GetYaxis()->GetXmin(), x_threshold, y_threshold);
	threshold_vertical.SetLineStyle(2);
	threshold_vertical.Draw("Same");
  canvas.Modified();
  canvas.Update();

	// Save fitted function
	TFile* outf = new TFile("peakHeightVsIncidenceAngle_result.root", "RECREATE"); 
	outf->WriteTObject(peakHeightVsSampleAngle);
	outf->WriteTObject(&canvas);
	outf->Close();
	delete outf;

	std::cout << "COMPLETE!" << std::endl;

  delete peakHeightVsSampleAngle;
}

int main(int argc, char* argv[]) {
	int samplemin = strToInt(argv[1]);
	int samplemax = strToInt(argv[2]); 
	int sampledelta = strToInt(argv[3]); 
	int normalmin = strToInt(argv[4]);
	int normalmax = strToInt(argv[5]); 
	int normaldelta = strToInt(argv[6]);  
	loopOverFiles(samplemin, samplemax, sampledelta, normalmin, normalmax, normaldelta); 
	return 0; 
}
