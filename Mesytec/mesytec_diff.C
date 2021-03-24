/***********************************************************************************
	> File Name: mesytec_diff.C
	> Author: Jianjin Zhou
	> Mail: zhoujianjin825@gmail.com 
	> Created Time: Mon 07 May 2018 12:24:02 AM DST
	> 
	> 用于mesytec电子学测量He3管位置分辨的刻度
	> 将同一条件下不同位置的测试数据放入同一个文件夹即可
	> 程序有两种工作模式：
	> 1表示对数据的分析得到刻度系数k和b，会生成一个"文件名.root"文件；
	> 2表示刻度后的结果,会生成一个"文件名_calibration.root"和"文件名_calibration.txt".
 ***********************************************************************************/
#include <iostream>
#include <string>
#include <dirent.h>
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TGraph.h"

using namespace std;

const int N = 10; //测试的位置个数，也就是原始数据文件的个数。
int tubeID = 0; //
const int fitN = 10;
int low[N], up[N]; //分析时拟合区间的相对峰值的左右bin的个数
int low_f[N], up_f[N]; //刻度时拟合区间的相对峰值的左右bin的个数

double xposition[N] = {100, 200}; //测试位置的坐标
const int amp_low = 150, amp_up = 960; //电荷的上下限
const double tof_low = 30, tof_up = 40.; // ms
const int pos_num = 960;
int pos_start = 0, pos_end = 960; //位置谱的bin个数和坐标最大值
double mean[N] = {0.}, sigma[N] = {0.}; 
int num = 0;
double calib_k = 1., calib_b = 1.; //刻度系数k和b
// int number = 0;
int work_mode; 
string dirname, filename, rootname, tofname, posname, ampname, posname_cut, ampname_cut;
char tubename[20];
int counts[N][pos_num];

TFile *f;
TH1F *h_pos, *h_tof, *h_amp, *h_pos_cut, *h_amp_cut;
TF1 *f1;

void analyze();
void calibration();
void traverse();
void process();
double pos_calculate(double &k, double &b, uint16_t &Position);
void TH1F_setting(TH1F * myHist);

