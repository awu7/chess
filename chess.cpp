#include <bits/extc++.h>
using namespace std;

#pragma GCC target("avx2")

#define ll long long
#define all(x) x.begin(), x.end()
#define debug(x) do{auto __y = x; cerr << #x << " = " << __y << endl;} while(0)

#define unordered_map __gnu_pbds::gp_hash_table

const int inf = 1 << 28;

using i8 = int8_t;

struct Pieces {
  static const int P = 1, N = 2, B = 3, R = 4, Q = 5, K = 6;
};
constexpr int N = -10, S = 10, E = 1, W = -1;
array<vector<int>, 7> dirs;
void init_dirs() {
  dirs[Pieces::N] = {N + N + E, N + N + W, S + S + E, S + S + W, N + E + E, N + W + W, S + E + E, S + W + W};
  dirs[Pieces::B] = {N + E, N + W, S + E, S + W};
  dirs[Pieces::R] = {N, S, E, W};
  dirs[Pieces::Q] = {N, S, E, W, N + E, N + W, S + E, S + W};
  dirs[Pieces::K] = dirs[Pieces::Q];
}

array<array<int, 120>, 7> pst;
void process_pst(int p, vector<int> pst_condensed) {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      pst[p][21 + i * 10 + j] = pst_condensed[i * 8 + j];
    }
  }
}
void init_pst() {
  process_pst(Pieces::P, {0,   0,   0,   0,   0,   0,   0,   0,
                          200, 200, 200, 200, 200, 200, 200, 200,
                          160, 160, 170, 180, 180, 170, 160, 160,
                          90,  90,  140, 160, 160, 140, 90,  90,
                          90,  90,  125, 150, 150, 125, 90,  90,
                          100, 100, 100, 140, 140, 100, 100, 100,
                          100, 100, 100, 90,  90,  100, 100, 100,
                          0,   0,   0,   0,   0,   0,   0,   0});
  process_pst(Pieces::N, {180, 190, 200, 200, 200, 200, 190, 180,
                          190, 200, 300, 280, 280, 300, 200, 190,
                          200, 220, 320, 330, 330, 320, 220, 200,
                          220, 290, 320, 330, 330, 320, 290, 220,
                          220, 220, 300, 300, 300, 300, 220, 220,
                          200, 290, 300, 280, 280, 300, 290, 200,
                          190, 200, 260, 280, 280, 260, 200, 190,
                          180, 190, 200, 200, 200, 200, 190, 180});
  process_pst(Pieces::B, {300, 300, 300, 300, 300, 300, 300, 300,
                          300, 300, 300, 300, 300, 300, 300, 300,
                          300, 300, 300, 300, 300, 300, 300, 300,
                          300, 330, 300, 330, 330, 300, 330, 300,
                          300, 300, 330, 330, 330, 330, 300, 300,
                          300, 300, 300, 320, 320, 300, 300, 300,
                          300, 320, 300, 300, 300, 300, 320, 300,
                          300, 300, 280, 300, 300, 280, 300, 300});
  process_pst(Pieces::R, {510, 520, 550, 550, 550, 550, 520, 510,
                          640, 670, 680, 690, 690, 680, 670, 640,
                          500, 500, 530, 540, 540, 530, 500, 500,
                          470, 500, 530, 540, 540, 530, 500, 470,
                          450, 500, 530, 540, 540, 530, 500, 450,
                          450, 500, 530, 540, 540, 530, 500, 450,
                          440, 500, 530, 540, 540, 530, 500, 440,
                          500, 510, 530, 540, 540, 530, 510, 500});
  process_pst(Pieces::Q, {900, 900, 930, 940, 940, 930, 900, 900,
                          920, 930, 950, 960, 960, 950, 930, 920,
                          960, 970, 980, 990, 990, 980, 970, 960,
                          930, 960, 960, 990, 990, 960, 960, 930,
                          930, 950, 960, 980, 980, 960, 950, 930,
                          920, 930, 930, 980, 980, 930, 930, 920,
                          900, 920, 930, 930, 930, 930, 920, 900,
                          900, 900, 920, 950, 950, 920, 900, 900});
  process_pst(Pieces::K, {40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40000, 40000, 40000, 40000, 40000, 40000, 40000, 40000,
                          40140, 40130, 40000, 40000, 40000, 40000, 40130, 40140,
                          40130, 40150, 40070, 40050, 40050, 40070, 40150, 40130});
}

