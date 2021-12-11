/*
 * day9.cpp
 *
 *  Created on: 11 Dec 2021
 *      Author: pgm
 */

/* find minima in 2D surface */

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <string.h>


struct Coord {
	int x;
	int y;
};


std::ostream& operator<< (std::ostream &out, Coord const& data) {
    out << "{ " << data.x << ", " << data.y << " }";
    return out;
}

std::ostream& operator<< (std::ostream &out, std::vector<Coord> const& data) {
	int ii = 0;
	for (Coord c: data){
		if (ii++) out << ",";
		out << c;
	}
    return out;
}

#define ANSI_BOLD_ON 	"\033[1m"
#define ANSI_BOLD_OFF	"\033[0m"

class Surface
{
	std::vector<int*>& rows;

	Surface(std::vector<int*>& _rows, const int _X, const int _Y):
		rows(_rows), X(_X), Y(_Y)
	{}

public:
	const int X;
	const int Y;

	int z(Coord cd) const {
		return (rows[cd.y])[cd.x];
	}
	std::vector<Coord> neighbours(Coord cd) const {
		std::vector<Coord> nb;
		if (cd.x > 0){
			nb.push_back({cd.x-1, cd.y});
		}
		if (cd.x < X-1){
			nb.push_back({cd.x+1, cd.y});
		}
		if (cd.y > 0){
			nb.push_back({cd.x, cd.y-1});
		}
		if (cd.y < Y-1){
			nb.push_back({cd.x, cd.y+1});
		}
		//std::cout << cd << "nb:" << nb << std::endl;
		return nb;
	}

	bool lowPoint(Coord cd) const {
		int zz = z(cd);
		for (Coord nb: neighbours(cd)){
			if (z(nb) <= zz){
				return false;
			}
		}
		return true;
	}

	void print() const {
		for (int yy = 0; yy < Y; ++yy){
			for (int xx = 0; xx < X; xx++){
				//std::cout << "xx:" << xx << " yy:" << yy << " z:" << z({xx, yy}) << std::endl;
				bool low = lowPoint({xx, yy});
				if (low){
					std::cout << ANSI_BOLD_ON;
				}
				std::cout << z({xx, yy});
				if (low){
					std::cout << ANSI_BOLD_OFF;
				}
			}
			std::cout << std::endl;
		}
	}

	int riskLevel() const {
		int risk_level = 0;
		for (int yy = 0; yy < Y; ++yy){
			for (int xx = 0; xx < X; ++xx){
				if (lowPoint({xx, yy})){
					risk_level += 1 + z({xx,yy});
				}
			}
		}
		return risk_level;
	}


	static Surface& factory(std::istream &in);
};


Surface& Surface::factory(std::istream &in){
	std::vector<int*>& rows = * new std::vector<int*>;

	std::string str;
	int XX = -1;

	while (std::getline(in, str)){
		if (XX == -1){
			XX = str.length();
		}else if (str.length() == 0){
			continue;
		}else if (XX != str.length()){
			std::cerr << "ERROR: row XX change from:" << XX << " to " << str.length() << " discard" << std::endl;
			continue;
		}

		int* row = new int[XX];
		for (int ii = 0; ii < XX; ++ii){
			row[ii] = str.c_str()[ii] - '0';
		}
		rows.push_back(row);
	}

	int YY = rows.size();
	Surface& instance = * new Surface(rows, XX, YY);
	return instance;
}

template <class T>
class Array2D {

	const int X;
	const int Y;
	T* data;
public:
	Array2D(int _x, int _y) :X(_x), Y(_y) {
		data = new T[X*Y];
		memset(data, 0, sizeof(T)*X*Y);
	}
	T& get(int x, int y){
		return data[y*X + x];
	}
	T& set(int x, int y, T z){
		return data[y*X + x] = z;
	}

	~Array2D() {
		delete [] data;
	}

	void print() {
		for (int yy = 0; yy < Y; ++yy){
			for(int xx = 0; xx < X; ++xx){
				std::cout << get(xx, yy)? '1': '0';
			}
			std::cout << std::endl;
		}
	}

	void print(Surface& surface){
		for (int yy = 0; yy < Y; ++yy){
			for(int xx = 0; xx < X; ++xx){
				if (get(xx, yy)){
					std::cout << ANSI_BOLD_ON;
				}
				std::cout << surface.z({xx, yy});
				if (get(xx, yy)){
					std::cout << ANSI_BOLD_OFF;
				}
			}
			std::cout << std::endl;
		}
	}
};

typedef Array2D<bool> Bool2D;

std::vector<Coord> explore_basin(Surface& surface, Bool2D& in_basin, std::vector<Coord>& basin, Coord this_point){
	for (Coord nb: surface.neighbours(this_point)){
		if (in_basin.get(nb.x, nb.y) || surface.z(nb) == 9){
			;
		}else{
			in_basin.set(nb.x, nb.y, true);
			basin.push_back(nb);
			explore_basin(surface, in_basin, basin, nb);
		}
	}
	return basin;
}



std::vector<std::vector<Coord>> find_basins(Surface& surface)
{
	Bool2D in_basin(surface.X, surface.Y);
	std::vector<std::vector<Coord>> basins = *new std::vector<std::vector<Coord>>;

	for (int yy = 0; yy < surface.Y; ++yy){
		for (int xx = 0; xx < surface.X; ++xx){
			if (in_basin.get(xx, yy) || surface.z({xx, yy}) == 9){
				continue;
			}else{
				std::vector<Coord> basin;
				basin.push_back({xx, yy});
				in_basin.set(xx, yy, true);
				basins.push_back(explore_basin(surface, in_basin, basin, {xx, yy}));
			}
		}
	}

	std::cout << "in_basin:" << std::endl; in_basin.print(surface);
	return basins;
}


int main(int argc, const char** argv)
{
	Surface& surface = Surface::factory(std::cin);

	std::cout << "created surface [" << surface.X << "][" << surface.Y << "]" << "riskLevel:" << surface.riskLevel() <<std::endl;
	surface.print();
	std::vector<std::vector<Coord>> basins = find_basins(surface);
	std::cout << "number of basins:" <<basins.size() << std::endl;

	for (int ii = 0; ii < basins.size(); ++ii){
		std::cout << "basin:" << ii << " :" << basins[ii] << std::endl;
	}
	while(basins.size() > 3){
		int imin;
		int min_count = 9999;
		for (int ii = 0; ii < basins.size(); ++ii){
			if (basins[ii].size() < min_count){
				min_count = basins[ii].size();
				imin = ii;
			}
		}
		basins.erase(basins.begin()+imin);
	}
	int top3_product = 1;
	for (int ii = 0; ii < basins.size(); ++ii){
		std::cout << "basin:" << ii << " :" << basins[ii] << std::endl;
		top3_product *= basins[ii].size();
	}
	std::cout << "Top3 product:"  << top3_product << std::endl;

}

