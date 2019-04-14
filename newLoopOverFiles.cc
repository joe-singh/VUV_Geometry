# include <iostream>
# include <sstream>
# include <exception>
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
# include <vector>
# include <algorithm>
# include <TCanvas.h>
# include <math.h>
# include <TMultiGraph.h>
# include <TLegend.h>
# include <TGraphErrors.h>
# include <TMath.h>
# include <TLine.h>
# include <RAT/DS/Root.hh>
# include <RAT/DS/MC.hh>
# include <RAT/DS/MCTrack.hh>
# include <RAT/DS/MCTrackStep.hh>
# include <TSystem.h>

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
	std::cout << fname << std::endl;
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
	std::cout << "Num Entries: " << nentries << std::endl;

	//for (int iEntry = 0; iEntry < nentries; iEntry++) {
	for (int iEntry = 0; iEntry < nentries; iEntry++) {

		if (iEntry % int(nentries/10) == 0) {
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
	delete rds; // clean up

	std::cout << "UPSTREAM NUMBER FOR ANGLE " << normalAngle << " " << numCube << std::endl;
	std::cout << "NUMBER FOR ANGLE " << normalAngle << " " << numPhotodiode << std::endl;
	diagnostic->WriteTObject(upstreamAngle);
	diagnostic->WriteTObject(downstreamAngle); 
	diagnostic->WriteTObject(yzPositions);
	diagnostic->WriteTObject(wavelength); 

	delete upstreamAngle; // clean up
	delete downstreamAngle;
	delete yzPositions;
	delete wavelength;

	return double(numPhotodiode); 
} 

void newLoopOverFiles(int sampleMin, int sampleMax, int sampleDelta, 
					  int normalMin, int normalMax, int normalDelta) {

	gROOT->SetBatch(kTRUE); 
	TFile* outf = new TFile("angular_sweeps.root", "RECREATE"); 
	TMultiGraph* mg = new TMultiGraph(); 
	TFile* diagnosticAngle = new TFile("angularDiagnostics.root", "RECREATE");   

	std::vector<double> intensityPeakHeights;
	std::vector<double> intensityPeakHeights_err;
	std::vector<double> sampleAngles;

	unsigned int color = 1;
	for (int sampleAngle = sampleMin; sampleAngle < sampleMax + sampleDelta; sampleAngle += sampleDelta) {
		//normalMin = sampleAngle - 5;
		//normalMax = sampleAngle + 5;
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
			double photonCount = -1; // initialize to nonsense value
			try {
				photonCount = photonTracker(filename, normalAngle, sampleAngle, diagnosticAngle);
			} catch (const std::exception& exc) {
				std::cout << "Exception raised: " << exc.what() << std::endl;
				TFile* file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(filename)); // clean up zombie ROOT file that was created in photonTracker() method
				delete file;
				continue;
			}

			normalAngles.push_back(normalAngle); 
			intensities.push_back(photonCount); 
			errors.push_back(sqrt(photonCount));      

			hist->Fill(normalAngle, photonCount); 
		}
		assert(normalAngles.size() == intensities.size()); // sanity checks
		assert(normalAngles.size() == errors.size());

		if (intensities.size() == 0) {
			std::cout << "Could not read any photon intensities. Are you setting the correct PMT viewing angles to read?" << std::endl;
			return;
		}

		// Skip colors I don't like
		color = color % 10;
		if (color % 10 == 0) {
			color++;
		}

		sampleAngles.push_back(sampleAngle);
		const double maxIntensity = *std::max_element(intensities.begin(), intensities.end());
		intensityPeakHeights.push_back(maxIntensity);
		intensityPeakHeights_err.push_back(sqrt(maxIntensity));

		double* normalArray = &normalAngles[0]; 
		double* intensityArray = &intensities[0]; 
		double* errorArray = &errors[0]; 

		TGraphErrors* gr = new TGraphErrors(normalAngles.size(), normalArray, intensityArray, 0, errorArray); 
		gr->SetMarkerStyle(21);
		gr->SetLineWidth(2);
		gr->SetLineStyle(9);
		gr->SetMarkerColor(color); 
		gr->SetLineColor(color); 
		gr->SetFillStyle(0);
		gr->GetXaxis()->SetTitle("Angle from Normal / degrees");
		gr->GetYaxis()->SetTitle("Number Of Photons");

		gr->SetTitle(histname); 
		gr->SetName(histname);  
		//outf->WriteTObject(gr); 
		outf->WriteTObject(hist); 
		mg->Add(gr); 

		color++;
	} 
	assert(sampleAngles.size() == intensityPeakHeights.size()); // sanity checks
	assert(sampleAngles.size() == intensityPeakHeights_err.size());
	std::cout << "LOOP COMPLETE" << std::endl;

	{ // Plot peak shapes vs incidence angle
		mg->SetName("multigraph_peakShapesVsIncidenceAngle");  
		outf->WriteTObject(mg); 
		TCanvas canvas_0("canvas_peakShapesVsIncidenceAngle", "canvas_peakShapesVsIncidenceAngle", 1000, 600);
		mg->Draw("a:p:l");
		mg->GetXaxis()->SetTitle("Angle from Normal [degree]");
		mg->GetYaxis()->SetTitle("Number of Photons");
		mg->GetXaxis()->SetLimits(sampleMin - 10, sampleMax + 10);
		canvas_0.BuildLegend();
		canvas_0.Modified();
		canvas_0.Update();
		outf->WriteTObject(&canvas_0);
	}

	{ // Plot peak heights vs incidence angle
		TGraphErrors* peakHeightVsIncidenceAngle = new TGraphErrors(intensityPeakHeights.size(), &sampleAngles[0], &intensityPeakHeights[0], NULL, &intensityPeakHeights_err[0]);
		peakHeightVsIncidenceAngle->SetTitle("");
		peakHeightVsIncidenceAngle->SetName("graph_peakHeightVsIncidenceAngle");

		// Draw
		TCanvas canvas_1("canvas_peakHeightVsIncidenceAngle", "canvas_peakHeightVsIncidenceAngle");
		canvas_1.Draw();
		canvas_1.cd();
		peakHeightVsIncidenceAngle->SetMarkerStyle(8);
		peakHeightVsIncidenceAngle->Draw("a:p");
		peakHeightVsIncidenceAngle->GetXaxis()->SetTitle("Nominal beam incidence angle [degree]");
		peakHeightVsIncidenceAngle->GetYaxis()->SetTitle("Number of photons");

		// Fit with function
		TF1 fitFunction("fitFunction", &logistic_general, sampleMin, sampleMax, 5);
		fitFunction.SetParNames("amplitude", "location", "scale", "Q", "nu");
		double amplitude_init, junk;
		peakHeightVsIncidenceAngle->GetPoint(0, junk, amplitude_init);
		fitFunction.SetParameters(amplitude_init, 0.5*(sampleMin + sampleMax), -1, 1, 1);
		peakHeightVsIncidenceAngle->Fit("fitFunction", "R"); // do fit

		//// Find threshold at 90 % of maximum intensity
		//double amplitude = fitFunction.GetParameter(0);
		//double y_threshold = 0.9*amplitude;
		//double x_threshold = fitFunction.GetX(y_threshold, sampleMin, sampleMax);
		//std::cout << "x_threshold: " << x_threshold - 180 << ";" << std::endl;
		//TLine threshold_horizontal(peakHeightVsIncidenceAngle->GetXaxis()->GetXmin(), y_threshold, x_threshold, y_threshold);
		//threshold_horizontal.SetLineStyle(2);
		//threshold_horizontal.Draw("Same");
		//TLine threshold_vertical(x_threshold, peakHeightVsIncidenceAngle->GetYaxis()->GetXmin(), x_threshold, y_threshold);
		//threshold_vertical.SetLineStyle(2);
		//threshold_vertical.Draw("Same");
		//canvas_1.Modified();
		//canvas_1.Update();

		// Save fitted function
		outf->WriteTObject(peakHeightVsIncidenceAngle);
		outf->WriteTObject(&canvas_1);
		outf->Close();
	}

	std::cout << "COMPLETE!" << std::endl;

	delete outf;
	//delete peakHeightVsIncidenceAngle;
	delete diagnosticAngle;
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