struct Move {
  int from, to;
  bool operator==(const Move& other) {
    return from == other.from && to == other.to;
  }
};

struct MoveDiff {
  vector<pair<int, i8>> sq;
  array<bool, 4> castling;
  int ep, kp, score;
};

struct Pos {
  array<i8, 120> board;
  array<bool, 4> castling;
  int ep, kp;
  int score;
  bool rot;
  i8 at(int p) {
    if(rot) return -board[119 - p];
    return board[p];
  }
  void set(int p, int x) {
    if(at(p) > 0) {
      score -= pst[at(p)][p];
    }
    if(at(p) < 0) {
      score += pst[-at(p)][119 - p];
    }
    if(rot) {
      board[119 - p] = -x;
    } else {
      board[p] = x;
    }
    if(at(p) > 0) {
      score += pst[at(p)][p];
    }
    if(at(p) < 0) {
      score -= pst[-at(p)][119 - p];
    }
  }
  vector<Move> moves() {
    vector<Move> res;
    for(int p = 21; p <= 98; p++) {
      if(at(p) <= 0) continue;
      if(at(p) == Pieces::P) {
        if(at(p + N) == 0) {
          res.push_back({p, p + N});
          if(p >= 81 && p <= 88 && at(p + N + N) == 0) {
            res.push_back({p, p + N + N});
          }
        }
        for(int d : {N + E, N + W}) {
          if(at(p + d) < 0 || p + d == ep) res.push_back({p, p + d});
        }
      } else {
        for(int d : dirs[at(p)]) {
          int q = p + d;
          while(at(q) == 0 && q % 10 != 0 && q % 10 != 9 && q >= 21 && q <= 98) {
            res.push_back({p, q});
            if(at(p) == Pieces::N || at(p) == Pieces::K) break;
            q += d;
          }
          if(at(q) * at(p) < 0) {
            res.push_back({p, q});
          }
          if(at(p) == Pieces::R && at(q) == Pieces::K) {
            if(p == 91 && castling[0]) res.push_back({q, q + W + W});
            else if(p == 98 && castling[1]) res.push_back({q, q + E + E});
          }
        }
      }
    }
    return res;
  }
  void rotate() {
    rot = !rot;
    reverse(all(castling));
    ep = 119 - ep;
    kp = 119 - kp;
    score = -score;
  }
  MoveDiff make_move(Move m) {
    MoveDiff res = {
      {
        {m.to, at(m.to)},
        {m.from, at(m.from)},
      },
      castling, ep, kp, score
    };
    set(m.to, at(m.from));
    set(m.from, 0);
    if(abs(m.to - kp) <= 1) {
      res.sq.push_back({kp, at(kp)});
      set(kp, 0);
      res.sq.push_back({kp + 1, at(kp + 1)});
      set(kp + 1, 0);
      res.sq.push_back({kp - 1, at(kp - 1)});
      set(kp - 1, 0);
    }
    kp = 0;
    if(at(m.to) == Pieces::K && abs(m.from - m.to) == 2) {
      res.sq.push_back({(m.from + m.to) / 2, 0});
      set((m.from + m.to) / 2, Pieces::R);
      res.sq.push_back({m.from < m.to ? 98 : 91, Pieces::R});
      set(m.from < m.to ? 98 : 91, 0);
      kp = (m.from + m.to) / 2;
    }
    if(at(m.to) == Pieces::P && m.to >= 21 && m.to <= 28) {
      set(m.to, Pieces::Q);
    }
    if(m.to == ep) {
      res.sq.push_back({ep + S, -Pieces::P});
      set(ep + S, 0);
    }
    if(at(m.to) == Pieces::P && m.from - m.to == 20) {
      ep = m.from + N;
    } else {
      ep = 0;
    }
    if(at(m.to) == Pieces::K) castling[0] = castling[1] = false;
    if(m.from == 91) castling[0] = false;
    if(m.from == 98) castling[1] = false;
    if(m.to == 28) castling[2] = false;
    if(m.to == 21) castling[3] = false;
    rotate();
    return res;
  }
  void undo_move(MoveDiff md) {
    rotate();
    for(auto x : md.sq) {
      set(x.first, x.second);
    }
    castling = md.castling;
    ep = md.ep;
    kp = md.kp;
    score = md.score;
  }
  bool operator==(const Pos& other) const {
    return board == other.board && castling == other.castling && ep == other.ep && kp == other.kp && rot == other.rot;
  }
};

