/*
 * Author: Ryan Galloway
 *
 * Class: CS5375 Multimedia Programming
 *
 * Assignment 1:
 *
 * Program to generate a quadtree based image of a
 * pgma file when considering a certain variance threshold
 * between 0 and 1024.
 */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>

const int H = 512;
const int W = 512;

//pull the values of the pgma from the file
void readPGM(int imageArr[H][W], int &h, int &w)
{
    char c;
    int scale;
    std::ifstream infile;
    infile.open("baboon.pgma");

    //read PGM header
    infile >> c;
    assert(c == 'P');
    infile >> c;
    assert(c == '2');

    //skip pgm comments
    while((infile >> std::ws).peek() == '#')
    {
        infile.ignore(4096, '\n');
    }
    infile >> w;
    infile >> h;

    infile >> scale;
    assert(scale == 255);

    //Copy ASCII values form the .pgma to a 2d array
    for(int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++) {
            infile >> imageArr[i][j];
        }
    }
    infile.close();
}


//Process the image on the given threshold
void quadrantCheck(int imageArr[H][W], int h, int w, int x, int y, int threshold)
{
    int i, j;
    int n = h * w;
    int meanSum = 0;
    int average;
    double varianceSum = 0;
    double variance;

    //if quadrant is single pixel return
    if(h == 1 && w == 1)
    {
        return;
    }

    //Calculate the average of the pixels in the quadrant
    for(i = x ; i < x + w; i++)
    {
        for(j = y; j < y + h; j++)
        {
            meanSum += imageArr[i][j];
        }
    }
    average = meanSum/(n);


    //calculate the variance
    for(i = x ; i < x + w; i++)
    {
        for(j = y; j < y + h; j++)
        {
            varianceSum += ((imageArr[i][j] - average) * (imageArr[i][j] - average));
        }
    }
    variance = varianceSum / (n - 1);

    //If the variance is less than the variance threshold set all pixels in the
    //quadrant to the average.
    if (variance <= threshold)
    {
        for(i = x; i < x + w; i++)
            for(j = y; j < y + h; j++)
            {
                imageArr[i][j] = average;
            }
        return;
    }

    //variance exceeds threshold call recursive functions
    //to continue to split the image into quadrants
    else
    {
        quadrantCheck(imageArr, h/2, w/2, x, y, threshold);
        quadrantCheck(imageArr, h/2, w/2, x + w/2, y, threshold);
        quadrantCheck(imageArr, h/2, w/2, x, y + h/2, threshold);
        quadrantCheck(imageArr, h/2, w/2, x + w/2, y +h/2, threshold);
    }
}

void writePGM(int imageArr[H][W], int &h, int &w)
{
    std::ofstream outFile;
    outFile.open("ModifiedBaboon.pgma");
    if(outFile.fail())
    {
        std::cout << "Could not write to file.";
        exit(1);
    }
    //Write the P2 Header and comments
    outFile << "P2" << std::endl;
    outFile << "# modifiedBaboon.pgma using quadtree." << std::endl;
    outFile << w << ' ';
    outFile << h << std::endl;
    outFile << 255 << std::endl;

    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++)
        {
            assert(imageArr[i][j] <= 255);
            assert(imageArr[i][j] >= 0);
            outFile << imageArr[i][j] << ' ';
        }
    outFile.close();
}

int main(int argc, char* argv[])
{
    int imageArr[H][W];
    int h, w;
    int x = 0;
    int y = 0;
    int threshold;
    if(argc != 2)
    {
        std::cerr << "Usage: Please enter a valid threshold.";
        exit(-1);
    }
    threshold = std::stoi(argv[1]);
    if(threshold < 0 || threshold > 1024)
    {
        std::cerr << "Variance threshold must be between 0 and 1024.";
        exit(-1);
    }

    h = H;
    w = W;

    readPGM(imageArr, h, w);
    quadrantCheck(imageArr, h, w, x, y, threshold);
    writePGM(imageArr, h, w);
    return 0;
}
