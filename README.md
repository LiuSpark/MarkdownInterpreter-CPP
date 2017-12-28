### Summary ###
This is a project found in [shiyanlou.com](https://www.shiyanlou.com/courses/569), a markdown interpreter written in CPP，[specific requirements](https://zhuanlan.zhihu.com/p/21951150)can be found in this page.



### Introduction
In this Markdown interpreter, the file I/O stream is used. The \*.md file is read into CPP. Then, strings are changed into HTML files.

### Function
1. Multi-level title
2. Code and code blocks
3. Multi-level references
4. Disordered, ordered list
5. Escape characters
6. Bold, italic
7. Dividing line
8. Hyperlinks and pictures

### Libraries
+ fstream
+ string
+ sstream
+ vector
+ stack
+ queue
+ regex

### Structure
+ code
  + main.cpp
  + mdtransform.h
  + mdtransform.cpp

+ markdown file
  + test.md
 
+ output file
  + index.html
  + my-markdown.css  

### Implementation
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
**function** getContents()
```cpp
std::string MarkdownTransform::getContents()
{
	std::string md2html_outfile = "";
	std::ifstream fin;
	std::queue<char> charqueue;
	fin.open(file_position);
	if (fin.is_open())
	{
		std::string get_oneline;
		std::vector<std::string> totalstring;

		while (getline(fin, get_oneline))
		{
			totalstring.push_back(get_oneline);

		}
		int strveclen = totalstring.size();
		auto vit = totalstring.begin();
		process_escape_first(vit, totalstring,charqueue);
		process_code(vit, totalstring);
		process_list(vit, totalstring);
		process_parting_line(vit, totalstring);
		process_title(vit, totalstring);
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
