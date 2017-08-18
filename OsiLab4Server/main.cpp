#include <QtCore/QCoreApplication>
#include "functionDataBase.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {

	setlocale(LC_ALL, "Russian");


	// Создание канала
	hPipe = CreateNamedPipe(L"\\\\.\\pipe\\myfirstpipe",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		254,
		1024,
		1024,
		5000,
		NULL);
	printf("Ожидание соединения...\n");
	// Ожидание подключения
	fConnected = ConnectNamedPipe(hPipe, NULL);

	if (!fConnected) {
		printf("Ошибка...\n");
		system("pause");
	}
	else {
		printf("Есть соединение\n");
		// Обработчик сигналов
		start();
		do {
			// Считывание сигнала
			ReadFile(hPipe, szBuf, 1024, &cbRead, NULL);

			// Создание новой записи
			if (!strcmp(szBuf, "> create")) {
				create();
			}

			// Удаление записи
			if (!strcmp(szBuf, "> del")) {
				del();
			}

			// Заполнение 10 записей
			if (!strcmp(szBuf, "> fill")) {
				fill();
			}

			// Сохранение записи
			if (!strcmp(szBuf, "> save")) {
				save();
			}

			// Откат (Отмена)
			if (!strcmp(szBuf, "> ret")) {
				ret();
			}

			// Формирование браузера записей
			if (!strcmp(szBuf, "> browser")) {
				browser();
			}
			
		} while (strcmp(szBuf, "> exit"));
		
		// Завершающие работы
		finish();
	}



	CloseHandle(hPipe);

	return 0;
}

// Функции при старте
void start() {
	char buf[20], *pch, str[20];
	DWORD dwBytesWritten;

	hConfig = CreateFile(L"config.txt", GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	ReadFile(hConfig, buf, 20, &dwBytesWritten, NULL);
	CloseHandle(hConfig);
	buf[dwBytesWritten] = NULL;

	pch = strtok(buf, "|");
	strcpy(str, pch);
	for (int i = 0; i < strlen(buf); i++) {
		databaseName[i] = str[i];
	}
	databaseName[strlen(str)] = '\0';

	pch = strtok(NULL, "|");
	time = atoi(pch);

	//-------------------------------------------

	readall();
	templates();

	// Отправить клиенту записи из бд
	sprintf(szBuf, "%d", pageNumber);
	WriteFile(hPipe, szBuf, strlen(szBuf) + 1, &cbWritten, NULL);	// Передача кол-ва стр

}

// Функции при окончании работы
void finish() {
	char buf[100];
	DeleteFile(databaseName);
	hDataBase = CreateFile(databaseName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD dwBytesWritten;
	sprintf(buf, "%d\r\n", pageNumber);
	WriteFile(hDataBase, buf, sizeof(char) * strlen(buf), &dwBytesWritten, NULL);

	for (int i = 0; i < pageNumber; i++) {
		write(i);
	}

	CloseHandle(hPipe);	// Закрыть порот
	CloseHandle(hDataBase);		// Закрыть файл бд
};

// Чтение всех записей из БД и формирование массива записей
void readall() {

	char buf[10];
	char *bufer, *pch;
	int valueNumber;
	DWORD dwBytesWritten;

	hDataBase = CreateFile(databaseName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	ReadFile(hDataBase, buf, 10, &dwBytesWritten, NULL);
	pch = strtok(buf, "\n");

	if (atoi(pch) != 0) {

		bufer = new char[atoi(pch) * 100];

		CloseHandle(hDataBase);
		hDataBase = CreateFile(databaseName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		ReadFile(hDataBase, (LPVOID)bufer, strlen(bufer), &dwBytesWritten, NULL);

		bufer[dwBytesWritten] = NULL;

		pch = strtok(bufer, "\n");
		valueNumber = atoi(pch);

		pageNumber += valueNumber;

		if (Save.size() < valueNumber)
		{
			Save.resize(valueNumber);
		}

		for (int i = 0; i < valueNumber; i++) {
			pch = strtok(NULL, "|");
			Save[i].ID = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].Name = QString::fromLocal8Bit(pch);
			pch = strtok(NULL, "|");
			Save[i].WaterTypeText = QString::fromLocal8Bit(pch);
			pch = strtok(NULL, "|");
			Save[i].Stok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].Istok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].NameStok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].NameIstok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].Length = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].WaterSqr = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].SeaSqr = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].WaterType = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].LakeType = atoi(pch);
		}

		delete[] bufer;
	}

	CloseHandle(hDataBase);
}

