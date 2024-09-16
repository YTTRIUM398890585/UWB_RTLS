#include "UWB_TAG_ANCHOR/utils.h"

void printMatrix(MatrixX3f M, String name)
{
	IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

	Serial.print(name);
	Serial.println(" = ");

	auto formatted = M.format(CleanFmt);

	// Convert the formatted output to a string
	std::stringstream ss;
	ss << formatted;
	std::string formattedString = ss.str();

	// Print the formatted string using Serial.print
	Serial.println(formattedString.c_str());
}

void printVector(VectorXf v, String name)
{
	IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

	Serial.print(name);
	Serial.println(" = ");

	auto formatted = v.format(CleanFmt);

	// Convert the formatted output to a string
	std::stringstream ss;
	ss << formatted;
	std::string formattedString = ss.str();

	// Print the formatted string using Serial.print
	Serial.println(formattedString.c_str());
}