/*************************************************************************
	> File Name: MultiChannel_multi.C
	> Author: Jianjin Zhou
	> Mail: zhoujianjin825@gmail.com 
	> Created Time: Fri 09 Nov 2018 01:43:13 PM DST
	>
	> 运行环境：WSL
	> 处理单个的多道数据结果
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include "TCanvas.h"
#include "TH1F.h"

void MultiChannel_single(string filename)
{
	gStyle->SetOptFit(0111);	   // set fit parameter
	gStyle->SetOptStat(0000);	   // set hist parameter

	ifstream data;
	data.open(filename.c_str());
	if(!data)
	{
		cerr << "Open error " << filename << endl;
		return;
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

	TH1F * h1 = new TH1F("", ";Channel;Counts", endChannel+1, startChannel, endChannel+1);

	// 数据读取并填充直方图
	for(Int_t i = 0; i <= endChannel; i++)
	{
		Int_t count;
		getline(data, line);
		ss.str(line);
		ss >> count;
		h1->SetBinContent(i+1, count);
		sumCounts += count;
	}
	data.close();

	// h1->Rebin(16);
	h1->GetXaxis()->CenterTitle(true);
	h1->GetXaxis()->SetMaxDigits(4);
	h1->GetXaxis()->SetTitleOffset(1.2);
	h1->GetYaxis()->CenterTitle(true);
	h1->GetYaxis()->SetMaxDigits(4);
	
	Double_t maxCount = h1->GetMaximum();
	Double_t maxValue = h1->GetBinCenter(h1->GetMaximumBin());
	Double_t leftValue = h1->GetBinCenter(h1->FindFirstBinAbove(maxCount/2));
	Double_t rightValue = h1->GetBinCenter(h1->FindLastBinAbove(maxCount/2));
	Double_t fwhm = (rightValue - leftValue) / maxValue;
	// cout << maxCount << "	" << maxValue << "	" << leftValue << "	" << rightValue << endl;

	cout << "计数率: " << sumCounts / measureTime << "    峰位: " << maxValue << "" << "    能量分辨率: " << fwhm << endl;
	
	TCanvas * c1 = new TCanvas();
    c1->SetLeftMargin(0.2);
    c1->SetRightMargin(0.2);
    c1->SetBottomMargin(0.12);
    c1->SetGridx();
    c1->SetGridy();
    c1->SetTickx();
    c1->SetTicky();
	h1->Draw();
	c1->Update();
}