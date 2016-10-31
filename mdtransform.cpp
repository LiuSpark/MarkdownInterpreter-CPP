#include<fstream>
#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<stack>
#include<queue>
#include<regex>
#include"mdtransform.h"

MarkdownTransform::MarkdownTransform(std::string the_position_of_mdfile)
{
	file_position = the_position_of_mdfile;
}

MarkdownTransform::~MarkdownTransform()
{
}

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
		//std::cout << get_oneline_new;
		fin.close();
	}
	return md2html_outfile;
}
//修改标题h1~h5
void MarkdownTransform::process_title(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	//如果已经遍历到了最后
	if (viter == totalstr.end())
	{
		return;
	}
	//处理title
	int j = 0;
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		if (!(*viter).empty())
		{
			j = 0;//计数器清零
			if (*striter == '#')
			{
				j++;
				striter++;
				for (; striter != (*viter).end(); striter++)
				{
					if (*striter == '#'&&*(striter - 1) == '#')
					{
						if (j < 5)
						{
							j++;
						}
					}
					else break;
				}

				int sign = j;
				//std::cout << sign;
				//1~5级标题标签加入
				if (sign >= 1 && sign <= 5)
				{
					std::string tempstr = (*viter);
					//两个for循环处理str数据，去除#
					for (auto iter = tempstr.end() - 1; iter != tempstr.begin(); iter--)
					{
						if (*iter == '#')
						{
							tempstr.erase(iter);
							iter = tempstr.end();
						}
						else break;
					}
					for (auto iter = tempstr.begin(); iter != tempstr.end(); )
					{
						if (*iter == '#')
						{
							tempstr.erase(iter);
							iter = tempstr.begin();
						}
						else break;
					}
					//整型转string
					std::stringstream ss;
					ss << sign;
					std::string hnumber = 'h' + ss.str();
					newstr = '<' + hnumber + '>' + tempstr + "</" + hnumber + '>';
					(*viter) = newstr;
					newstr = "";
				}
			}
			else if (*striter == '>')
			{
				striter++;
				while (*striter == '>'&& striter != (*viter).end())
				{
					striter++;
				}
				if (*striter == '#'&& striter != (*viter).end())
				{
					j++;
					striter++;
					for (; striter != (*viter).end(); striter++)
					{
						if (*striter == '#'&&*(striter - 1) == '#')
						{
							if (j < 5)
							{
								j++;
							}
						}
						else break;
					}
					int sign = j;
					if (sign >= 1 && sign <= 5)
					{
						std::string tempstr = (*viter);
						for (auto iter = tempstr.end() - 1; iter != tempstr.begin(); iter--)
						{
							if (*iter == '#')
							{
								tempstr.erase(iter);
								iter = tempstr.end();
							}
							else break;
						}
						int refer_counter = 0;
						auto iter = tempstr.begin();

						while (*iter == '>' && iter != tempstr.end())
						{
							refer_counter++;
							tempstr.erase(iter);
							iter = tempstr.begin();
						}
						for (; iter != tempstr.end();)
						{
							if (*iter == '#')
							{
								tempstr.erase(iter);
								iter = tempstr.begin();
							}
							else break;
						}
						while (refer_counter--)
						{
							newstr += '>';
						}
						std::stringstream ss;
						ss << sign;
						std::string hnumber = 'h' + ss.str();
						newstr += '<' + hnumber + '>' + tempstr + "</" + hnumber + '>';
						(*viter) = newstr;
						newstr = "";
					}

				}
			}
		}

	}
	viter = initialviter;
}

