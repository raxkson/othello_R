#ifndef OTHELLOSTATE_H
#define OTHELLOSTATE_H
#include<cstdio>
#include<iostream>
#include<algorithm>
#include<vector>
#include<set>
using namespace std;
#define ULL unsigned long long
#define PULL pair<ULL, ULL>


class OthelloState {
public:
    ULL board_empty = 0;
    ULL board = 0;
    int sz;
    bool last_player = false;
    // last_player = false, next is black
    // last_player = true,  next is white
    inline ULL pos(int &x, int &y) {
        return (1ULL << ((ULL)x*sz+y));
    }
    inline ULL pos(int &xy) {
        return (1ULL << ((ULL)xy));
    }
    inline void setbit(int x, int y, bool k) {
        // cout << x << ' ' << y << ' '<< k << endl;
        board_empty |= pos(x, y);
        if (k)
            board |= pos(x,y);
        else
            board &= ~pos(x,y);
    }
    inline void setbit(int xy, bool k) {
        // cout << x << ' ' << y << ' '<< k << endl;
        board_empty |= pos(xy);
        if (k)
            board |= pos(xy);
        else
            board &= ~pos(xy);
    }
    inline int getbit(int x, int y) {
        return board_empty & pos(x,y) ? (board & pos(x,y))>0 : -1;
    }
    inline int getbit(int xy) {
        return board_empty & pos(xy) ? (board & pos(xy))>0 : -1;
    }
    void ShowBoard() {
        set<int> xy;
        GetAllMoves(xy);
        // system("clear");
        if(last_player) printf("White next:\n");
        if(!last_player) printf("Black next:\n");
        for(int i = 0; i < sz; i++) {
            for(int j = 0; j < sz; j++) {
                int k = getbit(i*sz+j);
                // if (k == -1 && xy.count(i*sz+j)) printf("P ");
                // else if (k != -1 && xy.count(i*sz+j)) printf("X ");
                if (k == -1) printf("+ ");
                else if (k == 0) printf("○ ");
                else if (k == 1) printf("● ");

            }
            printf(" %d\n",i);
        }

        printf("\n");
        for(int i = 0; i < sz; i++)
            printf("%d ",i);
        printf("\n\n");
    }
    OthelloState(int size=8, ULL emp=0, ULL ply=0, bool last = false) {
        sz = size;
        if(emp > 0) {
            board_empty = emp;
            board = ply;
            last_player = last;
        }
        else {
            setbit(sz/2, sz/2, false);
            setbit(sz/2-1, sz/2-1, false);
            setbit(sz/2, sz/2-1, true);
            setbit(sz/2-1, sz/2, true);
        }
    }


    ULL flip_vertical(ULL x){
        ULL k1 = 0x00FF00FF00FF00FFULL;
        ULL k2 = 0x0000FFFF0000FFFFULL;
        x = ((x >> 8) & k1) | ((x & k1) << 8);
        x = ((x >> 16) & k2) | ((x & k2) << 16);
        x = (x >> 32) | (x << 32);
        return x;
    }

    ULL flip_diag_a1h8(ULL x) {
        ULL k1 = 0x5500550055005500ULL;
        ULL k2 = 0x3333000033330000ULL;
        ULL k4 = 0x0f0f0f0f00000000ULL;
        ULL t = k4 & (x ^ (x << 28));
        x ^= t ^ (t >> 28);
        t = k2 & (x ^ (x << 14));
        x ^= t ^ (t >> 14);
        t = k1 & (x ^ (x << 7));
        x ^= t ^ (t >> 7);
        return x;
    }


    ULL rotate90(ULL x) {
        return flip_diag_a1h8(flip_vertical(x));
    }


    ULL rotate180(ULL x) {
        return rotate90(rotate90(x));
    }

    ULL _calc_flip_half(int pos, ULL own, ULL enemy) {
        ULL el[4] = {enemy, enemy & 0x7e7e7e7e7e7e7e7eULL, enemy & 0x7e7e7e7e7e7e7e7eULL, enemy & 0x7e7e7e7e7e7e7e7eULL};
        ULL masks[4] = {0x0101010101010100ULL, 0x00000000000000feULL, 0x0002040810204080ULL, 0x8040201008040200ULL};
        for(int i = 0; i < 4; i++)
            masks[i] = masks[i] << pos;
        ULL flipped = 0LL;
        for(int i = 0; i < 4; i++) {
            ULL outflank = masks[i] & ((el[i] | ~masks[i]) + 1) & own;
            flipped |= (outflank - (outflank != 0)) & masks[i];
        }
        return flipped;
    }

