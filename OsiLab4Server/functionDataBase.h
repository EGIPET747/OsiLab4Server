#pragma once
#include <QtCore/QCoreApplication>
#include <QVector>
#include <QTimer>
#include "water.h"
#include <windows.h>
#include <string.h>
#include <process.h>

int pageIndex;					// Индекс записи
int pageNumber;					// Счётчик записей
water fillMas[10];			// Массив шаблонов

QTimer timer;
QVector <water> Save;	// Массив записей
HANDLE hDataBase;				// Дескриптор файла базы данных
HANDLE hConfig;					// Дескриптор файла конфига
TCHAR databaseName[100];		// Для хранения имени бд
int time;

// Для работы с каналами
HANDLE PipeName;				// Дескриптор канала
BOOL   fConnected;
HANDLE hPipe;
char   szBuf[1024];
DWORD  cbRead;
DWORD  cbWritten;


void start();			// Подготовительные работы
void finish();			// Завершающие работы
void write(int i);		// Передача данных из бд клиенту
void readall();			// Считывание из базы данных
void browser();			// Формирование списка записей
void ret();				// Откат (Отмена)
void create();			// Создание новой записи
void save();			// Сохранение записи
void sortMas();			// Сортировка массива
void del();				// Удаление записи
void fill();			// Заполнение 10 записей
void templates();		// Формирование шаблонов записей для заполнения

void read(int i) {};		// Приём данных от клиента
void writeFile(int i) {}	// Запись в базу данных