//修改代码块和小代码块
void MarkdownTransform::process_code(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	//处理代码块
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		//找到了一行```,下面判断有无匹配的另一行```//对newstr进行了修改
		if ((*viter).size() >= 3 && (*striter) == '`' && (*(striter + 1)) == '`' && (*(striter + 2)) == '`')
		{
			std::vector<std::string>::iterator tempviter = viter;
			std::string temp = *viter;
			*viter = "<pre><code>";
			//遍历文档寻找第二行```
			for (; tempviter != totalstr.end(); tempviter++)
			{
				std::string::iterator tempstriter = (*tempviter).begin();
				//if找到了第二行时，并输出，并更改viter的值，结束循环
				if ((*tempviter).size() >= 3 && (*tempstriter) == '`' && (*(tempstriter + 1)) == '`' && (*(tempstriter + 2)) == '`')
				{
					viter = tempviter;
					*tempviter = "</code></pre>";
					break;
				}
				else continue;
			}
			if (tempviter == totalstr.end())//强行修复
			{
				*viter = temp;
			}
		}
		//寻找行内代码块//对newstr进行了修改
		else
		{
			std::stack<char> tempb, outc;
			std::string::iterator striter = (*viter).begin();
			if (!(*viter).empty())
			{
				tempb.push((*striter++));
				int counter = 0;//记录行内代码块总数的2倍
				for (; striter != (*viter).end(); striter++)
				{
					if ((*striter) == '`'&&tempb.top() == '`')
					{
						continue;
					}
					else if ((*striter) == '`')
					{
						counter++;
					}
					tempb.push((*striter));
				}
				while (!tempb.empty())
				{
					outc.push(tempb.top());
					tempb.pop();
				}

				int totalcount = counter;
				while (!outc.empty())
				{
					if (outc.top() == '`')
					{
						if (totalcount > 1 && counter - 1 == 0 && totalcount % 2 == 1)
						{
						}
						else if (totalcount > 1 && (totalcount - counter) % 2 == 0)
						{
							newstr = newstr + "<code>";
						}
						else if (totalcount > 1 && (totalcount - counter) % 2 == 1)
						{
							newstr = newstr + "</code>";
						}
						outc.pop();
						counter--;
					}
					else
					{
						char ch = outc.top();
						newstr = newstr + ch;
						outc.pop();
					}
				}
				*viter = newstr;
				newstr = "";
			}
		}
	}
	viter = initialviter;
}

//处理引用
void MarkdownTransform::process_refer(std::vector<std::string>::iterator &viter, std::vector<std::string> &totalstr)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	int viterincreasement = 0;
	std::stack<int> refer_level;
	if (viter == totalstr.end())
	{
		return;
	}
	//处理代码块
	for (; viter != totalstr.end(); viter++)
	{
		
		if (!(*viter).empty())
		{
			std::string::iterator striter = (*viter).begin();
			if (*striter == '>')
			{
				newstr = "";
				while ((*viter) != "")//空行时引用结束
				{
					viter++;
					viterincreasement++;
				}
				for (int i=0; i<viterincreasement; i++)
				{
					viter--;
				}
				for (int i = 0; i < viterincreasement; i++,viter++)
				{
					int temptotal = 0;
					
					std::string tempstr = (*viter);
					std::string::iterator striter2 = tempstr.begin();
					if (*striter2 == '>'&&striter2!=tempstr.end()-1)
					{
						temptotal++;
						striter2++;
						for (; striter2 != tempstr.end(); striter2++)
						{
							if (*striter2 == '>'&&*(striter2 - 1) == '>')
							{
								temptotal++;
								
							}
							else break;
						}
						
						if (!refer_level.empty() && refer_level.top() < temptotal)
						{
							int x = temptotal;
							auto tempstriter = tempstr.begin();
							while(x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < (temptotal - refer_level.top());i++)
							{
								tempstr = "<blockquote>" + tempstr;
							}
							refer_level.push(temptotal);//add <>
						}
						else if (refer_level.empty())
						{
							int x = temptotal;
							auto tempstriter = tempstr.begin();
							while(x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < temptotal; i++)
							{
								tempstr = "<blockquote>" + tempstr;
							}
							refer_level.push(temptotal);//add <>
						}
						else if (!refer_level.empty() && refer_level.top() == temptotal)
						{
							continue;
						}
						else if (!refer_level.empty() && refer_level.top() > temptotal)
						{
							int n=refer_level.top()-temptotal;//add </>
							int x = temptotal;
							auto tempstriter = tempstr.begin();
							while (x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < n; i++)
							{
								tempstr =  "</blockquote>"+ tempstr ;
							}
							refer_level.pop();
							refer_level.push(temptotal);//add <>
						}
						*viter = tempstr;
					}
				}
				int n = refer_level.top();
				for (int i = 0; i < n; i++)
				{
					newstr+= "</blockquote>";
				}
				while (!refer_level.empty())
				{
					refer_level.pop();
				}
				(*viter) = newstr;
				newstr = "";
			}
		}
	}
	viter = initialviter;//使迭代器指回原来的地方
}

