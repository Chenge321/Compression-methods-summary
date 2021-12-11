/**
 * @file classes.h
 * @author Chenge Liu (liu1217@mcmaster.ca)
 * @brief This file contains all classes of compression methods.
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include <deque>
#include <cmath>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <stack>
#include "parameter.h"
using namespace std;
/**
 * @brief The class to record high and low range for arithmetic coding.
 * 
 */
class Range
{
public:
	//Constructor
	Range()
	{
		low = 0.0;
		high = 1.0;
		deltaLevel = 1.0;
	}
	~Range() = default;
	long double getLow()
	{
		return this->low;
	}

	long double getHigh()
	{
		return this->high;
	}

	void setLow(double low)
	{
		this->low = low;
	}

	void setHigh(double high)
	{
		this->high = high;
	}

	void setDelta(double delta)
	{
		this->deltaLevel = delta;
	}

private:
	long double low;		//Low range
	long double high;		//High range
	long double deltaLevel; //The difference between hig and low ranges
};
/**
 * @brief The arithmetic class
 * 
 */
class Arithmetic
{
public:
	//Constructor
	Arithmetic()
	{
		length = 0;
	}
	~Arithmetic() = default;

	// 获取编码，实际上就是创建对应表
	/**
	 * @brief Calculate the high and low ranges based on the frequency of each letters in the input string
	 * 
	 * @param str: input string
	 */
	void getProbabilities(string str)
	{
		char ch;
		double lowLevel = 0.0;
		double highLevel = 0.0;
		double probability;
		uint64_t freq[128] = {0}; // Frequency table to record the frequency of every letters in the input string
		for (uint64_t i = 0; str[i]; i++)
			freq[uint64_t(str[i])]++;
		for (uint64_t i = 0; i < 128; i++)
		{
			if (freq[i])
			{
				ch = (char)i;
				probability = (double)freq[i] / (double)str.length();
				lowLevel = highLevel;
				highLevel = lowLevel + probability;
				// calculate the range of every letters and store it in a map container
				Range range;
				range.setLow(lowLevel);
				range.setHigh(highLevel);
				range.setDelta(probability);
				map.insert(std::pair<char, Range>(ch, range));
			}
		}
	}
	/**
	 * @brief To do the encoding process of Arithmetic coding
	 * 
	 * @param str: the input string needs to be encoded
	 * @return compression rate of Arithmetic coding
	 */
	long double encode(string str)
	{
		long double lowRange = 0.0, highRange = 1.0;
		for (char &i : str)
		{
			// Calculate high range and low range by using map
			long double delta = highRange - lowRange;
			highRange = lowRange + delta * map[i].getHigh();
			lowRange = lowRange + delta * map[i].getLow();
			++length;
		}
		return lowRange;
	}
	/**
	 * @brief Transform decimal number into binary code
	 * 
	 * @param n The result of Arithmetic coding
	 */
	void dec2bin(long double n)
	{
		stack<char> s;
		uint64_t m = (uint64_t)n;
		long double t = n - (long double)m;
		binLength = 0;
		while (m) // Deal with integer
		{
			s.push(m % 2);
			m /= 2;
		}
		while (!s.empty())
		{
			cout << s.top();

			s.pop();
		}
		while (t - uint64_t(t) != 0) //Deal with decimal number
		{
			binLength++;
			t = 2 * t - uint64_t(2 * t);
		}
	}
	/**
	 * @brief To run the whole process of Arithmetic coding
	 * 
	 * @param str the string needs to be encoded
	 * @return compression rate of Arithmetic coding
	 */
	double runArithmetic(const string &str)
	{
		getProbabilities(str);
		cout << "Resulting code of Arithmetic coding: " << encode(str) << '\n';
		dec2bin(encode(str));
		//Calculate compression rate by using the binary code's size devides the length of the input string (8 bits for one character)
		double compression_rate = double(binLength) / double((str.length() * 8));
		cout
			<< "Compression rate of Arithmetic coding: " << compression_rate << endl;
		return compression_rate;
	}

private:
	uint64_t binLength;		   //The length of binary code
	uint64_t length;		   //The length of range
	std::map<char, Range> map; //Map to record the range for every letters
};
/**
 * @brief The LZW method class
 * 
 */
