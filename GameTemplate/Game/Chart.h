#pragma once
#include <string>
#include <vector>

struct NoteData {
    float time;   // 出現タイミング（秒）
    int lane;     // レーン番号
    std::string type;
    float duration; // 長押しノーツ用。
};
struct ChartData {
    std::string songName;
    int bpm;
    float offset;
    std::vector<NoteData> notes;
};