//list
//修改列表，需要调用好几个函数
void MarkdownTransform::process_list(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	std::stack<int> storageulol;
	int flag = 0, preflag = 0;
	int importantflag = 0;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		if ((*viter) == "")
		{
			while (!(storageulol.empty()))//排除特殊情况
			{
				int elem = storageulol.top();
				if (elem == 1)
				{
					(*viter) = (*viter) + "</ol>";//在文段的最后加入</ol>
				}
				else
				{
					(*viter) = (*viter) + "</ul>";//在文段的最后加入</ul>
				}
				storageulol.pop();
			}
			importantflag = 0;
			preflag = flag;
			flag = 0;
		}
		else
		{
			while (*striter == '>') { striter++; }
			while (*striter == ' ')
			{
				if (*striter == ' ') { flag++; }
				striter++;
			}
			if (flag == 0  && importantflag == 0)
			{
				int position = striter - (*viter).begin();
				int e = assist_list(position, *viter);
				if (e != 0)
				{
					storageulol.push(e);
					change_list(position, viter, e);
					importantflag = 1;
				}
				
			}
			else if (((flag + 3) / 4) > ((preflag + 3) / 4))
			{
				int n = ((flag + 3) / 4) - ((preflag + 3) / 4);
				if (n >= 2) { continue; }
				else
				{
					int position = striter - (*viter).begin();
					int e = assist_list(position, *viter);
					if (e != 0)
					{
						storageulol.push(e);
						change_list(position, viter, e);
					}
				}
			}
			else if (((flag + 3) / 4) == ((preflag + 3) / 4))
			{
				int position = striter - (*viter).begin();
				int e = assist_list(position, *viter);
				if (e != 0)
				{
					change_list(position, viter, 3);
				}
			}
			else //(((flag + 3) / 4) < ((preflag + 3) / 4))
			{
				int n = ((preflag + 3) / 4) - ((flag + 3) / 4);
				int position = striter - (*viter).begin();
				int e = assist_list(position, *viter);
				if (e != 0)
				{
					change_list(position, viter, 3);
					while (n--)
					{
						if (storageulol.top())
						{
							int elem = storageulol.top();
							if (elem == 1)
							{
								*viter = "</ol>"+(*viter);//在语句的最后加入</ol>
							}
							else
							{
								*viter = "</ul>" + (*viter);//在这一行的最后加入</ul>
							}
							storageulol.pop();
						}
					}
				}
			}
			preflag = flag;
			flag = 0;
		
		}
	}
	
	viter = initialviter;
}
//注意这里用string[i]于ascii码比对，可能出错
//返回1代表ol 2代表ul，必须把striter复位
int MarkdownTransform::assist_list(int position, std::string str)
{
	std::string::iterator tempstriter = str.begin() + position;
	if ((str.end() - tempstriter) > 2 && ((*tempstriter) == '*' || (*tempstriter) == '+' || (*tempstriter) == '-') && *(tempstriter + 1) == ' ')
	{
		return 2;//ul无序列表
	}
	else if ((int)(*tempstriter)>=48&& (int)(*tempstriter)<=57)//根据ASCII判断是否数字
	{ 
		tempstriter++;
		while ((int)(*tempstriter) >= 48 && (int)(*tempstriter) <= 57)
		{
			tempstriter++;
		}
		if ((str.end() - tempstriter) > 2 && *tempstriter == '.'&& *(tempstriter + 1) == ' ')
		{
			return 1;//ol有序列表
		}
		else { return 0; }
	}
	else { return 0; }//都不是
}
//删除标识符并插入ul或ol以及<li>和</li> 
void MarkdownTransform::change_list(int position, std::vector<std::string>::iterator &viter,int sign)
{
	std::string tempstr = (*viter);
	std::string::iterator tempstriter = tempstr.begin() + position;

	while (((*tempstriter) == '*' || (*tempstriter) == '+' || (*tempstriter) == '-' || ((int)(*tempstriter) >= 48 && (int)(*tempstriter) <= 57)) && tempstriter != tempstr.end())
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
	}

	if(tempstriter != tempstr.end())//删除空格
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
	}
	if (sign == 3)
	{
		tempstr.insert(position, "<li>");
		tempstr = tempstr + "</li>";
		(*viter) = tempstr;
	}
	else if (sign == 1)
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
		tempstr.insert(position, "<ol><li>");
		tempstr = tempstr + "</li>";
		(*viter) = tempstr;
	}
	else if (sign == 2)
	{
		tempstr.insert(position, "<ul><li>");
		tempstr = tempstr + "</li>";
		(*viter) = tempstr;
	}
}