class LZW_coding
{
public:
	char StringForEncoding[MAX_ENCODE]; //The string needs to be encoded
	//Constructor
	LZW_coding()
	{
		StringForEncoding[0] = '\0';

		for (int i = 0; i < MAX_ENCODE; i++)
		{
			this->Output[i] = -INT_MAX;
		}

		for (int i = 0; i < MAX_ENCODE; i++)
		{
			this->originalDictionary[i][0] = '\0';
		}

		NumberOfTheOriginalDictionary = 0;
		length = 0;
	}

	/**
	 * @brief To initialize the original dictionary
	 * 
	 * @return true: initialize successfully
	 * @return false: initialize unsuccessfully
	 */
	bool InitializationFororiginalDictionary()
	{
		//Check the input string is empty or not
		if (StringForEncoding[0] == '\0')
		{
			return false;
		}
		//Adds the first character of the string to be encoded to the initial dictionary
		originalDictionary[0][0] = StringForEncoding[0];
		originalDictionary[0][1] = '\0';
		length = 1;
		int i, j;
		//Adds all the different characters in the string to the original dictionary
		for (i = 1; StringForEncoding[i] != '\0'; i++)
		{
			for (j = 0; originalDictionary[j][0] != '\0'; j++)
			{
				if (originalDictionary[j][0] == StringForEncoding[i])
				{
					break;
				}
			}
			if (originalDictionary[j][0] == '\0')
			{
				originalDictionary[j][0] = StringForEncoding[i];
				originalDictionary[j][1] = '\0';
				length++;
			}
		}
		//Gets the length of the initial dictionary
		NumberOfTheOriginalDictionary = length;
		return true;
	}

	/**
	 * @brief Encode the input string by LZW method and print the LZW table.
	 * 
	 * @return The compression rate
	 */
	double Encoding()
	{
		double compression_rate = 0;
		//The initial encoding in the original dictionary has no output encoding, so it is set to -1
		for (uint64_t g = 0; g < NumberOfTheOriginalDictionary; g++)
		{
			Output[g] = -1;
		}
		uint64_t num = NumberOfTheOriginalDictionary;
		char *q, *p, *m;
		//Pointer q is the flag pointer used to confirm position
		q = StringForEncoding;
		//The pointer p is the first character of the string matching
		p = StringForEncoding;
		//Matching by moving the M pointer
		m = p;
		//Loop until all characters are matched
		while (uint64_t(p - q) != uint64_t(strlen(StringForEncoding)))
		{
			uint64_t index = 0;
			//Match by moving the pointer m
			for (uint64_t i = 0; originalDictionary[i][0] != '\0' && uint64_t(m - q) != uint64_t(strlen(StringForEncoding)); i++)
			{
				char temp[MAX_ENCODE];
				//Every time find a matching character, one character is added to the matching string temp
				strncpy(temp, p, m - p + 1);
				temp[m - p + 1] = '\0';
				//Character matching succeeded
				if (strcmp(temp, originalDictionary[i]) == 0)
				{
					m++;
					index = i;
				}
			}
			//If an unmatched character is encountered or there are no characters left to match, the matched string is output
			Output[num++] = index;
			//If an unmatched character is found and there are still unmatched characters, added to the dictionary
			if (uint64_t(m - q) != uint64_t(strlen(StringForEncoding)))
			{
				strncpy(originalDictionary[length], p, m - p + 1);
				originalDictionary[length][m - p + 1] = '\0';
				length++;
			}
			p = m;
		}
		uint64_t TheBitForAfter = 0;
		uint64_t TheBitForBefore = 0;
		//Print the LZW table
		cout << endl
			 << "The encoding process table of LZW: " << endl
			 << endl;
		cout << "Index\t\t\tString\t\t\tOutput" << endl;
		// Calculate the size of string after encoding
		for (uint64_t i = 0; i < length; i++)
		{
			cout << "\t" << i + 1 << "\t\t\t" << originalDictionary[i] << "\t\t\t";
			if (i >= NumberOfTheOriginalDictionary)
			{
				cout << Output[i] + 1;
				TheBitForAfter++;
			}
			cout << endl;
		}
		cout << "\t-\t\t\t-\t\t\t" << Output[length] + 1 << endl;
		TheBitForBefore = uint64_t(strlen(StringForEncoding));
		//Calculate compression rate
		compression_rate = double((TheBitForAfter + 1)) / (double)TheBitForBefore;
		cout
			<< "Compresstion Ratio of LZW: " << compression_rate << "\n";
		return compression_rate;
	}
	/**
	 * @brief To run the whole process of LZW method
	 * 
	 * @param str: input string
	 * @return compression rate
	 */
	double run_LZW(const string &str)
	{
		double compression_rate = 0;
		strcpy(StringForEncoding, str.c_str());
		InitializationFororiginalDictionary();
		compression_rate = Encoding();
		return compression_rate;
	}

private:
	//The size of the original dictionary
	uint64_t NumberOfTheOriginalDictionary;
	//The length of dictionary
	uint64_t length;
	//The original dictionary
	char originalDictionary[MAX_ENCODE][MAX_ENCODE];
	//The output
	uint64_t Output[MAX_ENCODE];
};
/**
 * @brief A three marks class to help lz77 encode
 * 
 */
