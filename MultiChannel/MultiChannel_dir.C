/*************************************************************************
	> File Name: MultiChannel_dir.C
	> Author: Jianjin Zhou
	> Mail: zhoujianjin825@gmail.com 
	> Created Time: Fri 09 Nov 2018 09:03:13 PM DST
	>
	> 运行环境：WSL
	> 对1层目录遍历，结果保存在相应的root文件，得到坪曲线与增益曲线（Peak）
	> 需对每个直方图手动拟合出最佳结果，得到mean、sigma，计算增益及能量分辨率
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"

// 获取目录下的所有文件名
vector<string> GetPath(string path); 

void MultiChannel_dir(string dataDir)
{
	gStyle->SetOptFit(0111);	   // set fit parameter
	gStyle->SetOptStat(0000);	   // set hist parameter
	
	const Double_t time = 300.;
	const Int_t headLine = 12;
	const Int_t channelNum = 2048;

	TFile *f = new TFile((dataDir+".root").c_str(), "recreate");

	vector<string> filename = GetPath(dataDir); // 得到该目录下的所有数据文件，存入迭代器

	Double_t Voltage[filename.size()], CountingRate[filename.size()], Peak[filename.size()], EnergyResolution[filename.size()];

	cout << "Voltage" << setw(15) << "CountingRate" << setw(10) << 	"Peak" << setw(20) << "EnergyResolution" << endl;

	for(Int_t n = 0; n < filename.size(); n++)
	{
		// cout << filename[n] << endl;
		
		// basic_string substr(size_type_Off = 0, size_type_Count = npos) const;
		// _Off 所需字符串的起始位置；_Count 复制的字符串数目；从指定位置复制指定长度的字符串
		string histName = filename[n].substr(filename[n].rfind("/")+1, filename[n].rfind(".Spe")-filename[n].rfind("/")-1);

		Voltage[n] = atoi(histName.substr(0, histName.rfind("V")).c_str());
		
		ifstream data(filename[n].c_str());
		if(data.fail())
		{
			cout << "Can not find the file \" " << filename[n] << " \" " << endl;
			return 0;				
		}

		istringstream ss;
		string line;
		Double_t measureTime = 0;
		Int_t startChannel = 0, endChannel = 0;
		Int_t sumCounts = 0;

		// 读取文件头
		for(Int_t i = 0; i < 9; i++)
			getline(data, line);

		getline(data, line);
		ss.str(line);
		ss >> measureTime >> measureTime; // 得到测试时长
		getline(data, line);
		getline(data, line);
		ss.str(line);
		ss >> startChannel >> endChannel; // 得到多道道数，需加1

		TH1F * h = new TH1F(histName.c_str(), ";Channel;Counts", endChannel+1, startChannel, endChannel+1);

		// 数据读取并填充直方图
		for(Int_t i = 0; i <= endChannel; i++)
		{
			Int_t count;
			getline(data, line);
			ss.str(line);
			ss >> count;
			h->SetBinContent(i+1, count);
			sumCounts += count;
		}
		data.close();

		h->GetXaxis()->CenterTitle(true);
		h->GetXaxis()->SetMaxDigits(4);
		h->GetXaxis()->SetTitleOffset(1.2);
		h->GetYaxis()->CenterTitle(true);
		h->GetYaxis()->SetMaxDigits(4);

		CountingRate[n] = sumCounts / measureTime;
		Peak[n] = h->GetMaximumBin();

		Double_t leftValue = h->FindFirstBinAbove(h->GetMaximum()/2);
		Double_t rightValue = h->FindLastBinAbove(h->GetMaximum()/2);
		EnergyResolution[n] = (rightValue - leftValue) / h->GetMaximumBin();
		// cout << leftValue << "	" << rightValue << "	" << h->GetMaximumBin() << endl;

		cout << histName << setw(15) << CountingRate[n] << setw(15) << Peak[n] << setw(15) << EnergyResolution[n] << endl;

		h->Write();

		delete h;
	}
	f->Close();

	TGraph *gr1 = new TGraph(filename.size(), Voltage, CountingRate);
	gr1->SetTitle();
    gr1->SetLineColor(kRed);
    gr1->SetLineWidth(2);
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerSize(1.2);
    gr1->SetMarkerStyle(21);
    gr1->GetXaxis()->SetTitle("Voltage [V]");
    gr1->GetXaxis()->CenterTitle(true);
    gr1->GetXaxis()->SetLimits(Voltage[0]-50, Voltage[filename.size()-1]+50);
    gr1->GetXaxis()->SetTitleOffset(1.2);
    gr1->GetYaxis()->SetTitle("CountingRate [S^{-1}]");
    gr1->GetYaxis()->CenterTitle(true);
    gr1->GetYaxis()->SetRangeUser(0, CountingRate[filename.size()-1]*1.2);

	TGraph *gr2 = new TGraph(filename.size(), Voltage, Peak);
	gr2->SetTitle();
    gr2->SetLineColor(kBlue);
    gr2->SetLineWidth(2);
    gr2->SetMarkerColor(kBlue);
    gr2->SetMarkerSize(1.5);
    gr2->SetMarkerStyle(22);
    gr2->GetXaxis()->SetTitle("Voltage [V]");
    gr2->GetXaxis()->CenterTitle(true);
    gr2->GetXaxis()->SetLimits(Voltage[0]-50, Voltage[filename.size()-1]+50);
    gr2->GetXaxis()->SetTitleOffset(1.2);
    gr2->GetYaxis()->SetTitle("Peak");
    gr2->GetYaxis()->CenterTitle(true);
    gr2->GetYaxis()->SetRangeUser(10, Peak[filename.size()-1]*2);

	TGraph *gr3 = new TGraph(filename.size(), Voltage, EnergyResolution);
	gr3->SetTitle();
    gr3->SetLineColor(6);
    gr3->SetLineWidth(0);
    gr3->SetMarkerColor(6);
    gr3->SetMarkerSize(1.2);
    gr3->SetMarkerStyle(20);
    gr3->GetXaxis()->SetTitle("Voltage [V]");
    gr3->GetXaxis()->CenterTitle(true);
    gr3->GetXaxis()->SetLimits(Voltage[0]-50, Voltage[filename.size()-1]+50);
    gr3->GetXaxis()->SetTitleOffset(1.2);
    gr3->GetYaxis()->SetTitle("EnergyResolution");
    gr3->GetYaxis()->CenterTitle(true);
    // gr3->GetYaxis()->SetRangeUser(0.1, 0.25);

	TCanvas *c1 = new TCanvas();
	c1->SetLeftMargin(0.2);
    c1->SetRightMargin(0.2);
    c1->SetGridx();
    c1->SetGridy();
    c1->SetTickx();
    c1->SetTicky();
    gr1->Draw("ACP");
	c1->Update();

	TCanvas *c2 = new TCanvas();
    c2->SetLeftMargin(0.2);
    c2->SetRightMargin(0.2);
    c2->SetGridx();
    c2->SetGridy();
    c2->SetTickx();
    c2->SetTicky();
    c2->SetLogy();
	gr2->Draw("ACP");
	c2->Update();

	TCanvas *c3 = new TCanvas();
    c3->SetLeftMargin(0.2);
    c3->SetRightMargin(0.2);
    c3->SetGridx();
    c3->SetGridy();
    c3->SetTickx();
    c3->SetTicky();
	gr3->Draw("ACP");
	c3->Update();
}
vector<string> GetPath(string path)
{
	vector<string> name;
	DIR *dir = opendir(path.c_str());
	if(!dir)
	{
		perror("opendir error");
		exit(1);
	}
	dirent *p = NULL;
	while((p = readdir(dir)) != NULL)
		if(p->d_name[0] != '.')
			name.push_back(path + "/" + string(p->d_name));

	closedir(dir);
	return name;
}