void print_board(Pos pos, bool rot = false) {
  if(rot) pos.rotate();
  for(int i = 21; i <= 98; i++) {
    if(i % 10 == 9) {
      cerr << endl;
      i += 2;
    }
    if(pos.at(i) == 0) cerr << " .";
    if(pos.at(i) > 0) cerr << " " << "PNBRQK"[pos.at(i) - 1];
    if(pos.at(i) < 0) cerr << " " << "pnbrqk"[-pos.at(i) - 1];
  }
  cerr << endl;
}

string to_uci(Move m, bool rot = false) {
  if(rot) {
    m.from = 119 - m.from;
    m.to = 119 - m.to;
  }
  return string() + "abcdefgh"[m.from % 10 - 1] + "87654321"[m.from / 10 - 2] +
         "abcdefgh"[m.to % 10 - 1] + "87654321"[m.to / 10 - 2];
}

Move from_uci(string s) {
  return {21 + s[0] - 'a' + ('8' - s[1]) * 10, 21 + s[2] - 'a' + ('8' - s[3]) * 10};
}

Pos parse_fen(string fen) {
  Pos pos{};
  int p = 21;
  for(char c : fen) {
    if(c == ' ') {
      break;
    }
    if(c == '/') {
      p += 2;
    } else if(c >= '1' && c <= '8') {
      p += c - '0';
    } else {
      switch(c) {
      case 'P': pos.set(p, Pieces::P); break;
      case 'N': pos.set(p, Pieces::N); break;
      case 'B': pos.set(p, Pieces::B); break;
      case 'R': pos.set(p, Pieces::R); break;
      case 'Q': pos.set(p, Pieces::Q); break;
      case 'K': pos.set(p, Pieces::K); break;
      case 'p': pos.set(p, -Pieces::P); break;
      case 'n': pos.set(p, -Pieces::N); break;
      case 'b': pos.set(p, -Pieces::B); break;
      case 'r': pos.set(p, -Pieces::R); break;
      case 'q': pos.set(p, -Pieces::Q); break;
      case 'k': pos.set(p, -Pieces::K); break;
      }
      p++;
    }
  }
  // TODO: parse castling, ep
  fill(all(pos.castling), true);
  return pos;
}

array<array<int, 13>, 120> zobrist;
void init_zobrist() {
  for(int i = 0; i < 120; i++) {
    for(int j = 0; j < 13; j++) {
      zobrist[i][j] = rand();
    }
  }
}

struct tt_entry {
  int dep;
  int a;
  Move best_move;
};

unordered_map<int, tt_entry> tt;

int tt_hash(Pos pos) {
  int res = 0;
  for(int i = 21; i <= 98; i++) {
    if(pos.at(i) != 0) {
      res ^= zobrist[i][pos.at(i) + 6];
    }
  }
  if(pos.castling[0]) res ^= zobrist[0][0];
  if(pos.castling[1]) res ^= zobrist[0][1];
  if(pos.castling[2]) res ^= zobrist[0][2];
  if(pos.castling[3]) res ^= zobrist[0][3];
  if(pos.ep) res ^= zobrist[0][pos.ep % 8];
  return res;
}