class Three_mark
{
public:
	uint64_t elem[3]; // To record location, length, symbol
	//Constructor
	Three_mark(uint64_t location = 0, uint64_t length = 0, uint64_t symbol = 0)
	{
		elem[0] = location;
		elem[1] = length;
		elem[2] = symbol;
	}
	/**
	 * @brief Set location, length, symbol
	 * 
	 * @param location Current location of letter
	 * @param length The length of matching string
	 * @param symbol The first letter in the buffer
	 */
	void set(uint64_t location, uint64_t length, uint64_t symbol)
	{
		elem[0] = location;
		elem[1] = length;
		elem[2] = symbol;
	}
	/**
	 * @brief To check does the mark reach the end or not
	 * 
	 * @return true: reach the end
	 * @return false: not reach the end
	 */
	bool is_end()
	{
		if (elem[0] == 0 && elem[1] == 0 && elem[2] == end_number)
		{
			return true;
		}
		else
			return false;
	}
};
/**
 * @brief Overload the operator to deal with Three_mark object. To calculate the binary code of the input object.
 * 
 * @param stream The input stream
 * @param o the object of Three_mark class
 * @return the binary code
 */
ostream &operator<<(ostream &stream, Three_mark o)
{
	stream << hex << "(0x" << setw(ceil(LogN / 4)) << setfill('0') << o.elem[0] << ",0x" << setw(ceil(LogN / 4)) << setfill('0') << o.elem[1] << "," << (char_A)o.elem[2] << "-0x" << setw(2 * A) << setfill('0') << o.elem[2] << ")" << dec;
	return stream;
};
/**
 * @brief The class to transform marks to binary code
 * 
 */
