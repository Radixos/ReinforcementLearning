#pragma once

#include "Includes.h"

class AHC1D
{
private:
	double epsilon = 0.01f;	//percentage of actions which are chosen randomly
	double gamma = 0.9f;	//discount factor (The closer to 1 gamma is (e.g. winning the chess game), the more important it is for the agent
						//to maximize gaining rewards in bigger time perspective. The closer to 0 the gamma is (e.g. breaking the pawn),
						//the more important it is for the agent to gain reward immediately. )
	double mi_alfa = 0.5f;	//learning rate of mi function
	double v_alfa = 0.5f;	//learning rate of V function
	double mi_lambda = 0.9f;	//forgetting factor ("freshness factor") for mi function
	double v_lambda = 0.9f;	//forgetting factor ("freshness factor") for V function

	int const static actionsCount = 2;	//there is always only 2 actions that can be performed in 1D environment
	int const actions[actionsCount] = { 0, 1 };	//1D actions: 0 - left, 1 - right

	int* reinforcementTable;	//declaring the environment
	double** mi;	//policy function
	double** e_mi;	//eligibility values function for policy

	double* V;	//values function
	double* e_V;	//eligibility function for values

	int attempt = 1;	//attempt number (starting as 1 and incrementing by 1)
	int greedyActionsIndicies[actionsCount] = { 0 };

	int nextState;
	int currentState;
	int action;

	std::list <int> path;
	std::list <int> optimalPath;
public:
	/*
		@brief learning how to reach the target
		@param reinforcementTableCount size of the environment
		@param rewardPlace position of the reward
		@param startState place where the agent starts
		@param useAHCLambda a boolean to say whether to use AHC(lambda) or AHC(0)
		@return Console output of agent actions
	*/
	AHC1D(int reinforcementTableCount, int rewardPlace, int startState, bool useAHCLambda);
};