#include "../Include/AHC2D.h"
#include <iomanip>

using namespace std;

AHC2D::AHC2D(int startState, int maxRepetition, int maxEpisode, bool useAHCLambda)
{
	performSimulation = true;

	nextState = startState;

#pragma region Running conditions
	state2rc(nextState, true);

	if (m_environment[m_new_r][m_new_c] != 0)
	{
		cout << "Agent placed on terminal state or on an obstacle." << endl;
		performSimulation = false;
	}

	if (nextState >= reinforcementTableCount || nextState < 0)
	{
		cout << "Agent placed outside the environment." << endl;
		performSimulation = false;
	}
#pragma endregion Running conditions

#pragma region Creating steps 2D array
	steps2D = new int* [maxEpisode];
	for (int i = 0; i < maxEpisode; i++)
		steps2D[i] = new int[maxRepetition];
#pragma endregion Creating steps 2D array

	/* initialize random seed: */
	srand(time(NULL));

	for (int repetition = 0; repetition < maxRepetition; repetition++)
	{

#pragma region Creating "mi" array - policy function
		mi = new double* [reinforcementTableCount];	//policy function
		for (int i = 0; i < reinforcementTableCount; i++)
			mi[i] = new double[actionsCount];

		for (int i = 0; i < reinforcementTableCount; i++)
			for (int j = 0; j < actionsCount; j++)
				mi[i][j] = 0;
#pragma endregion Creating "mi" array - policy function

#pragma region Creating "e_mi" array - eligibility values function for policy
		e_mi = new double* [reinforcementTableCount];	//eligibility values function for policy
		for (int i = 0; i < reinforcementTableCount; i++)
			e_mi[i] = new double[actionsCount];

		for (int i = 0; i < reinforcementTableCount; i++)
			for (int j = 0; j < actionsCount; j++)
				e_mi[i][j] = 0;
#pragma endregion Creating "e_mi" array - eligibility values function for policy

#pragma region Creating "v" array - values function
		V = new double[reinforcementTableCount];	//values function
		for (int i = 0; i < reinforcementTableCount; i++)
			V[i] = { 0 };
#pragma endregion Creating "v" array - values function

#pragma region Creating "e_v" array - eligibility function for values
		e_V = new double[reinforcementTableCount];	//eligibility function for values
		for (int i = 0; i < reinforcementTableCount; i++)
			e_V[i] = { 0 };
#pragma endregion Creating "e_v" array - eligibility function for values

		for (int episode = 0; episode < maxEpisode; episode++)
		{
			nextState = startState;

			state2rc(nextState, true);

			while (performSimulation)
			{
				currentState = nextState;

				double max_mi = mi[currentState][0];

				for (int i = 1; i < actionsCount; i++)
					if (mi[currentState][i] > max_mi)
						max_mi = mi[currentState][i];

				int j = 0;

				for (int i = 0; i < actionsCount; i++)
					if (mi[currentState][i] == max_mi)
						greedyActionsIndicies[j++] = i;

				double randNumber = (rand() % 100) / 100.f;

				if ((rand() % 100) / 100.f > epsilon)
					action = greedyActionsIndicies[(int)floor(randNumber * j)];
				else
					action = actions[(int)floor(randNumber * actionsCount)];

#pragma region AHC(lambda)
				if (useAHCLambda)
					e_mi[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion AHC(lambda)

				nextState = getState(currentState, action);
				state2rc(nextState, true);

				actionsPath.push_back(action);
				statesPath.push_back(nextState);

				double reward = m_environment[m_new_r][m_new_c];

				double delta = reward + gamma * V[nextState] - V[currentState];

#pragma region AHC(0)
				if (!useAHCLambda)
				{
					mi[currentState][action] += mi_alfa * delta;	//AHC(0)
					V[currentState] += v_alfa * delta;
				}
#pragma endregion AHC(0)

#pragma region AHC(lambda)
				if (useAHCLambda)
				{
					for (int s = 0; s < reinforcementTableCount; s++)	//s - state
						for (int a = 0; a < actionsCount; a++)	//a - action
							mi[s][a] += mi_alfa * delta * e_mi[s][a];

					if (reward == 1)
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							for (int a = 0; a < actionsCount; a++)	//a - action
								e_mi[s][a] = 0.f;
					else
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							for (int a = 0; a < actionsCount; a++)	//a - action
								e_mi[s][a] *= gamma * mi_lambda;

					for (int s = 0; s < reinforcementTableCount; s++)	//s - state
						V[s] += v_alfa * delta * e_V[s];

					if (reward == 1)
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							e_V[s] = 0.f;
					else
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							e_V[s] *= gamma * v_lambda;
				}
#pragma endregion AHC(lambda)

				if (reward == 1)
				{
					cout << "Repetition: " << repetition + 1 << " ";
					cout << "Episode: " << episode + 1 << endl;

					/*cout << "Performed Actions: " << endl;
					for (auto a : actionsPath)
					{
						cout << a;
					}
					cout << endl;

					cout << "Visited States: " << endl;
					for (auto a : statesPath)
					{
						cout << a << ", ";
					}
					cout << endl;*/
					cout << endl;

					steps2D[episode][repetition] = statesPath.size();

					actionsPath.clear();
					statesPath.clear();
					nextState = startState;

					break;
				}
			}
		}

		delete[] mi;
		delete[] e_mi;
		delete[] V;
		delete[] e_V;
	}

	DisplayInfo(maxEpisode, maxRepetition);

	delete[] steps2D;
}

void AHC2D::DisplayInfo(int maxEpisode, int maxRepetition)
{
	list <double> avgEpisode;
	double minEpisode;
	int minEpisodeInd;

	for (int episode = 0; episode < maxEpisode; episode++)
	{
		double tempEpisode = 0;

		for (int repetition = 0; repetition < maxRepetition; repetition++)
			tempEpisode += steps2D[episode][repetition];

		tempEpisode /= maxRepetition;

		if (episode == 0)
		{
			minEpisode = tempEpisode;
			minEpisodeInd = episode;
		}

		if (tempEpisode < minEpisode)
		{
			minEpisode = tempEpisode;
			minEpisodeInd = episode;
		}

		avgEpisode.push_back(tempEpisode);
	}

	cout << "Avg for all episodes: " << endl;
	for (auto a : avgEpisode)
		cout << setprecision(2) << fixed << a << ", ";

	cout << endl << endl;

	cout << "minEpisode: " << minEpisode << " at episode: " << minEpisodeInd << endl;
	//cout << "minEpisode: {0} at episode: {1}", minEpisode, minEpisodeInd;
}

int AHC2D::rc2state(int r, int c)
{
	m_state = r * m_environmentY + c;

	return m_state;
}

void AHC2D::state2rc(int state, bool ifNextState)
{
	if (ifNextState)
	{
		m_new_r = floor(state / m_environmentY);
		m_new_c = state % m_environmentY;
	}
	else
	{
		m_r = floor(state / m_environmentY);
		m_c = state % m_environmentY;
	}

}

int AHC2D::getState(int currentState, int action)
{
	state2rc(currentState, false);

	switch (action)
	{
	case 0:
		m_new_r = m_r;
		m_new_c = m_c - 1;
		break;
	case 1:
		m_new_r = m_r;
		m_new_c = m_c + 1;
		break;
	case 2:
		m_new_r = m_r - 1;
		m_new_c = m_c;
		break;
	case 3:
		m_new_r = m_r + 1;
		m_new_c = m_c;
		break;
	}

	if (!(m_new_r >= 0 && m_new_r < m_environmentX && m_new_c >= 0 &&
		m_new_c < m_environmentY) || m_environment[m_new_r][m_new_c] == -1)
	{
		m_new_r = m_r;
		m_new_c = m_c;
	}

	return m_nextState = rc2state(m_new_r, m_new_c);
}