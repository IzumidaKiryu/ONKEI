#pragma once
// JSONファイルの読み込み用クラス(ReadJSON.h)

#include <string>
#include <vector>
#include "Chart.h"   // ChartData / NoteData の定義を含む想定

/// <summary>
/// 譜面(JSON)を読み込み、ChartData として返しつつ、
/// 直近の読み込み結果のノーツ配列を保持するユーティリティ。
/// </summary>
class ReadJSON : public IGameObject {
public:
    ReadJSON() = default;
    ~ReadJSON();

    /// <summary>
    /// 指定パスのJSONを読み込んで ChartData を構築して返す。
    /// 失敗時は空の ChartData を返す（songName=Unknown, notes空）。
    /// </summary>
    /// <param name="filePath">UTF-8想定のJSONファイルパス</param>
    ChartData LoadChart(const std::string& filePath);

    /// <summary>
    /// 直近の LoadChart() で読み込んだノーツ配列を取得。
    /// </summary>
    const std::vector<NoteData>& GetNotes() const { return m_notes; }

private:
    // JSONから読み込んだノーツを保持（直近の読み込み結果）
    std::vector<NoteData> m_notes;
};
