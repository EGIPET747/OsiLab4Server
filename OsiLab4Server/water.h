#pragma once
#ifndef WATER_H
#define WATER_H
#include <QtCore/QCoreApplication>
#include <QString>

class water
{
public:
	water();
	~water();

	int ID;
	QString Name;
	QString WaterTypeText;
	bool Stok;
	bool Istok;
	int NameStok;
	int NameIstok;
	int Length;
	int WaterSqr;
	int SeaSqr;
	int WaterType;
	int LakeType;
};

#endif // WATER_H


