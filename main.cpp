#define HAVE_CURL
#include <time.h>
#include <string>
#include <vector>
#include <random>
#include <tgbot/tgbot.h>
template <typename T> void exception_thrower(T e)
{
    if(!e)
        throw std::runtime_error("assert");
}
#define RAPIDJSON_ASSERT(x) exception_thrower(x);
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "lunardate.h"
#include "string.h"

using namespace TgBot;

rapidjson::Document dict;

constexpr int currentTimezone = 8;
static const char* weekDayChar[] = {"日", "一", "二", "三", "四", "五", "六"};
static const char* seasonEmoji[] = {"🌻", "🏝️", "🍂", "⛄"};

struct Date
{
    int year;
    int month;
    int day;
    int weekday;
    int season;
    LunarDate lunarDate;

    Date(time_t t = time(nullptr) + currentTimezone * 3600)
    {
        struct tm *gtm = gmtime(&t);
        year = gtm->tm_year + 1900;
        month = gtm->tm_mon + 1;
        day = gtm->tm_mday;
        weekday = gtm->tm_wday;
        lunarDate = getLunarDate(year, month, day);
        switch(month)
        {
            case 3:
                if(day < 21)
                {
                    season = 3;
                    break;
                }
            case 4:
            case 5:
                season = 0;
                break;
            case 6:
                if(day < 22)
                {
                    season = 0;
                    break;
                }
            case 7:
            case 8:
                season = 1;
                break;
            case 9:
                if(day < 23)
                {
                    season = 1;
                    break;
                }
            case 10:
            case 11:
                season = 2;
                break;
            case 12:
                if(day < 22)
                {
                    season = 2;
                    break;
                }
            case 1:
            case 2:
                season = 3;
                break;
        }
    }

    std::string dumpShort() const
    {
        return std::to_string(year) + " 年 " + std::to_string(month) + " 月 " + std::to_string(day) + " 日星期" + weekDayChar[weekday];
    }

    std::string dump(bool today = true) const
    {
        return "📅 " + std::string(today ? "今" : "这") + "天是 " + std::to_string(year) + " 年 " + std::to_string(month) + " 月 " + std::to_string(day) + " 日\n"
        + seasonEmoji[season] + " " + lunarDate.dump() + "\n\n";
    }
};

struct GoodOrBad
{
    std::string goodName;
    std::string goodReason;
    std::string badName;
    std::string badReason;

    explicit GoodOrBad(float random1, float random2, float random3, float random4)
    {
        static size_t totalCount = dict["goodorbaddict"].Size();
        size_t index1 = totalCount * random1, index2 = totalCount * random2;
        size_t goodCount = dict["goodorbaddict"][index1]["reasonforgood"].Size(), badCount = dict["goodorbaddict"][index2]["reasonforbad"].Size();
        size_t goodIndex = goodCount * random3, badIndex = badCount * random4;
        goodName = dict["goodorbaddict"][index1]["action"].GetString();
        goodReason = dict["goodorbaddict"][index1]["reasonforgood"][goodIndex].GetString();
        badName = dict["goodorbaddict"][index2]["action"].GetString();
        badReason = dict["goodorbaddict"][index2]["reasonforbad"][badIndex].GetString();
    }

    std::string dump(const std::string &nickname, bool today = true) const
    {
        return "<b>黄历姬掐指一算，" + nickname + " " + (today ? "今" : "这") + "天：</b>\n"
        "· 宜" + goodName + "：" + goodReason + "\n"
        "· 忌" + badName + "：" + badReason + "\n\n";
    }
};

struct Recommendation
{
    std::string mugOfTheDay;
    std::string direction;
    std::string slot;

    std::vector<std::string> directions {"东", "东南", "南", "西南", "西", "西北", "北", "东北"};
    std::vector<std::string> slots {"P1", "P2"};

    explicit Recommendation(float random1, float random2, float random3)
    {
        static size_t mugCount = dict["mugoftheday"].Size(), dirCount = directions.size(), slotCount = slots.size();
        size_t mugIndex = mugCount * random1, dirIndex = dirCount * random2, slotIndex = slotCount * random3;
        mugOfTheDay = dict["mugoftheday"][mugIndex].GetString();
        direction = directions[dirIndex];
        slot = slots[slotIndex];
    }

    std::string dump(const std::string &nickname, bool today = true) const
    {
        return "<b>黄历姬为 " + nickname + " 推荐：</b>\n"
        "· " + (today ? "今" : "本") + "日音游：" + mugOfTheDay + "\n"
        "· 打手机或平板音游最佳朝向：" + direction + "\n"
        "· 街机音游黄金位：" + slot;
    }
};

std::string getTungShing(int64_t userid, const std::string &nickname, const Date &date = Date(), bool today = true)
{
    std::string seed = std::to_string(userid) + std::to_string(date.year) + std::to_string(date.month) + std::to_string(date.day);
    std::mt19937 generator(std::stoull(seed));
    std::uniform_real_distribution<double> distrib(0.0, 1.0);
    float random1 = distrib(generator), random2 = distrib(generator), random3 = distrib(generator), random4 = distrib(generator), random5 = distrib(generator), random6 = distrib(generator), random7 = distrib(generator);
    GoodOrBad goodOrBad{random1, random2, random3, random4};
    Recommendation recommendation{random5, random6, random7};
    return date.dump(today) + goodOrBad.dump(nickname, today) + recommendation.dump(nickname, today);
}