    ULL calc_flip(int pos) {
        ULL own = board_empty & board;
        ULL enemy = board_empty & (~board);
        if (last_player) swap(own, enemy);
       ///return flip stones of enemy by bitboard when I place stone at pos.
        ULL f1 = _calc_flip_half(pos, own, enemy);
        ULL f2 = _calc_flip_half(63 - pos, rotate180(own), rotate180(enemy));
        return f1 | rotate180(f2);
    }

    ULL search_offset_left(ULL &own, ULL &enemy, ULL &mask, ULL offset) {
        ULL e = enemy & mask;
        ULL blank = ~(own | enemy);
        ULL t = e & (own >> offset);
        t |= e & (t >> offset);
        t |= e & (t >> offset);
        t |= e & (t >> offset);
        t |= e & (t >> offset);
        t |= e & (t >> offset);  // Up to six stones can be turned at once
        return blank & (t >> offset);  // Only the blank squares can be started
    }

    ULL search_offset_right(ULL &own, ULL &enemy, ULL &mask, ULL offset) {
        ULL e = enemy & mask;
        ULL blank = ~(own | enemy);
        ULL t = e & (own << offset);
        t |= e & (t << offset);
        t |= e & (t << offset);
        t |= e & (t << offset);
        t |= e & (t << offset);
        t |= e & (t << offset);  // Up to six stones can be turned at once
        return blank & (t << offset);  // Only the blank squares can be started
    }
    ULL get_valid_move_bit(){
        //return legal moves
        ULL left_right_mask = 0x7e7e7e7e7e7e7e7eULL;  // Both most left-right edge are 0, else 1
        ULL top_bottom_mask = 0x00ffffffffffff00ULL;  // Both most top-bottom edge are 0, else 1
        ULL mask = left_right_mask & top_bottom_mask;
        ULL mobility = 0ULL;
        ULL own = board_empty & board;
        ULL enemy = board_empty & (~board);
        if (last_player) swap(own, enemy);
        mobility |= search_offset_left(own, enemy, left_right_mask, 1ULL);  // Left
        mobility |= search_offset_left(own, enemy, mask, 9ULL);  // Left Top
        mobility |= search_offset_left(own, enemy, top_bottom_mask, 8ULL);  // Top
        mobility |= search_offset_left(own, enemy, mask, 7ULL);  // Top Right
        mobility |= search_offset_right(own, enemy, left_right_mask, 1ULL);  // Right
        mobility |= search_offset_right(own, enemy, mask, 9ULL);  // Bottom Right
        mobility |= search_offset_right(own, enemy, top_bottom_mask, 8ULL);  // Bottom
        mobility |= search_offset_right(own, enemy, mask, 7ULL);  // Left bottom
        return mobility;
    }
    void GetAllMoves(set<int> &xy) {
        ULL mobility = get_valid_move_bit();

        xy.clear();
        for(int i = 0; i < sz; i++) {
            for(int j = 0; j < sz; j++) {
                if (mobility & pos(i, j)) {
                    xy.insert(i*sz+j);
                }
            }
        }
    }
    bool is_end() {
        set<int> xy;
        GetAllMoves(xy);
        if(xy.size() > 0) return false;
        bool ret = true;
        last_player = !last_player;
        GetAllMoves(xy);
        if(xy.size() > 0) ret = false;
        last_player = !last_player;
        return ret;
    }
    void printbit(ULL b) {
        for(int i = 0; i < sz; i++){
            for(int j = 0; j < sz; j++)
                printf("%d ",(b & pos(i,j)) != 0);
            printf("\n");
        }
        printf("\n");

    }
    int bit_count(ULL x)
    {
        x = x - ((x >> 1) & 0x5555555555555555UL);
        x = (x & 0x3333333333333333UL) + ((x >> 2) & 0x3333333333333333UL);
        return (int)((((x + (x >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
    }
    bool DoMove(int xy) {

        if ((board_empty & pos(xy)) != 0) {
            return false;
        }
        int cnt = 0;

        ULL flip_mat = calc_flip(xy);
        board ^= flip_mat;

        if (bit_count(flip_mat) == 0)
            return false;

        last_player = !last_player;

        // setbit(x, y, last_player);
        setbit(xy, last_player);

        return true;
    }
    void init(){
        board = 0;
        board_empty = 0;
    }

    void skip() {
        last_player = !last_player;
    }
    //TODO: can be accelerated and make better precision


};

#endif
