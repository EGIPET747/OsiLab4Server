#pragma once
#include <QtCore/QCoreApplication>
#include <QVector>
#include <QTimer>
#include "water.h"
#include <windows.h>
#include <string.h>
#include <process.h>

int pageIndex;					// ������ ������
int pageNumber;					// ������� �������
water fillMas[10];			// ������ ��������

QTimer timer;
QVector <water> Save;	// ������ �������
HANDLE hDataBase;				// ���������� ����� ���� ������
HANDLE hConfig;					// ���������� ����� �������
TCHAR databaseName[100];		// ��� �������� ����� ��
int time;

// ��� ������ � ��������
HANDLE PipeName;				// ���������� ������
BOOL   fConnected;
HANDLE hPipe;
char   szBuf[1024];
DWORD  cbRead;
DWORD  cbWritten;


void start();			// ���������������� ������
void finish();			// ����������� ������
void write(int i);		// �������� ������ �� �� �������
void readall();			// ���������� �� ���� ������
void browser();			// ������������ ������ �������
void ret();				// ����� (������)
void create();			// �������� ����� ������
void save();			// ���������� ������
void sortMas();			// ���������� �������
void del();				// �������� ������
void fill();			// ���������� 10 �������
void templates();		// ������������ �������� ������� ��� ����������

void read(int i) {};		// ���� ������ �� �������
void writeFile(int i) {}	// ������ � ���� ������