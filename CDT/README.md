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
&emsp;&emsp;用于 `TOF` 模式获取的数据的分析，需要修改飞行距离 `Length` 与像素大小 `padSize` ，可以从 `txt` 文件获取通道数、 `Bin宽` 与 `Bin数` 的值，没有 `txt` 文件时，需要核对这三个参数是否正确。在 `Fill` 三维直方图中，需要根据实际情况调换 `X` 与 `Y` 的位置。

运行命令：
```bash
root -l 'CDT_TOF.C("par1", "par2", "par3")'
```
总共3个参数，
- `par1` 为不带后缀的文件名，不可缺省。
- `par2` 为工作模式选择，缺省值为 `all`。
    - `all` 表示不卡任何条件；
    - `cut_t` 表示卡飞行时间；
    - `cut_w` 表示卡中子波长；
    - `cut_p` 表示卡X与Y方向位置；
    - `cut_wp` 表示卡中子波长及位置。
- `par3` 为投影选择，缺省值为 `no`。
    - `yes` 表示分别向X与Y方向投影；
    - `no` 表示不投影。

例如：
- 不卡任何条件，一般看束斑形状与TOF谱。
    ```bash
    root -l 'CDT_TOF.C("xxx")'
    ```
- 卡特定波长范围，可修改程序实现所需功能。
    ```bash
    root -l 'CDT_TOF.C("xxx","cut_w")'
    ```
- 卡特定位置范围，可修改程序实现所需功能。
    ```bash
    root -l 'CDT_TOF.C("xxx","cut_p")'
    ```
- 不卡任何条件，向X与Y方向投影。
    ```bash
    root -l 'CDT_TOF.C("xxx","all","yes")'
    ```
- 卡特定波长，并向X与Y方向投影，一般用于位置分辨计算。
    ```bash
    root -l 'CDT_TOF.C("xxx","cut_w","yes")'
    ```
