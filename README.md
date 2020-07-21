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

## MultiChannel_dir.C

## MultiChannel_multi.C

## MultiChannel_stability.C

# Mesytec

## mesytec_single.C

## 
