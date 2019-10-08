#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include <time.h>

using namespace std;

class Transmitter {
	public:	
	Transmitter() {}
	Transmitter(int id, int transmissionFrequency, int transmissionLength) {
		this->id = id;
		this->transmissionFrequency = transmissionFrequency;
		this->transmissionLength = transmissionLength;
	}

	void generateRandomTransmission() {
		if ((rand() % 100) < transmissionFrequency)
			transmissions.push(transmissionLength);
	}
	bool Transmit() {
		if (!hasTransmission())
			return false;		

		transmissions.front()--;

		if (transmissions.front() == 0) 
			transmissions.pop();

		return true;
	}

	bool hasTransmission() {
		return (transmissions.size() > 0);
	}

	int getWaitTime() {
		return wait_timer;
	}

	void decrementWaitTime() {
		wait_timer--;
	}

	int getId() {
		return id;
	}

	void setLine(void* line) {
		this->ptrline = line;
	}

	virtual void setWaitTime() = 0;

	protected:
	int id;
	queue<int> transmissions;
	int wait_timer = 0;
	int transmissionFrequency = 0;
	int transmissionLength;
	void* ptrline;
};

template <class T> 
class Line {
	public:
	double calculateThroughput(int numTransmitters, int numFrames, int frameFrequency, int transmissionLength, int end = -1) {
		this->numFrames = numFrames;

	//	transmitters.clear();

		//create transmitters
		for (int i = 0; i < numTransmitters; i++) {
			T t(i, frameFrequency, transmissionLength);
			t.setLine(this);
			transmitters.push_back(t);
		}

		int maxTransmitters = 0;
		if (end == -1) {
			maxTransmitters = transmitters.size();
		}
		else {
			maxTransmitters = end;
		}

		//go through every frame over timeframe
		for (int frame = 0; frame < numFrames; frame++) {
			vector<T*> attempted;

			//check each transmitter
			for (int i = 0; i < maxTransmitters; i++) {
				//alias for transmitters[i]
				T& t = transmitters[i];

				t.generateRandomTransmission();

				if (t.getWaitTime() == 0 && t.hasTransmission() && (!inUse || t.getId() == talkerId)) {
					attempted.push_back(&t);
				}

				if (t.getWaitTime() > 0) {
					t.decrementWaitTime();
				}
			}

			if (attempted.size()) {
				//only one transmitter attemped to send, so send the message
				if (attempted.size() == 1) {
					inUse = true;
					talkerId = attempted[0]->getId();
					if (attempted[0]->Transmit()) {
						numTransmissions++;
						if (!attempted[0]->hasTransmission()) {
							inUse = false;
							talkerId = -1;
						}
					}
					else {
						inUse = false;
						talkerId = -1;
					}
				}					
				else {
					numCollisions++;
					talkerId = -1;
					inUse = false;

					for (int i = 0; i < attempted.size(); i++) {
						attempted[i]->setWaitTime();
					}
				}
			}
		}

		//calculate and return throughput
		return (double)numTransmissions / (double)numFrames * 100;
	}

	int getCollisions() {
		return numCollisions;
	}

	int getNumTransmissions() {
		return numTransmissions;
	}

	int getNumFrames() {
		return numFrames;
	}

	int getNumTransmitters() {
		return transmitters.size();
	}

	void reset() {
		talkerId = -1;
		inUse = false;
		numCollisions = 0;
		numTransmissions = 0;
		numFrames = 0;
		transmitters.clear();
	}

	private:
	int talkerId = -1;
	bool inUse = false;
	int numCollisions = 0;
	int numTransmissions = 0;
	int numFrames = 0;
	vector<T> transmitters;
};

class NonPersistentTransmitter: public Transmitter {
	public:
	NonPersistentTransmitter() {}
	NonPersistentTransmitter(int id, int transmissionFrequency, int transmissionLength) {
		this->id = id;
		this->transmissionFrequency = transmissionFrequency;
		this->transmissionLength = transmissionLength;
	}
	
	void setWaitTime() {
		Line<NonPersistentTransmitter>* line = ((Line<NonPersistentTransmitter>*)ptrline);

		int max = 100 - (int)((double)line->getNumTransmissions() / (double)line->getNumFrames() * 100);
		int min = max / 2;

	//	min = 1;
	//	max = 5;

		wait_timer = min + (rand() % (int)(max - min + 1));
	}
};

class OnePersistentBEBTransmitter: public Transmitter {
	public:
	OnePersistentBEBTransmitter() {}
	OnePersistentBEBTransmitter(int id, int transmissionFrequency, int transmissionLength) {
		this->id = id;
		this->transmissionFrequency = transmissionFrequency;
		this->transmissionLength = transmissionLength;
	}

	void setWaitTime() {
		if (collisionCount == 0) {	
			int min = 2;
			int max = 4;

			baseWaitTime = min + (rand() % (int)(max - min + 1));
			wait_timer = baseWaitTime;
		}
		else if (collisionCount <= 10) {
			int min = 0;
			int max = baseWaitTime;

			int variance = min + (rand() % (int)(max - min + 1));

			baseWaitTime *= 2;
			wait_timer = baseWaitTime + variance;
		}

		if (collisionCount > 16) {
			cout << "error" << endl;
			Transmit();
		}

		collisionCount++;
	}

	bool Transmit() {
		if (!hasTransmission())
			return false;

		transmissions.front()--;

		if (transmissions.front() == 0)
			transmissions.pop();

		collisionCount = 0;

		return true;
	}

	private:
	int collisionCount = 0;
	int baseWaitTime = 0;
};

int main() {
	srand(clock());

	int numFrames = 86400;
	int frameFrequency = 20;
	int transmissionLength = 1;

	ofstream out;
	out.open("output.txt");

	Line<NonPersistentTransmitter> nonPersistentCSMA;
	Line<OnePersistentBEBTransmitter> onePersistentBEBCSMA;
	for (int numTransmitters = 1; numTransmitters <= 30; numTransmitters++) {
		cout << numTransmitters << endl;

		double throughput = nonPersistentCSMA.calculateThroughput(numTransmitters, numFrames, frameFrequency, 1);
		out << numTransmitters << "\t" << throughput << "\t" << nonPersistentCSMA.getCollisions() << "\t\t";

		throughput = onePersistentBEBCSMA.calculateThroughput(numTransmitters, numFrames, frameFrequency, transmissionLength);
		out << numTransmitters << "\t" << throughput << "\t" << onePersistentBEBCSMA.getCollisions() << '\n';

		nonPersistentCSMA.reset();
		onePersistentBEBCSMA.reset();
	}

	out.close();








	vector<int> v;
	int max = 0;
	for (int i = 0; i < 100; i += 10) {
		int val = (int)(6 * sin(3.14159265 / 50 * i) + 7);
		v.push_back(val);
		max = val > max ? val : max;
	}
	Line<NonPersistentTransmitter> np;
	Line<OnePersistentBEBTransmitter> op;

	cout << max << '\n';

	out.open("sim.txt");
	for (unsigned int i = 0; i < v.size(); i++) {
		cout << i << '\n';
		out << i << "\t" << v[i] << "\t\t";


		double throughput = np.calculateThroughput(max, numFrames, frameFrequency, transmissionLength, v[i]);
		out << i << "\t" << throughput << "\t" << np.getCollisions() << "\t\t";




		throughput = op.calculateThroughput(max, numFrames, frameFrequency, transmissionLength, v[i]);
		out << i << "\t" << throughput << "\t" << op.getCollisions() << '\n';
	}
	out.close();

	return 0;
}