class Marks2bin : public ::deque<uint64_t>
{
private:
	int64_t index_b = 0; // The index of deque
	bool complete = true;
	bool flag = false;

public:
	void marks2bin(deque<Three_mark> binbuffer)
	{
		if (binbuffer.empty())
			return;
		int64_t index_a = 0; //The index of input mark
		int64_t index = 2;	 // The index of middle
		uint64_t b_temp = 0; //The temporary index
		if (!complete)
		{
			b_temp = deque::back();
			deque::pop_back();
		} //If the last transformation is not complete, the element at the end of deque is fetched
		Three_mark a_temp(binbuffer.front().elem[0], binbuffer.front().elem[1], binbuffer.front().elem[2]);
		binbuffer.pop_front(); //Fetch the element from binbuffer
		if (a_temp.is_end())
		{
			if (flag == false)
			{
				binbuffer.push_front(a_temp);
			}
			flag = !flag;
		}
		// Loop to transfor binary code until the binbuffer empty
		while (!((binbuffer.empty()) && (index == 0 && index_a == LogN)))
		{
			for (; index_b < 32; index_b++, index_a++)
			{
				if (index == 2 && index_a == A * 8)
				{
					index = 1;
					index_a = 0;
				}
				if (index == 1 && index_a == LogL)
				{
					index = 0;
					index_a = 0;
				}
				if (index == 0 && index_a == LogN)
				{
					if (binbuffer.empty())
					{
						if (index_b == 0)
						{
							complete = true;
						}
						else
						{
							deque::push_back(b_temp);
							complete = false;
						}
						return;
					}
					//Set the first element in binbuffer into a temporary object
					a_temp.set(binbuffer.front().elem[0], binbuffer.front().elem[1], binbuffer.front().elem[2]);
					binbuffer.pop_front();
					//If reach the end
					if (a_temp.is_end())
					{
						if (flag == false)
						{
							binbuffer.push_front(a_temp);
						}
						flag = !flag;
					}
					index = 2;
					index_a = 0;
				}
				if ((a_temp.elem[index]) & 1 << index_a)
					b_temp |= 1 << index_b;
			}
			index_b = 0;
			deque::push_back(b_temp);
			b_temp = 0;
		}
	}
	/**
	 * @brief Deal with elements in the end of mark.
	 * 
	 */
	void to_end()
	{
		uint64_t b_temp = 0;
		if (!complete)
		{
			b_temp = deque::back();
			deque::pop_back(); //Get the last element in deque
		}
		Three_mark a_temp(0, 0, end_number);
		int64_t index = 2;	 //The index of current element in mark ,from 0,1,2
		int64_t index_a = 0; //
		flag = true;
		while (flag)
		{
			for (; index_b < 32; index_b++, index_a++)
			{
				//Deal with the last element
				if (index == 2 && index_a == A * 8)
				{
					index = 1;
					index_a = 0;
				}
				//Deal with the second element
				if (index == 1 && index_a == LogL)
				{
					index = 0;
					index_a = 0;
				}
				//Deal with the first element
				if (index == 0 && index_a == LogN)
				{
					flag = false;
					break;
				}
				// Transform element into binary code
				if ((a_temp.elem[index]) & 1 << index_a)
					b_temp |= 1 << index_b;
			}
			//Put temporary into deque
			if (index_b == 32)
			{
				index_b = 0;
				deque::push_back(b_temp);
				b_temp = 0;
			}
		}
		// Transform element into binary code
		for (; index_b < 32; index_b++, index_a++)
		{
			b_temp |= 1 << index_b;
		}
		deque::push_back(b_temp);
		complete = true;
		return;
	}
	/**
	 * @brief Retrun if the process is completed or not
	 * 
	 * @return true 
	 * @return false 
	 */
	bool is_complete()
	{
		return complete;
	}
	/**
	 * @brief Clear the deque container
	 * 
	 */
	void clear()
	{
		deque::clear();
		index_b = 0;
		complete = true;
		flag = false;
	}
};
/**
 * @brief Class to help decode the file
 * 
 */
