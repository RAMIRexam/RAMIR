#ifndef STATISTIC_HPP
#define STATISTIC_HPP

#include "Tools.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

using namespace std;

class Statistic
{
	public:
		Statistic(string filename);
		~Statistic();
		void saveStat(int up, int down);

	private:
		int nFrame;
		ofstream *ofile;

};

#endif // !STATISTIC_HPP