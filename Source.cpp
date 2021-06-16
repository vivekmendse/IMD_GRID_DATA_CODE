/* This program reads binary data for 365/366 days and writes in ascii file. */

#include <Windows.h>
#include <fstream>
#include <boost/date_time/gregorian/gregorian.hpp> //you need boost library for using this
using namespace std;

// C code provided by IMD on their official website
//https://imdpune.gov.in/Clim_Pred_LRF_New/Grided_Data_Download.html

int ImdCode() 
{
	float rf[135][129], rainfall;
	float lo[135], la[129];
	int i, j, k, year, year1, month, date, nd;
	FILE* fptr1, * fptr2;
	int nd1[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
	int nd2[13] = { 0,31,29,31,30,31,30,31,31,30,31,30,31 };
	year = 2020;
	printf("Year = %d", year);
	fopen_s(&fptr1, "Rainfall_ind2020_rfp25.grd", "rb"); // input file
	fopen_s(&fptr2, "Rainfall_ind2020_rfp25.out", "w");

	if (fptr1 == NULL)
	{
		printf("Can't open file");
		return 0;
	}

	if (fptr2 == NULL)
	{
		printf("Can't open file");
		return 0;
	}

	for (j = 0; j < 135; j++)
		lo[j] = 66.5 + j * 0.25;

	for (j = 0; j < 129; j++)
		la[j] = 6.5 + j * 0.25;

	year1 = year / 4;
	year1 = year1 * 4;

	for (month = 1; month < 13; month++)
	{
		nd = nd1[month];

		if (year == year1)
			nd = nd2[month];

		for (date = 1; date <= nd; date++)
		{
			fprintf(fptr2, "\n%02d%02d%04d", date, month, year);

			for (j = 0; j < 135; j++)
				fprintf(fptr2, "%7.2f", lo[j]);

			for (i = 0; i < 129; i++)
			{
				fprintf(fptr2, "\n%8.2f", la[i]);

				for (j = 0; j < 135; j++)
				{
					if (fread(&rainfall, sizeof(rainfall), 1, fptr1) != 1)
						return 0;

					rf[j][i] = rainfall;

					fprintf(fptr2, "%7.1f", rf[j][i]);
				}
			}
			printf("%4d %02d %02d \n", year, month, date);
		}
	}
	fclose(fptr1);
	fclose(fptr2);
	printf("Year = %d", year);
	return 0;
}


const int LONGITUDE = 135;
const int LATITUDE = 129;

int MyCodeCSV()
{
	// I create CSV so i can import Data in MSSQL DB and extract information as per my requirement
	// I use  boost date so i don't need to take care of leap year etc.

	int year = 2020;

	ifstream fin;
	ofstream fout;

	fout.open("Rainfall_ind2020_rfp25.csv", ios::binary);

	if (fout.is_open() == false)
		return 0;

	fin.open("Rainfall_ind2020_rfp25.grd", ios::binary);

	if (fin.is_open() == false)
		return 0;

	boost::gregorian::date startPt(year, 01, 01);
	boost::gregorian::date end(year, 12, 31);
	boost::gregorian::date_duration  oneDay(1);

	float rainfall = 0.0;

	while (startPt <= end)
	{
		for (size_t la = 0; la < LATITUDE; la++)
		{
			float laCal = 6.5 + la * 0.25;

			for (size_t lo = 0; lo < LONGITUDE; lo++)
			{
				float loCal = 66.5 + lo * 0.25;

				fin.read(reinterpret_cast<char*>(&rainfall), sizeof(float));

				fout << startPt << ","
					<< fixed << setprecision(2) << laCal << ","
					<< fixed << setprecision(2) << loCal << ","
					<< fixed << setprecision(2) << rainfall << "\n";
			}
		}
		startPt += oneDay;
	}

	fin.close();
	fout.close();
	return 0;
}

int main()
{
	ImdCode();
	MyCodeCSV();
	return 0;
}