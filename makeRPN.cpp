
/************************************************************************************************************
*								This Source File is part of RPN Project										*
* 								    Copyright (c) wjh776a68 2020								    		*
* 																											*
*								File Name:			makeRPN.cpp						        				*
*								Author:				wjh776a68												*
*  								Function:			Functions of RPN Analyse, source code					*
*	 							CreateTime:			2020/10/24												*
* 								LastUpdateTime:		2020/10/24												*
* 																											*
*************************************************************************************************************/

#include "makeRPN.h"


makeRPNclass::makeRPNclass() {
	//()
	//*/
	//+-
	string defaultprivilege = "*/\r\n+-\r\n()";

	
	buildAnalyseSheet(defaultprivilege);

	lstrcpy(equaldivision, TEXT(" → "));
	lstrcpy(nulldivision, TEXT("ε"));
}

//input为符号优先级字符串,优先级从高到低，由\r\n分开优先级
void makeRPNclass::buildAnalyseSheet(string input) {
	input += "\r\n#";	//最底层
	int privilegelevel = 0;
	for (auto i = input.begin(); i != input.end(); i++) {
		if (*i == '\r' || *i == '\n') {
			privilegelevel++;
			i++;
		}
		else {
			relationnetwork[privilegelevel].insert(*i);
			Symbolset.insert(*i);
		}
	}

	for (auto i = relationnetwork.begin(); i != relationnetwork.end(); i++) {//相等关系
		auto theend = i->second.end();
		theend--;
		for (auto selected_i = i->second.begin(); selected_i != theend; selected_i++) {
			auto j = i->second.begin();
			j++;
			for (auto selected_i2 = j; selected_i2 != i->second.end(); selected_i2++) {
				privilegemap[make_tuple(*selected_i, *selected_i2)] = RPN_RELATION_EQUAL;
				privilegemap[make_tuple(*selected_i2, *selected_i)] = RPN_RELATION_EQUAL;
			}
		}
	}

	auto theend = relationnetwork.end();
	theend--;
	for (auto i = relationnetwork.begin(); i != theend; i++) {//大小关系
		auto j = i;
		j++;
		for (auto ii = j; ii != relationnetwork.end(); ii++) {
			if (i == ii)
				continue;
			for (auto selected_i = i->second.begin(); selected_i != i->second.end(); selected_i++) {
				for (auto selected_ii = ii->second.begin(); selected_ii != ii->second.end(); selected_ii++) {
					privilegemap[make_tuple(*selected_i, *selected_ii)] = RPN_RELATION_HIGH;
					privilegemap[make_tuple(*selected_ii, *selected_i)] = RPN_RELATION_LOW;
				}
			}
		}
	}
}



void makeRPNclass::outputaline(const char currentsymbol, const string reststring, const deque<char>& symbolstack, const  string outputstring) {
	LV_ITEM lv;
	lv.mask = LVIF_TEXT;
	lv.iImage = 0;

	lv.iSubItem = 0;
	lv.iItem = indexlevel;

	lv.pszText = toTCHAR(string{ currentsymbol });
	ListView_InsertItem(Output_Bind_RichTextDialogclass, &lv);
	lv.iSubItem++;

	lv.pszText = toTCHAR(reststring.substr(0,reststring.length()-1));
	ListView_SetItem(Output_Bind_RichTextDialogclass, &lv);
	lv.iSubItem++;

	std::string tmp;
	auto i = symbolstack.begin();
	for (i++; i != symbolstack.end(); i++) {
		tmp += *i;
	}

	lv.pszText = toTCHAR(tmp);
	ListView_SetItem(Output_Bind_RichTextDialogclass, &lv);
	lv.iSubItem++;

	lv.pszText = toTCHAR(outputstring);
	ListView_SetItem(Output_Bind_RichTextDialogclass, &lv);
	lv.iSubItem++;

	indexlevel++;
}

