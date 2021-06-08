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

typedef struct
{
	string fileName;
	int channel;
	double padSize;
	double widthBin;
	int binNum;
	double length;

	string startT;
	string realT;
	string stopT;
	double counts;
	double countRate;

	TH3D *hxyt;

} ExpInfo;

void ProcessTxt(ExpInfo &);
void ProcessTOF(ExpInfo &, string);
void DrawResult(ExpInfo, string, string);
void TH1D_setting(TH1D *);
void TH2D_setting(TH2D *);
void CDT_TOF(string filename, string mode = "all", string projection = "no")
{
	const double Length = 11.4 + 4.768 + 0.04; // m
	const double padSize = 1.5625;			   // mm

	int channel = 64;		// channels of detector (x or y)
	double width_bin = 80.; // per bin (us)
	int n_line = 500;		// number of bin

	gStyle->SetPalette(1);	  // set 2D color
	gStyle->SetOptFit(0111);  // set fit parameter
	gStyle->SetOptStat(0000); // set hist parameter

	ExpInfo expInfo;
	expInfo.fileName = filename;
	expInfo.padSize = padSize;
	expInfo.channel = channel;
	expInfo.widthBin = width_bin;
	expInfo.binNum = n_line;
	expInfo.length = Length;

	ProcessTxt(expInfo);
	ProcessTOF(expInfo, mode);
	DrawResult(expInfo, mode, projection);
}
// Process TXT file
void ProcessTxt(ExpInfo &expInfo)
{
	ifstream rawData((expInfo.fileName + ".txt").c_str());
	if (rawData.fail())
	{
		cout << "Can not find the file " << expInfo.fileName << ".txt\n"
			 << "Choose default value: \n"
			 << "Number of strips in X or Y: " << expInfo.channel << "\n"
			 << "Width of bin:  " << expInfo.widthBin << " us\n"
			 << "Number of bin: " << expInfo.binNum << endl;

		expInfo.fileName += ".tof";
		cout << "Please confirm the file " << expInfo.fileName << " exists." << endl;
	}
	else
	{
		cout << "\nProcessing " << expInfo.fileName << ".txt ..." << endl;
		while (!rawData.eof())
		{
			char line[256];
			rawData.getline(line, 256);

			char *token = strtok(line, ":");
			while (token)
			{
				if (strcmp(token, "File Name ") == 0)
				{
					// windows system line feed contains /r.
					token = strtok(NULL, " \r");
					expInfo.fileName = expInfo.fileName.substr(0, expInfo.fileName.find_last_of("/") + 1) + string(token);
				}
				else if (strcmp(token, "No of X Stripes or Pixel ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.channel = atoi(token);
					cout << "Number of strips in X or Y: " << expInfo.channel << endl;
				}
				else if (strcmp(token, "Dwell Time ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.widthBin = atoi(token) / 10.;
					cout << "Width of bin:  " << expInfo.widthBin << " us" << endl;
				}
				else if (strcmp(token, "No of Bins ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.binNum = atoi(token);
					cout << "Number of bin: " << expInfo.binNum << endl;
				}
				else if (strcmp(token, "Start Time   ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.startT = string(token);
					cout << "Start Time:    " << token << endl;
				}
				else if (strcmp(token, "Real Time    ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.stopT = string(token);
					cout << "Real Time:     " << token << endl;
				}
				else if (strcmp(token, "Stop Time    ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.realT = string(token);
					cout << "Stop Time:     " << token << endl;
				}
				else if (strcmp(token, "No of Counts ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.counts = atoi(token);
					cout << "Number of counts: " << token << endl;
				}
				else if (strcmp(token, "Count Rate   ") == 0)
				{
					token = strtok(NULL, " ");
					expInfo.countRate = atof(token);
					cout << "Counting rate:    " << token << endl;
				}

				token = strtok(NULL, ":");
			}
		}
		rawData.close();
	}
}
// Process TOF file
void ProcessTOF(ExpInfo &expInfo, string mode)
{
	// transform the raw data into 3D histogram
	ifstream rawData(expInfo.fileName.c_str());
	if (rawData.fail())
	{
		cout << "Can not find the file " << expInfo.fileName << endl;
		exit(0);
	}
	cout << "\nProcessing " << expInfo.fileName << " ..." << endl;

	char name[128] = "";
	double timeStart = 0., timeEnd = 0.;
	double waveStart = 0., waveEnd = 0.;
	double xPosStart = 0., xPosEnd = 0.;
	double yPosStart = 0., yPosEnd = 0.;
	if (mode == "cut_t")
	{
		// set the tof range
		cout << "Please input the tof range [ms]: ";
		scanf("%lf %lf", &timeStart, &timeEnd);
		sprintf(name, "%.1lfms-%.1lfms", timeStart, timeEnd);
		// get the wavelength range
		waveStart = timeStart * 1000 / (252.778 * expInfo.length);
		waveEnd = timeEnd * 1000 / (252.778 * expInfo.length);
		cout << "Wavelength range: " << waveStart << " A - " << waveEnd << " A." << endl;
	}
	else if (mode == "cut_w")
	{
		// set the wavelength range
		cout << "Please input the wavelength range [A]: ";
		scanf("%lf %lf", &waveStart, &waveEnd);
		sprintf(name, "%.1lfA-%.1lfA", waveStart, waveEnd);
		// get the tof range
		timeStart = 252.778 * expInfo.length * waveStart / 1000.;
		timeEnd = 252.778 * expInfo.length * waveEnd / 1000.;
		cout << "TOF range: " << timeStart << " ms - " << timeEnd << " ms." << endl;
	}
	else if (mode == "cut_p")
	{
		// set the X and Y range
		cout << "Please input the X range [mm]: ";
		scanf("%lf %lf", &xPosStart, &xPosEnd);
		cout << "Please input the Y range [mm]: ";
		scanf("%lf %lf", &yPosStart, &yPosEnd);
		sprintf(name, "X(%.1lf-%.1lfmm)_Y(%.1lf-%.1lfmm)", xPosStart, xPosEnd, yPosStart, yPosEnd);
		cout << "X range: " << xPosStart << " mm - " << xPosEnd << " mm." << endl;
		cout << "Y range: " << yPosStart << " mm - " << yPosEnd << " mm." << endl;
	}
	else if (mode == "cut_wp")
	{
		// set the wavelength range
		cout << "Please input the wavelength range [A]: ";
		scanf("%lf %lf", &waveStart, &waveEnd);
		// set the X and Y range
		cout << "Please input the X range [mm]: ";
		scanf("%lf %lf", &xPosStart, &xPosEnd);
		cout << "Please input the Y range [mm]: ";
		scanf("%lf %lf", &yPosStart, &yPosEnd);
		sprintf(name, "%.1lf-%.1lfA_X(%.1lf-%.1lfmm)_Y(%.1lf-%.1lfmm)", waveStart, waveEnd, xPosStart, xPosEnd, yPosStart, yPosEnd);
		// get the tof range
		timeStart = 252.778 * expInfo.length * waveStart / 1000.;
		timeEnd = 252.778 * expInfo.length * waveEnd / 1000.;
		cout << "TOF range:        " << timeStart << " ms - " << timeEnd << " ms." << endl;
		cout << "Wavelength range: " << waveStart << " A  - " << waveEnd << " A." << endl;
		cout << "X range:          " << xPosStart << " mm - " << xPosEnd << " mm." << endl;
		cout << "Y range:          " << yPosStart << " mm - " << yPosEnd << " mm." << endl;
	}

	// create 3D histogram (x, y, t)
	expInfo.hxyt = new TH3D("", ";x [mm];y [mm];tof [#mus]", expInfo.channel, 0, expInfo.padSize * expInfo.channel, expInfo.channel, 0, expInfo.padSize * expInfo.channel, expInfo.binNum, 0, expInfo.binNum * expInfo.widthBin);
	expInfo.hxyt->SetNameTitle(name, name);

	uint64_t temp;
	for (int i = 0; i < expInfo.binNum; i++)
	{
		rawData >> temp >> temp; // bin number, bin counts(total)

		for (int j = 0; j < expInfo.channel; j++) // Y
		{
			for (int k = 0; k < expInfo.channel; k++) // X
			{
				rawData >> temp; // counts of every pad (channel*channel) in every width_bin

				// Fill 3D hist, intercept a specific range, maybe need to exchange j and k.
				if (mode == "cut_t" || mode == "cut_w")
				{
					if ((i >= timeStart * 1000 / expInfo.widthBin) && (i <= timeEnd * 1000 / expInfo.widthBin))
						expInfo.hxyt->SetBinContent(k + 1, j + 1, i + 1, temp);
				}
				else if (mode == "cut_p")
				{
					if ((j >= yPosStart / expInfo.padSize) && (j <= yPosEnd / expInfo.padSize) && (k >= xPosStart / expInfo.padSize) && (k <= xPosEnd / expInfo.padSize))
						expInfo.hxyt->SetBinContent(k + 1, j + 1, i + 1, temp);
				}
				else if (mode == "cut_wp")
				{
					if ((i >= timeStart * 1000 / expInfo.widthBin) && (i <= timeEnd * 1000 / expInfo.widthBin) && (j >= yPosStart / expInfo.padSize) && (j <= yPosEnd / expInfo.padSize) && (k >= xPosStart / expInfo.padSize) && (k <= xPosEnd / expInfo.padSize))
						expInfo.hxyt->SetBinContent(k + 1, j + 1, i + 1, temp);
				}
				else
					expInfo.hxyt->SetBinContent(k + 1, j + 1, i + 1, temp);
			}
		}
	}
	rawData.close();
}
// Draw result
void DrawResult(ExpInfo expInfo, string mode, string projection)
{
	string name = expInfo.hxyt->GetName();
	// 2D position spectrum
	TH2D *hxy = (TH2D *)expInfo.hxyt->Project3D("yx");

	TH1D *h[4]; // TOF, Wavelength, X, Y

	// time of flight spectrum
	h[0] = (TH1D *)expInfo.hxyt->Project3D("z");
	h[0]->GetYaxis()->SetTitle("Counts");

	// wavelength spectrum
	h[1] = new TH1D("", ";Neutron wavelength [#AA];Counts", expInfo.binNum, 0., (expInfo.binNum * expInfo.widthBin) / (252.778 * expInfo.length));
	for (int i = 0; i < expInfo.binNum; i++)
		h[1]->SetBinContent(i + 1, h[0]->GetBinContent(i + 1));

	// position resolution is obtained by projection.
	if (projection != "no")
	{
		h[2] = (TH1D *)expInfo.hxyt->Project3D("x"); // x
		h[2]->GetYaxis()->SetTitle("Counts");

		h[3] = (TH1D *)expInfo.hxyt->Project3D("y"); // y
		h[3]->GetYaxis()->SetTitle("Counts");
	}

	// create canvases for different mode
	const int nCanvas = 5;
	string c_name[nCanvas] = {"2D Imaging ", "Time of Flight ", "Neutron wavelength ", "X Projection ", "Y Projection "};
	TCanvas *c[nCanvas];
	for (int i = 0; i < (projection != "no" ? nCanvas : (nCanvas - 2)); i++)
	{
		c[i] = new TCanvas();
		c[i]->SetTitle((c_name[i] + name).c_str());
		c[i]->SetName((c_name[i] + name).c_str());
		if (i == 0)
		{
			c[i]->SetTicks();
			c[i]->SetRightMargin(0.2);
			c[i]->SetLeftMargin(0.2);

			// set 2D image x and y range
			if(name.find_first_of("mm") != -1)
			{
				string xstart = (name.substr(name.find_first_of("mm")-9, 9)).substr(0, 4);
				string xend = (name.substr(name.find_first_of("mm")-9, 9)).substr(5, 4);
				string ystart = (name.substr(name.find_last_of("mm")-10, 9)).substr(0, 4);
				string yend = (name.substr(name.find_last_of("mm")-10, 9)).substr(5, 4);

				hxy->GetXaxis()->SetRangeUser(atof(xstart.c_str()), atof(xend.c_str()));
				hxy->GetYaxis()->SetRangeUser(atof(ystart.c_str()), atof(yend.c_str()));
			}

			TH2D_setting(hxy);
			hxy->SetTitle("");
			// hxy->SetContour(50);
			// hxy->SetMaximum(1000);
			hxy->GetXaxis()->SetTitleOffset(1.2);
			hxy->GetYaxis()->SetTitleOffset(1);
			hxy->Draw("COLZ");
		}
		else
		{
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
