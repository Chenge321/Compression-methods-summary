/**
 * @file main.cpp
 * @author Chenge Liu (liu1217@mcmaster.ca)
 * @brief A program to rank four compression methods based on their compression rate, and decode a file that compressed by lz77 method.
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <iostream>
#include <fstream>
#include "classes.h"
using namespace std;
/**
 * @brief The main function to run encode or decode process
 * 
 * @param argc The number of input arguments from command line
 * @param argv The inputarguments from command line
 * @return int
 */
int main(int argc, char *argv[])
{
	// Check the invalid input command line arguments
	if (*argv[1] != 'E' and *argv[1] != 'D')
	{
		cout << "Please enter correct arguments." << '\n';
		cout
			<< "To encode, 1. E 2. file name" << '\n';
		cout << "To decode, 1. D 2. encoded file name 3. decoded file name" << '\n';
		return -1;
	}
	if ((*argv[1] == 'E' and argc != 3) || (*argv[1] == 'D' and argc != 4))
	{

		cout << "Please enter correct arguments." << '\n';
		cout
			<< "To encode, 1. E 2. file name" << '\n';
		cout << "To decode, 1. D 2. encoded file name 3. decoded file name" << '\n';
		return -1;
	}
	// This part does the encoding process
	if (*argv[1] == 'E')
	{
		lz77 lz77_coding;			  // lz77 encoding
		Arithmetic Arithmetic_coding; //Arithmetic encoding
		LZW_coding LZW_coding;		  //LZW encoding
		string filename1;			  //Input file name
		string filename2;
		string str; //Input string
		filename1 = argv[2];
		ifstream input(filename1);
		double ari_rate, lzw_rate, lr_rate, lz77_rate; // Compression rates for methods
		// Read string from the input file
		if (!input.is_open())
		{
			cout << "Error opening file!";
			return -1;
		}
		while (getline(input, str))
			input.close();
		//Doing the encoding process
		lzw_rate = LZW_coding.run_LZW(str);
		ari_rate = Arithmetic_coding.runArithmetic(str);
		lr_rate = RLencode(str);
		filename2 = filename1;
		//Remove .txt in the input file name
		eraseSubStr(filename2, ".txt");
		filename2 = filename2 + ".lz77";
		lz77_coding.file_encode(filename1, true, filename2);
		ifstream in_file(filename2, ios::binary);
		in_file.seekg(0, ios::end);
		uint64_t file_size = in_file.tellg();
		lz77_rate = double(file_size) / double(8 * str.length());
		cout
			<< "Compression rate of lz77:  " << lz77_rate << '\n';
		// Rank four methods based on the compression rate
		map<double, string> method_name = {{
											   ari_rate,
											   "Arithmetic",
										   },
										   {
											   lzw_rate,
											   "LZW",
										   },
										   {
											   lr_rate,
											   "RLencode",
										   },
										   {
											   lz77_rate,
											   "lz77",
										   }};
		// Print the result
		cout << "The rank of methods (in increasing compression rate): " << '\n';
		for (auto x : method_name)
		{
			cout << x.first << " " << x.second << endl;
		}
	}
	// This part does the decoding process
	if (*argv[1] == 'D')
	{
		lz77_decode decode;
		decode.file_decode(argv[2], true, argv[3]);
	}
	return 0;
}
