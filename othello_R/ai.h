#ifndef AI_H
#define AI_H
#include<cstdio>
#include<iostream>
#include<algorithm>
#include<set>
#include<vector>
#include<ctime>
#include "OthelloState.h"
using namespace std;
namespace ai
{

class Node {
public:
    vector<Node*> children;
    Node* parent;
    OthelloState state;
    double wins = 0;
    int visits = 0;
    double ucb_coe = 0.5;
    set<int> untried_moves;
    int move;
    Node(OthelloState _state, int _move = -1, Node* parent_node=NULL) {
        parent = parent_node;
        state = _state;
        move = _move;
        state.GetAllMoves(untried_moves);
    }

    inline double UCB(Node *a) {
        return a->wins / a->visits + ucb_coe * sqrt(2 * log(visits)/a->visits);
    }
    Node* SelectChild() {
        Node* ret = children[0];
        for(auto c: children) {
            if (UCB(ret) < UCB(c)) {
                ret = c;
            }
        }
        return ret;
    }

    Node* AddChild(int m, OthelloState _state) {
        Node *n = new Node(_state, m, this);
        children.push_back(n);
        untried_moves.erase(m);
        return n;
    }

    void update(int result) {
        visits += 1;
        if(state.last_player)
            wins += result;
        else
            wins += (1-result);
    }

};


int UCT(OthelloState , clock_t, int);
int step(OthelloState, clock_t, int);
double GetResult(OthelloState);

}
#endif // AI_H
