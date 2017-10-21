// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// gameError.h v1.0

// �Q�[���G���W���i��1�j�ɂ���ăX���[�����d���������N���X

#ifndef _GAMEERROR_H            // prevent multiple definitions if this 
#define _GAMEERROR_H            // ..file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace gameErrorNS
{
    // Error codes
    // ���̐��� �Q�[�����V���b�g�_�E������K�v������v���I errors.
    // ���̐��� �Q�[�����V���b�g�_�E������K�v���Ȃ��x��.
    const int FATAL_ERROR = -1;
    const int WARNING = 1;
}

// Game Error class. game engine �ɂ���ăG���[���o���ꂽ�Ƃ��ɃX���[�����.
// std::exception ���p��
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
    // constructor ��������
    GameError(int code, const std::string &s) throw() :errorCode(code), message(s) {}
    // ������Z�q
    GameError& operator= (const GameError& rhs) throw() 
    {
        std::exception::operator=(rhs);
        this->errorCode = rhs.errorCode;
        this->message = rhs.message;
    }
    // �f�X�g���N�^
    virtual ~GameError() throw() {};

    // base class �ɑ΂���I�[�o�[���C�h
    virtual const char* what() const throw() { return this->getMessage(); }

    const char* getMessage() const throw() { return message.c_str(); }
    int getErrorCode() const throw() { return errorCode; }
};
#endif

/*
��1�Q�[���G���W���Ƃ́A�R���s���[�^�Q�[���̃\�t�g�E�F�A�ɂ�����
���ʂ��ėp�������v�ȏ������s������������\�t�g�E�F�A�̑��̂ł���B
*/