#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10gR2 
#define OTL_STL
#include "otlv4.h" // include the OTL 4.0 header file
#include <string>
#include <vector>
using namespace std;
struct Department{
	string id;
	string name;
	string location;
};

void createSchema(otl_connect& db)
{
	otl_cursor::direct_exec(db, "create table department(id int, name varchar(20), location varchar(50))", otl_exception::disabled);
}

void findDepartmentC(otl_connect& db, const string& id, Department &d)
{
	char sql[64] = "";
	sprintf(sql, "select * from Department "
		"where id = '%s'", id.c_str());
	//sprintf(sql,"select * from Department");
	otl_stream stream(10,sql,db);
	

	if (!stream.eof())
	{
		stream >> d.id >> d.name>> d.location;
		cout << d.id << "\t"<<d.name<<"\t" << d.location << endl;
	}
}

void findDepartments(otl_connect& db) {
	otl_stream stream(50, "select * from Department", db);
	Department d;
	while (!stream.eof()) {
		stream >> d.id >> d.name >> d.location;
		cout << d.id << "\t" << d.name << "\t" << d.location << endl;
	}
}

/*void callProc(otl_connect& db)
{
	otl_stream stream(1,
		"begin"
		" test_proc(:a1<char(6),in out>,"
		"     :a2<char[16],in>,:a3<int,out>);"
		"end;",
		db);
	stream.set_commit(0);
	stream << "RND" << "Huang zhong";
	int res;
	stream >> res;
	cout << "test_proc result:" << res << endl;
} */

void addDepartments(otl_connect& db){
	otl_stream stream(1,// 对于插入语句为1
		"insert into Department values "
		"(:did<char(6)>,:dname<char(32)>,:dloc<char(45)>)",
		db);
	stream << "ABC" << "ABC Department" << "Suzhou";//一次执行
}

int main() {
	using namespace std;
	otl_connect db; // connect object #2
	otl_connect::otl_initialize(); // initialize OCI environment
	try {
		//db.rlogon("scott/tiger"); // connect to Oracle 
		db.rlogon("scott/tiger@192.168.1.40:1521/orcl"); // connect to Oracle
		
		cout << "Connected to oracle 11g." << endl;
		Department d1;
		string id1 = "2";
		//createSchema(db); // 创建表
		//findDepartmentC(db,id1,d1);	// 查询一个
		findDepartments(db);// //查询更多记录
		addDepartments(db);	  //插入数据

	
	}
	catch (otl_exception& p) { // intercept OTL exceptions
		cerr << p.msg << endl; // print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.var_info << endl; // print out the variable that caused the error
	}
	db.logoff(); // disconnect from Oracle 
	system("pause");
}