void makeRPNclass::makeRPN() {
	indexlevel = 0;
	SendMessage(Output_Bind_RichTextDialogclass, LVM_DELETEALLITEMS, 0, 0);

	string outputstring = "";//分析得到的结果串
	string loutputstring = "";//分析得到的结果串
	string reststring = "";
	deque<int> numstack;		//算术运算栈
	int tmpnum = 0;
	deque<char> symbolstack;	
	symbolstack.push_back('#');

	for (int i = 0; i < Input_Bind_RichTextDialogclass.RichTextDialog_GetLength(); i++) {
		reststring += string{ Input_Bind_RichTextDialogclass.RichTextDialog_GetChar(i) };
	}
	reststring += "#";

	//outputaline(*reststring.begin(), reststring.substr(1), symbolstack, outputstring);
	short lastflag = -1;
	while (reststring.length()>1) {
		auto i = reststring.begin();
		if (Symbolset.find(*i)!=Symbolset.end()) {
			if (lastflag == 3) {
				numstack.push_back(tmpnum);
				_itoa_s(tmpnum, numbertmpbuffer, 10);
				outputstring += string{ numbertmpbuffer };
				outputstring += " ";
				tmpnum = 0;
			}

			if (privilegemap[make_pair(*i, symbolstack.back())] == RPN_RELATION_HIGH) {
				
				outputaline(*i, reststring.substr(1), symbolstack, loutputstring);
				loutputstring = outputstring;
				symbolstack.push_back(*i);				
				reststring.erase(i);
				lastflag = 1;
			}
			else if (privilegemap[make_pair(*i, symbolstack.back())] == RPN_RELATION_EQUAL) {	//相等则互相抵消
				
				outputaline(*i, reststring.substr(1), symbolstack, loutputstring);
				loutputstring = outputstring;
				symbolstack.pop_back();		
				reststring.erase(i);
				lastflag = 2;
			}
			else {//== RPN_RELATION_LOW

					outputaline(*i, reststring.substr(1), symbolstack, loutputstring);
					loutputstring = outputstring;
					outputstring += symbolstack.back();
					if (numstack.size() > 1) {
						int tmp2 = numstack.back();
						numstack.pop_back();
						int tmp1 = numstack.back();
						numstack.pop_back();
						int tmpresult;
						if (symbolstack.back() == '+')
							tmpresult = tmp1 + tmp2;					
						else if (symbolstack.back() == '-')
							tmpresult = tmp1 - tmp2;
						else if (symbolstack.back() == '*')
							tmpresult = tmp1 * tmp2;
						else if (symbolstack.back() == '/')
							tmpresult = tmp1 / tmp2;

						numstack.push_back(tmpresult);

						int deletelength = (int)log10(tmp2) + 2 + (int)log10(tmp1) + 2 + 1/*strlen(symbolstack.back())*/;
						while(deletelength--)
							outputstring.erase(outputstring.length() - 1, outputstring.length());
						_itoa_s(tmpresult, numbertmpbuffer, 10);
						outputstring += string{ numbertmpbuffer };
						outputstring += " ";
					}
					symbolstack.pop_back();
					lastflag = 4;
	
				//symbolstack.push_back(*i);
			}

			
			
		}
		else {
			if (*i <= '9' && *i >= '0') {
				if (lastflag == 3) {
					tmpnum = 10 * tmpnum + (*i - '0');
				}
				else {
					tmpnum = *i - '0';
				}
				lastflag = 3;
				reststring.erase(i);
			}
			else {
				if (lastflag == 3) {
					numstack.push_back(tmpnum);
					
					_itoa_s(tmpnum, numbertmpbuffer, 10);
					outputstring += string{ numbertmpbuffer };
					outputstring += " ";
					tmpnum = 0;
				}
				outputstring += *i;
				outputaline(*i, reststring.substr(1), symbolstack, loutputstring);
				loutputstring = outputstring;
				reststring.erase(i);
			}
		}

		
	}
	
	//outputaline(' ', " ", symbolstack, loutputstring);

	//loutputstring.substr(0, loutputstring.length() - 1);

	while(symbolstack.size()>1){
		auto i = symbolstack.back();
		loutputstring = outputstring;
		outputstring += i;

		if (lastflag == 3) {
			numstack.push_back(tmpnum);

			_itoa_s(tmpnum, numbertmpbuffer, 10);
			outputstring += string{ numbertmpbuffer };
			outputstring += " ";
			tmpnum = 0;
		}
		lastflag = -1;
		if (numstack.size() > 1) {
			int tmp2 = numstack.back();
			numstack.pop_back();
			int tmp1 = numstack.back();
			numstack.pop_back();
			int tmpresult;
			if (symbolstack.back() == '+')
				tmpresult = tmp1 + tmp2;
			else if (symbolstack.back() == '-')
				tmpresult = tmp1 - tmp2;
			else if (symbolstack.back() == '*')
				tmpresult = tmp1 * tmp2;
			else if (symbolstack.back() == '/')
				tmpresult = tmp1 / tmp2;

			numstack.push_back(tmpresult);

			int deletelength = (int)log10(tmp2) + 2 + (int)log10(tmp1) + 2 + 1/*strlen(symbolstack.back())*/;
			while (deletelength--)
				outputstring.erase(outputstring.length() - 1, outputstring.length());
			_itoa_s(tmpresult, numbertmpbuffer, 10);
			outputstring += string{ numbertmpbuffer };
			outputstring += " ";
		}

		outputaline(' ', " ", symbolstack, loutputstring);
		symbolstack.pop_back();
	}

	loutputstring = outputstring;
	outputaline(' ', " ", symbolstack, loutputstring);

}

