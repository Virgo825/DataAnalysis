# GEM自研电子学数据分析

&emsp;&emsp;首先根据数据是否经过 `SOC` 生成，设置 `soc` 为 `true` 或 `false` ，根据实际情况修改飞行距离 `Length` 与像素大小 `padSize` ， Bin宽 `binSize` 默认 10$\mu$s，飞行时间范围 `tofRange` 默认40ms。

运行命令：
```bash
root -l 'GEM.C("par1", "par2", "par3")'
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

&emsp;&emsp;如果需要将结果保存到 `ROOT` 文件中，将 `saveData` 的值设为 `true` ，`ROOT` 文件包含一个含有事例信息 `xytw` 的 `Tree` 、不同条件下关于 `xyt` 的三位直方图以及若干二维图、飞行时间谱、波长谱和位置投影。新结果不会覆盖之前的结果，会更新到 `ROOT` 文件中。