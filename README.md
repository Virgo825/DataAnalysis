<center><font color=magenta size=6>ROOT 数据分析程序</font></center>

# 程序运行说明
- 函数名不带参数
    ```bash
    root -l xxx.C
    ```
- 函数名带参数
    ```bash
    root -l 'xxx.C(par1, par2, ...)'
    ```
    有些程序有默认参数，一般第一个参数为要处理的原始数据文件名，后续参数根据程序不同功能进行设置。

# CDT Software
## CDT_2D.C
用于2D模式获取数据的分析，核对探测器的通道数。
运行命令：
```bash
root -l 'CDT_TOF.C("xxx.pad")'
```
不卡任何条件，得到束斑形状。
```bash
root -l 'CDT_TOF.C("xxx.pad","yes")'
```
向X与Y方向投影。
## CDT_TOF.C
用于TOF模式获取的数据的分析，核对设置的Bin宽与Bin数是否正确，修改测试距离与探测器通道数，在Fill三维直方图中，可能需要调换x与y的位置。
运行命令：
```bash
root -l 'CDT_TOF.C("xxx.tof")'
```
不卡任何条件，一般看束斑形状与TOF谱。
```bash
root -l 'CDT_TOF.C("xxx.tof","cut")'
```
卡特定波长范围，可修改程序实现所需功能。
```bash
root -l 'CDT_TOF.C("xxx.tof","cut","yes")'
```
卡特定波长，并向X与Y方向投影，一般用于位置分辨计算。
# MultiChannel
## MultiChannel_single.C
处理单个`Spe`格式的能谱文件，得到计数率，峰位，能量分辨率。
```bash
root -l 'MultiChannel_single.C("xxx.Spe")'
```
## MultiChannel_dir.C
处理不同电压下多个能谱文件的结果，能谱文件需放在同一个文件夹中，程序与文件夹同级或上级，程序需要设置测试时间、多道道数，运行得到一个以文件名命名的root文件，包含每个能谱文件生成的hist，生成计数率曲线、峰位曲线、能量分辨率线三张图。
```bash
root -l 'MultiChannel_dir.C("xxx")'
```

## MultiChannel_multi.C
对两层目录进行遍历，第一层目录为不同的GEM膜，第二层为每片GEM膜的测试结果，运行生成一个以文件名命名的root文件，包含每一片的结果，每个结果分为原始的能谱图与处理后的计数率曲线、峰位曲线、能量分辨率线三张图。
```bash
root -l 'MultiChannel_multi.C("xxx")'
```

## MultiChannel_stability.C
用于稳定性结果分析，设置测试时间，测试的点的个数，运行生成一个包含原始能谱的root文件以及三张稳定行曲线：计数率、峰位、能量分辨率
```bash
root -l 'MultiChannel_stability.C("xxx")'
```


# Mesytec

## mesytec_single.C

## mesytec_diff.C

## MesytecREADME.md

