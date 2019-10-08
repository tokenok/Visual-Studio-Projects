#include <iostream>
#include <vector>
#include <string>
#include "curlingStatistics.h"

using namespace std;

CurlingStatistics::CurlingStatistics(const string name, const int score, const int count)
	: curlerName(name), totalScore(score), scoresCount(count) {
}

// Getters
string CurlingStatistics::getCurlerName() const {
	return curlerName;
}
int CurlingStatistics::getScore() const {
	return totalScore;
}
int CurlingStatistics::getScoresCount() const {
	return scoresCount;
}

// Setters
void CurlingStatistics::setScore(int score) {
	totalScore = score;
}
void CurlingStatistics::setScoresCount(int count) {
	scoresCount = count;
}
void CurlingStatistics::incScoresCount(int num) {
	scoresCount = scoresCount + num;
}
double CurlingStatistics::calcAvg(){
	return static_cast<double>(totalScore) / (4 * scoresCount) * 100;
}