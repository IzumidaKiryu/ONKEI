#include "stdafx.h"
#include "ReadJSON.h"

#include "json.hpp"
#include "fstream"

// using 宣言で短く
using json = nlohmann::json;


ReadJSON::~ReadJSON() = default;

ChartData ReadJSON::LoadChart(const std::string& filePath)
{
    ChartData chart;

    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {//ファイルが見つからない、開けないとき。
        printf("譜面ファイルが開けません: %s\n", filePath.c_str());
        return chart;
    }

    json j;
    ifs >> j;

    chart.songName = j.value("songName", "Unknown");
    chart.bpm = j.value("bpm", 120);
    chart.offset = j.value("offset", 0.0);

    for (auto& v : j["notes"]) {
        NoteData note;
        note.time = v.value("time", 0.0f);
        note.lane = v.value("lane", 0);
        note.type = v.value("type", "tap");
        note.duration = v.value("duration", 0);
        chart.notes.push_back(note);//スタックへ格納
        m_notes.push_back(note);//プライベート変数へ格納
    }

    return chart;
}
