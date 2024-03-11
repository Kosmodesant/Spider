#include "wordSearch.h"

const std::wregex
WordSearch::space_reg{ LR"(\s+)" },
WordSearch::body_reg{ LR"(< ?body[^>]*>(.+)< ?/ ?body>)" },
WordSearch::url_reg{ LR"!!(<\s*A\s+[^>]*href\s*=\s*"(http[^"]*)")!!", std::regex::icase },
WordSearch::title_reg{ LR"(< ?title ?>(.+)< ?/ ?title>)" },
WordSearch::token_reg{ LR"(<[^>]*>)" },
WordSearch::punct_reg{ LR"([[:punct:]])" },
WordSearch::number_reg{ LR"(\w*[0-9]\w*)" };

std::pair<WordMap, LinkList> WordSearch::getWordLink(std::wstring page, unsigned int recLevel)
{
    page = std::regex_replace(page, space_reg, L" ");
    auto it = std::wsregex_token_iterator(page.begin(), page.end(), title_reg, 1);
    std::wstring title;
    if (it != std::wsregex_token_iterator{}) title = *it;
    it = std::wsregex_token_iterator(page.begin(), page.end(), body_reg, 1);
    if (it != std::wsregex_token_iterator{}) page = *it;
    else page.clear();
    LinkList links;
    auto it_start(std::wsregex_token_iterator{ page.begin(), page.end(), url_reg, 1 });
    auto it_end(std::wsregex_token_iterator{});
    --recLevel;
    for (auto it(it_start); it != it_end; ++it)
    {
        std::wstring link_ws(*it);
        std::string link_str(wideUtf2utf8(link_ws));
        links.push_back( { link_str, recLevel } );
    }
    page += std::move(title);
    page = std::regex_replace(page, token_reg, L" ");
    page = std::regex_replace(page, punct_reg, L" ");
    page = std::regex_replace(page, number_reg, L" ");
    boost::locale::generator gen;
    std::locale loc = gen("");
    std::locale::global(loc);
    page = boost::locale::to_lower(page);
    std::wstringstream stream(std::move(page));
    WordMap wordmap;
    std::wstring word;
    while (std::getline(stream, word, L' '))
    {
        if (word.size() > 2 && word.size() < 33)
            ++wordmap[word];
    }

    return { wordmap, links };
}