//escape
//处理转义字符串,基于windows
void MarkdownTransform::process_escape_first(std::vector<std::string>::iterator & viter, 
	std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		if (!(*viter).empty())
		{
			for (; striter != (*viter).end(); )
			{
				if ((*striter) == '\\' && striter != ((*viter).end()-1))
				{
					char ch = *(striter + 1);
					charqueue.push(ch);
					striter++;
					int position = striter - (*viter).begin();
					(*viter).erase(striter);
					striter = (*viter).begin() + position ;
				}
				else striter++;
			}
		}
	}
	viter = initialviter;
	
}
//把转义字符给删了
void MarkdownTransform::process_escape_last(std::vector<std::string>::iterator & viter,
	std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::string newstr;
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		if (!(*viter).empty())
		{
			for (; striter != (*viter).end(); striter++)
			{
				if (*striter == '\\'&& !charqueue.empty())
				{
					char ch=charqueue.front();
					*striter = ch;
					charqueue.pop();
				}
			}
		}
	}
	viter = initialviter;
}

//处理粗体斜体
void MarkdownTransform::process_bold_and_italic(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::string tempstr;
	std::vector<std::string>::iterator initialviter = viter;
	int emcounter = 0, strongcounter=0;//em为斜体，strong为粗体
	//如果已经遍历到了最后
	if (viter == totalstr.end())
	{
		return;
	}

	for (; viter != totalstr.end(); viter++)
	{
		tempstr = (*viter);
		std::string::iterator striter = tempstr.begin();
		if (!tempstr.empty())
		{
			for (; striter != tempstr.end(); striter++)
			{
				int position = striter - tempstr.begin();
				if ((tempstr.end()-striter)>2&&(*striter) == '*'&&(*(striter + 1)) == '*'&& strongcounter==0)
				{
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.insert(position, "<strong>");
					(*viter) = tempstr;
					striter = tempstr.begin() + position;
					strongcounter = 1;
				}
				else if ((tempstr.end() - striter)>2 && (*striter) == '*' && (*(striter + 1)) == '*'&& strongcounter == 1)
				{
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.insert(position, "</strong>");
					(*viter) = tempstr;
					striter = tempstr.begin() + position;
					strongcounter = 0;
				}
				else if ((*striter) == '*'&& emcounter == 0)
				{
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.insert(position, "<em>");
					(*viter) = tempstr;
					striter = tempstr.begin() + position;
					emcounter = 1;
				}
				else if ((*striter) == '*'&& emcounter == 1)
				{
					tempstr.erase(striter);
					striter = tempstr.begin() + position;
					tempstr.insert(position, "</em>");
					(*viter) = tempstr;
					striter = tempstr.begin() + position;
					emcounter = 0;
				}
			}
			if (emcounter == 1) { (*viter) = (*viter) + "</em>"; emcounter = 0; }
			if (strongcounter == 1) { (*viter) = (*viter) + "</strong>"; strongcounter = 0; }
		}
	}
	viter = initialviter;
}

