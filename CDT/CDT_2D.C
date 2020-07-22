
#include <iostream>
#include <fstream>

using namespace std;

void CDT_2D(string filename, string projection = "no")
{
	const int channel = 64;
	const double padSize = 1.56;

	gStyle->SetPalette(1);	  // set 2D color
	gStyle->SetOptFit(0111);  // set fit parameter
	gStyle->SetOptStat(0000); // set hist parameter

	ifstream rawData;
	rawData.open(filename.c_str());
	if (rawData.fail())
	{
		cout << "Can not find the file \" " << filename << " \" " << endl;
		exit(-1);
	}
	// create 2D histogram (x, y)
	TH2D *hxy = new TH2D("hxy", ";X [mm];Y [mm]", channel, 0, padSize * channel, channel, 0, padSize * channel);

	for (int i = 0; i < channel; i++)
	{
		for (int j = 0; j < channel; j++)
		{
			uint64_t counts;
			rawData >> counts;
			hxy->SetBinContent(i + 1, j + 1, counts);
		}
	}
	rawData.close();

	TH1D *h[2]; // X, Y
	if (projection != "no")
	{
		h[0] = (TH1D *)hxy->ProjectionX();
		h[1] = (TH1D *)hxy->ProjectionY();
	}

	const int nCanvas = 3;
	TCanvas *c[nCanvas];
	for (int i = 0; i < (projection != "no" ? nCanvas : (nCanvas - 2)); i++)
	{
		c[i] = new TCanvas();
		c[i]->SetGrid();
		c[i]->SetRightMargin(0.2);
		c[i]->SetLeftMargin(0.2);
		c[i]->SetTickx(1);
		c[i]->SetTicky(1);
		if (i == 0)
		{
			hxy->SetTitle(" ");
			hxy->GetXaxis()->CenterTitle(true);
			hxy->GetXaxis()->SetMaxDigits(4);
			hxy->GetYaxis()->CenterTitle(true);
			hxy->GetYaxis()->SetMaxDigits(4);
			hxy->GetZaxis()->CenterTitle(true);
			hxy->GetZaxis()->SetMaxDigits(4);
			// hxy->SetContour(50);
			// hxy->SetMaximum(1000);
			hxy->Draw("COLZCONT4");
		}
		else
		{
			h[i - 1]->SetTitle(" ");
			h[i - 1]->GetXaxis()->CenterTitle(true);
			h[i - 1]->GetXaxis()->SetMaxDigits(4);
			h[i - 1]->GetYaxis()->CenterTitle(true);
			h[i - 1]->GetYaxis()->SetMaxDigits(4);
			h[i - 1]->Draw();
		}
	}
}
