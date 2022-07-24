# N0vaResourceExtractor帮助

### 这东西干嘛用的

人工桌面的内容都是**预渲染视频与图片**，因此我们提供了一个刻意把人工桌面的内容转换为其原本格式的工具。

### 怎么用

确保已在[人工桌面官方网站](https://n0va.mihoyo.com/#/)下载并安装完成软件本体，然后打开人工桌面，转到左下角的设置，找到**资源存储**这个输入框，复制路径，例如我的路径是`D:/N0vaDesktop/N0vaDesktopCache/N0vaDesktopCache`，并在后面加入`/game`。

双击打开`N0vaResourceExtractor.exe`，你可以选择提取单个文件或提取目录（我建议提取目录，因为你没法从文件名里猜出来里面装的是什么）。

提取单个文件：

点击**选择单个文件转换**，然后会打开一个文件选择框，找到你刚刚复制的路径，接着选择你需要转换的文件（我就随便选一个了）。选择好后，点击开始。等待进度条走完，进入程序所在的目录，再进入`out`文件夹，可以看到一个与刚刚选择的文件同名的文件，即转换完成的文件。

提取目录：

点击**选择目录批量转换**，然后会打开一个目录选择框，如上所述找到`game`目录，完成后的文件也会输出到`out`目录中。

### 常见错误

> 由于找不到VCRUNTIMExxx.dll/MSVCPxxx.dll，无法继续执行代码。重新安装程序可能会解决此问题。

下载[此文件](https://aka.ms/vs/17/release/vc_redist.x64.exe)后按照指示安装并重启，再运行此程序即可。



若按照上述措施仍未能解决问题，请发邮件至shandiankulishe@gmail.com或shandiankulishe@163.com，也可在[本程序仓库](https://github.com/youfantan/N0vaResourceExtractor)提出Issues，作者看到后会及时回复。



## 附录Ⅰ：从源代码构建本程序

使用git clone[本仓库](https://github.com/youfantan/N0vaResourceExtractor)。

安装Microsoft Visual Studio 2019并安装`使用C++的桌面开发`工作负荷。

用上述程序打开仓库，等待VS索引完成后点击`生成解决方案`，在`x64/Debug`或`x64/Release`中找到生成的`UIApp.exe`或`CLIApp`。



## 附录Ⅱ：获取适用于MacOS/Linux的相似工具

请参见本仓库`#2`与`#3`两个issues。