// Формирование списка записей для браузера
void browser() {
	// Приём индекса записи
	ReadFile(hPipe, szBuf, 1024, &cbRead, NULL);
	pageIndex = atoi(szBuf);
	char buf[1024];
	DWORD dwBytesWritten;

	// Отправка данных
	sprintf(buf, "%d|%s|%s\n\r", Save[pageIndex].ID,
		Save[pageIndex].Name.toLocal8Bit().toStdString().c_str(),
		Save[pageIndex].WaterTypeText.toLocal8Bit().toStdString().c_str());

	WriteFile(hPipe, buf, strlen(buf) + 1, &dwBytesWritten, NULL);
};

// Откат/Показать (Отмена)
void ret() {
	// Приём индекса записи
	char buf[1024];
	ReadFile(hPipe, szBuf, 1024, &cbRead, NULL);
	int ProcessedID = atoi(szBuf);					// Обрабатываемый ID

	for (int i = 0; i < pageNumber; i++) {
		if (ProcessedID == Save[i].ID) {
			// Передача данных
			sprintf(buf, "%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n",
				Save[i].Name.toLocal8Bit().toStdString().c_str(),
				Save[i].WaterTypeText.toLocal8Bit().toStdString().c_str(),
				Save[i].Stok,
				Save[i].Istok,
				Save[i].NameStok,
				Save[i].NameIstok,
				Save[i].Length,
				Save[i].WaterSqr,
				Save[i].SeaSqr,
				Save[i].WaterType,
				Save[i].LakeType);
			WriteFile(hPipe, buf, strlen(buf) + 1, &cbWritten, NULL);
		}
	}
}

// Создание новой записи
void create() {
	char buf[1024];
	DWORD dwBytesWritten;
	// Увеличение счётчика записей
	pageNumber++;

	// Расширение массива
	if (Save.size() < pageNumber)
	{
		Save.resize(pageNumber);
		int i = 0;
		bool f = true;

		do {
			bool coincidence = false;
			for (int j = 0; j <Save.size(); j++) {
				if (Save[j].ID == i) {
					coincidence = true;
					break;
				}
			}
			if (!coincidence) {
				Save[pageNumber - 1].ID = i;
				sprintf(buf, "%d\n", Save[pageNumber - 1].ID);
				WriteFile(hPipe, buf, strlen(buf) + 1, &dwBytesWritten, NULL);
				f = false;
			}
			else { i++; }
		} while (f);
	}
}

// Сохранение записи
void save() {
	int ProcessedID;
	char *pch;
	ReadFile(hPipe, szBuf, 1024, &cbRead, NULL);
	pch = strtok(szBuf, "|");
	ProcessedID = atoi(pch);
	/*			Save[i].Name,
				Save[i].WaterTypeText,
				Save[i].Stok,
				Save[i].Istok,
				Save[i].NameStok,
				Save[i].NameIstok,
				Save[i].Length,
				Save[i].WaterSqr,
				Save[i].SeaSqr,
				Save[i].WaterType,
				Save[i].LakeType*/
	for (int i = 0; i<pageNumber; i++) {
		if (ProcessedID == Save[i].ID) {
			pch = strtok(NULL, "|");
			Save[i].Name = QString::fromLocal8Bit(pch);
			pch = strtok(NULL, "|");
			Save[i].WaterTypeText = QString::fromLocal8Bit(pch);
			pch = strtok(NULL, "|");
			Save[i].Stok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].Istok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].NameStok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].NameIstok = atoi(pch);
			pch = strtok(NULL, "|");
			Save[i].Length = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].WaterSqr = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].SeaSqr = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].WaterType = atoi(pch);
			pch = strtok(NULL, "|\n");
			Save[i].LakeType = atoi(pch);
		}
	}

	sortMas();
};

