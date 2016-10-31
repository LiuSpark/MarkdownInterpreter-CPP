//
//  main.cpp
//  MarkdownParser
//

#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<stack>
#include<queue>
#include<regex>
#include "mdtransform.h"// ��Ҫʵ�ֵ� Markdown ������


int main() {
	// װ�ع��� Markdown �ļ�
	MarkdownTransform transformer("test.md");

	// ��дһ�� `getTableOfContents()` ��������ȡ Markdown �ļ� HTML ��ʽ��Ŀ¼
	//std::string table = transformer.getTableOfContents();

	// ��дһ�� `getContents()` ��������ȡ Markdown ת�� HTML �������
	std::string contents = transformer.getContents();

	// ׼��Ҫд��� HTML �ļ�ͷβ��Ϣ
	std::string head = "<!DOCTYPE html><html><head>\
        <meta charset=\"utf-8\">\
        <title>Markdown</title>\
        <link rel=\"stylesheet\" href=\"my-markdown.css\">\
        </head><body><article class=\"markdown-body\">";
	std::string end = "</article></body></html>";

	// �����д�뵽�ļ�
	std::ofstream out;
	out.open("output\\index.html");//Windows��д�Ĵ���
								   // ��ת��������ݹ��쵽 HTML �� <article></article> ��ǩ�ڲ�
	out << head + contents + end;
	out.close();


	//transformer.oneline_check("###aaa");//test


	return 0;
}