void makeRPNclass::outputFirstSet() {
	SendMessage(FirstSet_Bind_RichTextDialogclass, LVM_DELETEALLITEMS, 0, 0);

	LV_ITEM lv;
	lv.mask = LVIF_TEXT;
	lv.iImage = 0;

	lv.iSubItem = 0;
	lv.iItem = 0;

	std::string tmp;
	for (auto i = relationnetwork.begin(); i != relationnetwork.end(); i++) {

		lv.pszText = toTCHAR(i->first);
		ListView_InsertItem(FirstSet_Bind_RichTextDialogclass, &lv);
		lv.iSubItem++;

		tmp = "{ ";
		auto thelast = i->second.end();
		thelast--;
		for (auto ii = i->second.begin(); ii != thelast; ii++) {
			tmp += *ii;
			tmp += " , ";
		}
		tmp += *thelast;
		tmp += " }";
		
		lv.pszText = toTCHAR(tmp);
		ListView_SetItem(FirstSet_Bind_RichTextDialogclass, &lv);

		lv.iItem++;
		lv.iSubItem = 0;
		
	}
}


TCHAR* makeRPNclass::toTCHAR(std::string input) {
	if (input == "") {
		output[0] = '\0';
		output[1] = '\0';
		output[2] = '\0';
		output[3] = '\0';
	}
	else {
		_tcscpy_s(output, CA2T(input.c_str()));
	}
	
	return output;
}

TCHAR* makeRPNclass::toTCHAR(int input) {
	_itot_s(input, output, 10);
	return output;
}






/******************************************************************************
*				函数名：		BindInputHWND
*				函数功能：	绑定输入文本框的窗口句柄
*				传入参数：	Input_RichTextDialogclass HWND 要绑定的窗口句柄
*				传出参数：	void
*******************************************************************************/
void makeRPNclass::BindInputHWND(HWND Input_RichTextDialogclass)
{
	// TODO: Add your implementation code here.
	HWND tmp = Input_RichTextDialogclass;
	Input_Bind_RichTextDialogclass.RichTextDialog_BindHWND(tmp);
}

/******************************************************************************
*				函数名：		BindOutputHWND
*				函数功能：	绑定输出列表框的窗口句柄
*				传入参数：	Output_RichTextDialogclass HWND 要绑定的窗口句柄
*				传出参数：	void
*******************************************************************************/
void makeRPNclass::BindOutputHWND(HWND Output_RichTextDialogclass)
{
	// TODO: Add your implementation code here.
	HWND tmp = Output_RichTextDialogclass;
	Output_Bind_RichTextDialogclass = tmp;
	//Output_Bind_RichTextDialogclass.RichTextDialog_BindHWND(tmp);
}

/******************************************************************************
*				函数名：		BindoutputFirstSetHWND
*				函数功能：	绑定FIRST列表框的窗口句柄
*				传入参数：	Output_RichTextDialogclass HWND 要绑定的窗口句柄
*				传出参数：	void
*******************************************************************************/
void makeRPNclass::BindoutputFirstSetHWND(HWND Output_RichTextDialogclass){
	// TODO: Add your implementation code here.
	HWND tmp = Output_RichTextDialogclass;
	FirstSet_Bind_RichTextDialogclass = tmp;
}

/******************************************************************************
*				函数名：		BindoutputFollowSetHWND
*				函数功能：	绑定FIRST列表框的窗口句柄
*				传入参数：	Output_RichTextDialogclass HWND 要绑定的窗口句柄
*				传出参数：	void
*******************************************************************************/
void makeRPNclass::BindoutputFollowSetHWND(HWND Output_RichTextDialogclass) {
	// TODO: Add your implementation code here.
	HWND tmp = Output_RichTextDialogclass;
	FollowSet_Bind_RichTextDialogclass = tmp;
}

/******************************************************************************
*				函数名：		BindoutputStatusSheetHWND
*				函数功能：	绑定FIRST列表框的窗口句柄
*				传入参数：	Output_RichTextDialogclass HWND 要绑定的窗口句柄
*				传出参数：	void
*******************************************************************************/
void makeRPNclass::BindoutputStatusSheetHWND(HWND Output_RichTextDialogclass) {
	// TODO: Add your implementation code here.
	HWND tmp = Output_RichTextDialogclass;
	StatusSheet_Bind_RichTextDialogclass = tmp;

}
