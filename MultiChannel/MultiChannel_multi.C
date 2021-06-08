/*************************************************************************
	> File Name: MultiChannel_multi.C
	> Author: Jianjin Zhou
	> Mail: zhoujianjin825@gmail.com 
	> Created Time: Fri 09 Nov 2018 09:03:13 PM DST
	>
	> 运行环境：WSL
	> 对两层目录遍历，结果保存在相应的root文件
	> 终端输出电压值、计数率、峰位（增益）、能量分辨率（粗糙）
	> 需对每个直方图手动拟合出最佳结果，得到mean、sigma，计算增益与能量分辨率
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TStyle.h"

using namespace std;

bool GetPath(string path, vector<string> &name); // 获取目录下的所有文件名

void MultiChannel_multi(string dataDir)
{
	gStyle->SetOptFit(0111);	   // set fit parameter
	gStyle->SetOptStat(0000);	   // set hist parameter

	TFile *f = new TFile((dataDir+".root").c_str(), "recreate"); // 创建目录名命名的ROOT文件

	vector<string> dataSubDir; // 子目录文件迭代器
	if(GetPath(dataDir, dataSubDir)) 
	{
		for(Int_t i = 0; i < dataSubDir.size(); i++)
		{
			string subDir = dataSubDir[i].substr(dataSubDir[i].rfind("/")+1);
			cout << subDir << endl;

			vector<string> filename; // 数据子目录下数据文件迭代器
			if(GetPath(dataSubDir[i], filename))
			{
				f->mkdir(subDir.c_str()); // 在ROOT文件中创建以数据子目录命名的目录
				f->cd(subDir.c_str()); // 进入该目录
				gDirectory->mkdir("EnergySpectrum"); // 在每个目录下创建能谱目录，用来保存不同电压的能谱图
				gDirectory->cd("EnergySpectrum");

				Double_t result[4][filename.size()]; // Voltage, CountingRate, Peak, EnergyResolution

				for(Int_t j = 0; j < filename.size(); j++)
				{ 
					size_t s1 = filename[j].find_first_of("/");
					size_t s2 = filename[j].find_last_of("/");
					size_t s3 = filename[j].find_first_of(".Spe");

					string histName = filename[j].substr(s2+1, s3-s2-1);
					result[0][j] = atoi(histName.substr(0, histName.rfind("V")).c_str());
					
					ifstream data(filename[j].c_str());
					if(data.fail())
					{
						cout << "Can not find the file \" " << filename[j] << " \" " << endl;
						exit(0);				
					}
					istringstream ss;
					string line;
					Double_t measureTime = 0, effTime = 0;
					Int_t startChannel = 0, endChannel = 0;
					Int_t sumCounts = 0;

					// 读取文件头
					for(Int_t i = 0; i < 9; i++)
						getline(data, line);

					getline(data, line);
					ss.str(line);
					ss >> effTime >> measureTime; // 得到测试时长
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

					// 能量分辨计算
					Double_t leftValue = h->FindFirstBinAbove(h->GetMaximum()/2);
					Double_t rightValue = h->FindLastBinAbove(h->GetMaximum()/2);

					result[1][j] = sumCounts / effTime;
					result[2][j] = h->GetMaximumBin();	
					result[3][j] = (rightValue - leftValue) / h->GetMaximumBin();

					cout << histName << setw(15) << result[1][j] << setw(15) << result[2][j] << setw(15) << result[3][j] << endl;

					h->Write();
					delete h;
				}
				gDirectory->cd(".."); // 返回上级目录

				// 创建Result文件夹，保存坪曲线，增益曲线，能量分辨率曲线
				gDirectory->mkdir("Result");
				gDirectory->cd("Result");

				const Int_t N = 3;
				TGraph *gr[N];
				char title[N][50] = {"CountingRate", "Peak", "EnergyResolution"};
				char yTitle[N][50] = {"CountingRate [S^{-1}]", "Peak", "EnergyResolution"};
				Int_t color[N] = {2, 4, 6};
				Double_t markerSize[N] = {1.2, 1.5, 1.4};
				Int_t markerStyle[N] = {21, 22, 20};
				for (Int_t j = 0; j < N; j++)
				{
					gr[j] = new TGraph(filename.size(), result[0], result[j+1]);
					gr[j]->SetNameTitle(title[j], title[j]);
					gr[j]->SetLineColor(color[j]);
					gr[j]->SetLineWidth(2);
					gr[j]->SetMarkerColor(color[j]);
					gr[j]->SetMarkerSize(markerSize[j]);
					gr[j]->SetMarkerStyle(markerStyle[j]);
			    	gr[j]->GetXaxis()->SetTitle("Voltage [V]");
			    	gr[j]->GetXaxis()->CenterTitle(true);
			    	gr[j]->GetXaxis()->SetLimits(result[0][0]-50, result[0][filename.size()-1]+50);
			    	gr[j]->GetXaxis()->SetTitleOffset(1.2);
				    gr[j]->GetYaxis()->SetTitle(yTitle[j]);
				    gr[j]->GetYaxis()->CenterTitle(true);
				    gr[j]->Write();

				    delete gr[j];
				}			    
			    f->cd(); // 直接返回打开目录
			}
		}
	}
	f->Close();
}
bool GetPath(string path, vector<string> &name)
{
	DIR *dir = opendir(path.c_str());
	if(!dir)
	{
		cout << "opendir " << path << " error." << endl;
		return false;
	}
	dirent *p = NULL;
	while((p = readdir(dir)) != NULL)
		if(p->d_name[0] != '.')
			name.push_back(path + "/" + string(p->d_name));

	closedir(dir);
	return true;
}