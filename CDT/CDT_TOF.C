// tof = 252.778 * L * lamda; (us, m and A)
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

using namespace std;

void TH1D_setting(TH1D *);
void TH2D_setting(TH2D *);
void CDT_TOF(string filename, string mode = "all", string projection = "no")
{
	const double Length = 8.971 + 4.955 + 0.04; // m
	const double padSize = 1.5625;				// mm

	int channel = 64;	// channels of detector (x or y)
	int width_bin = 80; // per bin (us)
	int n_line = 500;	// number of bin

	gStyle->SetPalette(1);	  // set 2D color
	gStyle->SetOptFit(0111);  // set fit parameter
	gStyle->SetOptStat(0000); // set hist parameter

	ifstream rawData;
	rawData.open((filename + ".txt").c_str());
	if (rawData.fail())
	{
		cout << "Can not find the file " << filename << ".txt\n"
			 << "Choose default value: \n"
			 << "Number of strips in X or Y: " << channel << "\n"
			 << "Width of bin:  " << width_bin << " us\n"
			 << "Number of bin: " << n_line << endl;
	}
	else
	{
		cout << "\nProcessing " << filename << ".txt ..." << endl;
		while (!rawData.eof())
		{
			char line[256];
			rawData.getline(line, 256);

			char *token = strtok(line, ":");
			while (token)
			{
				if (strcmp(token, "No of X Stripes or Pixel ") == 0)
				{
					token = strtok(NULL, " ");
					channel = atoi(token);
					cout << "Number of strips in X or Y: " << channel << endl;
				}
				else if (strcmp(token, "Dwell Time ") == 0)
				{
					token = strtok(NULL, " ");
					width_bin = atoi(token) / 10;
					cout << "Width of bin:  " << width_bin << " us" << endl;
				}
				else if (strcmp(token, "No of Bins ") == 0)
				{
					token = strtok(NULL, " ");
					n_line = atoi(token);
					cout << "Number of bin: " << n_line << endl;
				}
				else if (strcmp(token, "Start Time   ") == 0)
				{
					token = strtok(NULL, " ");
					cout << "Start Time:    " << token << endl;
				}
				else if (strcmp(token, "Real Time    ") == 0)
				{
					token = strtok(NULL, " ");
					cout << "Real Time:     " << token << endl;
				}
				else if (strcmp(token, "Stop Time    ") == 0)
				{
					token = strtok(NULL, " ");
					cout << "Stop Time:     " << token << endl;
				}
				else if (strcmp(token, "No of Counts ") == 0)
				{
					token = strtok(NULL, " ");
					cout << "Number of counts: " << token << endl;
				}
				else if (strcmp(token, "Count Rate   ") == 0)
				{
					token = strtok(NULL, " ");
					cout << "Counting rate:    " << token << endl;
				}

				token = strtok(NULL, ":");
			}
		}
		rawData.close();
	}

	// process tof file
	// transform the raw data into 3D histogram
	rawData.open((filename + ".tof").c_str());
	if (rawData.fail())
	{
		cout << "Can not find the file " << filename << ".tof" << endl;
		exit(0);
	}
	cout << "\nProcessing " << filename << ".tof ..." << endl;

	char name[50] = "";
	double waveStart = 0., waveEnd = 0.;
	if (mode != "all")
	{
		// set the wavelength range
		cout << "Please input the wavelength range: ";
		scanf("%lf %lf", &waveStart, &waveEnd);
		cout << "TOF range: " << 252.778 * Length * waveStart << " us - " << 252.778 * Length * waveEnd << " us." << endl;
		sprintf(name, "%.1lfA_%.1lfA", waveStart, waveEnd);
	}

	// create 3D histogram (x, y, t)
	TH3D *hxyt = new TH3D("hxyt", ";x [mm];y [mm];tof [#mus]", channel, 0, padSize * channel, channel, 0, padSize * channel, n_line, 0, n_line * width_bin);
	hxyt->SetTitle(name);

	uint64_t temp;
	for (int i = 0; i < n_line; i++)
	{
		rawData >> temp >> temp; // bin number, bin counts(total)
		for (int j = 0; j < channel; j++)
		{
			for (int k = 0; k < channel; k++)
			{
				rawData >> temp; // counts of every pad (channel*channel) in every width_bin
				// Intercept a specific wavelength range
				if (mode != "all")
					if ((i <= 252.778 * Length * waveStart / width_bin) || (i >= 252.778 * Length * waveEnd / width_bin))
						continue;
				// Fill 3D hist
				hxyt->SetBinContent(j + 1, k + 1, i + 1, temp); // maybe need to exchange j and k.
			}
		}
	}
	rawData.close();

	// 2D position spectrum
	TH2D *hxy = (TH2D *)hxyt->Project3D("yx");

	TH1D *h[4]; // TOF, Wavelength, X, Y

	// time of flight spectrum
	h[0] = (TH1D *)hxyt->Project3D("z");
	h[0]->SetTitle(("Time of Flight " + string(name)).c_str());
	h[0]->GetYaxis()->SetTitle("Counts");

	// wavelength spectrum
	h[1] = new TH1D("", ";Neutron wavelength [#AA];Counts", n_line, 0., (n_line * width_bin) / (252.778 * Length));
	h[1]->SetTitle(("Neutron wavelength " + string(name)).c_str());
	for (int i = 0; i < n_line; i++)
		h[1]->SetBinContent(i + 1, h[0]->GetBinContent(i + 1));

	// position resolution is obtained by projection.
	if (projection != "no")
	{
		h[2] = (TH1D *)hxyt->Project3D("x"); // x
		h[2]->SetTitle(("X Projection " + string(name)).c_str());
		h[2]->GetYaxis()->SetTitle("Counts");

		h[3] = (TH1D *)hxyt->Project3D("y"); // y
		h[3]->SetTitle(("X Projection " + string(name)).c_str());
		h[3]->GetYaxis()->SetTitle("Counts");
	}
	// create canvases for different mode
	const int nCanvas = 5;
	TCanvas *c[nCanvas];
	for (int i = 0; i < (projection != "no" ? nCanvas : (nCanvas - 2)); i++)
	{
		c[i] = new TCanvas();
		if (i == 0)
		{
			c[i]->SetName("2D Profile");
			c[i]->SetTitle("2D Profile");
			c[i]->SetTicks();
			c[i]->SetRightMargin(0.2);
			c[i]->SetLeftMargin(0.2);

			TH2D_setting(hxy);
			hxy->SetTitle("");
			// hxy->SetContour(50);
			// hxy->SetMaximum(1000);
			hxy->GetXaxis()->SetTitleOffset(1.2);
			hxy->GetYaxis()->SetTitleOffset(1);
			hxy->Draw("COLZCONT4");
		}
		else
		{
			c[i]->SetName(h[i - 1]->GetTitle());
			c[i]->SetTitle(h[i - 1]->GetTitle());
			c[i]->SetGrid();
			c[i]->SetTicks();
			TH1D_setting(h[i - 1]);
			h[i - 1]->SetTitle("");
			h[i - 1]->Draw();
		}
	}
}
// 1D histogram setting
void TH1D_setting(TH1D *myHist)
{
	myHist->GetXaxis()->CenterTitle(true);
	myHist->GetXaxis()->SetMaxDigits(4);
	myHist->GetXaxis()->SetLabelFont(22);
	myHist->GetXaxis()->SetTitleFont(22);
	myHist->GetYaxis()->CenterTitle(true);
	myHist->GetYaxis()->SetMaxDigits(4);
	myHist->GetYaxis()->SetLabelFont(22);
	myHist->GetYaxis()->SetTitleFont(22);
}
// 2D histogram setting
void TH2D_setting(TH2D *myHist)
{
	myHist->GetXaxis()->CenterTitle(true);
	myHist->GetXaxis()->SetMaxDigits(4);
	myHist->GetXaxis()->SetLabelFont(22);
	myHist->GetXaxis()->SetTitleFont(22);
	myHist->GetYaxis()->CenterTitle(true);
	myHist->GetYaxis()->SetMaxDigits(4);
	myHist->GetYaxis()->SetLabelFont(22);
	myHist->GetYaxis()->SetTitleFont(22);
	myHist->GetZaxis()->CenterTitle(true);
	myHist->GetZaxis()->SetMaxDigits(4);
	myHist->GetZaxis()->SetLabelFont(22);
	myHist->GetZaxis()->SetTitleFont(22);
}
