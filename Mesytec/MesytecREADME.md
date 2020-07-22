
Mesytec电子学硬件部分由MCPD与MPSD-8+组成，均为NIM机箱插件，一块MPSD-8+可以为8根相同的3He管提供高压并获取数据，由一路高压输入和8组高压输出组成，MPSD-8+数据通过插件后端的单芯lemo接头传输到MCPD的前面板Bus Section，  Monitor/Chopper Section接入T0，T0为单芯lemo，数据经过以太网（最大支持百兆）传到上位机，需要的上位机为Debian7操作系统，软件安装步骤如下：

192.168.168.121

安装完成后，在终端输入qmesydaq，即可打开软件GUI界面。
1. Files中，主要使用Save Histogram File，保存数据为mtxt格式，Export PDF导出PDF，Export SVG导出矢量图（svg格式）。
1. Configuration->General，可以设置文件、直方图保存路径。
2. Configuration->Mode，选择Superuser。
3. Configuration->MCPD->Add，在弹出的窗口等5秒，Next，勾选Master，Finish。
4. Configuration->MCPD->Setup，在弹出的窗口，
5. Configuration->Module，其中MCPD ID为0，表示当前的，Module(Bus-#)为0，表示MPSD-8+接入MCPD模块的Bus Section，Mode/Gain/Threshold，可以设置阈值与增益（0-255），Histogram，可以设置MPSD-8+的通道数，共8个，每个表示一根管子。
6. Tools->Pluser中，可以进行电子学的自检测，手动设置MCPD ID，Module(Bus-#)，Channel数，幅度大小，以及信号位置；也可以自动测试，设置幅度。
7. Display面板中：
    - 第一块表示一维图（二维的Y方向投影）、二维图、衍射图（二维的X方向投影）；
    - 第二块表示位置（原始）、位置（刻度）、幅度（电荷）、delta-t（没有使用）；
    - 第三块表示线性与对数；
    - 第四块表示Counts的范围，不勾选会自动设置合理的值。
    - 最底下的Sum spectra只有上面先择Spectra（一维）的时候，可以去掉，也就是可以单独看某根3He管的一维谱（位置谱或电荷谱），Counts in displayed region表示当前区域的计数（总计数或单根管计数）。
8. DAQ面板中，表示控制启停，Listfile表示event by event模式（mdat格式），数据以二进制保存，histogram表示将直方图的数据保存（）。
9. 左Status面板中：
    - 第一块表示一些基本信息，软件版本、系统时间、测试时长、真正的测试时长；
    - 另外还有Run ID；Parameters中MCPD ID，P0为T0个数，Header time表示；Timing Delta Histogram中设置时间精度，最小分辨为100ns。
    - 第二块中Measurement，Timer表示设置测试时长（单位：秒），不勾选表示不限时长，需要手动Stop，Events表示设置要记录的事例数，一般不勾选，Monitor1到Monitor6分表表示外部触发，对应Monitor/Chopper1-4与Digital Input 1-2，可以设置触发次数（一般指T0的个数），针对特定的MPCD模块；Module表示MCPD ID的选择以及每个MCPD对应的8个MPSD的选择，Statistics表示；Files表示；Disk space表示。
10. 右Status面板中：ROI表示；Sliding Mean表示；Sigma表示。

