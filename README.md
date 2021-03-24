# ROOT 数据分析程序
用于 `MultiChannel` 、`CDT` 、`Mesytec` 日常测试结果分析。
## 程序运行说明
### 函数名不带参数
```bash
root -l xxx.C
```
### 函数名带参数
```bash
root -l 'xxx.C(par1, par2, ...)'
```
有些程序有默认参数，一般第一个参数为要处理的原始数据文件名，后续参数根据程序不同功能进行设置。
