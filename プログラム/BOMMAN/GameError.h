// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// gameError.h v1.0

// ゲームエンジン（※1）によってスローされるＥｒｒｏｒクラス

#ifndef _GAMEERROR_H            // prevent multiple definitions if this 
#define _GAMEERROR_H            // ..file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace gameErrorNS
{
    // Error codes
    // 負の数は ゲームをシャットダウンする必要がある致命的 errors.
    // 正の数は ゲームをシャットダウンする必要がない警告.
    const int FATAL_ERROR = -1;
    const int WARNING = 1;
}

// Game Error class. game engine によってエラー検出されたときにスローされる.
// std::exception を継承
class GameError : public std::exception
{
private:
    int     errorCode;
    std::string message;
public:
    // default constructor
    GameError() throw() :errorCode(gameErrorNS::FATAL_ERROR), message("Undefined Error in game.") {}
    // copy constructor
    GameError(const GameError& e) throw(): std::exception(e), errorCode(e.errorCode), message(e.message) {}
    // constructor 引数あり
    GameError(int code, const std::string &s) throw() :errorCode(code), message(s) {}
    // 代入演算子
    GameError& operator= (const GameError& rhs) throw() 
    {
        std::exception::operator=(rhs);
        this->errorCode = rhs.errorCode;
        this->message = rhs.message;
    }
    // デストラクタ
    virtual ~GameError() throw() {};

    // base class に対するオーバーライド
    virtual const char* what() const throw() { return this->getMessage(); }

    const char* getMessage() const throw() { return message.c_str(); }
    int getErrorCode() const throw() { return errorCode; }
};
#endif

/*
※1ゲームエンジンとは、コンピュータゲームのソフトウェアにおいて
共通して用いられる主要な処理を代行し効率化するソフトウェアの総称である。
*/