#include "Statistic.hpp"






Statistic::Statistic(string filename)
{
	nFrame = 1;
	ofile = new ofstream();
	ofile->open(filename + ".csv");
}

Statistic::~Statistic()
{
	ofile->close();
	delete ofile;
}



void Statistic::saveStat(int up, int down)
{
	if (nFrame % 250 == 0 || nFrame == 11211)
	{
		string temp = Tools::int2String(nFrame) + "," + Tools::int2String(up) + "," + Tools::int2String(down);
		if (ofile->is_open())
		{
			*ofile << temp << endl;
		}

		cout << "=====" << nFrame << "=====" << endl;
		cout << "Up counter: " << up << endl;
		cout << "Down counter: " << down << endl;
		cout << "=====================" << endl << endl;

	}
	nFrame++;
}


