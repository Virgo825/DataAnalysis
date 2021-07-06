#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TH3F.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TStyle.h"

#define MAXEVENTLENGTH 65532

using namespace std;

typedef struct
{
    bool saveData;
    bool soc;

    string fileName;
    string mode;
    string projection;

    int channel;
    double padSize;
    double widthBin;
    double tofRange;
    double length;

    uint64_t lastPluseT0;
    uint64_t lastPluseT1;

    TH3F *hxyt;
    TFile *f;
    TTree *t;
    bool treeStatu = true;
    double neutron_x;
    double neutron_y;
    double neutron_t;
    double neutron_w;
} ExpInfo;

void ProcessData(ExpInfo &);
void ProcessPackage(uint64_t *, ExpInfo &);
void DrawResult(ExpInfo &);
void TH1F_setting(TH1F *);
void TH2F_setting(TH2F *);

void GEM(string filename, string mode = "all", string projection = "no")
{
    const bool saveData = true; // save result to root file
    const bool soc = false;     // data from soc or not

    const double Length = 11.4 + 1.2;            // flight distance [m]
    const double padSize = 1.5625;               // [mm]
    const double tofRange = 40000, binSize = 10; // [us]
    const int channels = 32;                     // channel number

    gStyle->SetPalette(1);
    gStyle->SetOptFit(0111);
    // gStyle->SetOptStat(0000);

    ExpInfo expInfo;
    expInfo.saveData = saveData;
    expInfo.soc = soc;
    expInfo.fileName = filename;
    expInfo.mode = mode;
    expInfo.projection = projection;
    expInfo.channel = channels;
    expInfo.tofRange = tofRange;
    expInfo.widthBin = binSize;
    expInfo.length = Length;
    expInfo.padSize = padSize;

    ProcessData(expInfo);
    DrawResult(expInfo);
}
void ProcessData(ExpInfo &expInfo)
{
    ifstream rawData((expInfo.fileName + ".bin").c_str(), ios::binary);
    if (!rawData)
    {
        cout << "Cannot open " << expInfo.fileName << ".bin" << endl;
        exit(-1);
    }
    else
        cout << "Analysing " << expInfo.fileName << ".bin ..." << endl;

    expInfo.hxyt = new TH3F("hxyt", ";x [mm]; y [mm]; tof [#mus]", expInfo.channel, 0, expInfo.channel * expInfo.padSize, expInfo.channel, 0, expInfo.channel * expInfo.padSize, expInfo.tofRange / expInfo.widthBin, 0, expInfo.tofRange);

    if (expInfo.saveData)
    {
        expInfo.f = new TFile((expInfo.fileName + ".root").c_str(), "UPDATE");
        if (expInfo.f->Get("xytw") == nullptr)
        {
            expInfo.t = new TTree("xytw", "neutron infomation");
            expInfo.t->Branch("neutron_x", &expInfo.neutron_x, "expInfo.neutron_x/D");
            expInfo.t->Branch("neutron_y", &expInfo.neutron_y, "expInfo.neutron_y/D");
            expInfo.t->Branch("neutron_t", &expInfo.neutron_t, "expInfo.neutron_t/D");
            expInfo.t->Branch("neutron_w", &expInfo.neutron_w, "expInfo.neutron_w/D");
        }
        else
            expInfo.treeStatu = false;
    }

    uint64_t totalEvent = 0;
    uint64_t effPluse = 0;

    const uint32_t socHead = 0x000290EB;
    uint32_t tmp = 0x000000ff;
    int num = 0;
    const uint32_t socTail = 0x5a5a5a5a;
    uint32_t packLengeth = 0;

    const uint8_t packHead = 0x55;                // 包头标识，16个连续 0x55,逐字节判断
    int numHead = 0;                              // 共16个
    const uint64_t packTail = 0xaaaaaaaaaaaaaaaa; // 包尾标识，2个连续 0xaaaaaaaaaaaaaaaa

    int flag = -2;    // 读取标识，0表示读取包头标识，1表示读取事例个数，2表示读取事例及包其他信息，3表示读取尾标识
    int readSize = 4; //每次读取字节数

    if (!expInfo.soc)
    {
        flag = 0;
        readSize = 1;
    }

    uint64_t *buffer = new uint64_t[2 * 2 + MAXEVENTLENGTH * 3 + 8]; // 包头标识16字节，包尾标识16字节，事例字节（每个事例3*8字节），包其他信息8*8字节
    char *dataPointer = (char *)&buffer[0];                          // 数据指针，数据存入buffer数组内

    while (!rawData.eof())
    {
        rawData.read(dataPointer, sizeof(char) * readSize);

        switch (flag)
        {
        case -2:
        {
            // if(*(uint8_t*)dataPointer == ((socHead&tmp) >> num*8))
            // {
            //     cout << package << "\t" << hex << tmp << "\t" << ((socHead&tmp) >> num*8) << endl;
            //     num++;
            //     if(num == 4)
            //     {
            //         num = 0;
            //         tmp = 0x000000ff;
            //         readSize = 36;
            //         flag = -1;
            //     }
            //     else
            //     {
            //         tmp = tmp<<8;
            //     }
            // }
            // else
            // {
            //     flag = -2;
            // }

            if (*(uint32_t *)dataPointer == socHead)
            {
                readSize = 36; // soc添加的其余字节，共4*9字节
                flag = -1;
            }
            else
            {
                flag = -2;
            }
            break;
        }
        case -1:
        {
            readSize = 1;
            flag = 0;
            break;
        }
        case 0:
        {
            if (*(uint8_t *)dataPointer == 0x55)
            {
                numHead++;
                dataPointer++; // 包头标识存入buffer
                if (numHead == 16)
                {
                    numHead = 0;
                    readSize = 4; // 事例数占4字节
                    flag = 1;
                }
            }
            else
            {
                flag = 0;
            }
            break;
        }
        case 1:
        {
            uint32_t nEvent = *(uint32_t *)dataPointer;
            if (nEvent >= MAXEVENTLENGTH)
            {
                break;
            }
            // if(nEvent > 10)
            {
                totalEvent += nEvent;
                effPluse++;
            }
            dataPointer += sizeof(nEvent);      // 事例个数信息保存在buffer，指针移至其后
            readSize = nEvent * 3 * 8 + 15 * 4; // 事例字节+其他信息字节
            flag = 2;
            break;
        }
        case 2:
        {
            dataPointer += readSize; // 事例及包其他信息保存在buffer中，指针移至其后
            readSize = 16;           // 读取包尾标识，16个0xaa
            flag = 3;
            break;
        }
        case 3:
        {
            if (*((uint64_t *)dataPointer) == packTail && *((uint64_t *)dataPointer + 1) == packTail)
            {
                ProcessPackage(buffer, expInfo);
            }
            else
            {
            }
            if (expInfo.soc)
            {
                readSize = 8; // 4个0x5a + 4字节包长
                flag = 4;
            }
            else
            {
                readSize = 1; // 下一个包的处理
                flag = 0;
            }
            numHead = 0;
            dataPointer = (char *)&buffer[0]; // 处理完包信息，重置指针至buffer头部
            break;
        }
        case 4:
        {
            readSize = 4; // 下一个soc包处理
            flag = -2;

            break;
        }
        default:
            break;
        }
    }
    delete[] buffer;
    rawData.close();
    cout << "TotalEvent: " << totalEvent << "\tEffectivePluse: " << effPluse << "\tAverageCountingRate: " << totalEvent / (effPluse / 25.) << endl;
}
void ProcessPackage(uint64_t *buffer, ExpInfo &expInfo)
{
    uint32_t nEvent = buffer[2] & 0xffffffff;
    uint64_t pulseId = buffer[3];
    uint64_t runId = buffer[4];
    uint64_t tl0 = buffer[5] >> 3;
    uint64_t tl1 = buffer[6];
    uint64_t t0 = buffer[7] >> 3;
    uint64_t t1 = buffer[8];

    uint64_t rawCh, tn0, tn1, tof;
    uint32_t channel_x, channel_y;

    for (int i = 0; i < nEvent; i++)
    {
        rawCh = buffer[9 + i * 3 + 0];
        tn0 = buffer[9 + i * 3 + 1] >> 3;
        tn1 = buffer[9 + i * 3 + 2];

        channel_x = rawCh >> 32;
        channel_y = rawCh & 0xffffffff;

        for (int n = 1; n <= 32; n++)
        {
            channel_x = channel_x >> 1;
            if (channel_x == 0)
            {
                expInfo.neutron_x = (32 - n) * expInfo.padSize;
                break;
            }
        }
        for (int n = 1; n <= 32; n++)
        {
            channel_y = channel_y >> 1;
            if (channel_y == 0)
            {
                expInfo.neutron_y = (32 - n) * expInfo.padSize;
                break;
            }
        }
        if (i == 0)
        {
            tof = (tn1 - tl1) * 125000000 + tn0 - tl0; //第一个事例先与自己包的时间相减
            if (tof > 5000000)
                tof = (tn1 - expInfo.lastPluseT1) * 125000000 + tn0 - expInfo.lastPluseT0; // 不满足条件则与上一个包的时间相减

            if (tof > 5000000) // 还是不满足，则存在错误，输出与自己包秒上与秒下的时间差
            {
                cout << "PluseID: " << pulseId << "\tEvent: " << nEvent << endl;
                if (tn1 - tl1 >= 1)
                    cout << "error tof from first event tn: tn1-tl1 = " << long(tn1 - tl1) << "\ttn0 - tl0 = " << long(tof) << endl;
                else
                    cout << "error tof from first event tl: tn1-tl1 = " << long(tn1 - tl1) << "\ttn0 - tl0 = " << long(tof) << endl;

                continue;
            }
        }
        else
        {
            if (tn1 - tl1 == 1)
                tof = tn0 - tl0 + 125000000;
            else if (tn1 - tl1 == 0)
                tof = tn0 - tl0;
            else
            {
                cout << "PluseID: " << pulseId << "\tEvent: " << i + 1 << "/" << nEvent << endl;
                cout << "error tof from tn: tn1-tl1 = " << long(tn1 - tl1) << "\ttn0 - tl0 = " << long(tn0 - tl0) << endl;
                continue;
            }
        }
        if (tof > 5000000)
        {
            cout << "PluseID: " << pulseId << "\tEvent: " << i + 1 << "/" << nEvent << endl;
            cout << "error tof from tl: tn1-tl1 = " << long(tn1 - tl1) << "\ttn0 - tl0 = " << long(tof) << endl;
            continue;
        }

        // cout << i << "/" << nEvent << "\t" << tl0 << "\t" << tl1 << "\t" << tn0 << "\t" << tn1 << "\t" << tn1-tl1 << "\t" << long(tn0-tl0) << "\t" << tn0 -tl0 << endl;

        expInfo.neutron_t = tof / 125.;
        expInfo.hxyt->Fill(expInfo.neutron_x, expInfo.neutron_y, expInfo.neutron_t);
        if (expInfo.saveData)
        {
            if (expInfo.treeStatu)
            {
                expInfo.neutron_w = expInfo.neutron_t / (252.778 * expInfo.length);
                expInfo.t->Fill();
            }
        }
    }

    expInfo.lastPluseT0 = tl0;
    expInfo.lastPluseT1 = tl1;
}
void DrawResult(ExpInfo &expInfo)
{
    char name[128] = "";
    double timeStart = 0., timeEnd = 0.;
    double waveStart = 0., waveEnd = 0.;
    double xPosStart = 0., xPosEnd = 0.;
    double yPosStart = 0., yPosEnd = 0.;
    if (expInfo.mode == "cut_t")
    {
        // set the tof range
        cout << "Please input the tof range [ms]: ";
        scanf("%lf %lf", &timeStart, &timeEnd);
        sprintf(name, "%.1lfms-%.1lfms", timeStart, timeEnd);
        // get the wavelength range
        waveStart = timeStart * 1000 / (252.778 * expInfo.length);
        waveEnd = timeEnd * 1000 / (252.778 * expInfo.length);
        cout << "Wavelength range: " << waveStart << " A - " << waveEnd << " A." << endl;

        expInfo.hxyt->GetZaxis()->SetRangeUser(timeStart * 1000, timeEnd * 1000);
    }
    else if (expInfo.mode == "cut_w")
    {
        // set the wavelength range
        cout << "Please input the wavelength range [A]: ";
        scanf("%lf %lf", &waveStart, &waveEnd);
        sprintf(name, "%.1lfA-%.1lfA", waveStart, waveEnd);
        // get the tof range
        timeStart = 252.778 * expInfo.length * waveStart;
        timeEnd = 252.778 * expInfo.length * waveEnd;
        cout << "TOF range: " << timeStart / 1000 << " ms - " << timeEnd / 1000 << " ms." << endl;

        expInfo.hxyt->GetZaxis()->SetRangeUser(timeStart, timeEnd);
    }
    else if (expInfo.mode == "cut_p")
    {
        // set the X and Y range
        cout << "Please input the X range [mm]: ";
        scanf("%lf %lf", &xPosStart, &xPosEnd);
        cout << "Please input the Y range [mm]: ";
        scanf("%lf %lf", &yPosStart, &yPosEnd);
        sprintf(name, "X(%.1lf-%.1lfmm)_Y(%.1lf-%.1lfmm)", xPosStart, xPosEnd, yPosStart, yPosEnd);
        cout << "X range: " << xPosStart << " mm - " << xPosEnd << " mm." << endl;
        cout << "Y range: " << yPosStart << " mm - " << yPosEnd << " mm." << endl;

        expInfo.hxyt->GetXaxis()->SetRangeUser(xPosStart, xPosEnd);
        expInfo.hxyt->GetYaxis()->SetRangeUser(yPosStart, yPosEnd);
    }
    else if (expInfo.mode == "cut_wp")
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
        timeStart = 252.778 * expInfo.length * waveStart;
        timeEnd = 252.778 * expInfo.length * waveEnd;
        cout << "TOF range:        " << timeStart / 1000 << " ms - " << timeEnd / 1000 << " ms." << endl;
        cout << "Wavelength range: " << waveStart << " A  - " << waveEnd << " A." << endl;
        cout << "X range:          " << xPosStart << " mm - " << xPosEnd << " mm." << endl;
        cout << "Y range:          " << yPosStart << " mm - " << yPosEnd << " mm." << endl;

        expInfo.hxyt->GetXaxis()->SetRangeUser(xPosStart, xPosEnd);
        expInfo.hxyt->GetYaxis()->SetRangeUser(yPosStart, yPosEnd);
        expInfo.hxyt->GetZaxis()->SetRangeUser(timeStart, timeEnd);
    }

    expInfo.hxyt->SetNameTitle(("3D" + string(name)).c_str(), ("3D" + string(name)).c_str());
    if (expInfo.saveData)
    {
        if (!expInfo.f->Get(expInfo.hxyt->GetName()))
            expInfo.hxyt->Write();
    }

    TH2F *hxy = (TH2F *)expInfo.hxyt->Project3D("yx");

    TH1F *htmp = (TH1F *)expInfo.hxyt->Project3D("z");

    int verbose = 0;
    if (expInfo.mode == "cut_t" || expInfo.mode == "cut_w" || expInfo.mode == "cut_wp")
        verbose = 50; // 设置左右空余bin数

    double left = htmp->GetBinLowEdge(1);
    double right = left + (htmp->GetNbinsX()) * expInfo.widthBin;
    double binNum = htmp->GetNbinsX() + 2 * verbose;

    TH1F *h[4];
    // time of flight spectrum and wavelength spectrum
    h[0] = new TH1F("", ";Time of Flight [#mus];Counts", binNum, left - verbose * expInfo.widthBin, right + verbose * expInfo.widthBin);
    h[1] = new TH1F("", ";Neutron wavelength [#AA];Counts", binNum, (left - verbose * expInfo.widthBin) / (252.778 * expInfo.length), (right + verbose * expInfo.widthBin) / (252.778 * expInfo.length));
    for (int i = verbose; i < binNum - verbose; i++)
    {
        h[0]->SetBinContent(i + 1, htmp->GetBinContent(i - verbose + 1));
        h[1]->SetBinContent(i + 1, htmp->GetBinContent(i - verbose + 1));
    }

    // position resolution is obtained by projection.
    if (expInfo.projection != "no")
    {
        h[2] = (TH1F *)expInfo.hxyt->Project3D("x"); // x
        h[2]->GetYaxis()->SetTitle("Counts");

        h[3] = (TH1F *)expInfo.hxyt->Project3D("y"); // y
        h[3]->GetYaxis()->SetTitle("Counts");
    }

    // create canvas for different mode
    const int nCanvas = 5;
    string c_name[nCanvas] = {"2DImaging", "TimeOfFlight", "NeutronWavelength", "XProjection", "YProjection"};
    TCanvas *c[nCanvas];
    for (int i = 0; i < (expInfo.projection != "no" ? nCanvas : (nCanvas - 2)); i++)
    {
        c[i] = new TCanvas();
        c[i]->SetTitle((c_name[i] + name).c_str());
        c[i]->SetName((c_name[i] + name).c_str());
        if (i == 0)
        {
            c[i]->SetTicks();
            c[i]->SetRightMargin(0.2);
            c[i]->SetLeftMargin(0.2);

            TH2F_setting(hxy);
            hxy->SetTitle("");
            // hxy->SetContour(50);
            // hxy->SetMaximum(1000);
            hxy->GetXaxis()->SetTitleOffset(1.2);
            hxy->GetYaxis()->SetTitleOffset(1);
            hxy->DrawCopy("COLZ");
        }
        else
        {
            // c[i]->SetGrid();
            c[i]->SetTicks();
            TH1F_setting(h[i - 1]);
            h[i - 1]->SetTitle("");
            h[i - 1]->DrawCopy();
        }
        if (expInfo.saveData)
        {
            if (!expInfo.f->Get(c[i]->GetName()))
                c[i]->Write();
        }
    }
    if (expInfo.saveData)
    {
        if (expInfo.treeStatu)
            expInfo.t->Write();

        expInfo.f->Close();
    }
}
// 1D histogram setting
void TH1F_setting(TH1F *myHist)
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
void TH2F_setting(TH2F *myHist)
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