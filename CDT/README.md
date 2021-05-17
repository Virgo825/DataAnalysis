# CDT Software
## CDT_2D.C
&emsp;&emsp;用于 `2D` 模式获取数据的分析，核对探测器的通道数。

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
&emsp;&emsp;用于 `TOF` 模式获取的数据的分析，需要修改飞行距离 `Length` 与像素大小 `padSize` ，可以从 `txt` 文件获取通道数、 `Bin宽` 与 `Bin数` 的值，没有 `txt` 文件时，需要核对这三个参数是否正确。在Fill三维直方图中，需要根据实际情况调换 `x` 与 `y` 的位置。

运行命令：
```bash
root -l 'CDT_TOF.C("xxx")'
```
不卡任何条件，一般看束斑形状与TOF谱。
```bash
root -l 'CDT_TOF.C("xxx","cut_t")'
```
卡特定TOF范围，可修改程序实现所需功能。
```bash
root -l 'CDT_TOF.C("xxx","cut_w")'
```
卡特定波长范围，可修改程序实现所需功能。
```bash
root -l 'CDT_TOF.C("xxx","all","yes")'
```
不卡任何条件，向X与Y方向投影。
```bash
root -l 'CDT_TOF.C("xxx","cut","yes")'
```
卡特定波长，并向X与Y方向投影，一般用于位置分辨计算。

其中 `xxx` 为不带后缀的文件名。