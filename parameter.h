/**
 * @file parameter.h
 * @author Chenge Liu (liu1217@mcmaster.ca)
 * @brief Contains all self-defined variables that are using in the classes.h
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#define MAX_ENCODE 1000
#define LogN 8
#define LogL 3
using namespace std;
typedef unsigned char char_A;
const unsigned A = sizeof(char_A);
const unsigned N = ((unsigned)1 << LogN) - 1;
const unsigned L = 1 << LogL;
const unsigned int end_number = 1;
const unsigned int charactereof = '#';
