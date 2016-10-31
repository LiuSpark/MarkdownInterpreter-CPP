20161031更新 by SparkLiu
-----------------------------
###概要###
这是一个在[实验楼](https://www.shiyanlou.com/courses/569)中找到的一个项目，用CPP写的Markdown解释器，[具体要求](https://zhuanlan.zhihu.com/p/21951150)可以点击这个网页查看  

###简介
在这个Markdown解释器中，采用的是文件输入输出流，将文件读入C++中然后进行字符串操作并输出成html文件  

###实现的功能
1. 处理多级标题
2. 处理代码和代码块
3. 处理多级引用
4. 处理无序、有序列表
5. 处理转义字符
6. 处理粗体、斜体
7. 处理分割线
8. 处理超链接和图片

###运用的库
+ fstream
+ string
+ sstream
+ vector
+ stack
+ queue
+ regex

###项目结构
+ 程序文件
 + main.cpp
 + mdtransform.h
 + mdtransform.cpp
+ 待处理文件
 + test.md
+ 输出文件
  + index.html
  + my-markdown.css  
 
//mdtransform.h
```cpp

```