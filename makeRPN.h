
/************************************************************************************************************
*								This Source File is part of RPN Project										*
* 								    Copyright (c) wjh776a68 2020								    		*
* 																											*
*								File Name:			makeRPN.h							        			*
*								Author:				wjh776a68												*
*  								Function:			Functions of RPN Analyse, header code					*
*	 							CreateTime:			2020/10/24												*
* 								LastUpdateTime:		2020/10/24												*
* 																											*
*************************************************************************************************************/

#pragma once

#include "RichTextDialog.h"

#include <atlstr.h>	//tchar to string
#include<deque>
#include<map>
#include<tuple>
#include<set>



typedef char relation;

#define NULLCHARACTER '$'
#define RPN_RELATION_HIGH	1
#define RPN_RELATION_EQUAL	2
#define RPN_RELATION_LOW	3

using namespace std;

class makeRPNclass {
public:
	makeRPNclass();																//���캯��
	~makeRPNclass() {};


	void BindInputHWND(HWND Input_RichTextDialogclass);
	void BindoutputFirstSetHWND(HWND Output_RichTextDialogclass);
	void BindoutputFollowSetHWND(HWND Output_RichTextDialogclass);
	void BindoutputStatusSheetHWND(HWND Output_RichTextDialogclass);
	void BindOutputHWND(HWND Output_RichTextDialogclass);

	void buildAnalyseSheet(string input);
	void outputFirstSet();
	void makeRPN();

	TCHAR* toTCHAR(std::string input);
	TCHAR* toTCHAR(int input);

private:
	void outputaline(const char currentsymbol, const string reststring, const deque<char>& symbolstack, const string outputstring);

	map< std::tuple<char, char>, relation> privilegemap;						//��ϵ��
	map<int, set<char>> relationnetwork;										//���ȼ�����
	set<char> Symbolset;
	
	int indexlevel = 0;															//������к�

	char numbertmpbuffer[MAXLEN] = { 0 };										//�����ݴ�����

	//RichTextDialogclass Output_Bind_RichTextDialogclass;
	HWND				Output_Bind_RichTextDialogclass;						//����б����
	HWND				FirstSet_Bind_RichTextDialogclass;						//First���б����
	HWND				FollowSet_Bind_RichTextDialogclass;						//Follow���б����
	HWND				StatusSheet_Bind_RichTextDialogclass;					//״̬ת��ͼ�б����
	RichTextDialogclass Input_Bind_RichTextDialogclass;							//�����ı����������
	
	TCHAR				output[MAXLEN] = { 0 };
	TCHAR				nulldivision[6];
	TCHAR				equaldivision[6];

	
};