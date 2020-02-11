#include "ai.h"	

int ai::UCT(OthelloState root_state, clock_t start_time, int sec) {	
    ai::Node root_node = ai::Node(root_state);	
    int iter_cnt = 0;	
    while((clock() - start_time)/ (double) CLOCKS_PER_SEC < sec) {	
        int itermax = 100;	
        while((clock() - start_time)/ (double) CLOCKS_PER_SEC < sec) {	
            iter_cnt++;	
            OthelloState state = root_state;	
            ai::Node* node = &root_node;	
            // cout << "select" << endl;	
            // Select	
            while (node->untried_moves.size() == 0 && node->children.size() != 0) { //node is fully expanded and non-terminal	
                node = node->SelectChild();	
                state = node->state;	
            }	
            // cout << "expand" << endl;	
            // Expand	
            if (node->untried_moves.size() > 0) {	
                int m = *(next(std::begin(node->untried_moves), rand()%(node->untried_moves.size())));	
                state.DoMove(m);	
                node = node->AddChild(m, state);	
            }	
            // cout << "rollout" << endl;	
            // Rollout	
            set<int> moves;	

            while(!state.is_end()) {	
                state.GetAllMoves(moves);	
                if (moves.size() > 0) {	
                    int x = rand()%moves.size();	
                    int m = *(next(std::begin(moves), x));	
                    state.DoMove(m);	
                }	
                else {	
                    state.last_player = !state.last_player;	
                }	
            }	

            // cout << "Update" << endl;	
            // Update	
            double ret = GetResult(state);	
            while (node != NULL) {	
                node->update(ret);	
                node = node->parent;	
            }	
        }	
    }	
    cout << "search times: " << iter_cnt << endl;	
    ai::Node* ret = root_node.children[0];	
    for(auto x:root_node.children) {	
        cout << "score: " << x->move/8 << ' ' << x->move%8 << ' ' << x->wins << ' ' << x->visits << ' '<< double(x->wins)/x->visits << endl;	
        if (x->wins/x->visits > ret->wins/ret->visits) {	
            ret = x;	
        }	

    }	
    return ret->move;	
}	
int ai::step(OthelloState state, clock_t start_time, int sec) {	
    set<int> moves;	
    state.GetAllMoves(moves);	
    if(moves.size() == 0) {	
        return -1;	
    }	
    else{	
        int m = ai::UCT(state, start_time, sec);	
        return m;	
    }	
}	

double ai::GetResult(OthelloState state) {	
    long long emp = state.board_empty;	
    long long ply = state.board;	
    int a = 0, b = 0;	
    for(int i = 0; i < state.sz * state.sz; i++) {	
        if (emp & 1) {	
            if(ply & 1) a++;	
            else b++;	
        }	
        emp >>= 1;	
        ply >>= 1;	
    }	
    if(a > b) return 0;	
    if(b > a) return 1;	
    return 0.5;	
}