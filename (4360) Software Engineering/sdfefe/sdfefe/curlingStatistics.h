#ifndef CURLINGSTATISTICS_H
#define CURLINGSTATISTICS_H

#include <iostream>
#include <vector>
#include <string>

class CurlingStatistics
{
private:
	std::string curlerName;
	int totalScore;
	int scoresCount;

public:
	// CurlingStatistics Constructor
	CurlingStatistics(const std::string, const int, const int);

	// Getters
	std::string getCurlerName() const;
	int getScore() const;
	int getScoresCount() const;

	// Setters
	void setScore(int);
	void setScoresCount(int);
	void incScoresCount(int);

	double calcAvg();
};

#endif
