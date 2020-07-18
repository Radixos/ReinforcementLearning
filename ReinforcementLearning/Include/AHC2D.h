#pragma once

#include "Includes.h"

class AHC2D
{
private:
	/*int const static m_environmentX = 6;
	int const static m_environmentY = 6;

	double m_environment[m_environmentX][m_environmentY] = {
		{0,-1, 0, 0, 0, 0},
		{0,-1, 0,-1,-1, 0},
		{0,-1, 0,-1, 0, 0},
		{0,-1, 0,-1, 0,-1},
		{0,-1, 0,-1, 0, 0},
		{0, 0, 0,-1, 0, 1}
	};*/

	int const static m_environmentX = 14;
	int const static m_environmentY = 14;

	double m_environment[m_environmentX][m_environmentY] = {
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,   0,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{-1,  -1,  -1,   0,  -1,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,  -1,   0,  -1,  -1,  -1},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,  -1,   0,   0,   0,   0,  -1,   0,   0,   0,   0},
		{ 0,   0,   0,   0,   0,   0,   0,   0,   0,  -1,   0,   0,   0,   1}
	};

	int m_state, m_nextState, m_r, m_c, m_new_r, m_new_c;

	int reinforcementTableCount = m_environmentX * m_environmentY;

	bool performSimulation = true;

	double epsilon = 0.1f;	//percentage of actions which are chosen randomly
	double gamma = 0.9f;	//discount factor (The closer to 1 gamma is (e.g. winning the chess game), the more important it is for the agent
						//to maximize gaining rewards in bigger time perspective. The closer to 0 the gamma is (e.g. breaking the pawn),
						//the more important it is for the agent to gain reward immediately. )
	double mi_alfa = 0.1f;	//learning rate of mi function
	double v_alfa = 0.1f;	//learning rate of V function
	double mi_lambda = 0.2f;	//forgetting factor ("freshness factor") for mi function
	double v_lambda = 0.2f;	//forgetting factor ("freshness factor") for V function

	int const static actionsCount = 4;	//in 2D environment there will always be 4 actions to perform, was: "sizeof(actions) / sizeof(actions[0])"
	int const actions[actionsCount] = { 0, 1, 2, 3 };	//2D actions: 0 - left, 1 - right, 2 - up, 3 - down

	double** mi;	//policy function
	double** e_mi;	//eligibility values function for policy

	double* V;	//values function
	double* e_V;	//eligibility function for values
	
	int** steps2D;	//steps 2D array

	int greedyActionsIndicies[actionsCount] = { 0 };

	int nextState;
	int currentState;
	int action;

	std::list <int> actionsPath;	//list of actions
	std::list <int> statesPath;	//list of visited states
public:
	/*
		@brief learning how to reach the target
		@param startState place where the agent starts
		@param maxRepetition number of repetitions
		@param maxEpisode number of episodes
		@param useAHCLambda a boolean to say whether to use AHC(lambda) or AHC(0)
		@return Console output of agent actions
	*/
	AHC2D(int startState, int maxRepetition, int maxEpisode, bool useAHCLambda);

	/*
		@brief displaying information about avg number of steps per episode and the best result
		@param maxEpisode maximum episode number given to the main function
		@param maxRepetition maximum repetition number given to the main function
		@return Console output of avg number of steps per episode and the best result
	*/
	void DisplayInfo(int maxEpisode, int maxRepetition);

	/*
		r - row
		c - column
	*/

	/*
		@brief function allowing conversion from a row and a column of the environment to the integer number
		@param r row
		@param c column
		@return int
	*/
	int rc2state(int r, int c);

	/*
		@brief function allowing conversion from an integer number to row and column of the environment
		@param state state of the agent (use: nextState - next state of the agent, currentState - current state of the agent)
		@param ifNextState if set to be true, function will update m_new_r and m_new_c. Otherwise, it will update m_r and m_c.
		@return int
	*/
	void state2rc(int state, bool ifNextState);

	/*
		@brief function that returns the m_nextState
		@param currentState current state of the agent (use: currentState)
		@param action action that can be performed (use: action)
		@return int
	*/
	int getState(int currentState, int action);
};