class Bin2marks : public ::deque<Three_mark>
{
private:
	int64_t index_a = 0;
	int64_t index = 2; //index of current element
	bool complete = true;
	bool flag = false;

public:
	/**
	 * @brief Transform binary code into character
	 * 
	 * @param b The deque contains binary code
	 */
	void bin2marks(deque<uint64_t> b)
	{
		if (b.empty())
			return;
		int index_b = 0;
		Three_mark a_temp(0, 0, 0); // A temporary mark
		if (!complete)
		{
			a_temp.set(deque::back().elem[0], deque::back().elem[1], deque::back().elem[2]);
			deque::pop_back();
		} //Fetch the element at the end of deque
		uint64_t b_temp = b.front();
		b.pop_front(); //Get the element in deque b
		//Transform binary code into character
		while (!((b.empty()) && (index_b == 32)))
		{
			switch (index)
			{
			//Transform upper case letter
			case 2:
				while (index_a < A * 8)
				{
					if (index_b == 32)
					{
						if (b.empty())
						{
							deque::push_back(a_temp);
							complete = false;
							return;
						}
						else
						{
							b_temp = b.front();
							b.pop_front();
							index_b = 0;
						}
					}
					if ((b_temp)&1 << index_b)
						(a_temp.elem[index]) |= 1 << index_a;
					index_a++;
					index_b++;
				}
				index = 1;
				index_a = 0;
			//Transform lower case letter
			case 1:
				while (index_a < LogL)
				{
					if (index_b == 32)
					{
						if (b.empty())
						{
							deque::push_back(a_temp);
							complete = false;
							return;
						}
						else
						{
							b_temp = b.front();
							b.pop_front();
							index_b = 0;
						}
					}
					if ((b_temp)&1 << index_b)
						(a_temp.elem[index]) |= 1 << index_a;
					index_a++;
					index_b++;
				}
				index = 0;
				index_a = 0;
			// Tansform integer and symbols
			case 0:
				while (index_a < LogN)
				{
					if (index_b == 32)
					{
						if (b.empty())
						{
							deque::push_back(a_temp);
							complete = false;
							return;
						}
						else
						{
							b_temp = b.front();
							b.pop_front();
							index_b = 0;
						}
					}
					if ((b_temp)&1 << index_b)
						(a_temp.elem[index]) |= 1 << index_a;
					index_a++;
					index_b++;
				}
				//Deal with the last element in mark
				index = 2;
				index_a = 0;
				if (flag)
				{
					if (a_temp.is_end())
					{
						flag = false;
						deque::push_back(a_temp);
					}
					else
					{
						complete = true;
						return;
					}
				}
				else
				{
					if (a_temp.is_end())
					{
						flag = true;
					}
					else
					{
						deque::push_back(a_temp);
					}
				}
				a_temp.set(0, 0, 0);
			}
		}
	}
	/**
	 * @brief Check the last element is dealed or not
	 * 
	 * @return true 
	 * @return false 
	 */
	bool to_end()
	{
		if (flag)
		{
			if (complete == false)
			{
				deque::pop_back();
			}
		} //Pop the not dealed element
		return flag;
	}
	/**
	 * @brief return the complete
	 * 
	 * @return true 
	 * @return false 
	 */
	bool is_complete()
	{
		return complete;
	}
	/**
	 * @brief Clear all containers
	 * 
	 */
	void clear()
	{
		deque::clear();
		index_a = 0;
		index = 2;
		complete = true;
		flag = false;
	}
};
/**
 * @brief The encode class for lz77 method
 * 
 */
class Encoder : public ::deque<char_A>
{
private:
	int64_t boundary = -1; //The start index of buffer
	int64_t location = 0;  //The current location
	int64_t length = 0;	   //The length of current matching stirng
public:
	/**
	 * @brief To encode the input mark, if complete return true
	 * 
	 * @param ch the input character
	 * @param mark the input three mark object
	 * @return true 
	 * @return false 
	 */
	bool encode(char_A ch, Three_mark *mark)
	{
		deque::push_back(ch); //push the character into deque container
		switch (length)
		{
		case 0:
			for (int i = 1; i <= boundary; i++)
			{
				if (ch == deque::at(boundary - i))
				{
					location = i;
					length++; //If match, boundary not change, increase length
					return false;
				}
			}
			break;
		case L - 1:
			break;
		default:
			if (ch == deque::at(boundary - location + length))
			{
				length++; //If match, boundary not change, increase length
				return false;
			}
			else
			{
				for (int64_t i = location + 1, j; i <= boundary; i++)
				{
					j = 0;
					while (j <= length && deque::at(boundary - i + j) == deque::at(boundary + j))
					{
						j++;
					}
					if (j > length)
					{
						location = i;
						length++;
						return false;
					}
				}
			}
			break;
		}
		//If there is no matching
		(*mark).set(location, length, ch);
		location = length = 0;
		for (int64_t i = (int64_t)deque::size() - N; i > 0; i--)
		{
			deque::pop_front();
		}
		boundary = deque::size(); //If there is not matching, boundary=size
		return true;
	}
	/**
	 * @brief Deal with the end element
	 * 
	 * @param mark Three mark object
	 */
	void to_end(Three_mark *mark)
	{
		deque::push_back(charactereof); //push character into deque
		(*mark).set(location, length, charactereof);
		location = length = 0;
		//Find is there any matchings
		for (int64_t i = (int64_t)deque::size() - N; i > 0; i--)
		{
			deque::pop_front();
		}
		boundary = deque::size();
		return;
	}
	/**
	 * @brief clear all container and reset variable
	 * 
	 */
	void clear()
	{
		deque::clear();
		boundary = -1;
		location = 0;
		length = 0;
	}
};
/**
 * @brief The class to decode a lz77 file
 * 
 */
