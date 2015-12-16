#define _CRT_SECURE_NO_WARNINGS
#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;


void err_handler(OCI_Error *err)
{
	printf(
		"code  : ORA-%05i\n"
		"msg   : %s\n"
		"sql   : %s\n",
		OCI_ErrorGetOCICode(err),
		OCI_ErrorGetString(err),
		OCI_GetSql(OCI_ErrorGetStatement(err))
		);
}

void  createTable(OCI_Connection *cn, OCI_Statement  *st){
	if (cn != NULL) {
		char sql[] = MT("create table test_table ")
			MT("( ")
			MT("val_int  number, ")
			MT("val_flt  float, ")
			MT("val_str  varchar2(30), ")
			MT("val_date date")
			MT(")");
		cout << "Create table:\n" << sql << endl;
		OCI_ExecuteStmt(st, sql);
	}
}

void dropTable(OCI_Connection *cn, OCI_Statement  *st){
	if (cn != NULL) {
		char sql[] = MT("drop table test_table");
		cout << "Drop table:\n" << sql << endl;
		OCI_ExecuteStmt(st, sql);
	}
}

void insertBind(OCI_Connection *cn, OCI_Statement  *st){
	OCI_Date *date;

	int i;
	double flt;
	if (cn != NULL) {
		char sql[] = MT("insert into test_table ")
			MT("( ")
			MT("   val_int,  val_flt, val_str, val_date")
			MT(") ")
			MT("values ")
			MT("( ")
			MT("   :val_int, :val_flt, :val_str, :val_date")
			MT(") ");
		cout << "Intsert table:\n" << sql << endl;
		OCI_Prepare(st, sql);
		i = 1;
		flt = 3.14;
		string s = "sfsdfsdfsfsdfsdfsd";
		date = OCI_DateCreate(cn);
		OCI_DateSysDate(date);

		OCI_BindInt(st, MT(":val_int"), &i);
		OCI_BindDouble(st, MT(":val_flt"), &flt);
		OCI_BindString(st, MT(":val_str"), const_cast<char *>(s.c_str()), 30);
		OCI_BindDate(st, MT(":val_date"), date);
		OCI_Execute(st);
		OCI_DateFree(date);
		OCI_Commit(cn);

	}
}

void updateData(OCI_Connection *cn, OCI_Statement  *st)
{
	if (cn != NULL) {
		char sql[] = MT("update test_table set val_int = ")
			MT(" :val_int1")
			MT(" where val_int = ")
			MT(" :val_int2");
		OCI_Prepare(st, sql);
		int i1 = 100;
		int i2 = 1;

		OCI_BindInt(st, MT(":val_int1"), &i1);
		OCI_BindInt(st, MT(":val_int2"), &i2);
		OCI_Execute(st);
		cout << "Update table:\n" << sql << endl;
		OCI_Commit(cn);
	}
}

void insertArray(OCI_Connection *cn, OCI_Statement  *st){
	OCI_Error      *err;
	int count = 20000;

	const int batchSize = 50;
	int tab_int[batchSize];
	double tab_flt[batchSize];
	char tab_str[batchSize][31];
	OCI_Date* tab_date[batchSize];

	int i;

	OCI_Prepare(st, "insert into test_table values(:i, :f, :s, :t)");
	OCI_BindArraySetSize(st, batchSize);
	OCI_BindArrayOfInts(st, ":i", tab_int, 0);
	OCI_BindArrayOfDoubles(st, ":f", tab_flt, 0);
	OCI_BindArrayOfStrings(st, ":s", (char*)tab_str, 30, 0);
	OCI_BindArrayOfDates(st, ":t", tab_date, 0);


	for (i = 0; i < batchSize; i++) {
		tab_int[i] = i + 1;
		tab_flt[i] = 3.14;
		sprintf(tab_str[i], "Name %d", i + 1);

		tab_date[i] = OCI_DateCreate(cn);
		OCI_DateSysDate(tab_date[i]);
	}

	int round = count / batchSize;

	clock_t start = clock();
	cout << start << endl;
	for (int j = 0; j < round; j++) {
		if (!OCI_Execute(st)) {
			printf("Number of DML array errors : %d\n", OCI_GetBatchErrorCount(st));

			err = OCI_GetBatchError(st);

			while (err) {
				printf("Error at row %d : %s\n", OCI_ErrorGetRow(err), OCI_ErrorGetString(err));

				err = OCI_GetBatchError(st);
			}
		}
		OCI_Commit(cn);
		//      printf("row processed : %d\n", OCI_GetAffectedRows(st));
	}

	clock_t stop = clock();
	cout << stop << endl;
	int costTime = stop - start;
	double numberPerSec = (count / costTime) * 1000;
	cout << "Insert records " << count << " cost time " << costTime << " ms" << endl;
	cout << "Insert records " << numberPerSec << "records/s " << endl;
	for (i = 0; i < batchSize; i++) {
		OCI_DateFree(tab_date[i]);
	}
	OCI_Commit(cn);
	return;
}

void selectData(OCI_Connection *cn, OCI_Statement  *st)
{
	if (cn != NULL) {
		char sql[512] = { 0 };
		sprintf(sql, "select VAL_INT,VAL_FLT,VAL_STR,VAL_DATE from test_table");
		OCI_ExecuteStmtFmt(st, MT(sql));
		static OCI_Resultset  *rs = OCI_GetResultset(st);
		while (OCI_FetchNext(rs))
		{
			int id = OCI_GetInt(rs, 1);
			float fl = OCI_GetFloat(rs, 2);
			string val_str1 = OCI_GetString(rs, 3);
			string val_date1 = OCI_GetString(rs, 4);
			cout << id << "\t" << fl << "\t" << val_str1 << "\t" << val_date1 << endl;

		}
	}
}

int main()
{
	OCI_Connection * cn = NULL;
	OCI_Statement  *st = NULL;
	

	if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
	{
		return -1;
	}

	cn = OCI_ConnectionCreate("192.168.1.40:1521/orcl", "scott", "tiger", OCI_SESSION_DEFAULT);

	st = OCI_StatementCreate(cn);
	if (cn != NULL)
	{
		printf("%s\n", OCI_GetVersionServer(cn));
		//OCI_ConnectionFree(cn);
	}  
	createTable(cn,st);	//创建表
	insertBind(cn, st); //  	插入一条记录
	//insertArray(cn,st); //  批量插入记录
	//updateData(cn,st); //更新数据
	//dropTable(cn, st);//	删除表
	selectData(cn,st);//		查询

	OCI_Cleanup();
	system("pause");
	return 0;
}