std::string fileGet(const std::string &path)
{
    std::string content;
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if(fp)
    {
        std::fseek(fp, 0, SEEK_END);
        long tot = std::ftell(fp);
        content.resize(tot);
        std::rewind(fp);
        std::fread(&content[0], 1, tot, fp);
        std::fclose(fp);
    }
    return content;
}

std::string getUserFullName(User::Ptr user)
{
    return trimOf(user->firstName + " " + user->lastName, ' ', true, true);
}

std::string getUserMention(User::Ptr user, bool markdown = true)
{
    if(markdown)
        return "[" + getUserFullName(user) + "](tg://user?id=" + std::to_string(user->id) + ")";
    else
        return "<a href=\\\"tg://user?id=" + std::to_string(user->id) + "\\\">" + escapeHTMLCharacters(getUserFullName(user)) + "</a>";
}

std::string getEnv(const std::string &name)
{
    std::string retVal;
#ifdef _WIN32
    char chrData[1024] = {};
    if(GetEnvironmentVariable(name.c_str(), chrData, 1023))
        retVal.assign(chrData);
#else
    char *env = getenv(name.c_str());
    if(env != NULL)
        retVal.assign(env);
#endif // _WIN32
    return retVal;
}

time_t parseDate(const std::string &date)
{
    if (date.size() != 8)
        return -1;
    try
    {
        return getTime(std::stoi(date.substr(0, 4)), std::stoi(date.substr(4, 2)), std::stoi(date.substr(6, 2)));
    }
    catch(const std::exception& e)
    {
        return -1;
    }
}

int main()
{
    dict.Parse(fileGet("dict.json").data());
    if(dict.HasParseError())
        throw std::runtime_error(rapidjson::GetParseError_En(dict.GetParseError()));

    std::string token = getEnv("BOT_TOKEN"), port = getEnv("PORT"), urlBase = getEnv("URL_BASE");
    if(token.empty())
        throw std::invalid_argument("missing BOT_TOKEN");

    Bot bot(token);
    bot.getEvents().onCommand("today", [&bot](Message::Ptr message)
    {
        std::string result = getTungShing(message->from->id, "您");
        bot.getApi().sendMessage(message->chat->id, result, false, message->messageId, {}, "HTML");
    });
    bot.getEvents().onCommand("history", [&bot](Message::Ptr message)
    {
        auto arguments = split(message->text, " ");
        std::string result;
        time_t t;
        if(arguments.size() < 2 || (t = parseDate(arguments[1])) == -1)
            result = "格式: /history [日期]";
        else
            result = getTungShing(message->from->id, "您", Date(t), false);
        bot.getApi().sendMessage(message->chat->id, result, false, message->messageId, {}, "HTML");
    });
    bot.getEvents().onCommand("ping", [&bot](Message::Ptr message)
    {
        std::string result {"Pong! | " + std::to_string(time(nullptr) - message->date)};
        bot.getApi().sendMessage(message->chat->id, result, false, message->messageId, {}, "HTML");
    });
    bot.getEvents().onInlineQuery([&bot](InlineQuery::Ptr query)
    {
        InlineQueryResultArticle res;
        InputTextMessageContent msg;
        
        InlineKeyboardButton btn;
        btn.text = "给我也整一个！";

        time_t t = 0;
        msg.parseMode = "HTML";
        if(query->query.empty() || (t = parseDate(query->query)) == -1)
        {
            std::string result = getTungShing(query->from->id, getUserMention(query->from, false));
            msg.messageText = std::move(result);
            res.title = "📅 " + getUserFullName(query->from) + " 的今日黄历";
            res.description = "查询 " + Date().dumpShort() + "的音游黄历";
            if(t == -1)
                res.description += "\n（不正确的日期格式，将使用今天）";
            btn.switchInlineQueryCurrentChat = " ";
        }
        else
        {
            Date date(t);
            std::string result = getTungShing(query->from->id, getUserMention(query->from, false), date, false);
            msg.messageText = std::move(result);
            res.title = "📅 " + getUserFullName(query->from) + " 的当日黄历";
            res.description = "查询 " + date.dumpShort() + "的音游黄历";
            btn.switchInlineQueryCurrentChat = query->query;
        }
        res.id = "0";
        res.thumbUrl = "https://i.ibb.co/WDf2wqB/image.png";
        res.inputMessageContent = std::make_shared<InputTextMessageContent>(msg);

        InlineKeyboardMarkup kb;
        kb.inlineKeyboard.push_back(std::vector{std::make_shared<InlineKeyboardButton>(btn)});
        res.replyMarkup = std::make_shared<InlineKeyboardMarkup>(kb);

        std::vector<InlineQueryResult::Ptr> res_vec;
        res_vec.emplace_back(std::make_shared<InlineQueryResultArticle>(res));
        bot.getApi().answerInlineQuery(query->id, res_vec, 1, true);
    });

    if(port.empty() || urlBase.empty())
    {
        while(true)
        {
            try
            {
                TgLongPoll longpoll(bot);
                while(true)
                {
                    std::cout << "longpoll started\n";
                    longpoll.start();
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }
    else
    {
       try
        {
            TgWebhookTcpServer webhook(std::stoi(port), bot);
            bot.getApi().setWebhook(urlBase + token);
            std::cout << "webhook started on port " << port << "\n";
            webhook.start();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;

}