class Decoder : public ::deque<char_A>
{
public:
	/**
	 * @brief To decode the input mark
	 * 
	 * @param buffer To store decoded character
	 * @param mark The input mark needs to be decoded
	 * @return true: decode successfully
	 * @return false: decode unsuccessfully
	 */
	bool decode(deque<char_A> *buffer, Three_mark *mark)
	{
		//Transform binary code into character
		for (uint64_t i = 0, j = (int64_t)deque::size() - (*mark).elem[0]; i < (*mark).elem[1]; i++)
		{
			deque::push_back(deque::at(j + i));
			(*buffer).push_back(deque::back());
		}
		deque::push_back((char_A)(*mark).elem[2]);
		(*buffer).push_back(deque::back());
		for (int64_t i = 0, j = (int64_t)deque::size() - N; i < j; i++)
		{
			deque::pop_front();
		} //Put the index into the end of mark
		return true;
	}
	/**
	 * @brief Deal with the end element 
	 * 
	 * @param buffer 
	 */
	void to_end(deque<char_A> *buffer)
	{
		if ((!(*buffer).empty()) && (*buffer).back() == charactereof)
		{
			(*buffer).pop_back();
		}
		return;
	}
	/**
	 * @brief Clear container
	 * 
	 */
	void clear()
	{
		deque::clear();
	}
};
/**
 * @brief The class of lz77 method
 * 
 */
class lz77
{
private:
	fstream file;								//FIle object
	Encoder encoder;							//Encoder object
	deque<char_A> char_deque;					//deque to store character
	deque<Three_mark> marks;					// Three mark object
	Marks2bin binbuffer;						//buffer to store binary code
	char_A char_A_temp;							//temporary character
	Three_mark mark_temp = Three_mark(0, 0, 0); //temp mark

public:
	/**
	 * @brief To do the encode process of lz77 method
	 * 
	 * @param input_file the file needs to be encoed
	 * @param o2file Decide write an putput file or not
	 * @param output_file The output file after encoding
	 * @return true 
	 * @return false 
	 */
	bool file_encode(string input_file, bool o2file = false, string output_file = "")
	{
		//Try to open the input file
		file.open(input_file, ios::in | ios::binary);
		if (!file)
		{
			cout << "cannot open file:" << input_file << endl;
			return false;
		}
		while (file.read((char *)&char_A_temp, A))
		{
			char_deque.push_back(char_A_temp);
		}
		char_deque.push_back(file.gcount() == 0 ? 0 : (char_A_temp & ((1 << 8 * (A - 1)) - 1)) | char_A((uint64_t)file.gcount() << 8 * (A - 1))); //补齐字
		file.close();
		//LZ77 encodes and stores the output three marks in the markdeque deque
		for (uint64_t i = 0, j = uint64_t(char_deque.size()); i < j; i++)
		{
			if (encoder.encode(char_deque[i], &mark_temp))
			{
				marks.push_back(mark_temp);
			}
		}
		encoder.to_end(&mark_temp);
		marks.push_back(mark_temp);
		//If no need to write an output file
		if (o2file == false)
		{
			return true;
		}
		//The three marks is converted to binarycode and stored in a binbuffer
		binbuffer.marks2bin(marks);
		binbuffer.to_end();
		//write binbuffer into the output file
		file.open(output_file, ios::out | ios::binary);
		if (!file)
		{
			cout << "cannot open file:" << output_file << endl;
			return false;
		}
		for (uint64_t i = 0; i < binbuffer.size(); i++)
		{
			file.write((char *)&binbuffer[i], 4);
		}
		clear();
		file.close();
		return true;
	}
	/**
	 * @brief Clear all the containers
	 * 
	 */
	void clear()
	{
		encoder.clear();
		char_deque.clear();
		marks.clear();
		binbuffer.clear();
		mark_temp = Three_mark(0, 0, 0);
	}
};
/**
 * @brief Class to do the decode process of lz77 method
 * 
 */
