#include "Tools.hpp"



string Tools::int2String(int value)
{
	ostringstream converter;
	converter << value;
	return converter.str();
}



int Tools::string2Int(char * str)
{
	int result;
	istringstream converter(str);
	converter >> result;
	return result;
}



int Tools::string2Int(string str)
{
	int result;
	istringstream converter(str);
	converter >> result;
	return result;
}