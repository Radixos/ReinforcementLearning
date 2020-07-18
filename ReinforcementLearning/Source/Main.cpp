#include "../Include/QLearning1D.h"
#include "../Include/QLearning1D2Failure.h"
#include "../Include/QLearning2D.h"
#include "../Include/Sarsa1D.h"
#include "../Include/Sarsa1D2Failure.h"
#include "../Include/Sarsa2D.h"
#include "../Include/AHC1D.h"
#include "../Include/AHC1D2Failure.h"
#include "../Include/AHC2D.h"

using namespace std;

int main()
{
	//QLearning1D(10, 10, 5, true);
	//Sarsa1D(6, 6, 2, false);
	//AHC1D(6, 6, 2, false);

	//QLearning1D2Failure(6, 2, true);
	//Sarsa1D2Failure(6, 2, true);
	//AHC1D2Failure(6, 2, true);

	//QLearning2D(0, 20, 50, true);
	//Sarsa2D(0, 20, 50, true);
	AHC2D(0, 20, 50, true);

	system("pause");
}