//处理分割线
void MarkdownTransform::process_parting_line(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	int j = 0;
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator striter = (*viter).begin();
		if (!(*viter).empty())
		{
			j = 0;//计数器清零
			if (*striter == '*')
			{
				j++;
				striter++;
				for (; striter != (*viter).end(); striter++)
				{
					if (*striter == '*'&&*(striter - 1) == '*')
					{
						if (j < 3)
						{
							j++;
							
						}
					}
					else break;
				}
				if (striter == (*viter).end()&&j==3) { (*viter) = "<hr>";  }
			}
		}
	}
	viter = initialviter;
}

//行末两空格换行
void MarkdownTransform::process_linebreak(std::vector<std::string>::iterator&viter, std::vector<std::string>&totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		if (!(*viter).empty())
		{
			if (*viter == "<pre><code>")//防止修改代码块内的内容
			{
				while (viter != totalstr.end() && *viter != "</code></pre>")
				{
					viter++;
				}
			}
			else 
			{
				std::string::iterator striter = (*viter).end() - 1;
				if ((*viter).length() > 1 && striter != (*viter).begin() && *striter == ' '&&*(striter - 1) == ' ')
				{
					*viter = (*viter) + "<br>";
				}
			}
		}
	}
	viter = initialviter;
}

//处理空行加入p标签
void MarkdownTransform::process_emptyline(std::vector<std::string>::iterator&viter, std::vector<std::string>&totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		if (!(*viter).empty())
		{
			continue;
		}
		else
		{
			*viter = "<p></p>";
		}
	}
	viter= initialviter;
}

//处理超链接
void MarkdownTransform::process_url(std::vector<std::string>::iterator&viter, std::vector<std::string>&totalstr)
{
	std::string tempstr;
	std::string newstr = "";
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	//创建正则表达式
	std::string totalpattern;
	totalpattern = "\\[.*\\]\\((https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|](((\\s+)\".*\")?)\\)";
	std::regex totalr(totalpattern);
	std::string urlpattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";
	std::string contentpattern = "\\[.*\\]\\(";
	std::string titlepattern = "\".*\"";
	std::regex urlr(urlpattern);
	std::regex titler(titlepattern);
	std::regex contentr(contentpattern);

	for (; viter != totalstr.end(); viter++)
	{
		tempstr = (*viter);
		std::string::iterator striter = tempstr.begin();
		if (!tempstr.empty())
		{
			//用vector容器存储,通过正则表达式分割后的片段
			std::vector<std::string> storage_tempstr;
			storage_tempstr.push_back(tempstr);
			if (!storage_tempstr.empty())
			{
				for (std::sregex_iterator riter(tempstr.begin(), tempstr.end(), totalr), end_it; riter != end_it; riter++)
				{
					std::string limit = riter->str();//当前正则表达式
					if (!storage_tempstr.empty())
					{
						std::string str = storage_tempstr.back();//vector最后一个元素
						storage_tempstr.pop_back();//删除vector最后一个元素
						split(str, limit,storage_tempstr);
					}
				}
				std::string after_limit;
				int veci = 0;
				for (std::sregex_iterator riter(tempstr.begin(), tempstr.end(), totalr), end_it; riter != end_it; riter++,veci++)
				{
					std::string limit = riter->str();
					std::string after_limit = assist_url(limit, urlr,titler,contentr);
					newstr = newstr + storage_tempstr[veci] + after_limit;
				}
				if (storage_tempstr[veci] != "")
				{
					newstr += storage_tempstr[veci];
				}
			}
			*viter = newstr;
			newstr = "";
		}
	}
	viter = initialviter;
}
//辅助函数 按照传入的limit将字符串分割为前后两段
void MarkdownTransform::split(std::string str, std::string limit, std::vector<std::string> & result)
{
	std::vector<std::string>::size_type ptr;
	int len = str.length();
	ptr = str.find(limit, 0);
	if ((int)ptr < len)
	{
		std::string substr = str.substr(0, ptr);
		result.push_back(substr);
		std::string substr2 = str.substr(ptr+ limit.length(),len);
		result.push_back(substr2);
	}
}
//处理字符串转化为<a href=""></a>
std::string MarkdownTransform::assist_url(std::string &str,std::regex &urlr,std::regex &titler,std::regex & contentr)
{
	std::string newstr;
	std::smatch urlresult,tlresult,ctresult;
	regex_search(str, urlresult, urlr);
	regex_search(str, ctresult, contentr);
	std::string ctresult_change = ctresult.str();//修正内容
	std::string::iterator siter = ctresult_change.begin();
	ctresult_change.erase(siter);
	siter= ctresult_change.begin();
	siter = ctresult_change.end() - 1;
	for (int i = 0; i < 2; i++,siter--)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.end();
	}
	if (!regex_search(str, tlresult, titler))
	{
		newstr = "<a href=\"" + urlresult.str() + "\">" + ctresult_change + "</a>";
	}
	else
	{
		regex_search(str, tlresult, titler);
		newstr = "<a href=\"" + urlresult.str() + "\" title=" +tlresult.str()+">"+ ctresult_change + "</a>";
	}
	return newstr;
}

