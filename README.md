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

###函数
//mdtransform.h  
```cpp
#ifndef MARKDOWNTRANSFORM_H
#define MARKDOWNTRANSFORM_H
class MarkdownTransform
{
public:
	MarkdownTransform(std::string);
	~MarkdownTransform();
	std::string getContents();
	void process_title(std::vector<std::string>::iterator &, std::vector<std::string> & );
	void process_code(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_refer(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_list(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_escape_first(std::vector<std::string>::iterator &, std::vector<std::string> & , std::queue<char>&);
	void process_escape_last(std::vector<std::string>::iterator &, std::vector<std::string> &, std::queue<char>&);
	void process_bold_and_italic(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_parting_line(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_linebreak(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void process_emptyline(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void process_url(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void process_image(std::vector<std::string>::iterator&, std::vector<std::string>&);
	//辅助函数
	int assist_list(int, std::string);
	void change_list(int, std::vector<std::string>::iterator &,int);

	void split(std::string str, std::string limit, std::vector<std::string> &);
	std::string assist_url(std::string &, std::regex &, std::regex &, std::regex &);
	std::string assist_image(std::string &, std::regex &, std::regex &, std::regex &);

private:
	std::string file_position;
};

#endif // !MARKDOWNTRANSFORM_H
```
//mdtransform.cpp
函数getContents()
```cpp
std::string MarkdownTransform::getContents()
{
	std::string md2html_outfile = "";//最后输出的总文件
	std::ifstream fin;
	std::queue<char> charqueue;//用于辨别转义字符
	fin.open(file_position);
	if (fin.is_open())
	{
		std::string get_oneline;
		std::vector<std::string> totalstring;

		while (getline(fin, get_oneline))//连续获取fin中的一行
		{
			totalstring.push_back(get_oneline);

		}
		int strveclen = totalstring.size();
		auto vit = totalstring.begin();
		process_escape_first(vit, totalstring,charqueue);
		process_code(vit, totalstring);//加入code标识符，必须最先，因为code中的东西无用
		process_list(vit, totalstring);
		process_parting_line(vit, totalstring);
		process_title(vit, totalstring); //加入h1~h5标识符，必须先于引用
		process_refer(vit, totalstring);
		process_bold_and_italic(vit, totalstring);
		process_image(vit, totalstring);
		process_url(vit, totalstring);
		process_linebreak(vit, totalstring);
		process_escape_last(vit, totalstring, charqueue);
		process_emptyline(vit, totalstring);

		for (auto ptr = totalstring.begin(); ptr != totalstring.end(); ptr++)
		{
			md2html_outfile += (*ptr) + '\n';
		}
		fin.close();
	}
	return md2html_outfile;
}
```
