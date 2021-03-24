#include <iostream>
#include <stdio.h>
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;

void mesytec_single()
{
	gStyle->SetOptFit(0111);
	gStyle->SetOptStat(11);

	const int amp_low = 80, amp_up = 960; //电荷的上下限
	const double tof_cut = 10.;
	int pos_end = 960;
	const int low = 10, up = 10; // 拟合区间的相对峰值的左右bin的个数

	uint16_t BufferLength, BufferType, HeaderLength, BufferNumber, Run_ID, MCPD_ID, Status;
	uint64_t HeaderTimestampLo, HeaderTimestampMid, HeaderTimestampHi;
	uint64_t Parameter0Lo, Parameter0Mid, Parameter0Hi; 
	uint64_t Parameter1Lo, Parameter1Mid, Parameter1Hi;
	uint64_t Parameter2Lo, Parameter2Mid, Parameter2Hi;
	uint64_t Parameter3Lo, Parameter3Mid, Parameter3Hi;
	uint64_t HeaderTimestamp, Parameter0, Parameter1, Parameter2, Parameter3;
	uint16_t ID, ModID, SlotID, Amplitude, Position;
	uint64_t Timestamp0, Timestamp1;


	string filename;
	int tubeID = 0; //
	int work_mode = 1;
	double calib_k = 0., calib_b = 0.; //刻度系数k和b

	cout << "Please Input rawData File name: ";
	cin >> filename;
	cout << "Please Input He3 tube ID: ";
	cin >> tubeID;
	cout << "Number 1 indicates analyze data, number 2 indicates calibration." << "\n";
	cout << "Please select work mode: ";
	cin >> work_mode;

	char tubename[20], pos_cut_name[20], amp_cut_name[20];
	sprintf(tubename, "tube%d", tubeID);
	sprintf(pos_cut_name, "pos_cut_%dms", int(tof_cut));
	sprintf(amp_cut_name, "amp_cut_%dms", int(tof_cut));

	string rootname;
	if(work_mode == 1)
	{
		rootname = filename + "_" + string(tubename) + ".root";
	}
	else
	{
		cout << "Please input calibration value k: ";
		cin >> calib_k;	
		cout << "Please input calibration value b: ";
		cin >> calib_b;	

		rootname = filename + "_" + string(tubename) + "_calibration.root"; 
	}

	TFile *f = new TFile(rootname.c_str(), "RECREATE");

	TH1F *h_tof = new TH1F("h_tof", ";ms;Counts", 800, 0, 40);
	TH1F *h_amp = new TH1F("h_amp", ";channel;Count", 960, 0, 960);
	TH1F *h_amp_cut = new TH1F(amp_cut_name, ";channel;Count", 960, 0, 960);
	TH1F *h_pos, *h_pos_cut;
	if(work_mode == 1)
	{	
		h_pos = new TH1F("h_pos", ";channel;Counts", 960, 0, pos_end);
		h_pos_cut = new TH1F(pos_cut_name, ";channel;Counts", 960, 0, pos_end);
	}
	else
	{
		// pos_end = 300;
		pos_end = 500;
		h_pos = new TH1F("h_pos", ";mm;Counts", 960, 0, pos_end);
		h_pos_cut = new TH1F(pos_cut_name, ";mm;Counts", 960, 0, pos_end);		
	}

	filename = filename + ".mdat";
	ifstream rawData;
	rawData.open(filename.c_str(), ios::binary);
	if(!rawData)
	{
		cout << "Could not open the file " << filename << endl;
		return;
	}
	const int FileHead_byte = 64;
	uint16_t data;
	std::vector<uint16_t> buffer;
	rawData.seekg(FileHead_byte, ios::beg);
	while(rawData.peek() != EOF)
	{
		rawData.read((char*)&data, sizeof(uint16_t));
		// cout << hex << data << endl;
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

					double tof = (Timestamp0-Timestamp1)*100/1000000.; // ms
					double position = 0.;
					if(work_mode == 1)
						position = Position;
					else
						position = calib_k*Position+calib_b;

					h_tof->Fill(tof);
					h_pos->Fill(position);
					h_amp->Fill(Amplitude);
					if(tof>tof_cut && Amplitude>amp_low && Amplitude<amp_up)
					{
						h_pos_cut->Fill(position);
						h_amp_cut->Fill(Amplitude);
					}
				}else continue;
			}
			buffer.clear();
		}
	}
	rawData.close();
	buffer.clear();	

	int peak = h_pos_cut->GetMaximumBin();
	TF1 *f1 = new TF1("f1", "gaus", (peak-low)*pos_end/960, (peak+up)*pos_end/960); //对卡过条件的位置谱拟合并获得拟合参数
	h_pos_cut->Fit(f1, "R+");

	f->Write();
	f->Close();
}