void mesytec_diff()
{
	gStyle->SetOptFit(0111);
	gStyle->SetOptStat(11);

	cout << "Please Input rawData File Directory: ";
	cin >> dirname;
	cout << "Please Input He3 tube ID: ";
	cin >> tubeID;
	cout << "Number 1 indicates analyze data, number 2 indicates calibration." << "\n";
	cout << "Please select work mode: ";
	cin >> work_mode;
	sprintf(tubename, "tube%d", tubeID);

	for(int i = 0; i < N; i++)
	{
		low[i] = fitN;
		up[i] = fitN;
		low_f[i] = fitN;
		up_f[i] = fitN;
	}

	switch(work_mode)
	{
		case 1:
			analyze();
			break;
		case 2:
			calibration();
			break;
		default:
			break;
	}
}
void analyze()
{
	rootname = dirname + "_" + string(tubename) + ".root";
	f = new TFile(rootname.c_str(), "RECREATE");

	traverse();

	f->Write();
	f->Close();

	TGraph *graph1 = new TGraph(N, mean, xposition); //得到刻度系数k和b
	TF1 *f1= new TF1("f1", "[0]*x+[1]");
	f1->SetParameters(1,1);
	graph1->Fit("f1");
	calib_k = f1->GetParameter(0);
	calib_b = f1->GetParameter(1);

	graph1->SetTitle(" ");
	graph1->GetXaxis()->SetTitle("channel");
	graph1->GetXaxis()->CenterTitle(true);
	graph1->GetYaxis()->SetTitle("position [mm]");
	graph1->GetYaxis()->CenterTitle(true);
	graph1->Draw("AC*");
}
void calibration()
{
	cout << "Please input calibration value k: ";
	cin >> calib_k;	
	cout << "Please input calibration value b: ";
	cin >> calib_b;	 

	pos_start = calib_b; 
	pos_end = calib_k*pos_num+calib_b;

	rootname = dirname + "_" + string(tubename) + "_calibration.root";
	f = new TFile(rootname.c_str(), "RECREATE");

	traverse();

	f->Write();
	f->Close();

	string resultData = dirname + "_" + string(tubename) + "_hist.txt"; //保存刻度后直方图信息
	ofstream result;
	result.open(resultData.c_str(), ios::out);


	for (int j = 0; j < pos_num; j++)
	{
		for(int i = 0; i < N; i++)
		{
			result << counts[i][j] << "	";
		}
		result << endl;
	}
	result.close();

	string resultname = dirname + "_" + string(tubename) + "_calibration.txt"; //保存刻度后的结果
	result.open(resultname.c_str(), ios::out);
	result << tubename << endl;
	for(int i = 0; i < N; i++)
		result << xposition[i] << setw(10) << mean[i] << setw(10) << sigma[i] << setw(10) << 2.355*sigma[i] << endl;
	result.close();
}
void traverse()
{
	DIR *dir = opendir(dirname.c_str());
	if(dir == NULL){
		perror("opendir error.");
		exit(1);
	}
	dirent *p = NULL;
	while((p = readdir(dir)) != NULL){
		if(p->d_name[0] != '.'){
			filename = dirname + "/" + string(p->d_name); //原始数据文件路径
			cout << filename << endl;
			tofname = string(p->d_name) + "_TOF";
			posname = string(p->d_name) + "_Pos";
			ampname = string(p->d_name) + "_Amp";
			posname_cut = string(p->d_name) + "_Pos_cut";
			ampname_cut = string(p->d_name) + "_Amp_cut";

			process();
		}
	}
	closedir(dir);
}
void process()
{
	h_tof = new TH1F(tofname.c_str(), ";tof[ms];Counts", 400, 0, 40);
	h_pos = new TH1F(posname.c_str(), ";Channel;Counts", pos_num, pos_start, pos_end);
	h_amp = new TH1F(ampname.c_str(), ";Channel;Counts", 960, 0, 960);
	h_pos_cut = new TH1F(posname_cut.c_str(), ";mm;Counts", pos_num, pos_start, pos_end);
	h_amp_cut = new TH1F(ampname_cut.c_str(), ";Channel;Counts", 960, 0, 960);
	TH1F_setting(h_tof);
	TH1F_setting(h_pos);
	TH1F_setting(h_amp);
	TH1F_setting(h_pos_cut);
	TH1F_setting(h_amp_cut);		

	ifstream rawData;
	rawData.open(filename.c_str(), ios::binary);
	if(!rawData)
	{
		cout << "Could not open the file " << filename << endl;
		return;
	}
	const int FileHead_byte = 64;
	rawData.seekg(FileHead_byte, ios::beg);

	uint16_t BufferLength, BufferType, HeaderLength, BufferNumber, Run_ID, MCPD_ID, Status;
	uint64_t HeaderTimestampLo, HeaderTimestampMid, HeaderTimestampHi;
	uint64_t Parameter0Lo, Parameter0Mid, Parameter0Hi;
	uint64_t Parameter1Lo, Parameter1Mid, Parameter1Hi;
	uint64_t Parameter2Lo, Parameter2Mid, Parameter2Hi;
	uint64_t Parameter3Lo, Parameter3Mid, Parameter3Hi;
	uint64_t HeaderTimestamp, Parameter0, Parameter1, Parameter2, Parameter3;
	uint16_t ID, ModID, SlotID, Amplitude, Position;
	uint64_t Timestamp0, Timestamp1;

	uint16_t data;
	std::vector<uint16_t> buffer;

	while(rawData.peek() != EOF)
	{
		rawData.read((char*)&data, sizeof(uint16_t));
		buffer.push_back(data);
		int ss = buffer.size();
		if((buffer[ss-1] == 0xaaaa) && (buffer[ss-2] == 0x5555) && (buffer[ss-3] == 0xffff) && (buffer[ss-4] == 0x0000)){
			BufferLength = ((buffer[0] & 0x00ff) << 8) | ((buffer[0] & 0xff00) >> 8);
			BufferType = ((buffer[1] & 0x00ff) << 8) | ((buffer[1] & 0xff00) >> 8);
			HeaderLength = ((buffer[2] & 0x00ff) << 8) | ((buffer[2] & 0xff00) >> 8);
			BufferNumber = ((buffer[3] & 0x00ff) << 8) | ((buffer[3] & 0xff00) >> 8);
			Run_ID = ((buffer[4] & 0x00ff) << 8) | ((buffer[4] & 0xff00) >> 8);
			MCPD_ID = buffer[5] & 0x00ff;
			Status = (buffer[5] & 0xff00) >> 8;
			HeaderTimestampLo = ((buffer[6] & 0x00ff) << 8) | ((buffer[6] & 0xff00) >> 8);
			HeaderTimestampMid = ((buffer[7] & 0x00ff) << 8) | ((buffer[7] & 0xff00) >> 8);
			HeaderTimestampHi = ((buffer[8] & 0x00ff) << 8) | ((buffer[8] & 0xff00) >> 8);
			Parameter0Lo = ((buffer[9] & 0x00ff) << 8) | ((buffer[9] & 0xff00) >> 8);
			Parameter0Mid = ((buffer[10] & 0x00ff) << 8) | ((buffer[10] & 0xff00) >> 8);
			Parameter0Hi = ((buffer[11] & 0x00ff) << 8) | ((buffer[11] & 0xff00) >> 8);
			Parameter1Lo = ((buffer[12] & 0x00ff) << 8) | ((buffer[12] & 0xff00) >> 8);
			Parameter1Mid = ((buffer[13] & 0x00ff) << 8) | ((buffer[13] & 0xff00) >> 8);
			Parameter1Hi = ((buffer[14] & 0x00ff) << 8) | ((buffer[14] & 0xff00) >> 8);
			Parameter2Lo = ((buffer[15] & 0x00ff) << 8) | ((buffer[15] & 0xff00) >> 8);
			Parameter2Mid = ((buffer[16] & 0x00ff) << 8) | ((buffer[16] & 0xff00) >> 8);
			Parameter2Hi = ((buffer[17] & 0x00ff) << 8) | ((buffer[17] & 0xff00) >> 8);
			Parameter3Lo = ((buffer[18] & 0x00ff) << 8) | ((buffer[18] & 0xff00) >> 8);
			Parameter3Mid = ((buffer[19] & 0x00ff) << 8) | ((buffer[19] & 0xff00) >> 8);
			Parameter3Hi = ((buffer[20] & 0x00ff) << 8) | ((buffer[20] & 0xff00) >> 8);
			HeaderTimestamp = (HeaderTimestampHi << 32) | (HeaderTimestampMid << 16) | HeaderTimestampLo;
			Parameter0 = (Parameter0Hi << 32) | (Parameter0Mid << 16) | Parameter0Lo;
			Parameter1 = (Parameter1Hi << 32) | (Parameter1Mid << 16) | Parameter1Lo;
			Parameter2 = (Parameter2Hi << 32) | (Parameter2Mid << 16) | Parameter2Lo;
			Parameter3 = (Parameter3Hi << 32) | (Parameter3Mid << 16) | Parameter3Lo;
			for(int i = 21; i < ss-4; i+=3)
			{
				ID = (buffer[i+2] & 0x0080) >> 7;
				if(ID == 0x1)
				{
					Timestamp1 = ((buffer[i+1] & 0x0700) << 8) | ((buffer[i] & 0x00ff) << 8) | ((buffer[i] & 0xff00) >> 8);
					Timestamp1 = (((buffer[i+1] & 0x0700) << 8) | ((buffer[i] & 0x00ff) << 8) | ((buffer[i] & 0xff00) >> 8)) + HeaderTimestamp;
				}
				if(ID == 0x0)
				{
					ModID = (buffer[i+2] & 0x0070) >> 4;	
					SlotID = ((buffer[i+2] & 0x000f) << 1) | ((buffer[i+2] & 0x8000) >> 15);
					Amplitude = ((buffer[i+2] & 0x7f00) >> 5) | ((buffer[i+1] & 0x00e0) >> 5);
					Position = ((buffer[i+1] & 0x001f) << 5) | ((buffer[i+1] & 0xf800) >> 11);
					Timestamp0 = ((buffer[i+1] & 0x0700) << 8) | ((buffer[i] & 0x00ff) << 8) | ((buffer[i] & 0xff00) >> 8);
					Timestamp0 = (((buffer[i+1] & 0x0700) << 8) | ((buffer[i] & 0x00ff) << 8) | ((buffer[i] & 0xff00) >> 8)) + HeaderTimestamp;

					if(SlotID == tubeID)
					{
						double tof = (Timestamp0-Timestamp1)*100/1000000.; //单位 ms

						double position = pos_calculate(calib_k, calib_b, Position);
						
						h_tof->Fill(tof);
						h_pos->Fill(position);
						h_amp->Fill(Amplitude);
						if((tof>tof_low)&&(tof<tof_up)&&(Amplitude>amp_low)&&(Amplitude<amp_up)) //卡TOF，卡总电荷量
						{ 
							h_pos_cut->Fill(position);
							h_amp_cut->Fill(Amplitude);
						}						
					}
				}else continue;
			}
			buffer.clear();
		}
	}
	rawData.close();
	buffer.clear();	

	if(work_mode != 1)
	{
		// for(int i = 1; i < pos_num-2; i++)
		// {
		// 	int temp = h_pos_cut->GetBinContent(i+1);
		// 	if(temp == 0)
		// 		h_pos_cut->SetBinContent(i+1, (h_pos_cut->GetBinContent(i)+h_pos_cut->GetBinContent(i+2))/2);
		// }
		for(int i = 0; i < pos_num; i++)
			counts[num][i] = h_pos_cut->GetBinContent(i+1);
	}

	int peak = h_pos_cut->GetMaximumBin();
	f1 = new TF1("f1", "gaus", (peak-low[num])*(pos_end-pos_start)/pos_num+pos_start, (peak+up[num])*(pos_end-pos_start)/pos_num+pos_start); //对卡过条件的位置谱拟合并获得拟合参数
	h_pos_cut->Fit(f1, "R+");
	mean[num] = f1->GetParameter(1);
	sigma[num] = f1->GetParameter(2);

	num++;
}
double pos_calculate(double &k, double &b, uint16_t &Position)
{
	if(work_mode == 1)
		return Position;
	else
		return k*Position+b; //位置修正
}
void TH1F_setting(TH1F * myHist)
{
	myHist->SetLineWidth(2);
	myHist->GetXaxis()->CenterTitle(true);
	// myHist->GetXaxis()->SetLabelFont(62);
	myHist->GetXaxis()->SetLabelSize(0.05);
	myHist->GetXaxis()->SetTitleSize(0.05);
	// myHist->GetXaxis()->SetTitleFont(62);
	// myHist->GetXaxis()->SetLabelFont(62);
	myHist->GetXaxis()->SetTitleOffset(0.90);
	myHist->GetYaxis()->CenterTitle(true);
	// myHist->GetYaxis()->SetLabelFont(62);
	myHist->GetYaxis()->SetLabelSize(0.05);
	myHist->GetYaxis()->SetTitleSize(0.05);
	// myHist->GetYaxis()->SetTitleFont(62);
	// myHist->GetYaxis()->SetLabelFont(62);
	myHist->GetYaxis()->SetTitleOffset(1.00);
}