pair<Move, int> eval(Pos& pos, int dep, int a = -inf, int b = inf, bool can_null = false) {
  if(pos.score < -30000) return {{}, -30000 - dep};
  if(rand() % 1000000 == 0) debug(tt.size());
  dep = max(dep, 0);
  if(dep >= 3 && can_null) {
    // null move
    auto md = pos.make_move({0, 0});
    auto res = eval(pos, dep - 3, -b, -b + 1, false);
    pos.undo_move(md);
    if(res.second >= b) return {{}, res.second};
  }
  vector<Move> moves = pos.moves();
  Move best_move{-1, -1};
  if(tt.find(tt_hash(pos)) != tt.end()) {
    auto tte = tt[tt_hash(pos)];
    if(tte.best_move.from != -1) {
      for(auto m : moves) {
        if(m == tte.best_move) {
          best_move = m;
          break;
        }
      }
    }
    if(tte.dep >= dep) {
      a = max(a, tte.a);
    }
  }
  if(dep) {
    if(best_move == Move{-1, -1}) {
      best_move = eval(pos, dep - 2, a, b).first;
    }
    if(!(best_move == Move{-1, -1})) {
      moves.insert(moves.begin(), best_move);
    }
  } else {
    a = max(a, pos.score);
  }
  random_shuffle(all(moves));
  for(auto m : moves) {
    if(a >= b) break;
    if(dep == 0 && pos.at(m.to) == 0) continue;
    // Pos q = pos;
    auto md = pos.make_move(m);
    int score = -eval(pos, dep - 1, -b, -a).second;
    if(score > a) {
      a = score;
      best_move = m;
    }
    pos.undo_move(md);
  }
  // tt[tt_hash(pos)] = {dep, a, best_move};
  if(tt.size() > 1e7) tt.clear();
  return {best_move, a};
}

int main() {
  srand(time(0));
  init_dirs();
  init_pst();
  init_zobrist();
  cout << "Print moves only (0/1): ";
  int PRINT_MOVES; cin >> PRINT_MOVES;
  if(PRINT_MOVES) {
    cerr.setstate(ios_base::badbit);
  }
  cout << "Bot colour (0/1): ";
  int BOT_COLOUR; cin >> BOT_COLOUR;
  string starting_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  // starting_pos = "7k/8/8/8/8/8/PPPPPPPP/RNBQKBNR";
  Pos pos = parse_fen(starting_pos);
  print_board(pos);
  vector<MoveDiff> hist;
  for(int i = 0; i < 6000; i++) {
    if(abs(pos.score) >= 30000) break;
    if(i % 2 == BOT_COLOUR) {
      int start_time = clock();
      Move m;
      int score;
      int dep = 1;
      while(true) {
        tie(m, score) = eval(pos, dep);
        if(score >= 30000) break;
        debug(((double)(clock() - start_time) / CLOCKS_PER_SEC));
        if(((double)(clock() - start_time) / CLOCKS_PER_SEC) > 1) break;
        dep++;
      }
      int end_time = clock();
      cout << to_uci(m, i % 2);
      if(pos.at(m.from) == Pieces::P && m.from >= 21 && m.from <= 28) cout << "q";
      cerr << " " << score << " dep " << dep << " time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << "secs";
      cout << endl;
      hist.push_back(pos.make_move(m));
      print_board(pos, i % 2 != BOT_COLOUR);
    } else {
      auto legal_moves = pos.moves();
      Move m = {0, 0};
      do {
        cout << "> ";
        string s; cin >> s;
        if(s == "undo") {
          if(hist.size() >= 2) {
            pos.undo_move(hist.back());
            hist.pop_back();
            pos.undo_move(hist.back());
            hist.pop_back();
            legal_moves = pos.moves();
            print_board(pos, i % 2 == BOT_COLOUR);
          }
          continue;
        }
        m = from_uci(s);
        if(BOT_COLOUR == 0) {
          m.from = 119 - m.from;
          m.to = 119 - m.to;
        }
      } while(find(all(legal_moves), m) == legal_moves.end());
      br:
      hist.push_back(pos.make_move(m));
      print_board(pos, i % 2 != BOT_COLOUR);
    }
  }
}
