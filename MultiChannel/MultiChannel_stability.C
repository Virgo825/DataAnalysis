/*************************************************************************
	> File Name: MultiChannel_dir.C
	> Author: Jianjin Zhou
	> Mail: zhoujianjin825@gmail.com 
	> Created Time: Fri 09 Nov 2018 09:03:13 PM DST
	>
	> 运行环境：WSL
	> 稳定性分析，每个测试点的结果保存在相应的root文件，得到坪曲线、能量分辨率曲线与增益曲线（Peak）
	> 需对每个直方图手动拟合出最佳结果，得到mean、sigma，计算增益及能量分辨率
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"


void MultiChannel_stability(string dataDir)
{
	const Double_t time = 600.;
	const Int_t headLine = 12;
	const Int_t channelNum = 2048;

	const Int_t numFile = 1001;

	TFile *f = new TFile((dataDir+".root").c_str(), "recreate");

	Double_t Measure[numFile], CountingRate[numFile], Peak[numFile], EnergyResolution[numFile];

	cout << "Measure" << setw(15) << "CountingRate" << setw(10) << 	"Peak" << setw(20) << "EnergyResolution" << endl;

	for(Int_t n = 0; n < numFile ; n++)
	{
		
		char filename[255];
		sprintf(filename, "%d.Spe", n);
		// cout << filename << endl;

		Measure[n] = (n+1)*time/3600.;
		
		TH1F *h = new TH1F(filename,";Channel;Counts", channelNum, 0, channelNum);

		ifstream data((dataDir + "/" + string(filename)).c_str());
		if(data.fail())
		{
			cout << "Can not find the file \" " << filename << " \" " << endl;
			return 0;				
		}

		string line;
		Int_t sumCounts = 0;
		// 读取文件头
		for(Int_t i = 0; i < headLine; i++)
			getline(data, line);

		// 数据读取并填充直方图
		for(Int_t i = 0; i < channelNum; i++)
		{
			Int_t count;
			getline(data, line);
			istringstream ss(line);
			ss >> count;
			if(i>80)
			h->SetBinContent(i+1, count);
			sumCounts += count;
		}
		data.close();

		CountingRate[n] = sumCounts / time;
		Peak[n] = h->GetMaximumBin();

		Double_t leftValue = h->FindFirstBinAbove(h->GetMaximum()/2);
		Double_t rightValue = h->FindLastBinAbove(h->GetMaximum()/2);
		EnergyResolution[n] = (rightValue - leftValue) / h->GetMaximumBin();
		// cout << leftValue << "	" << rightValue << "	" << h->GetMaximumBin() << endl;

		cout << Measure[n] << setw(15) << CountingRate[n] << setw(15) << Peak[n] << setw(15) << EnergyResolution[n] << endl;

		h->Write();

		delete h;
	}
	f->Close();

	TGraph *gr1 = new TGraph(numFile, Measure, CountingRate);
	gr1->SetTitle();
    gr1->SetLineColor(2);
    gr1->SetLineWidth(0);
    gr1->SetMarkerColor(2);
    gr1->SetMarkerSize(0.5);
    gr1->SetMarkerStyle(21);
    gr1->GetXaxis()->SetTitle("");
    gr1->GetXaxis()->CenterTitle(true);
    // gr1->GetXaxis()->SetLimits(-10, 1010);
    // gr1->GetXaxis()->SetLimits(-10, 230);
    gr1->GetXaxis()->SetTitleOffset(1.2);
    gr1->GetYaxis()->SetTitle("CountingRate [S^{-1}]");
    gr1->GetYaxis()->CenterTitle(true);
    // gr1->GetYaxis()->SetRangeUser(100, 120);

	TGraph *gr2 = new TGraph(numFile, Measure, Peak);
	gr2->SetTitle();
    gr2->SetLineColor(4);
    gr2->SetLineWidth(0);
    gr2->SetMarkerColor(4);
    gr2->SetMarkerSize(0.8);
    gr2->SetMarkerStyle(22);
    gr2->GetXaxis()->SetTitle("");
    gr2->GetXaxis()->CenterTitle(true);
    // gr2->GetXaxis()->SetLimits(-10, 1010);
    // gr2->GetXaxis()->SetLimits(-10, 230);
    gr2->GetXaxis()->SetTitleOffset(1.2);
    gr2->GetYaxis()->SetTitle("Peak");
    gr2->GetYaxis()->CenterTitle(true);

	TGraph *gr3 = new TGraph(numFile, Measure, EnergyResolution);
	gr3->SetTitle();
    gr3->SetLineColor(6);
    gr3->SetLineWidth(0);
    gr3->SetMarkerColor(6);
    gr3->SetMarkerSize(0.5);
    gr3->SetMarkerStyle(20);
    gr3->GetXaxis()->SetTitle("");
    gr3->GetXaxis()->CenterTitle(true);
    // gr3->GetXaxis()->SetLimits(-10, 1010);
    // gr3->GetXaxis()->SetLimits(-10, 230);
    gr3->GetXaxis()->SetTitleOffset(1.2);
    gr3->GetYaxis()->SetTitle("EnergyResolution");
    gr3->GetYaxis()->CenterTitle(true);
    // gr3->GetYaxis()->SetRangeUser(0.1, 0.25);

	TCanvas *c1 = new TCanvas();
    c1->SetGridx();
    c1->SetGridy();
    c1->SetTickx();
    c1->SetTicky();
    gr1->Draw("ACP");
	c1->Update();

	TCanvas *c2 = new TCanvas();
    c2->SetGridx();
    c2->SetGridy();
    c2->SetTickx();
    c2->SetTicky();
	gr2->Draw("ACP");
	c2->Update();

	TCanvas *c3 = new TCanvas();
    c3->SetGridx();
    c3->SetGridy();
    c3->SetTickx();
    c3->SetTicky();
	gr3->Draw("ACP");
	c3->Update();
}