//处理超链接
void MarkdownTransform::process_image(std::vector<std::string>::iterator&viter, std::vector<std::string>&totalstr)
{
	std::string tempstr;
	std::string newstr = "";
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	//创建正则表达式
	std::string totalpattern;
	totalpattern = "!\\[.*\\]\\((https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|](((\\s+)\".*\")?)\\)";
	std::regex totalr(totalpattern);
	std::string urlpattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";
	std::string contentpattern = "!\\[.*\\]\\(";
	std::string titlepattern = "\".*\"";
	std::regex urlr(urlpattern);
	std::regex titler(titlepattern);
	std::regex contentr(contentpattern);

	for (; viter != totalstr.end(); viter++)
	{
		tempstr = (*viter);
		std::string::iterator striter = tempstr.begin();
		if (!tempstr.empty())
		{
			//用vector容器存储,通过正则表达式分割后的片段
			std::vector<std::string> storage_tempstr;
			storage_tempstr.push_back(tempstr);
			if (!storage_tempstr.empty())
			{
				for (std::sregex_iterator riter(tempstr.begin(), tempstr.end(), totalr), end_it; riter != end_it; riter++)
				{
					std::string limit = riter->str();//当前正则表达式
					if (!storage_tempstr.empty())
					{
						std::string str = storage_tempstr.back();//vector最后一个元素
						storage_tempstr.pop_back();//删除vector最后一个元素
						split(str, limit, storage_tempstr);
					}
				}
				std::string after_limit;
				int veci = 0;
				for (std::sregex_iterator riter(tempstr.begin(), tempstr.end(), totalr), end_it; riter != end_it; riter++, veci++)
				{
					std::string limit = riter->str();
					std::string after_limit = assist_image(limit, urlr, titler, contentr);
					newstr = newstr + storage_tempstr[veci] + after_limit;
				}
				if (storage_tempstr[veci] != "")
				{
					newstr += storage_tempstr[veci];
				}
			}
			*viter = newstr;
			newstr = "";
		}
	}
	viter = initialviter;
}
//处理字符串转化为<a href=""></a>
std::string MarkdownTransform::assist_image(std::string &str, std::regex &urlr, std::regex &titler, std::regex & contentr)
{
	std::string newstr;
	std::smatch urlresult, tlresult, ctresult;
	regex_search(str, urlresult, urlr);
	regex_search(str, ctresult, contentr);
	std::string ctresult_change = ctresult.str();//修正内容
	std::string::iterator siter = ctresult_change.begin();
	for (int i = 0; i < 2; i++)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.begin();
	}
	siter = ctresult_change.end() - 1;
	for (int i = 0; i < 2; i++, siter--)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.end();
	}
	if (!regex_search(str, tlresult, titler))
	{
		newstr = "<img src=\"" + urlresult.str() + "\" alt=\"" + ctresult_change + "\" />";
	}
	else
	{
		regex_search(str, tlresult, titler);
		newstr = "<img src=\"" + urlresult.str() + "\" title=" + tlresult.str() + "\" alt=\"" + ctresult_change + "\" />";
	}
	return newstr;
}