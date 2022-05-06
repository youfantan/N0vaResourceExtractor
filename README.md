# N0vaResourceExtractor
## 这是什么
提取`人工桌面（米哈游旗下一款动态壁纸软件）`的动态壁纸资源的工具。
## 如何使用
### 如果需要批量拆资源文件
直接把二进制文件复制到人工桌面的安装目录然后运行即可。生成的文件为`源文件_changed.mp4`
### 如果需要单个替换
+ Linux：输入 `./N0vaResourceExtractor [需要替换的文件]`
+ Windows: 直接把ndf文件拖到二进制文件里即可
## 从源代码构建
### Windows(安装cmake和MinGW-W64)
```
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```
在`build`目录下寻找产出的二进制文件
## 开源
本项目使用MIT协议开源。模型为米哈游版权所有。

## CI/CD
本项目使用Github Actions作为其CI