// Сортировка массива записей
void sortMas() {
	for (int i = 0; i < Save.size(); i++)
	{
		for (int j = 0; j < Save.size() - i - 1; j++)
		{
			if (QString::compare(Save[j].Name, Save[j + 1].Name) >= 0)
			{
				water pop = Save[j];
				Save[j] = Save[j + 1];
				Save[j + 1] = pop;
			}
		}
	}
};

// Удаление записи
void del() {
	int ProcessedID;
	// Приём ID записи
	ReadFile(hPipe, szBuf, 1024, &cbRead, NULL);
	ProcessedID = atoi(szBuf);
	cout << ProcessedID << endl;
	for (int i = 0; i<pageNumber; i++) {
		if (ProcessedID == Save[i].ID) {
			Save.remove(i);				// Удаление элемента из массива
			pageNumber--;
			Save.resize(pageNumber);		// Уменьшение массива
			break;
		}
	}

};

// Заполнение 10 записей
void fill() {

	pageNumber += 10;

	for (int k = 0; k < 10; k++) {
		int i = 0;
		bool f = true;

		do {
			bool coincidence = false;
			for (int j = 0; j <Save.size(); j++) {
				if (Save[j].ID == i) {
					coincidence = true;
					break;
				}
			}
			if (!coincidence) {
				fillMas[k].ID = i;
				Save.push_back(fillMas[k]);
				f = false;
			}
			else { i++; }
		} while (f);
	}

	sortMas();

	// Отправить клиенту записи из бд
	sprintf(szBuf, "%d", pageNumber);
	WriteFile(hPipe, szBuf, strlen(szBuf) + 1, &cbWritten, NULL);	// Передача кол-ва стр

};

