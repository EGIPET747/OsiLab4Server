#include "water.h"
#include <QtCore/QCoreApplication>

water::water(void)
{
	Name = " ";
	WaterType = 1;
	WaterTypeText = " ";
	Stok = false;
	NameStok = 0;
	Istok = false;
	NameIstok = 0;
	Length = 1;
	LakeType = 0;
	WaterSqr = 100000;
	SeaSqr = 10000;
	ID = 0;

}

water::~water(void)
{
}