# MultiChannel
## MultiChannel_single.C
处理单个 `Spe` 格式的能谱文件，得到计数率，峰位，能量分辨率。
```bash
root -l 'MultiChannel_single.C("xxx.Spe")'
```
## MultiChannel_dir.C
处理不同电压下多个能谱文件的结果，能谱文件需放在同一个文件夹中，程序与文件夹同级或上级，程序需要设置测试时间、多道道数，运行得到一个以文件名命名的root文件，包含每个能谱文件生成的 `hist` ，生成计数率曲线、峰位曲线、能量分辨率线三张图。
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