class lz77_decode
{
private:
	fstream file;			   //The file boject
	Decoder decoder;		   //The decoder object
	deque<char_A> charout;	   //The deque to store output character
	deque<uint64_t> binbuffer; // The deque to store binary code
	Bin2marks marks;		   // Help to transform binary code into mark
	unsigned unsigned_temp;	   //Help to read the input file

public:
	/**
	 * @brief 
	 * 
	 * @param input_file The input file needs to be decoded
	 * @param o2file  Decide write the decode text into the output file or not
	 * @param output_file The output file after decoding
	 * @return true 
	 * @return false 
	 */
	bool file_decode(string input_file, bool o2file = false, string output_file = "")
	{
		//Read data from the file into binbuffer
		file.open(input_file, ios::in | ios::binary);
		if (!file)
		{
			cout << "cannot open file:" << input_file << endl;
			return false;
		}
		while (file.read((char *)&unsigned_temp, 4))
		{
			binbuffer.push_back(unsigned_temp);
		}
		file.close();
		//The binary data is converted into marks and stored in buffer
		marks.bin2marks(binbuffer);
		marks.to_end();
		//Decode the mark and store into the charout
		for (uint64_t i = 0; i < marks.size(); i++)
		{
			decoder.decode(&charout, &marks[i]);
		}
		decoder.to_end(&charout);
		//If not need to write the output file
		if (o2file == false)
		{
			return true;
		}
		//Write the output file
		file.open(output_file, ios::out | ios::binary);
		if (!file)
		{
			cout << "cannot open file:" << output_file << endl;
			return false;
		}
		bool thelast = true;
		if (charout.empty())
			thelast = false;
		for (uint64_t i = 0, j = charout.size() - thelast; i < j; i++)
		{
			file.write((char *)&charout[i], A);
		}
		if (thelast)
		{
			file.write((char *)&charout.back(), charout.back() >> 8 * (A - 1));
			clear();
		}
		file.close();
		return true;
	}
	//Clear all containers
	void clear()
	{
		decoder.clear();
		charout.clear();
		binbuffer.clear();
		marks.clear();
	}
};

/**
 * @brief To encode the input string by RLencode method
 * 
 * @param str input string
 * @return double the compression rate of RLencode method
 */
double RLencode(const string &str)
{
	uint64_t i = uint64_t(str.length());
	string letters;
	double compression_rate = 0;
	// Find all repeating substirng and represent them by "repeating times + substirng"
	for (uint64_t j = 0; j < i; ++j)
	{
		uint64_t count = 1;
		while (str[j] == str[j + 1])
		{
			count++;
			j++;
		}
		letters += to_string(count);
		letters.push_back(str[j]);
	}
	//Calculate compression rate
	compression_rate = double(letters.length()) / double(str.length());
	cout
		<< "The result of RLencode: " << letters << '\n';
	cout << "Compresstion Ratio of RLencode: " << compression_rate << "\n";
	return compression_rate;
}
/**
 * @brief Remove a substring in the input stirng, this code is copied from https://thispointer.com/how-to-remove-substrings-from-a-string-in-c/
 * 
 * @param mainStr The original stirng
 * @param toErase The substring needs to be removed
 */
void eraseSubStr(string &mainStr, const string &toErase)
{
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}