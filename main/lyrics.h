#pragma once
#include <string>
#include <vector>
#include <algorithm>

struct LyricLine {
    int time_ms;
    std::string text;
};

inline std::vector<LyricLine> ParseLrc(const std::string& lrc_text) {
    std::vector<LyricLine> out;
    size_t i = 0;
    while (i < lrc_text.size()) {
        size_t start = lrc_text.find('[', i);
        if (start == std::string::npos) break;
        size_t end = lrc_text.find(']', start);
        if (end == std::string::npos) break;
        std::string tag = lrc_text.substr(start + 1, end - start - 1);
        i = end + 1;
        int min = 0, sec = 0, ms = 0;
        int parsed = 0;
        if (tag.find('.') != std::string::npos) {
            parsed = sscanf(tag.c_str(), "%d:%d.%d", &min, &sec, &ms);
        } else {
            parsed = sscanf(tag.c_str(), "%d:%d:%d", &min, &sec, &ms);
        }
        if (parsed >= 2) {
            size_t line_end = lrc_text.find('\n', i);
            if (line_end == std::string::npos) line_end = lrc_text.size();
            std::string text = lrc_text.substr(i, line_end - i);
            while (!text.empty() && text[0] == ' ') text.erase(0, 1);
            while (!text.empty() && (text.back() == ' ' || text.back() == '\r')) text.pop_back();
            if (!text.empty()) {
                out.push_back({min * 60000 + sec * 1000 + ms, text});
            }
            i = line_end + 1;
        }
    }
    std::sort(out.begin(), out.end(), [](const LyricLine& a, const LyricLine& b) {
        return a.time_ms < b.time_ms;
    });
    return out;
}

inline int FindLyricLine(const std::vector<LyricLine>& lyrics, int position_ms) {
    if (lyrics.empty()) return -1;
    int idx = -1;
    for (int i = 0; i < (int)lyrics.size(); i++) {
        if (position_ms >= lyrics[i].time_ms) idx = i;
        else break;
    }
    return idx;
}