// Формирование шаблонов записей для заполнения
void templates() {
	/*			Save[i].Name,
	Save[i].WaterTypeText,
	Save[i].Stok,
	Save[i].Istok,
	Save[i].NameStok,
	Save[i].NameIstok,
	Save[i].Length,
	Save[i].WaterSqr,
	Save[i].SeaSqr,
	Save[i].WaterType,
	Save[i].LakeType*/
	fillMas[0].Name = QString("Volga");
	fillMas[0].WaterTypeText = QString("River");
	fillMas[0].Stok = 0;
	fillMas[0].Istok = 0;
	fillMas[0].NameStok = 0;
	fillMas[0].NameIstok = 0;
	fillMas[0].Length = 3530;
	fillMas[0].WaterSqr = 1360000;
	fillMas[0].SeaSqr = 0;
	fillMas[0].WaterType = 0;
	fillMas[0].LakeType = 0;


	fillMas[1].Name = QString("Volga 1");
	fillMas[1].WaterTypeText = QString("River");
	fillMas[1].Stok = 0;
	fillMas[1].Istok = 0;
	fillMas[1].NameStok = 0;
	fillMas[1].NameIstok = 0;
	fillMas[1].Length = 3530;
	fillMas[1].WaterSqr = 1360000;
	fillMas[1].SeaSqr = 0;
	fillMas[1].WaterType = 0;
	fillMas[1].LakeType = 0;

	fillMas[2].Name = QString("Volga 2");
	fillMas[2].WaterTypeText = QString("River");
	fillMas[2].Stok = 0;
	fillMas[2].Istok = 0;
	fillMas[2].NameStok = 0;
	fillMas[2].NameIstok = 0;
	fillMas[2].Length = 3530;
	fillMas[2].WaterSqr = 1360000;
	fillMas[2].SeaSqr = 0;
	fillMas[2].WaterType = 0;
	fillMas[2].LakeType = 0;

	fillMas[3].Name = QString("Volga 3");
	fillMas[3].WaterTypeText = QString("River");
	fillMas[3].Stok = 0;
	fillMas[3].Istok = 0;
	fillMas[3].NameStok = 0;
	fillMas[3].NameIstok = 0;
	fillMas[3].Length = 3530;
	fillMas[3].WaterSqr = 1360000;
	fillMas[3].SeaSqr = 0;
	fillMas[3].WaterType = 0;
	fillMas[3].LakeType = 0;

	fillMas[4].Name = QString("Volga 4");
	fillMas[4].WaterTypeText = QString("River");
	fillMas[4].Stok = 0;
	fillMas[4].Istok = 0;
	fillMas[4].NameStok = 0;
	fillMas[4].NameIstok = 0;
	fillMas[4].Length = 3530;
	fillMas[4].WaterSqr = 1360000;
	fillMas[4].SeaSqr = 0;
	fillMas[4].WaterType = 0;
	fillMas[4].LakeType = 0;

	fillMas[5].Name = QString("Volga 5");
	fillMas[5].WaterTypeText = QString("River");
	fillMas[5].Stok = 0;
	fillMas[5].Istok = 0;
	fillMas[5].NameStok = 0;
	fillMas[5].NameIstok = 0;
	fillMas[5].Length = 3530;
	fillMas[5].WaterSqr = 1360000;
	fillMas[5].SeaSqr = 0;
	fillMas[5].WaterType = 0;
	fillMas[5].LakeType = 0;

	fillMas[6].Name = QString("Volga 6");
	fillMas[6].WaterTypeText = QString("River");
	fillMas[6].Stok = 0;
	fillMas[6].Istok = 0;
	fillMas[6].NameStok = 0;
	fillMas[6].NameIstok = 0;
	fillMas[6].Length = 3530;
	fillMas[6].WaterSqr = 1360000;
	fillMas[6].SeaSqr = 0;
	fillMas[6].WaterType = 0;
	fillMas[6].LakeType = 0;

	fillMas[7].Name = QString("Volga 7");
	fillMas[7].WaterTypeText = QString("River");
	fillMas[7].Stok = 0;
	fillMas[7].Istok = 0;
	fillMas[7].NameStok = 0;
	fillMas[7].NameIstok = 0;
	fillMas[7].Length = 3530;
	fillMas[7].WaterSqr = 1360000;
	fillMas[7].SeaSqr = 0;
	fillMas[7].WaterType = 0;
	fillMas[7].LakeType = 0;

	fillMas[8].Name = QString("Volga 8");
	fillMas[8].WaterTypeText = QString("River");
	fillMas[8].Stok = 0;
	fillMas[8].Istok = 0;
	fillMas[8].NameStok = 0;
	fillMas[8].NameIstok = 0;
	fillMas[8].Length = 3530;
	fillMas[8].WaterSqr = 1360000;
	fillMas[8].SeaSqr = 0;
	fillMas[8].WaterType = 0;
	fillMas[8].LakeType = 0;

	fillMas[9].Name = QString("Volga 9");
	fillMas[9].WaterTypeText = QString("River");
	fillMas[9].Stok = 0;
	fillMas[9].Istok = 0;
	fillMas[9].NameStok = 0;
	fillMas[9].NameIstok = 0;
	fillMas[9].Length = 3530;
	fillMas[9].WaterSqr = 1360000;
	fillMas[9].SeaSqr = 0;
	fillMas[9].WaterType = 0;
	fillMas[9].LakeType = 0;

};

void write(int i) {
	char buf[100];
	DWORD dwBytesWritten;
	//Запись всех параметров "страницы" в буфер
	sprintf(buf, "%d|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n", Save[i].ID,
		Save[i].Name.toLocal8Bit().toStdString().c_str(),
		Save[i].WaterTypeText.toLocal8Bit().toStdString().c_str(),
		Save[i].Stok,
		Save[i].Istok,
		Save[i].NameStok,
		Save[i].NameIstok,
		Save[i].Length,
		Save[i].WaterSqr,
		Save[i].SeaSqr,
		Save[i].WaterType,
		Save[i].LakeType);
	WriteFile(hPipe, buf, strlen(buf) + 1, &cbWritten, NULL);
	WriteFile(hDataBase, buf, sizeof(char) * strlen(buf), &dwBytesWritten, NULL);
};

